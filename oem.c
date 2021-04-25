/*
** Oem.c
**
** Contains OEM specific functions.
**
** Amiga Version v1.0
**
** 17 Aug 1992 - Created          - J. Terrell
**
*/
#include <Clib/all_protos.h>

#undef IsListEmpty

#include "headers/tracer.h"

unsigned char *fbr,*fbg,*fbb;

void *IntuitionBase;

unsigned long secs,mics;

short pt_init_oem(void)
{

  short retval;
  short total_x,total_y,start_x,start_y;
  float pa;

  retval = ERROR_OK;

  IntuitionBase = OpenLibrary("intuition.library",0L);

  gl_oem = oem_AllocMem(sizeof(struct OemInfo));
  if(!gl_oem) {

    retval = ERROR_NOMEM;
    goto init_done;

  }

  gl_oem->oem_HorzRes = 320;
  gl_oem->oem_VertRes = 200;

  gl_oem->oem_DisplayAspectRatio = 4.0 / 3.0;
  gl_oem->oem_ImageAspectRatio = 4.0 / 3.0;

  if(pt_loadprefs()) {

    printf("** Error loading preferences file. **\n");

  }

  pa = gl_oem->oem_ImageAspectRatio / gl_oem->oem_DisplayAspectRatio;

  if(pa >= 1.0) {

    total_x = gl_oem->oem_HorzRes;
    total_y = (1.0 / pa) * gl_oem->oem_VertRes;

    start_x = 0;
    start_y = (gl_oem->oem_VertRes - total_y) >> 1;

  }
  else {

    total_x = pa * gl_oem->oem_HorzRes;
    total_y = gl_oem->oem_VertRes;

    start_x = (gl_oem->oem_HorzRes - total_x) >> 1;
    start_y = 0;

  }

  gl_oem->oem_CropWindow[0] = 0.0;
  gl_oem->oem_CropWindow[1] = 1.0;
  gl_oem->oem_CropWindow[2] = 0.0;
  gl_oem->oem_CropWindow[3] = 1.0;

  /*
  ** Frame Buffer Information.
  */
  gl_oem->oem_StartPx = start_x;
  gl_oem->oem_StartPy = start_y;

  gl_oem->oem_EndPx = start_x;
  gl_oem->oem_EndPy = start_y;

  gl_oem->oem_StartPx += gl_oem->oem_CropWindow[0] * (float)total_x;
  gl_oem->oem_StartPy += gl_oem->oem_CropWindow[2] * (float)total_y;

  gl_oem->oem_EndPx += (gl_oem->oem_CropWindow[1] * (float)(total_x - 1));
  gl_oem->oem_EndPy += (gl_oem->oem_CropWindow[3] * (float)(total_y - 1));

  gl_oem->oem_CurrentPx = gl_oem->oem_StartPx;
  gl_oem->oem_CurrentPy = gl_oem->oem_StartPy;

  /*
  ** Image Plane Information.
  */
  gl_oem->oem_StartH = gl_oem->oem_CropWindow[0];
  gl_oem->oem_EndH   = gl_oem->oem_CropWindow[1];

  gl_oem->oem_StartV = gl_oem->oem_CropWindow[2];
  gl_oem->oem_EndV   = gl_oem->oem_CropWindow[3];

  gl_oem->oem_StepH = (gl_oem->oem_EndH - gl_oem->oem_StartH) /
                      (gl_oem->oem_EndPx - gl_oem->oem_StartPx);

  gl_oem->oem_StepV = (gl_oem->oem_EndV - gl_oem->oem_StartV) /
                      (gl_oem->oem_EndPy - gl_oem->oem_StartPy);

  gl_oem->oem_CurrentH = gl_oem->oem_StartH;
  gl_oem->oem_CurrentV = gl_oem->oem_StartV;

  /*
  ** Init oem frame buffer here.
  */
  oem_initframebuffer();

init_done:

  return(retval);

}

void pt_cleanup_oem(void)
{

  oem_FreeMem(gl_oem,sizeof(struct OemInfo));

  if(IntuitionBase) CloseLibrary(IntuitionBase);

}

