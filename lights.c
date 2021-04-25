/*
** Lights.c
**
** Functions to manipulate lights.
**
** 19 Oct 1992 - Created    - J. Terrell
**
*/
#include "Headers/Tracer.h"

void lsh_AmbientSourceHandler(struct Ray *ray,struct LightSource *ls,
  struct TextureArg *ta)
{

  ta->ta_IlluminationModel(ta,ls,NULL);

}

void lsh_PointSourceHandler(struct Ray *ray,struct LightSource *ls,
  struct TextureArg *ta)
{

  struct Ray shadowray;
  float lightdist;
  /*
  ** Build a shadow ray
  */
  shadowray.o[0] = ray->ip[0];
  shadowray.o[1] = ray->ip[1];
  shadowray.o[2] = ray->ip[2];

  shadowray.d[0] = ls->ls_position[0] - shadowray.o[0];
  shadowray.d[1] = ls->ls_position[1] - shadowray.o[1];
  shadowray.d[2] = ls->ls_position[2] - shadowray.o[2];

  lightdist = normalize(shadowray.d);

  pt_InitRay(&shadowray,1);

  shadowray.ci[0] = ls->ls_color[0];
  shadowray.ci[1] = ls->ls_color[1];
  shadowray.ci[2] = ls->ls_color[2];
  shadowray.maxt = lightdist;

  if(pt_FilterShadowRay(ray,&shadowray,ls)) {

    ta->ta_IlluminationModel(ta,ls,&shadowray);

  }

}

void lsh_DistantSourceHandler(struct Ray *ray,struct LightSource *ls,
  struct TextureArg *ta)
{

  struct Ray shadowray;

  /*
  ** Build a shadow ray
  */
  shadowray.o[0] = ray->ip[0];
  shadowray.o[1] = ray->ip[1];
  shadowray.o[2] = ray->ip[2];

  shadowray.d[0] = ls->ls_direction[0];
  shadowray.d[1] = ls->ls_direction[1];
  shadowray.d[2] = ls->ls_direction[2];

  pt_InitRay(&shadowray,1);

  shadowray.ci[0] = ls->ls_color[0];
  shadowray.ci[1] = ls->ls_color[1];
  shadowray.ci[2] = ls->ls_color[2];
  shadowray.maxt = INFINITY;

  if(pt_FilterShadowRay(ray,&shadowray,ls)) {

    ta->ta_IlluminationModel(ta,ls,&shadowray);

  }

}

void lsh_SpotSourceHandler(struct Ray *ray,struct LightSource *ls,
  struct TextureArg *ta)
{

  struct Ray shadowray;
  float lightdist,dp;

  /*
  ** Build a shadow ray
  */
  shadowray.o[0] = ray->ip[0];
  shadowray.o[1] = ray->ip[1];
  shadowray.o[2] = ray->ip[2];

  shadowray.d[0] = ls->ls_position[0] - shadowray.o[0];
  shadowray.d[1] = ls->ls_position[1] - shadowray.o[1];
  shadowray.d[2] = ls->ls_position[2] - shadowray.o[2];

  lightdist = normalize(shadowray.d);

  shadowray.ci[0] = ls->ls_color[0];
  shadowray.ci[1] = ls->ls_color[1];
  shadowray.ci[2] = ls->ls_color[2];

  pt_InitRay(&shadowray,1);

  shadowray.maxt = lightdist;

  /*
  ** Check to see if the intersection point is inside the spot light's spot.
  */
  dp = MNEGDOT(shadowray.d,ls->ls_direction);
  if(dp >= ls->ls_spotminangle) {

    if(pt_FilterShadowRay(ray,&shadowray,ls)) {

      ta->ta_IlluminationModel(ta,ls,&shadowray);

    }

  }

}

void lsh_SphericalSourceHandler(struct Ray *ray,struct LightSource *ls,
  struct TextureArg *ta)
{

  struct Ray shadowray;
  float lightdist,frac;
  struct Voxel *vox;

  /*
  ** Build a shadow ray
  */
  shadowray.o[0] = ray->ip[0];
  shadowray.o[1] = ray->ip[1];
  shadowray.o[2] = ray->ip[2];

  shadowray.d[0] = ls->ls_position[0] - shadowray.o[0];
  shadowray.d[1] = ls->ls_position[1] - shadowray.o[1];
  shadowray.d[2] = ls->ls_position[2] - shadowray.o[2];

  lightdist = normalize(shadowray.d);

  pt_InitRay(&shadowray,1);

  shadowray.maxt = lightdist;

