/*
** Mapping.c
**
** 26 Oct 1992 - Created    - J. Terrell
**
*/
#include <Exec/Types.h>
#include <Dos/Dos.h>
#include <Clib/All_protos.h>

#undef IsListEmpty

#include "Headers/Tracer.h"

struct ImageBuffer *im_FindImage(char *name)
{

  struct ImageBuffer *ib;

  ib = gl_env->ImageList.lh_Head;
  while(ib->im_Node.ln_Succ) {

    if(strcmp(name,ib->im_Name) == 0) return(ib);

    ib = ib->im_Node.ln_Succ;

  }

  return(0);

}

short im_LoadRAWColorMap(char *filename,struct ImageBuffer *im,short xsize,short ysize)
{

  long fbsize;
  short retval;
  char name[96];
  BPTR file_r,file_g,file_b;

  fbsize = ((long)xsize * (long)ysize);
  retval = 0;

  im->im_xsize = (long)xsize;
  im->im_ysize = (long)ysize;

  im->im_RedBuffer = oem_AllocMem(fbsize);
  im->im_GrnBuffer = oem_AllocMem(fbsize);
  im->im_BluBuffer = oem_AllocMem(fbsize);

  if(!im->im_RedBuffer || !im->im_GrnBuffer || !im->im_BluBuffer) {

    printf("Unable to allocate framebuffer. %ld bytes.\n",fbsize);

    if(im->im_RedBuffer) oem_FreeMem(im->im_RedBuffer,fbsize);
    if(im->im_GrnBuffer) oem_FreeMem(im->im_GrnBuffer,fbsize);
    if(im->im_BluBuffer) oem_FreeMem(im->im_BluBuffer,fbsize);

    return(ERROR_NOMEM);

  }

  file_g = file_b = 0;

  strcpy(name,filename);
  strcat(name,".red");
  file_r = Open((unsigned char *)name,MODE_OLDFILE);
  if(!file_r) {

    printf("Unable to open red file. '%s'\n",name);
    retval = ERROR_FILE_NOFILE;
    goto done;

  }

  strcpy(name,filename);
  strcat(name,".grn");
  file_g = Open((unsigned char *)name,MODE_OLDFILE);
  if(!file_g) {

    printf("Unable to open grn file. '%s'\n",name);
    retval = ERROR_FILE_NOFILE;
    goto done;

  }

  strcpy(name,filename);
  strcat(name,".blu");
  file_b = Open((unsigned char *)name,MODE_OLDFILE);
  if(!file_b) {

    printf("Unable to open blu file. '%s'\n",name);
    retval = ERROR_FILE_NOFILE;
    goto done;

  }

  Read(file_r,im->im_RedBuffer,fbsize);
  Read(file_g,im->im_GrnBuffer,fbsize);
  Read(file_b,im->im_BluBuffer,fbsize);

done:

  if(file_r) Close(file_r);
  if(file_g) Close(file_g);
  if(file_b) Close(file_b);

  return(retval);

}

