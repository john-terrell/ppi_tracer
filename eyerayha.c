/*
** EyeRayHandlers.c
**
** Functions to shoot eye rays.
**
** 18 Aug 1992 - Created    - J. Terrell
*/
#include "headers/tracer.h"

void rayhandler_simple(struct Ray *ray,struct AAInfo *aai)
{

  (gl_env->fuzzyrayhandler)(ray,gl_env->CameraVoxel);

  if(!ray->vxo && gl_env->backdrop_shader) {

    (gl_env->backdrop_shader)(ray);

  }

}

void rayhandler_stochastic_low(struct Ray *ray,struct AAInfo *aai)
{

  float color[3];
  float pl[3];
  struct Ray aray;
  float pwidth,pheight;
  float jmaxv,jmaxh;
  float offset;
  short i;
  float voff,hoff;

  pwidth = gl_oem->oem_StepH * 0.5;
  pheight= gl_oem->oem_StepV * 0.5;

  jmaxh = pwidth * 0.25;
  jmaxv = pwidth * 0.25;

	color[0] = aai->aai_StartIntensity[0];
	color[1] = aai->aai_StartIntensity[1];
	color[2] = aai->aai_StartIntensity[2];

	aai->aai_StartIntensity[0] = 0.0;
	aai->aai_StartIntensity[1] = 0.0;
	aai->aai_StartIntensity[2] = 0.0;

	aai->aai_StartSection = 2;

  for(i = aai->aai_StartSection; i < 4; i++) {

    aray.o[0] = ray->o[0];
    aray.o[1] = ray->o[1];
    aray.o[2] = ray->o[2];

    switch(i) {

      case 0:   /* Lower Left */
        hoff = gl_oem->oem_CurrentH - pwidth;
        voff = gl_oem->oem_CurrentV - pheight;
        break;

      case 1:   /* Upper Left */
        hoff = gl_oem->oem_CurrentH - pwidth;
        voff = gl_oem->oem_CurrentV + pheight;
        break;

      case 2:   /* Lower Right */
        hoff = gl_oem->oem_CurrentH + pwidth;
        voff = gl_oem->oem_CurrentV - pheight;
        break;
      
      case 3:   /* Upper Right */
        hoff = gl_oem->oem_CurrentH + pwidth;
        voff = gl_oem->oem_CurrentV + pheight;
        break;

    }

    offset = (2.0 * MGETRND) - 1.0;
    hoff += (jmaxh * offset);

    MRAYPOS(gl_env->Camera->cam_ViewPlaneUpperLeft,gl_env->Camera->cam_ViewRight,
      hoff,pl);

    offset = (2.0 * MGETRND) - 1.0;
    voff += (jmaxv * offset);

    pl[0] += (-gl_env->Camera->cam_ViewUp[0] * voff);
    pl[1] += (-gl_env->Camera->cam_ViewUp[1] * voff);
    pl[2] += (-gl_env->Camera->cam_ViewUp[2] * voff);

    aray.d[0] = pl[0] - aray.o[0];
    aray.d[1] = pl[1] - aray.o[1];
    aray.d[2] = pl[2] - aray.o[2];

    normalize(aray.d);

    pt_InitRay(&aray,1);

    (gl_env->fuzzyrayhandler)(&aray,gl_env->CameraVoxel);
    
    if(!ray->vxo && gl_env->backdrop_shader) {

      (gl_env->backdrop_shader)(&aray);

    }

    color[0] += aray.ci[0];
    color[1] += aray.ci[1];
    color[2] += aray.ci[2];

		if(i > 1) {

			aai->aai_StartIntensity[0] += aray.ci[0];
			aai->aai_StartIntensity[1] += aray.ci[1];
			aai->aai_StartIntensity[2] += aray.ci[2];
			
		}

  }

  ray->ci[0] = color[0] * 0.25;
  ray->ci[1] = color[1] * 0.25;
  ray->ci[2] = color[2] * 0.25;

}

