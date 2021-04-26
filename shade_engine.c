/*
** Shade_Engine.c
**
** 17 Sep 1992 - Created    - J. Terrell
**
*/
#include "Headers/tracer.h"

/* sh_Shade_Ray() ; Shades a ray that intersects with a primitive. */
void sh_Shade_Ray(struct Ray *ray)
{

  struct Attribute *attr;
  struct TextureArg ta;
  struct Voxel *vox;

  /*
  ** Normalize surface normal if necessary.
  */
  if(!(ray->flags & RFLAG_NORMAL)) {

    normalize(ray->norm);

  }

  /*
  ** See if surface normal needs to be reversed before shading.
  */
  ray->d_dot_norm = MNEGDOT(ray->d,ray->norm);
  if(ray->d_dot_norm < 0.0) {

    ray->norm[0] *= -1.0;
    ray->norm[1] *= -1.0;
    ray->norm[2] *= -1.0;

    ray->d_dot_norm *= -1.0;

  }

  /*
  ** Set up the texture argument structure.
  */
  attr = ((struct Primitive *)ray->vxo->vxo_Prim)->prm_Attribute;

  ta.ta_Flags = 0;
  ta.ta_Normal[0] = ray->norm[0];
  ta.ta_Normal[1] = ray->norm[1];
  ta.ta_Normal[2] = ray->norm[2];
  ta.ta_Color[0]  = attr->attr_Color[0];
  ta.ta_Color[1]  = attr->attr_Color[1];
  ta.ta_Color[2]  = attr->attr_Color[2];

  if(gl_env->flags & ENVFLAG_NOREFRACTIONS)
    ta.ta_Opacity = 0.0;
  else
    ta.ta_Opacity = attr->attr_Transparency;

  ta.ta_Smoothness = attr->attr_Smoothness;
  ta.ta_Metalness = attr->attr_Metalness;
  ta.ta_IlluminationModel = attr->attr_IlluminationModel;

  /*
  ** If a texture transformation structure exists, transform the world
  ** coordinate of the intersection to texture space.
  */
  if(ray->vxo->vxo_Obj->obj_World2Texture) {

    vertmult(ray->ip,ta.ta_Position,ray->vxo->vxo_Obj->obj_World2Texture);

  }
  else {

    ta.ta_Position[0] = ray->ip[0];
    ta.ta_Position[1] = ray->ip[1];
    ta.ta_Position[2] = ray->ip[2];

  }

  ta.ta_OutputColor[0] = 0.0;
  ta.ta_OutputColor[1] = 0.0;
  ta.ta_OutputColor[2] = 0.0;

  /*
  ** Call any shaders associated with this surface.
  */
  if(!IsListEmpty(&attr->attr_Shaders)) {
   
    sh_GetModifiedSurface(&ta,attr);

  }

  /*
  ** If any texture clipped away the current position, retrace this same ray
  ** starting at the surface position.
  */  
  if(ta.ta_Flags & TAFLAG_CLIP) {

    vox = (ray->primvox == 0 ? &gl_env->RootVoxel : (struct Voxel *)ray->primvox);

    ray->o[0] = ray->ip[0];
    ray->o[1] = ray->ip[1];
    ray->o[2] = ray->ip[2];

    ray->shootingvxo = ray->vxo;

    pt_InitRay(ray,ray->depth);

    pt_trace_ray(ray,vox);

  }
  else {

    /*
    ** Shade this point in space.
    */
    sh_GetLightSourceContributions(ray,&ta);

    ray->ci[0] = MCLAMP(ta.ta_OutputColor[0],0.0,1.0);
    ray->ci[1] = MCLAMP(ta.ta_OutputColor[1],0.0,1.0);
    ray->ci[2] = MCLAMP(ta.ta_OutputColor[2],0.0,1.0);

    /*
    ** Find the contributions from reflected and refracted rays.
    */
    sh_GetSecondaryRayContributions(ray,attr,&ta);

    /*
    ** If global haze is enabled, called the global volume attenuation function.
    */
    if(gl_env->flags & ENVFLAG_HAZEENABLED) {

      sh_ApplyVolumeAttenuation(ray,attr);

    }

  }

}