void im_ProcessFlatMap(struct Texture *txt,struct TextureArg *ta)
{

  struct Ray pray;
  struct FlatMap *fm;
  float p[3];
  float wdv;
  float t;

  float Pa[3],Pb[3],Pc[3],Pd[3];
  float Na[3],Nb[3],Nc[3];
  float Du0,Du1,Du2;
  float Dv0,Dv1,Dv2;
  float Qux[3],Quy[3];
  float Qvx[3],Qvy[3];
  float Dux,Duy,Ka,Kb;
  float Dvx,Dvy;
  float uv[2];

  float twoxdvu;
  float oneoverdu2;
  float twoxdvv;
  float oneoverdv2;
  float sqrtval;

  fm = txt->tex_TextureData;

  /* Determine where a ray fired back toward the image plane intersects.
  ** The ray is the reversed image plane normal.
  */
  pray.o[0] = ta->ta_Position[0];
  pray.o[1] = ta->ta_Position[1];
  pray.o[2] = ta->ta_Position[2];

  pray.d[0] = -fm->fm_normal[0];
  pray.d[1] = -fm->fm_normal[1];
  pray.d[2] = -fm->fm_normal[2];

  wdv = MDOT(pray.d,fm->fm_normal);

  p[0] = fm->fm_origin[0] - pray.o[0];
  p[1] = fm->fm_origin[1] - pray.o[1];
  p[2] = fm->fm_origin[2] - pray.o[2];

  t = MDOT(p,fm->fm_normal) / wdv;

  MRAYPOS(pray.o,pray.d,t,p);

  Pa[0] = (fm->fm_c1[0] - fm->fm_c4[0]) + (fm->fm_c3[0] - fm->fm_c2[0]);
  Pa[1] = (fm->fm_c1[1] - fm->fm_c4[1]) + (fm->fm_c3[1] - fm->fm_c2[1]);
  Pa[2] = (fm->fm_c1[2] - fm->fm_c4[2]) + (fm->fm_c3[2] - fm->fm_c2[2]);

  Pb[0] = fm->fm_c4[0] - fm->fm_c1[0];
  Pb[1] = fm->fm_c4[1] - fm->fm_c1[1];
  Pb[2] = fm->fm_c4[2] - fm->fm_c1[2];

  Pc[0] = fm->fm_c2[0] - fm->fm_c1[0];
  Pc[1] = fm->fm_c2[1] - fm->fm_c1[1];
  Pc[2] = fm->fm_c2[2] - fm->fm_c1[2];

  Pd[0] = fm->fm_c1[0];
  Pd[1] = fm->fm_c1[1];
  Pd[2] = fm->fm_c1[2];

  MCROSS(Pa,fm->fm_normal,Na);
  MCROSS(Pc,fm->fm_normal,Nc);

  Du0 = MDOT(Nc,Pd);
  Du1 = MDOT(Na,Pd) + MDOT(Nc,Pb);
  Du2 = MDOT(Na,Pb);

  if(Du2 == 0.0) { /* u axis are parallel */

    uv[1] = (MDOT(p,Nc) - Du0) / (Du1 - MDOT(p,Na));

  }
  else {

    twoxdvu = 1.0 / (2.0 * Du2);
    oneoverdu2 = 1.0 / Du2;

    Dux = -Du1 * twoxdvu;
    Duy = Du0 * oneoverdu2;
    Qux[0] = Na[0] * twoxdvu;
    Qux[1] = Na[1] * twoxdvu;
    Qux[2] = Na[2] * twoxdvu;

    Quy[0] = -Nc[0] * oneoverdu2;
    Quy[1] = -Nc[1] * oneoverdu2;
    Quy[2] = -Nc[2] * oneoverdu2;

    Ka = Dux + MDOT(Qux,p);
    Kb = Duy + MDOT(Quy,p);

    sqrtval = sqrt((Ka * Ka) - Kb);

    uv[1] = Ka - sqrtval;
    if(uv[1] > 1.0 || uv[1] < 0.0) {

      uv[1] = Ka + sqrtval;

    }

  }

  /*
  ** See if the u value is between 0 and 1, if not, the point is outside the
  ** quad.
  */
  if(fm->header.map_flags & MAPFLAG_REPEAT) {

    uv[1] -= floor(uv[1]);

  }
  else {

    if(uv[1] < 0.0) return;
    if(uv[1] > 1.0) return;

  }

  MCROSS(Pb,fm->fm_normal,Nb);

  Dv0 = MDOT(Nb,Pd);
  Dv1 = MDOT(Na,Pd) + MDOT(Nb,Pc);
  Dv2 = MDOT(Na,Pc);

  if(Dv2 == 0.0) {  /* V axis are parallel. */

    uv[0] = (MDOT(p,Nb) - Dv0) / (Dv1 - MDOT(p,Na));

  }
  else {

    twoxdvv = 1.0 / (2.0 * Dv2);
    oneoverdv2 = 1.0 / Dv2;


    Dvx = -Dv1 * twoxdvv;
    Dvy = Dv0 * oneoverdv2;
    Qvx[0] = Na[0] * twoxdvv;
    Qvx[1] = Na[1] * twoxdvv;
    Qvx[2] = Na[2] * twoxdvv;

    Qvy[0] = -Nb[0] * oneoverdv2;
    Qvy[1] = -Nb[1] * oneoverdv2;
    Qvy[2] = -Nb[2] * oneoverdv2;

    Ka = Dvx + MDOT(Qvx,p);
    Kb = Dvy + MDOT(Qvy,p);

    sqrtval = sqrt((Ka * Ka) - Kb);

    uv[0] = Ka - sqrtval;
    if(uv[0] > 1.0 || uv[0] < 0.0) {

      uv[0] = Ka + sqrtval;

    }


  }

  /*
  ** See if the v valve is between 0 and 1, if not, the point is outside the
  ** quad.
  */
  if(fm->header.map_flags & MAPFLAG_REPEAT) {

    uv[0] -= floor(uv[0]);

  }
  else {

    if(uv[0] < 0.0) return;
    if(uv[0] > 1.0) return;

  }

  if(txt->tex_Type == TXTYPE_IMAP) {  /* Image Color Map */

    getmapcolor(fm->header.map_image,uv,ta->ta_Color);

  }
  else
  if(txt->tex_Type == TXTYPE_BMAP) {  /* Bump Map */

    getmapnormal(fm->header.map_image,uv,ta->ta_Normal);

  }
  else
  if(txt->tex_Type == TXTYPE_CMAP) {  /* Clip Map */

    if(getmapclip(fm->header.map_image,uv)) {

      ta->ta_Flags |= TAFLAG_CLIP;

    }

  }
  else
  if(txt->tex_Type == TXTYPE_TMAP) {  /* Transparency Map */

    ta->ta_Opacity = getmaptransparency(fm->header.map_image,uv);

  }

}

