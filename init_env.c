/*
** Init_Env.c
**
** functions to init and cleanup an environment structure.
**
** 17 Aug 1992 - Created          - J. Terrell
*/
#include "headers/tracer.h"

short pt_init_environment(void)
{

  gl_env = (struct trace_env *) oem_AllocMem(sizeof(struct trace_env));
  if(!gl_env) return(ERROR_NOMEM);

  gl_env->flags = 0;
  gl_env->Camera = &gl_env->DefaultCamera;

  newlist(&gl_env->CameraList);
  newlist(&gl_env->ImageList);
  newlist(&gl_env->ObjectList);
  newlist(&gl_env->LightSourceList);
  newlist(&gl_env->AttributeList);
  newlist(&gl_env->TextureList);
  newlist(&gl_env->AlwaysTestList);

  gl_env->backdrop_shader     = NULL;
  gl_env->sky_shader          = skyshader_gradient;
  gl_env->eyerayhandler       = rayhandler_stochastic_medium;

  if(gl_env->Camera->cam_Aperture > 0.0) {

    gl_env->fuzzyrayhandler = pt_trace_fuzzyray;

  }
  else {

    gl_env->fuzzyrayhandler = pt_trace_ray;

  }

  gl_env->maxvoxelprims           = 15;
  gl_env->maxvoxeldepth           = 6;
  gl_env->maxtracedepth           = 10;
  gl_env->maxsamples_depthoffield = 8;
  gl_env->maxsamples_reflection   = 8;
  gl_env->maxsamples_transmission = 3;

  gl_env->ZenithColor[0] = 0.0;
  gl_env->ZenithColor[1] = 0.0;
  gl_env->ZenithColor[2] = 1.0;

  gl_env->HorizonColor[0] = 0.4;
  gl_env->HorizonColor[1] = 0.4;
  gl_env->HorizonColor[2] = 1.0;

  gl_env->AZenithColor[0] = 0.0;
  gl_env->AZenithColor[1] = 0.2;
  gl_env->AZenithColor[2] = 0.8;

  gl_env->BackDropColor[0] = 0.0;
  gl_env->BackDropColor[1] = 0.4;
  gl_env->BackDropColor[2] = 0.9;

  gl_env->HazeColor[0] = 0.8;
  gl_env->HazeColor[1] = 0.8;
  gl_env->HazeColor[2] = 0.8;

  gl_env->HazeDistance = 2500.0;

  gl_env->BackDropImage = NULL;

  return(ERROR_OK);

}

void pt_cleanup_environment(void)
{

  struct ImageBuffer *ib;
  struct Camera *cam;
  struct Object *obj;
  struct LightSource *ls;
  struct Attribute *attr;
  struct Texture *tex;
  struct VoxObject *vxo;

  long fbsize;

  /* Camera Cleanup */
  while(!IsListEmpty(&gl_env->CameraList)) {

    cam = (struct Camera *)remtail(&gl_env->CameraList);

    oem_FreeMem(cam,sizeof(struct Camera));

  }

  /* Image Cleanup */
  while(!IsListEmpty(&gl_env->ImageList)) {

    ib = (struct ImageBuffer *)remtail(&gl_env->ImageList);

    fbsize = (long)ib->im_xsize * (long)ib->im_ysize;

    if(ib->im_RedBuffer) oem_FreeMem(ib->im_RedBuffer,fbsize);
    if(ib->im_GrnBuffer) oem_FreeMem(ib->im_GrnBuffer,fbsize);
    if(ib->im_BluBuffer) oem_FreeMem(ib->im_BluBuffer,fbsize);

    oem_FreeMem(ib,sizeof(struct ImageBuffer));

  }

  /* Object Cleanup */
  while(!IsListEmpty(&gl_env->ObjectList)) {

    obj = (struct Object *)remtail(&gl_env->ObjectList);

    pt_FreeObject(obj);

    oem_FreeMem(obj,sizeof(struct Object));

  }

  /* Lightsource Cleanup */
  while(!IsListEmpty(&gl_env->LightSourceList)) {

    ls = (struct LightSource *)remtail(&gl_env->LightSourceList);

    oem_FreeMem(ls,sizeof(struct LightSource));

  }

  /* Attribute Cleanup */
  while(!IsListEmpty(&gl_env->AttributeList)) {

    attr = (struct Attribute *)remtail(&gl_env->AttributeList);

    pt_FreeAttribute(attr);

    oem_FreeMem(attr,sizeof(struct Attribute));

  }

  /* Texture Cleanup */
  while(!IsListEmpty(&gl_env->TextureList)) {

    tex = (struct Texture *)remtail(&gl_env->TextureList);

    if(tex->tex_TextureDataSize > 0)
      oem_FreeMem(tex->tex_TextureData,tex->tex_TextureDataSize);

    oem_FreeMem(tex,sizeof(struct Texture));

  }

  /* Always Test List Cleanup */
  while(!IsListEmpty(&gl_env->AlwaysTestList)) {

    vxo = (struct VoxObject *)remtail(&gl_env->AlwaysTestList);

    oem_FreeMem(vxo,sizeof(struct VoxObject));

  }

  oem_FreeMem(gl_env,sizeof(struct trace_env));

}

void pt_FreeObject(struct Object *obj)
{

  struct Primitive *prim;
  struct VertexNode *vn;
  short type;

  /* Free texture matrix if one exists. */
  if(obj->obj_World2Texture)
    oem_FreeMem(obj->obj_World2Texture,sizeof(struct Matrix44));

  /* Free local matrix if one exists. */
  if(obj->obj_Local2World)
    oem_FreeMem(obj->obj_Local2World,sizeof(struct Matrix44));

  if(obj->obj_Flags & OFLG_INSTANCE) return;

  while(!IsListEmpty(&obj->obj_PrimitiveList)) {

    prim = (struct Primitive *)remtail(&obj->obj_PrimitiveList);

    type = GET_PRIMTYPE(prim->prm_Flags);

    switch(type) {

      case PTYPE_SPHERE:

        oem_FreeMem(prim,sizeof(struct prim_Sphere));
        break;

      case PTYPE_TRIANGLE:

        oem_FreeMem(prim,sizeof(struct prim_Triangle));
        break;

      case PTYPE_PLANE:

        oem_FreeMem(prim,sizeof(struct prim_Plane));
        break;

    }

  }

  while(!IsListEmpty(&obj->obj_VertexList)) {

    vn = (struct VertexNode *)remtail(&obj->obj_VertexList);

    oem_FreeMem(vn,sizeof(struct VertexNode));

  }

}

void pt_FreeAttribute(struct Attribute *attr)
{

  struct TextureEntry *te;

  while(!IsListEmpty(&attr->attr_Shaders)) {

    te = (struct TextureEntry *)remtail(&attr->attr_Shaders);

    oem_FreeMem(te,sizeof(struct TextureEntry));

  }

}