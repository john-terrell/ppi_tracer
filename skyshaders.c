/*
** SkyShaders.c
**
** Functions that shade rays that enter the sky.
**
** 18 Aug 1992 - Created    - J. Terrell
*/
#include "headers/tracer.h"

/* skyshader_simple() ; simple single color background */
void skyshader_simple(struct Ray *ray)
{

  ray->ci[0] = gl_env->BackDropColor[0];
  ray->ci[1] = gl_env->BackDropColor[1];
  ray->ci[2] = gl_env->BackDropColor[2];

}

/* skyshader_gradient() ; simple color gradient */
void skyshader_gradient(struct Ray *ray)
{

  float length,zenith,horiz;

  length = 1.0 / MDOT(ray->d,ray->d);

  zenith = (ray->d[1] * ray->d[1]) * length;
  horiz  = (ray->d[0] * ray->d[0] + ray->d[2] * ray->d[2]) * length;

  if(ray->d[1] > 0.0) {

    ray->ci[0] = gl_env->ZenithColor[0] * zenith + gl_env->HorizonColor[0] * horiz;
    ray->ci[1] = gl_env->ZenithColor[1] * zenith + gl_env->HorizonColor[1] * horiz;
    ray->ci[2] = gl_env->ZenithColor[2] * zenith + gl_env->HorizonColor[2] * horiz;

  }
  else {

    ray->ci[0] = gl_env->AZenithColor[0] * zenith + gl_env->HorizonColor[0] * horiz;
    ray->ci[1] = gl_env->AZenithColor[1] * zenith + gl_env->HorizonColor[1] * horiz;
    ray->ci[2] = gl_env->AZenithColor[2] * zenith + gl_env->HorizonColor[2] * horiz;

  }

}

/* skyshader_environmentmap() ; Spherical environment map. */
void skyshader_environmentmap(struct Ray *ray)
{

  im_ProcessEnvironmentMap(&gl_env->EnvironmentMap,ray);

}
