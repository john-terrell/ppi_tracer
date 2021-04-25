/*
** Ray.c
**
** Functions that manipulate rays
**
** 18 Aug 1992 - Created    - J. Terrell
**
*/
#include "headers/tracer.h"

/* pt_trace_fuzzyray() ; trace a fuzzy depth of field ray */
void pt_trace_fuzzyray(struct Ray *ray,struct Voxel *startvox)
{

  float sum_color[3],random;
  float dir[3];
  short maxsamples,sample;
  struct Ray ray2;

  sum_color[0] = 0.0;
  sum_color[1] = 0.0;
  sum_color[2] = 0.0;

  maxsamples = gl_env->maxsamples_depthoffield;

  for(sample = 0; sample < maxsamples; sample++) {

    dir[0] = ray->o[0] + (ray->d[0] * gl_env->Camera->cam_FocalDistance);
    dir[1] = ray->o[1] + (ray->d[1] * gl_env->Camera->cam_FocalDistance);
    dir[2] = ray->o[2] + (ray->d[2] * gl_env->Camera->cam_FocalDistance);

    ray2.o[0] = ray->o[0];
    ray2.o[1] = ray->o[1];
    ray2.o[2] = ray->o[2];

    random = 0.5 - MGETRND;

    ray2.o[0] += (gl_env->Camera->cam_LensI[0] * random);
    ray2.o[1] += (gl_env->Camera->cam_LensI[1] * random);
    ray2.o[2] += (gl_env->Camera->cam_LensI[2] * random);

    random = 0.5 - MGETRND;

    ray2.o[0] += (gl_env->Camera->cam_LensJ[0] * random);
    ray2.o[1] += (gl_env->Camera->cam_LensJ[1] * random);
    ray2.o[2] += (gl_env->Camera->cam_LensJ[2] * random);

    ray2.d[0] = dir[0] - ray2.o[0];
    ray2.d[1] = dir[1] - ray2.o[1];
    ray2.d[2] = dir[2] - ray2.o[2];

    normalize(ray2.d);

    pt_InitRay(&ray2,1);

    pt_trace_ray(&ray2,gl_env->CameraVoxel);

    if(!ray2.vxo && gl_env->backdrop_shader) {

      (gl_env->backdrop_shader)(&ray2);

    }

    sum_color[0] += ray2.ci[0];
    sum_color[1] += ray2.ci[1];
    sum_color[2] += ray2.ci[2];

  }

  ray->ci[0] = sum_color[0] / (float)maxsamples;
  ray->ci[1] = sum_color[1] / (float)maxsamples;
  ray->ci[2] = sum_color[2] / (float)maxsamples;

}

/* pt_trace_ray() ; trace ray */
void pt_trace_ray(struct Ray *ray,struct Voxel *startvox)
{

  /*
  ** Check current recursion depth.
  */
  if(ray->depth <= gl_env->maxtracedepth) {

    if(startvox->PrimCount != 0) {

      if(vx_IntersectVoxel(startvox,ray,TRUE)) {

        pt_GetIntersection(startvox,ray);

      }

    }

    if(!(gl_env->flags & ENVFLAG_ATLEMPTY)) {

      pt_CheckAlwaysTestList(ray);

    }

    if(ray->vxo) {

      sh_Shade_Ray(ray);

    }
    else {

      if(gl_env->flags & ENVFLAG_HAZEENABLED) {

        ray->ci[0] = gl_env->HazeColor[0];
        ray->ci[1] = gl_env->HazeColor[1];
        ray->ci[2] = gl_env->HazeColor[2];

      }
      else {

        gl_env->sky_shader(ray);

      }

    }

    pt_RenderLights(ray);

  }
  else {

    ray->ci[0] = 0.0;
    ray->ci[1] = 0.0;
    ray->ci[2] = 0.0;

  }

}

/*
** Make sure ray direction is normalized before here.
*/
struct Ray *pt_InitRay(struct Ray *ray,short depth)
{

  ray->flags = 0;

  ray->rd[0] = 1.0 / ray->d[0];
  ray->rd[1] = 1.0 / ray->d[1];
  ray->rd[2] = 1.0 / ray->d[2];

  ray->t = INFINITY;
  ray->maxt = INFINITY;
  ray->index = 1.0;
  ray->depth = depth;
  ray->primvox = 0;
  ray->tdepth = 0;
  ray->shootingvxo = 0;
  ray->vxo = 0;
  return(ray);

}
