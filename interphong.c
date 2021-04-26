/*
** InterPhong.c
**
** 07 Dec 1992 - Created      - J. Terrell
**
*/
#include "Headers/Tracer.h"

#define SQRT3_2 3.464101615

void InterPhong(struct Object *obj,float *nestime,float *noriginal)
{

  float bias,tension,fact,len;
  float vtemp[3];

  bias = 0.0;
  tension = obj->obj_SmoothingTension;

  vtemp[0] = noriginal[0] - nestime[0];
  vtemp[1] = noriginal[1] - nestime[1];
  vtemp[2] = noriginal[2] - nestime[2];

  fact = fabs(vtemp[0]) + fabs(vtemp[1]) + fabs(vtemp[2]);

  fact = (fact + bias * (SQRT3_2 - fact)) * tension;

  len = fact * MLENGTH(vtemp);

  vtemp[0] *= len;
  vtemp[1] *= len;
  vtemp[2] *= len;

  nestime[0] += vtemp[0];
  nestime[1] += vtemp[1];
  nestime[2] += vtemp[2];

}
