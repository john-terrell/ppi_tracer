/*
** Fuzzy.c
**
** 18 Nov 1992 - Created    - J. Terrell
**
*/
#include "Headers/Tracer.h"

void fz_trace_ray(struct Ray *ray,struct Voxel *vox,float smoothness,long maxsamples)
{

  struct Ray sample;
  float col[3],di;
  long i;

  col[0] = 0.0;
  col[1] = 0.0;
  col[2] = 0.0;

  sample.o[0] = ray->o[0];
  sample.o[1] = ray->o[1];
  sample.o[2] = ray->o[2];

  for(i = 0; i < maxsamples; i++) {

    sample.d[0] = ray->d[0];
    sample.d[1] = ray->d[1];
    sample.d[2] = ray->d[2];

    perturbnormal(sample.d,(float)(0.25 * (1.0 - smoothness)));

    pt_InitRay(&sample,ray->depth);

    sample.tdepth = ray->tdepth;
    sample.index  = ray->index;
    sample.shootingvxo = ray->shootingvxo;

    pt_trace_ray(&sample,vox);

    col[0] += sample.ci[0];
    col[1] += sample.ci[1];
    col[2] += sample.ci[2];

  }

  di = 1.0 / (float) maxsamples;

  ray->ci[0] = col[0] * di;
  ray->ci[1] = col[1] * di;
  ray->ci[2] = col[2] * di;

}
