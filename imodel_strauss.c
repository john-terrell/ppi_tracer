/*
** IModel_Strauss.c
**
** The Strauss Illumination Model.
**
** 24 Sep 1992 - Coded                                - J. Terrell
** 02 Dec 1992 - Removed specular curve calculations  - J. Terrell
**
*/
#include "Headers/Tracer.h"

void im_Strauss(struct TextureArg *ta,struct LightSource *ls,struct Ray *sray)
{

  float Qd[3];          /* Diffuse contributions */
  float s;              /* Smoothness value */
  float rd;             /* Diffuse reflectivity */
  float temp;           /* Scratch */

  s = ta->ta_Smoothness;
  rd = (1.0 - (s * s * s)) * (1.0 - ta->ta_Opacity);

  if((ls->ls_flags & 0xFF) == LSTYPE_AMBIENT) {

    rd *= ls->ls_intensity;

    ta->ta_OutputColor[0] += (rd * ls->ls_color[0] * ta->ta_Color[0]);
    ta->ta_OutputColor[1] += (rd * ls->ls_color[1] * ta->ta_Color[1]);
    ta->ta_OutputColor[2] += (rd * ls->ls_color[2] * ta->ta_Color[2]);

  }
  else {

    /*
    ** Get diffuse contribution
    */
    temp = sray->d_dot_norm * (1.0 - (ta->ta_Metalness * s)) * rd;

    Qd[0] = ta->ta_Color[0] * temp;
    Qd[1] = ta->ta_Color[1] * temp;
    Qd[2] = ta->ta_Color[2] * temp;

    /*
    ** For each type of lightsource, determine intensity.
    ** (Distance to each lightsource is stored int sray->maxt)
    */
    switch(ls->ls_flags & 0xFF) {

      case LSTYPE_SPOT:

        temp = (ls->ls_intensity * pow(sray->t,ls->ls_dropoff)) *
          (1.0 / pow(sray->maxt,ls->ls_decay));

        break;

      case LSTYPE_POINT:
      case LSTYPE_SPHERICAL:

        temp = ls->ls_intensity * (1.0 / pow(sray->maxt,ls->ls_decay));

        break;

      case LSTYPE_DISTANT:

        temp = ls->ls_intensity;

        break;

    }

    ta->ta_OutputColor[0] += (sray->ci[0] * temp * Qd[0]);
    ta->ta_OutputColor[1] += (sray->ci[1] * temp * Qd[1]);
    ta->ta_OutputColor[2] += (sray->ci[2] * temp * Qd[2]);

  }

}

void im_GetSurfaceZR(struct TextureArg *ta,struct Ray *ray,struct Ray *reflect)
{

  float s,temp,rd,rs,rn;

  s = ta->ta_Smoothness;

  temp = (1.0 - ta->ta_Opacity);
  rd = (1.0 - (s * s * s)) * temp;
  rn = MCLAMP((temp - rd),0.0,1.0);

  /*
  ** Make Reflection Vector
  */
  reflect->d[0] = ((2.0 * ray->norm[0]) * ray->d_dot_norm) + ray->d[0];
  reflect->d[1] = ((2.0 * ray->norm[1]) * ray->d_dot_norm) + ray->d[1];
  reflect->d[2] = ((2.0 * ray->norm[2]) * ray->d_dot_norm) + ray->d[2];

  temp = 1.0 - fabs(MDOT(ray->d,reflect->d));

  rs = temp * rn;

  /*
  ** transmittance is whatever is not reflected diffusely and specularly.
  */
  ray->rz = ((1.0 - rs) / (1.0 - rn)) * ta->ta_Opacity;
  ray->rs = 1.0 - (ray->rz + rd);

}