void sh_GetModifiedSurface(struct TextureArg *ta,struct Attribute *attr)
{

  struct TextureEntry *te;
  
  /*
  ** Call all non-post textures first.
  */
  te = attr->attr_Shaders.lh_Head;
  while(te->te_Node.ln_Succ) {

    if(te->te_Texture->tex_Type != TXTYPE_POST &&
      !(te->te_Texture->tex_Flags & TXFLG_DISABLED)) {

      te->te_Texture->tex_TextureFunc(te->te_Texture,ta);

    }

    te = te->te_Node.ln_Succ;

  }

  /*
  ** Call all post textures after all non-post textures.
  */
  te = attr->attr_Shaders.lh_Head;
  while(te->te_Node.ln_Succ) {

    if(te->te_Texture->tex_Type == TXTYPE_POST &&
      !(te->te_Texture->tex_Flags & TXFLG_DISABLED)) {
 
      te->te_Texture->tex_TextureFunc(te->te_Texture,ta);

    }

    te = te->te_Node.ln_Succ;

  }

}

void sh_GetLightSourceContributions(struct Ray *ray,struct TextureArg *ta)
{

  struct LightSource *ls;
  short index;

  ls = gl_env->LightSourceList.lh_Head;
  while(ls->ls_Node.ln_Succ) {

    if(!(ls->ls_flags & LSFLG_DISABLED)) {

      index = ls->ls_flags & 0xFF;

      (LightSourceHandler[index])(ray,ls,ta);

    }

    ls = ls->ls_Node.ln_Succ;

  }

}

void sh_GetSecondaryRayContributions(struct Ray *ray,struct Attribute *attr,
  struct TextureArg *ta)
{

  float aindex;
  float kr,kt;          /* Reflection and Transmission coefficients */
  float m;              /* Metalness parameter */
  struct Voxel *vox;

  struct Ray reflect;   /* A reflection ray. */
  struct Ray refract;   /* A refraction ray. */

  /*
  ** Determine surface reflectance and transmittance.
  */
  im_GetSurfaceZR(ta,ray,&reflect);