void oem_raystart(struct Ray *ray,float *position,struct AAInfo *aai)
{

  float pl[3];

  ray->o[0] = gl_env->Camera->cam_Origin[0];
  ray->o[1] = gl_env->Camera->cam_Origin[1];
  ray->o[2] = gl_env->Camera->cam_Origin[2];

  MRAYPOS(gl_env->Camera->cam_ViewPlaneUpperLeft,gl_env->Camera->cam_ViewRight,
    gl_oem->oem_CurrentH,pl);

  pl[0] += (-gl_env->Camera->cam_ViewUp[0] * gl_oem->oem_CurrentV);
  pl[1] += (-gl_env->Camera->cam_ViewUp[1] * gl_oem->oem_CurrentV);
  pl[2] += (-gl_env->Camera->cam_ViewUp[2] * gl_oem->oem_CurrentV);

  ray->d[0] = pl[0] - ray->o[0];
  ray->d[1] = pl[1] - ray->o[1];
  ray->d[2] = pl[2] - ray->o[2];

  normalize(ray->d);

  pt_InitRay(ray,1);

  position[0] = gl_oem->oem_CurrentPx;
  position[1] = gl_oem->oem_CurrentPy;

  if(position[0] == gl_oem->oem_StartPx) {

    aai->aai_StartSection = 0;
    aai->aai_StartIntensity[0] = 0.0;
    aai->aai_StartIntensity[1] = 0.0;
    aai->aai_StartIntensity[2] = 0.0;

  }

}

BOOL oem_rayend(void)
{

  BOOL tracedone;

  gl_oem->oem_CurrentPx ++;
  gl_oem->oem_CurrentH += gl_oem->oem_StepH;

  tracedone = FALSE;

  if(gl_oem->oem_CurrentPx > gl_oem->oem_EndPx) {

    gl_oem->oem_CurrentPy ++;

    if(gl_oem->oem_CurrentPy <= gl_oem->oem_EndPy) {

      gl_oem->oem_CurrentPx = gl_oem->oem_StartPx;
      gl_oem->oem_CurrentH = gl_oem->oem_StartH;
      gl_oem->oem_CurrentV += gl_oem->oem_StepV;

      printf("Rendering line: %ld\t(%ld%%) \r",(unsigned long)gl_oem->oem_CurrentPy,
        (unsigned long)((gl_oem->oem_CurrentV/gl_oem->oem_EndV) * 100.0));

    }
    else tracedone = TRUE;

  }

  return(tracedone);

}

void oem_initframebuffer(void)
{

  unsigned long size;

  size = (unsigned long)gl_oem->oem_HorzRes * (unsigned long)gl_oem->oem_VertRes;

  fbr = oem_AllocMem(size);
  fbg = oem_AllocMem(size);
  fbb = oem_AllocMem(size);

  if(!fbr || !fbg || !fbb) {

    printf("NO Memory for frame buffer.\n");
    if(fbr) oem_FreeMem(fbr,size);
    if(fbg) oem_FreeMem(fbg,size);
    if(fbb) oem_FreeMem(fbb,size);

    fbr = NULL;
    fbg = NULL;
    fbb = NULL;

  }

  oem_clearmemory(fbr,size);
  oem_clearmemory(fbg,size);
  oem_clearmemory(fbb,size);

}

void oem_clearmemory(unsigned char *buffer,unsigned long size)
{

  while(size--) {

    *buffer = 0x0;
    buffer++;

  }

}

void *oem_AllocMem(unsigned long size)
{

  return(malloc(size));

}

void oem_FreeMem(void *ptr,unsigned long size)
{

  free(ptr);

}

void oem_setpixel(short x,short y,unsigned long val)
{

  unsigned long r,g,b;
  unsigned long offset;

  offset = ((unsigned long)y * (unsigned long)gl_oem->oem_HorzRes)
    + (unsigned long)x;

  r = ((val >> 16) & 0xFF);
  g = ((val >> 8) & 0xFF);
  b = (val & 0xFF);

  fbr[offset] = (unsigned char)r;
  fbg[offset] = (unsigned char)g;
  fbb[offset] = (unsigned char)b;

}

void oem_savebuffer(void)
{

  FILE *file;
  unsigned long size;

  size = (unsigned long)gl_oem->oem_HorzRes * (unsigned long)gl_oem->oem_VertRes;

  file = fopen("T:trace.red","wb");
  if(file) {

    fwrite(fbr,size,1,file);
    fclose(file);

  }

  file = fopen("T:trace.grn","wb");
  if(file) {

    fwrite(fbg,size,1,file);
    fclose(file);

  }

  file = fopen("T:trace.blu","wb");
  if(file) {

    fwrite(fbb,size,1,file);
    fclose(file);

  }

}

void oem_StartTimer(void)
{

  if(IntuitionBase) CurrentTime(&secs,&mics);

}

void oem_StopTimer(unsigned long *secs2)
{

  unsigned long s,m;

  if(!IntuitionBase) {

    printf("Timer not implemented.\n");
    *secs2 = 0;

  }
  else {

    CurrentTime(&s,&m);

    *secs2 = s - secs;

  }

}