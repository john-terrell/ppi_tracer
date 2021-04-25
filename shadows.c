/*
** Shadows.c
**
** 20 Oct 1992 - Created    - J. Terrell
**
*/
#include "Headers/Tracer.h"

BOOL pt_FilterShadowRay(struct Ray *ray,struct Ray *sray,struct LightSource *ls)
{

  struct Voxel *vox;

  /*
  ** See if surface normal is pointing away from the light source.
  */
  sray->d_dot_norm = MDOT(sray->d,ray->norm);

  if(gl_env->flags & ENVFLAG_NOSHADOWS) return(TRUE);

  if(sray->d_dot_norm <= 0.0) {

    return(FALSE);

  }

  sray->shootingvxo = ray->vxo;
  sray->tdepth = ray->tdepth;

  if(!pt_CheckShadowCache(sray,ls)) {

    vox = (ray->primvox == 0 ? &gl_env->RootVoxel : (struct Voxel *)ray->primvox);

    if(vox->PrimCount != 0) pt_GetShadowIntersection(vox,sray);

    if(!(gl_env->flags & ENVFLAG_ATLEMPTY)) {

      pt_ShadowCheckAlwaysTestList(sray);

    }

    ls->ls_shadowcache = sray->vxo;

  }

  if(sray->vxo) {

    return(FALSE);

  }

  return(TRUE);

}

/* pt_GetShadowIntersection(); Gets any intersections with the shadowray 'ray' */
void pt_GetShadowIntersection(struct Voxel *vox,struct Ray *ray)
{

  struct VoxObject *vxo;
  struct Primitive *prim;
  struct Voxel *newvox;
  struct Voxel *vox_start;
  struct Attribute *attr;
  struct TextureArg ta;

  float oldraymaxt,oldt;
  short functionindex;
  struct Ray H;

  if(vx_IntersectVoxel(vox,ray,TRUE)) {

    if(vox->hitparms[0] > ray->maxt) return;

    if(vox->PrimCount == -1) {     /* This voxel contains more voxels */

      vox_start = pt_GetInternalVoxelIntersection(vox,ray);
      if(vox_start) pt_GetShadowIntersection(vox_start,ray);
      else {

        printf("(S) Voxel ERROR: No intersection in internal voxels.\n");
        ray->vxo = 0;
        return;

      }

    }
    else {

      /*
      ** Run through the primitives in this voxel and find any intersections.
      */
      if(vox->hitparms[1] < ray->maxt) {

        oldraymaxt = ray->maxt;
        ray->maxt = vox->hitparms[1];

      }
      else {

        oldraymaxt = ray->maxt;

      }

      vxo = vox->VoxelContents.lh_Head;
      while(vxo->vxo_Node.ln_Succ) {

        prim = vxo->vxo_Prim;

        functionindex = GET_PRIMTYPE(prim->prm_Flags);

        oldt = ray->t;
        if(IsPrimitiveRayIntersect[functionindex](vxo,ray)) {

          if(!(ray->flags & RFLAG_NORMAL)) normalize(ray->norm);
          if((ray->d_dot_norm = MNEGDOT(ray->d,ray->norm)) < 0.0) {

            ray->norm[0] *= -1.0;
            ray->norm[1] *= -1.0;
            ray->norm[2] *= -1.0;

            ray->d_dot_norm *= -1.0;

          }

          /*
          ** If an object intersected our shadowray, the .prim field will hold it.
          */
          attr = ((struct Primitive *)ray->vxo->vxo_Prim)->prm_Attribute;

          ta.ta_Color[0] = attr->attr_Color[0];
          ta.ta_Color[1] = attr->attr_Color[1];
          ta.ta_Color[2] = attr->attr_Color[2];

          ta.ta_Normal[0] = ray->norm[0];
          ta.ta_Normal[1] = ray->norm[1];
          ta.ta_Normal[2] = ray->norm[2];

          ta.ta_Opacity = attr->attr_Transparency;
          ta.ta_Smoothness = attr->attr_Smoothness;
          ta.ta_Flags = 0;

          if(vxo->vxo_Obj->obj_World2Texture) {

            vertmult(ray->ip,ta.ta_Position,vxo->vxo_Obj->obj_World2Texture);

          }
          else {

            ta.ta_Position[0] = ray->ip[0];
            ta.ta_Position[1] = ray->ip[1];
            ta.ta_Position[2] = ray->ip[2];

          }

          if(!IsListEmpty(&attr->attr_Shaders)) {
   
            sh_GetModifiedSurface(&ta,attr);

          }

          if(ta.ta_Flags & TAFLAG_CLIP) {

            ray->vxo = 0;
            ray->t = oldt;

            ta.ta_Flags &= ~TAFLAG_CLIP;

          }
          else {

            if(ta.ta_Opacity > 0.0) {

              im_GetSurfaceZR(&ta,ray,&H);

              ray->ci[0] *= (ta.ta_Color[0] * ray->rz);
              ray->ci[1] *= (ta.ta_Color[1] * ray->rz);
              ray->ci[2] *= (ta.ta_Color[2] * ray->rz);

              ray->vxo = (void *)0;
              ray->t = oldt;

            }
            else {

              break;  /* Totally opaque object intersected */

            }

          }

        }

        vxo = (struct VoxObject *)vxo->vxo_Node.ln_Succ;

      }

      ray->maxt = oldraymaxt;

      if(!ray->vxo) {            /* No intersections were found in this voxel. */

        newvox = pt_GetNeighborVoxel(vox,vox->hitsides[1]);
        if(newvox) {

          pt_GetShadowIntersection(newvox,ray);

        }

      }

    }

  }

}

