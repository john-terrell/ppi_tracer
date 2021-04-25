/*
** FrameStore.c
**
** Functions that allow access to the system frame store.
**
** 17 Aug 1992 - Created          - J. Terrell
*/
#include "Headers/Tracer.h"

short pt_init_framestore(void)
{

  return(0);

}

void pt_cleanup_framestore(void)
{

  oem_savebuffer();

}

void pt_savepixel(float *pos,float *value)
{

  unsigned long val,r,g,b;
  short x,y;

  x = (short)pos[0];
  y = (short)pos[1];

  r = (unsigned long)(value[0] * 255.0);
  g = (unsigned long)(value[1] * 255.0);
  b = (unsigned long)(value[2] * 255.0);

  val = r << 16;
  val |= g << 8;
  val |= b;

  oem_setpixel(x,y,val);

}