void im_ProcessSphericalMap(struct Texture *txt,struct TextureArg *ta)
{

  struct SphericalMap *sm;
  float uv[2];
  float tmp[3],norm[3];
  float phi,theta;

  sm = txt->tex_TextureData;

  norm[0] = ta->ta_Position[0] - sm->sm_Center[0];
  norm[1] = ta->ta_Position[1] - sm->sm_Center[1];
  norm[2] = ta->ta_Position[2] - sm->sm_Center[2];

  normalize(norm);

  phi = MNEGDOT(norm,sm->sm_Sp);
  phi = acos(phi);

  uv[1] = phi / PI;

  if(uv[1] == 0.0 || uv[1] == 1.0) {

    uv[0] = 0.0;

  }
  else {

    theta = acos( MDOT(sm->sm_Se,norm) / sin(phi)) / (2.0 * PI);

    MCROSS(sm->sm_Sp,sm->sm_Se,tmp);
    if(MDOT(tmp,norm) > 0.0) uv[0] = theta;
    else uv[0] = 1.0 - theta;

  }

  if(txt->tex_Type == TXTYPE_IMAP) {  /* Image Color Map */

    getmapcolor(sm->header.map_image,uv,ta->ta_Color);

  }
  else
  if(txt->tex_Type == TXTYPE_BMAP) {  /* Bump Map */

    getmapnormal(sm->header.map_image,uv,ta->ta_Normal);

  }
  else
  if(txt->tex_Type == TXTYPE_CMAP) {  /* Clip Map */

    if(getmapclip(sm->header.map_image,uv)) {

      ta->ta_Flags |= TAFLAG_CLIP;

    }

  }
  else
  if(txt->tex_Type == TXTYPE_TMAP) {  /* Transparency Map */

    ta->ta_Opacity = getmaptransparency(sm->header.map_image,uv);

  }

}

