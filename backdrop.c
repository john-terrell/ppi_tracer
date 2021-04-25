/*
** BackDrop.c
**
** 19 Nov 1992 - Created    - J. Terrell
**
*/
#include "Headers/Tracer.h"

void bd_BackdropImage(struct Ray *ray)
{

  float uv[2];

  if(!gl_env->BackDropImage) return;

  uv[0] = gl_oem->oem_CurrentPx/gl_oem->oem_EndPx;
  uv[1] = gl_oem->oem_CurrentPy/gl_oem->oem_EndPy;

  getmapcolor(gl_env->BackDropImage,uv,ray->ci);

}