void rayhandler_stochastic_medium(struct Ray *ray,struct AAInfo *aai)
{

  float color[3];
  float pl[3];
  struct Ray aray;
  float pwidth,pheight;
  float jmaxv,jmaxh;
  float offset;
  short i;
  float voff,hoff;

  pwidth = gl_oem->oem_StepH * 0.5;
  pheight= gl_oem->oem_StepV * 0.5;

  jmaxh = pwidth * 0.25;
  jmaxv = pwidth * 0.25;

	color[0] = aai->aai_StartIntensity[0];
	color[1] = aai->aai_StartIntensity[1];
	color[2] = aai->aai_StartIntensity[2];
	
	aai->aai_StartIntensity[0] = 0.0;			
	aai->aai_StartIntensity[1] = 0.0;			
	aai->aai_StartIntensity[2] = 0.0;			
	aai->aai_StartSection = 3;

  for(i = aai->aai_StartSection; i < 9; i++) {

    aray.o[0] = ray->o[0];
    aray.o[1] = ray->o[1];
    aray.o[2] = ray->o[2];

    switch(i) {

      case 0:   /* Lower Left */
        hoff = gl_oem->oem_CurrentH - pwidth;
        voff = gl_oem->oem_CurrentV - pheight;
        break;

      case 1:   /* Center Left */
        hoff = gl_oem->oem_CurrentH - pwidth;
        voff = gl_oem->oem_CurrentV;
        break;

      case 2:   /* Upper Left */
        hoff = gl_oem->oem_CurrentH - pwidth;
        voff = gl_oem->oem_CurrentV + pheight;
        break;

      case 3:   /* Lower Middle */
        hoff = gl_oem->oem_CurrentH;
        voff = gl_oem->oem_CurrentV - pheight;
        break;

      case 4:   /* Center Middle */
        hoff = gl_oem->oem_CurrentH;
        voff = gl_oem->oem_CurrentV;
        break;
      
      case 5:   /* Upper Middle */
        hoff = gl_oem->oem_CurrentH;
        voff = gl_oem->oem_CurrentV + pheight;
        break;

      case 6:   /* Lower Right */
        hoff = gl_oem->oem_CurrentH + pwidth;
        voff = gl_oem->oem_CurrentV - pheight;
        break;

      case 7:   /* Center Right */
        hoff = gl_oem->oem_CurrentH + pwidth;
        voff = gl_oem->oem_CurrentV;
        break;
      
      case 8:   /* Upper Right */
        hoff = gl_oem->oem_CurrentH + pwidth;
        voff = gl_oem->oem_CurrentV + pheight;
        break;

    }

    offset = (2.0 * MGETRND) - 1.0;
    hoff += (jmaxh * offset);


    MRAYPOS(gl_env->Camera->cam_ViewPlaneUpperLeft,gl_env->Camera->cam_ViewRight,
      hoff,pl);

    offset = (2.0 * MGETRND) - 1.0;
    voff += (jmaxv * offset);

    pl[0] += (-gl_env->Camera->cam_ViewUp[0] * voff);
    pl[1] += (-gl_env->Camera->cam_ViewUp[1] * voff);
    pl[2] += (-gl_env->Camera->cam_ViewUp[2] * voff);

    aray.d[0] = pl[0] - aray.o[0];
    aray.d[1] = pl[1] - aray.o[1];
    aray.d[2] = pl[2] - aray.o[2];

    normalize(aray.d);

    pt_InitRay(&aray,1);

    (gl_env->fuzzyrayhandler)(&aray,gl_env->CameraVoxel);

    if(!ray->vxo && gl_env->backdrop_shader) {

      (gl_env->backdrop_shader)(&aray);

    }

    color[0] += aray.ci[0];
    color[1] += aray.ci[1];
    color[2] += aray.ci[2];

		if(i > 5) {

			aai->aai_StartIntensity[0] += aray.ci[0];
			aai->aai_StartIntensity[1] += aray.ci[1];
			aai->aai_StartIntensity[2] += aray.ci[2];

		}

  }

  ray->ci[0] = color[0] * 0.1111;	/* 0.1111 = 1/9 */
  ray->ci[1] = color[1] * 0.1111;
  ray->ci[2] = color[2] * 0.1111;

}