void im_ProcessEnvironmentMap(struct SphericalMap *sm,struct Ray *ray)
{

  float uv[2];
  float tmp[3],norm[3];
  float phi,theta;

  norm[0] = ray->ip[0] - sm->sm_Center[0];
  norm[1] = ray->ip[1] - sm->sm_Center[1];
  norm[2] = ray->ip[2] - sm->sm_Center[2];

  normalize(norm);

  phi = MNEGDOT(norm,sm->sm_Sp);
  phi = acos(phi);

  uv[1] = phi / PI;

  if(uv[1] == 0.0 || uv[1] == 1.0) {

    uv[0] = 0.0;

  }
  else {

    theta = acos( MDOT(sm->sm_Se,norm) / sin(phi)) / (2.0 * PI);

    MCROSS(sm->sm_Sp,sm->sm_Se,tmp);
    if(MDOT(tmp,norm) > 0.0) uv[0] = theta;
    else uv[0] = 1.0 - theta;

  }

  getmapcolor(sm->header.map_image,uv,ray->ci);

}

void getmapcolor(struct ImageBuffer *imap,float *uv,float *color)
{

  float iu, iv;

  iu = imap->im_xsize * uv[0];
  iv = imap->im_ysize * uv[1];

  getmapRGB(imap,(long)iu,(long)iv, color);

}

void getmapnormal(struct ImageBuffer *imap,float *uvval,float *norm)
{

  float u,v;
  float eu,ev,fu,fv;
  float uh,ul,vh,vl;
  float uv[3],vv[3];

  u = uvval[0];
  v = uvval[1];

  eu = 1.0 / (imap->im_xsize - 1);
  ev = 1.0 / (imap->im_ysize - 1);

  uh = getmapFVAL(imap, (float)(u + eu), v);
  ul = getmapFVAL(imap, (float)(u - eu), v);

  fu = uh - ul;

  vh = getmapFVAL(imap, u, (float)(v + ev));
  vl = getmapFVAL(imap, u, (float)(v - ev));

  fv = vh - vl;

  uv[0] = 0.0;
  uv[1] = 1.0;
  uv[2] = 0.0;

  if(norm[0] != 1.0) {

    MCROSS(norm,uv,vv);
    MCROSS(norm,vv,uv);

  }
  else {

    uv[0] = 0.0;
    uv[1] = 0.0;
    uv[2] = 1.0;

    MCROSS(norm,uv,vv);
    MCROSS(norm,vv,uv);

  }

  norm[0] += ((uv[0] * fu) + (vv[0] * fv));
  norm[1] += ((uv[1] * fu) + (vv[1] * fv));
  norm[2] += ((uv[2] * fu) + (vv[2] * fv));

}

BOOL getmapclip(struct ImageBuffer *imap,float *uvval)
{

  float u,v;

  u = uvval[0];
  v = uvval[1];

  if(getmapFVAL(imap, u, v) == 0.0) {

    return(TRUE);

  }

  return(FALSE);

}

float getmaptransparency(struct ImageBuffer *imap,float *uvval)
{

  float u,v,val;

  u = uvval[0];
  v = uvval[1];

  val = 1.0 - getmapFVAL(imap, u, v);

  return(val);

}

float getmapFVAL(struct ImageBuffer *imap,float u,float v)
{

  float color[3];
  short iu, iv;

  iu = (imap->im_xsize - 1) * u;
  iv = (imap->im_ysize - 1) * v;

  getmapRGB(imap,(long) iu, (long) iv, color);

  return( (float) ((color[0] + color[1] + color[2]) * 0.33333333334) );

}

void getmapRGB(struct ImageBuffer *imap,long x,long y,float *co)
{

  long offset;

  offset = (y * imap->im_xsize) + x;

  if(imap->im_RedBuffer)
    co[0] = (imap->im_RedBuffer[offset]) * 0.0039215686;  // = (1 / 255.0);

  if(imap->im_GrnBuffer)
    co[1] = (imap->im_GrnBuffer[offset]) * 0.0039215686;  // = (1 / 255.0);

  if(imap->im_BluBuffer)
    co[2] = (imap->im_BluBuffer[offset]) * 0.0039215686;  // = (1 / 255.0);

}