void pt_ShadowCheckAlwaysTestList(struct Ray *ray)
{

  struct VoxObject *vxo;
  struct Primitive *prim;
  struct Attribute *attr;
  struct TextureArg ta;

  short functionindex;
  float oldt;
  struct Ray H;

  vxo = gl_env->AlwaysTestList.lh_Head;
  while(vxo->vxo_Node.ln_Succ) {

    prim = vxo->vxo_Prim;

    functionindex = GET_PRIMTYPE(prim->prm_Flags);

    oldt = ray->t;
    if(IsPrimitiveRayIntersect[functionindex](vxo,ray)) {

      /*
      ** If an object intersected our shadowray, the .prim field will hold it.
      */
      if(!(ray->flags & RFLAG_NORMAL)) normalize(ray->norm);
      if((ray->d_dot_norm = MNEGDOT(ray->d,ray->norm)) < 0.0) {

        ray->norm[0] *= -1.0;
        ray->norm[1] *= -1.0;
        ray->norm[2] *= -1.0;

        ray->d_dot_norm *= -1.0;

      }

      attr = ((struct Primitive *)ray->vxo->vxo_Prim)->prm_Attribute;

      ta.ta_Color[0] = attr->attr_Color[0];
      ta.ta_Color[1] = attr->attr_Color[1];
      ta.ta_Color[2] = attr->attr_Color[2];

      ta.ta_Normal[0] = ray->norm[0];
      ta.ta_Normal[1] = ray->norm[1];
      ta.ta_Normal[2] = ray->norm[2];

      ta.ta_Opacity = attr->attr_Transparency;
      ta.ta_Smoothness = attr->attr_Smoothness;
      ta.ta_Flags = 0;

      if(!IsListEmpty(&attr->attr_Shaders)) {
   
        sh_GetModifiedSurface(&ta,attr);

      }

      if(ta.ta_Flags & TAFLAG_CLIP) {

        ray->vxo = 0;
        ray->t = oldt;
        ta.ta_Flags &= ~TAFLAG_CLIP;

      }
      else {

        if(ta.ta_Opacity > 0.0) {

          im_GetSurfaceZR(&ta,ray,&H);

          ray->ci[0] *= (ta.ta_Color[0] * ray->rz);
          ray->ci[1] *= (ta.ta_Color[1] * ray->rz);
          ray->ci[2] *= (ta.ta_Color[2] * ray->rz);

          ray->vxo = (void *)0;
          ray->t = oldt;

    }
        else {

          break;  /* Totally opaque object intersected */

        }

      }

    }

    vxo = vxo->vxo_Node.ln_Succ;

  }

}

short pt_CheckShadowCache(struct Ray *ray,struct LightSource *ls)
{

  struct VoxObject *vxo;
  short functionindex;

  if(!ls->ls_shadowcache) return(0);

  vxo = ls->ls_shadowcache;

  functionindex = GET_PRIMTYPE(vxo->vxo_Prim->prm_Flags);
  if(IsPrimitiveRayIntersect[functionindex](vxo,ray)) {

    ray->vxo = ls->ls_shadowcache;
    return(1);

  }

  return(0);

}