void rayhandler_stochastic_high(struct Ray *ray,struct AAInfo *aai)
{

  float color[3];
  float pl[3];
  struct Ray aray;
  float pwidth,pheight,pw,ph,pw2,ph2;
  float jmaxv,jmaxh;
  float offset;
  short i;
  float voff,hoff;

  pwidth = gl_oem->oem_StepH * 0.333333;
  pheight= gl_oem->oem_StepV * 0.333333;

	pw = gl_oem->oem_StepH * 0.5;
	ph = gl_oem->oem_StepV * 0.5;

	pw2 = pw * 2.0;
	ph2 = ph * 2.0;

  jmaxh = pwidth * 0.1;
  jmaxv = pwidth * 0.1;

	color[0] = aai->aai_StartIntensity[0];
	color[1] = aai->aai_StartIntensity[1];
	color[2] = aai->aai_StartIntensity[2];
	
	aai->aai_StartIntensity[0] = 0.0;			
	aai->aai_StartIntensity[1] = 0.0;			
	aai->aai_StartIntensity[2] = 0.0;			
	aai->aai_StartSection = 4;

  for(i = aai->aai_StartSection; i < 16; i++) {

    aray.o[0] = ray->o[0];
    aray.o[1] = ray->o[1];
    aray.o[2] = ray->o[2];

    switch(i) {

      case 0:   /* Lower Left */
        hoff = (gl_oem->oem_CurrentH - pw);
        voff = (gl_oem->oem_CurrentV - ph);
        break;

      case 1:   /* Left, First Third */
        hoff = (gl_oem->oem_CurrentH - pw);
        voff = (gl_oem->oem_CurrentV - ph) + pheight;
        break;

			case 2:   /* Left, Second Third */
        hoff = (gl_oem->oem_CurrentH - pw);
        voff = (gl_oem->oem_CurrentV - ph) + ph2;
        break;

			case 3:   /* Upper Left Third */
        hoff = (gl_oem->oem_CurrentH - pw);
        voff = (gl_oem->oem_CurrentV + ph);
        break;

			case 4:   /* First Third, Lower */
        hoff = (gl_oem->oem_CurrentH - pw) + pwidth;
        voff = (gl_oem->oem_CurrentV - ph);
        break;

      case 5:   /* First Third, First Third */
        hoff = (gl_oem->oem_CurrentH - pw) + pwidth;
        voff = (gl_oem->oem_CurrentV - ph) + pheight;
        break;

			case 6:   /* First Third, Second Third */
        hoff = (gl_oem->oem_CurrentH - pw) + pwidth;
        voff = (gl_oem->oem_CurrentV - ph) + ph2;
        break;

			case 7:   /* First Third, Upper */
        hoff = (gl_oem->oem_CurrentH - pw) + pwidth;
        voff = (gl_oem->oem_CurrentV + ph);
        break;

			case 8:   /* Second Third, Lower */
        hoff = (gl_oem->oem_CurrentH - pw) + pw2;
        voff = (gl_oem->oem_CurrentV - ph);
        break;

      case 9:   /* Second Third, First Third */
        hoff = (gl_oem->oem_CurrentH - pw) + pw2;
        voff = (gl_oem->oem_CurrentV - ph) + pheight;
        break;

			case 10:   /* Second Third, Second Third */
        hoff = (gl_oem->oem_CurrentH - pw) + pw2;
        voff = (gl_oem->oem_CurrentV - ph) + ph2;
        break;

			case 11:   /* Second Third, Upper */
        hoff = (gl_oem->oem_CurrentH - pw) + pw2;
        voff = (gl_oem->oem_CurrentV + ph);
        break;

			case 12:   /* Right, Lower */
        hoff = (gl_oem->oem_CurrentH + pw);
        voff = (gl_oem->oem_CurrentV - ph);
        break;

      case 13:   /* Second Third, First Third */
        hoff = (gl_oem->oem_CurrentH + pw);
        voff = (gl_oem->oem_CurrentV - ph) + pheight;
        break;

			case 14:   /* Second Third, Second Third */
        hoff = (gl_oem->oem_CurrentH + pw);
        voff = (gl_oem->oem_CurrentV - ph) + ph2;
        break;

			case 15:   /* Second Third, Upper */
        hoff = (gl_oem->oem_CurrentH + pw);
        voff = (gl_oem->oem_CurrentV + ph);
        break;

    }

    offset = (2.0 * MGETRND) - 1.0;
    hoff += (jmaxh * offset);


    MRAYPOS(gl_env->Camera->cam_ViewPlaneUpperLeft,gl_env->Camera->cam_ViewRight,
      hoff,pl);

    offset = (2.0 * MGETRND) - 1.0;
    voff += (jmaxv * offset);

    pl[0] += (-gl_env->Camera->cam_ViewUp[0] * voff);
    pl[1] += (-gl_env->Camera->cam_ViewUp[1] * voff);
    pl[2] += (-gl_env->Camera->cam_ViewUp[2] * voff);

    aray.d[0] = pl[0] - aray.o[0];
    aray.d[1] = pl[1] - aray.o[1];
    aray.d[2] = pl[2] - aray.o[2];

    normalize(aray.d);

    pt_InitRay(&aray,1);

    (gl_env->fuzzyrayhandler)(&aray,gl_env->CameraVoxel);

    if(!ray->vxo && gl_env->backdrop_shader) {

      (gl_env->backdrop_shader)(&aray);

    }

    color[0] += aray.ci[0];
    color[1] += aray.ci[1];
    color[2] += aray.ci[2];

		if(i > 11) {

			aai->aai_StartIntensity[0] += aray.ci[0];
			aai->aai_StartIntensity[1] += aray.ci[1];
			aai->aai_StartIntensity[2] += aray.ci[2];

		}

  }

  ray->ci[0] = color[0] * 0.0625;  /* 0.0625 = 1/16 */
  ray->ci[1] = color[1] * 0.0625;
  ray->ci[2] = color[2] * 0.0625;

}