  /*
  ** See if the intersection point is transparent at all
  */
  if(ray->rz > 0.0 && !(gl_env->flags & ENVFLAG_NOREFRACTIONS)) {

    /*
    ** Determine the index of refraction of the material the ray is about to enter.
    */
    if(ray->tdepth & 1) {

      aindex = 1.0;

    }
    else {

      aindex = attr->attr_IndexOfRefr;

    }

    /*
    ** Build the refraction ray.
    ** If function result is not zero, total internal reflection occurs
    */
    ray_refract(ray->d,ray->norm,refract.d,(float)(ray->index/aindex));

    kt = ray->rz;
    kr = ray->rs;

    if(kt > 0.0) {

      pt_InitRay(&refract,ray->depth + 1);

      refract.o[0] = ray->ip[0];
      refract.o[1] = ray->ip[1];
      refract.o[2] = ray->ip[2];

      refract.tdepth = ray->tdepth + 1;
      refract.index = aindex;
      refract.shootingvxo = ray->vxo;

      vox = (ray->primvox == 0 ? &gl_env->RootVoxel : (struct Voxel *)ray->primvox);

      if( !(gl_env->flags & ENVFLAG_NOFUZZYREFR) && 
          (attr->attr_Flags & ATTRFLAG_FUZZYTRANS) && 
          (ray->rs < 1.0) ) {

        fz_trace_ray(&refract,vox,ray->rs,gl_env->maxsamples_transmission);

      }
      else {

        pt_trace_ray(&refract,vox);

      }

      refract.ci[0] *= (ta->ta_Color[0] * kt);
      refract.ci[1] *= (ta->ta_Color[1] * kt);
      refract.ci[2] *= (ta->ta_Color[2] * kt);

      ray->ci[0] += refract.ci[0];
      ray->ci[1] += refract.ci[1];
      ray->ci[2] += refract.ci[2];

    }

    /*
    ** Now, if necessary, trace the reflection ray.
    */
    if(kr > 0.0) {

      pt_InitRay(&reflect,ray->depth + 1);

      reflect.o[0] = ray->ip[0];
      reflect.o[1] = ray->ip[1];
      reflect.o[2] = ray->ip[2];

      reflect.tdepth = ray->tdepth;
      reflect.index = ray->index;
      reflect.shootingvxo = ray->vxo;

      vox = (ray->primvox == 0 ? &gl_env->RootVoxel : (struct Voxel *)ray->primvox);

      if( !(gl_env->flags & ENVFLAG_NOFUZZYREFL) && 
          (attr->attr_Flags & ATTRFLAG_FUZZYREFLECT) && 
          (ray->rs < 1.0) ) {

        fz_trace_ray(&reflect,vox,ray->rs,gl_env->maxsamples_reflection);

      }
      else {

        pt_trace_ray(&reflect,vox);

      }

      m = attr->attr_Metalness;
      if(m > 0.0) {

        reflect.ci[0] += (m * (ta->ta_Color[0] - 1.0));
        reflect.ci[1] += (m * (ta->ta_Color[1] - 1.0));
        reflect.ci[2] += (m * (ta->ta_Color[2] - 1.0));

      }

      ray->ci[0] += (reflect.ci[0] * kr);
      ray->ci[1] += (reflect.ci[1] * kr);
      ray->ci[2] += (reflect.ci[2] * kr);

    }

    ray->ci[0] = MCLAMP(ray->ci[0],0.0,1.0);
    ray->ci[1] = MCLAMP(ray->ci[1],0.0,1.0);
    ray->ci[2] = MCLAMP(ray->ci[2],0.0,1.0);

  }
  else {

    if(ray->rs > 0.0 && !(gl_env->flags & ENVFLAG_NOREFLECTIONS)) {

      reflect.o[0] = ray->ip[0];
      reflect.o[1] = ray->ip[1];
      reflect.o[2] = ray->ip[2];

      /*
      ** Make reflection ray.
      */
      reflect.d[0] = ((2.0 * ray->norm[0]) * ray->d_dot_norm) + ray->d[0];
      reflect.d[1] = ((2.0 * ray->norm[1]) * ray->d_dot_norm) + ray->d[1];
      reflect.d[2] = ((2.0 * ray->norm[2]) * ray->d_dot_norm) + ray->d[2];

      pt_InitRay(&reflect,ray->depth + 1);

      reflect.tdepth = ray->tdepth;
      reflect.index = ray->index;
      reflect.shootingvxo = ray->vxo;

      vox = (ray->primvox == 0 ? &gl_env->RootVoxel : (struct Voxel *)ray->primvox);

      if( !(gl_env->flags & ENVFLAG_NOFUZZYREFL) &&
          (attr->attr_Flags & ATTRFLAG_FUZZYREFLECT) && 
          (ray->rs < 1.0) ) {

        fz_trace_ray(&reflect,vox,ray->rs,gl_env->maxsamples_reflection);

      }
      else {

        pt_trace_ray(&reflect,vox);

      }

      m = attr->attr_Metalness;
      if(m > 0.0) {

        reflect.ci[0] += (m * (ta->ta_Color[0] - 1.0));
        reflect.ci[1] += (m * (ta->ta_Color[1] - 1.0));
        reflect.ci[2] += (m * (ta->ta_Color[2] - 1.0));

      }

      ray->ci[0] += (reflect.ci[0] * ray->rs);
      ray->ci[1] += (reflect.ci[1] * ray->rs);
      ray->ci[2] += (reflect.ci[2] * ray->rs);

      ray->ci[0] = MCLAMP(ray->ci[0],0.0,1.0);
      ray->ci[1] = MCLAMP(ray->ci[1],0.0,1.0);
      ray->ci[2] = MCLAMP(ray->ci[2],0.0,1.0);

    }

  }

}

void sh_ApplyVolumeAttenuation(struct Ray *ray,struct Attribute *attr)
{

  float a;

  a = MCLAMP((ray->t / gl_env->HazeDistance),0.0,1.0);

  ray->ci[0] += (a * (gl_env->HazeColor[0] - ray->ci[0]));
  ray->ci[1] += (a * (gl_env->HazeColor[1] - ray->ci[0]));
  ray->ci[2] += (a * (gl_env->HazeColor[2] - ray->ci[0]));

}