  if(!(gl_env->flags & ENVFLAG_NOSHADOWS)) {

    if(gl_env->flags & ENVFLAG_NOSOFTSHADOWS) {

      frac = 1.0;

    }
    else {

      shadowray.shootingvxo = ray->vxo;
      shadowray.tdepth = ray->tdepth;

      vox = (ray->primvox == 0 ? &gl_env->RootVoxel : (struct Voxel *)ray->primvox);
      frac = soft_shadow(&shadowray,ls,vox);

    }

    shadowray.ci[0] = ls->ls_color[0] * frac;
    shadowray.ci[1] = ls->ls_color[1] * frac;
    shadowray.ci[2] = ls->ls_color[2] * frac;

    if(pt_FilterShadowRay(ray,&shadowray,ls)) {

      ta->ta_IlluminationModel(ta,ls,&shadowray);

    }

  }

}

float radius_values[] = { 1.0, 0.75, 0.5, 0.25, -1.0 };

short lighthits(struct Ray *ray,struct LightSource *ls,struct Voxel *vx,float radius,short seriesnum);
float soft_shadow(struct Ray *ray,struct LightSource *ls,struct Voxel *startvox)
{

  float visible_fraction,*radptr,visible;
  short series_number,hits;

  radptr = &radius_values[0];

  series_number = 0;

  hits = lighthits(ray,ls,startvox,(float)(*radptr * ls->ls_radius),series_number);
  if(hits == 0) return(1.0);

  hits = 0;
  series_number++;

  while(*radptr > 0.0) {

    hits += lighthits(ray,ls,startvox,(float)(*radptr * ls->ls_radius),series_number);

    radptr++;
    series_number++;

  }

  visible = (12.0 * (float)series_number);

  visible_fraction = (visible - (float)hits) / visible;

  return(visible_fraction);

}

float circledata_x[] = { 0.0, 0.707106, 1.0, 0.707106, 0.0, -0.707106, -1.0, -0.707106};
float circledata_y[] = { 1.0, 0.707106, 0.0, -0.707106,-1.0,-0.707106,0.0, 0.707106 };

float circledata_x2[] = {

  0.0,
  0.5,
  0.8660,
  1.0,
  0.8660,
  0.5,
  0.0,
 -0.5,
 -0.8660,
 -1.0,
 -0.8660,
 -0.5,

};

float circledata_y2[] = {

  1.0,
  0.8660,
  0.5,
  0.0,
 -0.5,
 -0.8660,
 -1.0,
 -0.8660,
 -0.5,
  0.0,
  0.5,
  0.8660,

};

short lighthits(struct Ray *ray,struct LightSource *ls,struct Voxel *vx,float radius,
  short seriesnum)
{

  short i,hits,maxi;
  float tmp[3],L[3],a,b,c,mu,oneovermu,theta,circle_radius,circle_distance,maxjitter;
  float deltax,deltay;
  struct Ray sray;

  hits = 0;

  sray.o[0] = ray->o[0];
  sray.o[1] = ray->o[1];
  sray.o[2] = ray->o[2];

  L[0] = ray->d[0];
  L[1] = ray->d[1];
  L[2] = ray->d[2];

  theta = (float) asin((double)(radius/ray->maxt));
  circle_radius = radius * cos( (double)theta );
  circle_distance = ray->maxt - radius * sin( (double) theta);

  if(seriesnum == 0) maxi = 8;
  else maxi = 12;

  maxjitter = radius * 0.005;

  for(i = 0; i < maxi; i++) {

    
    if(seriesnum == 0) {

      tmp[0] = circle_radius * circledata_x[i];
      tmp[1] = circle_radius * circledata_y[i];

    }
    else {

      deltax = maxjitter * ((2.0 * MGETRND) - 1.0);
      deltay = maxjitter * ((2.0 * MGETRND) - 1.0);

      tmp[0] = circle_radius * (circledata_x2[i] + deltax);
      tmp[1] = circle_radius * (circledata_y2[i] + deltay);

    }

    tmp[2] = circle_distance;
  
    a = L[0];
    b = L[1];
    c = L[2];

    mu = sqrt((double)(b * b + c * c));
    oneovermu = 1.0 / mu;

    sray.d[0] = mu * tmp[0] + a * tmp[2];
    sray.d[1] = -a * b * tmp[0] * oneovermu + c * tmp[1] * oneovermu + b * tmp[2];
    sray.d[2] = -a * c * tmp[0] * oneovermu - b * tmp[1] * oneovermu + c * tmp[2];

    normalize(sray.d);

    pt_InitRay(&sray,1);

    sray.maxt = ray->maxt;
    sray.shootingvxo = ray->shootingvxo;
    sray.tdepth = ray->tdepth;

    if(!pt_CheckShadowCache(&sray,ls)) {

      if(vx->PrimCount != 0) pt_GetShadowIntersection(vx,&sray);

      if(!(gl_env->flags & ENVFLAG_ATLEMPTY)) {

        pt_ShadowCheckAlwaysTestList(ray);

      }

    }

    if(sray.vxo) hits++;

  }

  return(hits);

}
