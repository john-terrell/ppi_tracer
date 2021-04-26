/*
** Texture Tools
**
** 07 Oct 1992 - Created    - J. Terrell
**
**
*/
#include "Headers/Tracer.h"

float trianglewave(float val)
{

  float off, rtn;

  if (val >= 0.0) {

    off = val - floor(val);

  }
  else {

    off = val - (-1.0 - floor(fabs(val)));

  }

  if (off >= 0.5) {

    rtn = 2.0 * (1.0 - off);

  }
  else {

    rtn = 2.0 * off;

  }

  return(rtn);

}

/*
** A Recursive implementation of the Perlin noise function.
** Graphic Gems II
*/
#define hermite(p0,p1,r0,r1,t) (  p0 * ((2.0 * t - 3.0) * t * t + 1.0) + \
                                  p1 * (-2.0 * t + 3.0) * t * t +        \
                                  r0 * ((t - 2.0) * t + 1.0) * t +       \
                                  r1 * (t - 1.0) * t * t  )

#define rand3a(x,y,z) frand(67 * (x) + 59 * (y) + 71 * (z))
#define rand3b(x,y,z) frand(73 * (x) + 79 * (y) + 83 * (z))
#define rand3c(x,y,z) frand(89 * (x) + 97 * (y) + 101 * (z))
#define rand3d(x,y,z) frand(103 * (x) + 107 * (y) + 109 * (z))


long xlim[3][2];
float xarg[3];

float frand(long s)
{

  s = s << 13 ^ s;

  return((float)(1.0 - ((s * (s * s * 15731 + 789221) + 1376312589) & 0x7fffffff) /
    1073741824.0));

}

void interpolate(float *f,long i,long n)
{

  float f0[4],f1[4];

  if(n == 0) {

    f[0] = rand3a(xlim[0][i & 1],xlim[1][i >> 1 & 1],xlim[2][i >> 2]);
    f[1] = rand3b(xlim[0][i & 1],xlim[1][i >> 1 & 1],xlim[2][i >> 2]);
    f[2] = rand3c(xlim[0][i & 1],xlim[1][i >> 1 & 1],xlim[2][i >> 2]);
    f[3] = rand3d(xlim[0][i & 1],xlim[1][i >> 1 & 1],xlim[2][i >> 2]);
    return;

  }

  n--;

  interpolate(f0,i,n);
  interpolate(f1,i | 1 << n,n);

  f[0] = (1.0 - xarg[n]) * f0[0] + xarg[n] * f1[0];
  f[1] = (1.0 - xarg[n]) * f0[1] + xarg[n] * f1[1];
  f[2] = (1.0 - xarg[n]) * f0[2] + xarg[n] * f1[2];

  f[3] = hermite(f0[3],f1[3],f0[n],f1[n],xarg[n]);

}

float noise3(float *x)
{

  static float f[4];

  xlim[0][0] = floor(x[0]); xlim[0][1] = xlim[0][0] + 1;
  xlim[1][0] = floor(x[1]); xlim[1][1] = xlim[1][0] + 1;
  xlim[2][0] = floor(x[2]); xlim[2][1] = xlim[2][0] + 1;

  xarg[0] = x[0] - xlim[0][0];
  xarg[1] = x[1] - xlim[1][0];
  xarg[2] = x[2] - xlim[2][0];

  interpolate(f,0,3);

  return(f[3]);

}

void dnoise(float *x,float *v)
{

  static float f[4];

  xlim[0][0] = floor(x[0]); xlim[0][1] = xlim[0][0] + 1;
  xlim[1][0] = floor(x[1]); xlim[1][1] = xlim[1][0] + 1;
  xlim[2][0] = floor(x[2]); xlim[2][1] = xlim[2][0] + 1;

  xarg[0] = x[0] - xlim[0][0];
  xarg[1] = x[1] - xlim[1][0];
  xarg[2] = x[2] - xlim[2][0];

  interpolate(f,0,3);

  v[0] = f[0];
  v[1] = f[1];
  v[2] = f[2];

}

void dturbulence(float *d,float *pnt)
{

   float v[3];
   float t[3];

   t[0] = pnt[0];
   t[1] = pnt[1];
   t[2] = pnt[2];

   dnoise(t,v);

   d[0] = v[0];
   d[1] = v[1];
   d[2] = v[2];

   t[0] = pnt[0] * 2.0;
   t[1] = pnt[1] * 2.0;
   t[2] = pnt[2] * 2.0;

   dnoise(t,v);

   d[0] += v[0] * 0.5;
   d[1] += v[1] * 0.5;
   d[2] += v[2] * 0.5;

   t[0] = pnt[0] * 4.0;
   t[1] = pnt[1] * 4.0;
   t[2] = pnt[2] * 4.0;

   dnoise(t,v);

   d[0] += v[0] * 0.25;
   d[1] += v[1] * 0.25;
   d[2] += v[2] * 0.25;

   t[0] = pnt[0] * 8.0;
   t[1] = pnt[1] * 8.0;
   t[2] = pnt[2] * 8.0;

   dnoise(t,v);

   d[0] += v[0] * 0.125;
   d[1] += v[1] * 0.125;
   d[2] += v[2] * 0.125;

}

float turbulence(float *pnt)
{

  float p2[3],t;

  t = fabs(noise3(pnt));

  p2[0] = pnt[0] * 2.0;
  p2[1] = pnt[1] * 2.0;
  p2[2] = pnt[2] * 2.0;

  t += fabs(noise3(p2)) * 0.5;

  p2[0] = pnt[0] * 4.0;
  p2[1] = pnt[1] * 4.0;
  p2[2] = pnt[2] * 4.0;

  t += fabs(noise3(p2)) * 0.25;

  p2[0] = pnt[0] * 8.0;
  p2[1] = pnt[1] * 8.0;
  p2[2] = pnt[2] * 8.0;

  t += fabs(noise3(p2)) * 0.125;

  return(t);

}

void make_waves(struct Texture *txt,struct TextureArg *ta)
{

  short i;
  float dist,amp;
  float diff[3];
  float ip[3];
  float norm[3];
  struct waveinfo *wi;
  struct wave *waves;

  i = 0;

  ip[0] = ta->ta_Position[0] * 0.14;
  ip[1] = ta->ta_Position[1] * 0.14;
  ip[2] = ta->ta_Position[2] * 0.14;

  norm[0] = ta->ta_Normal[0];
  norm[1] = ta->ta_Normal[1];
  norm[2] = ta->ta_Normal[2];

  wi = txt->tex_TextureData;

  waves = wi->firstwave;

  while(i < wi->wavecount) {

    SUB(ip,waves[i].center,diff);

    dist = normalize(diff);

    dist /= waves[i].wavelength;
    dist += waves[i].phase;

    if(waves[i].damp < 1.0) {

      amp = waves[i].amp * pow(waves[i].damp,dist);

    }
    else {

      amp = waves[i].amp;

    }

    amp *= cos(dist * 6.28);

    norm[0] += amp * diff[0] + norm[0];
    norm[1] += amp * diff[1] + norm[1];
    norm[2] += amp * diff[2] + norm[2];

    i++;

  }

  if(MLENGTH(norm) != 0.0) {

    ta->ta_Normal[0] = norm[0];
    ta->ta_Normal[1] = norm[1];
    ta->ta_Normal[2] = norm[2];

    normalize(ta->ta_Normal);

  }

}

void checkerboard(struct Texture *txt,struct TextureArg *ta)
{

  long ix, iy, iz;
  float val;
  float xs,ys,zs;
  float px,py,pz;
  struct checkerinfo *ci;

  px = ta->ta_Position[0];
  py = ta->ta_Position[1];
  pz = ta->ta_Position[2];

  ci = txt->tex_TextureData;

  xs = ci->xscale;
  ys = ci->yscale;
  zs = ci->zscale;

  val = px * xs;
  if (val < 0.0) {

    val = 1.0 - val;

  }

  ix = (long)val;

  val = py * ys;
  if (val < 0.0) {

    val = 1.0 - val;

  }

  iy = (long)val;

  val = pz * zs;
  if (val < 0.0) {

    val = 1.0 - val;

  }

  iz = (long)val;

  if ((ix + iy + iz) & 1) {

    ta->ta_Color[0] = ci->color0[0];
    ta->ta_Color[1] = ci->color0[1];
    ta->ta_Color[2] = ci->color0[2];

  }
  else {

    ta->ta_Color[0] = ci->color1[0];
    ta->ta_Color[1] = ci->color1[1];
    ta->ta_Color[2] = ci->color1[2];

  }

}

void marble2(struct Texture *txt,struct TextureArg *ta)
{

  struct marble2info *mi;

  short i;
  float t;

  float s[3];

  mi = txt->tex_TextureData;

  s[0] = ta->ta_Position[0] * mi->scale;
  s[1] = ta->ta_Position[1] * mi->scale;
  s[2] = ta->ta_Position[2] * mi->scale;

  t = sin((
    (ta->ta_Position[0] * mi->scale2[0]) +
    (ta->ta_Position[1] * mi->scale2[1]) +
    (ta->ta_Position[2] * mi->scale2[2])
    ) + (turbulence(s) * mi->turbscale));

  for (i = 0; i < mi->order; i++) t = t * t;

  t = MCLAMP(t,0.0,1.0);

  ta->ta_Color[0] = mi->b[0] + (mi->v[0] - mi->b[0]) * t;
  ta->ta_Color[1] = mi->b[1] + (mi->v[1] - mi->b[1]) * t;
  ta->ta_Color[2] = mi->b[2] + (mi->v[2] - mi->b[2]) * t;

}

void marble3(struct Texture *txt,struct TextureArg *ta)
{

  struct marble3info *mi;
  float n, s;
  float p[3];
  float t1, t2, t3, t4;
  float rs, gs, bs, re, ge, be;

  mi = txt->tex_TextureData;

  p[0] = ta->ta_Position[0] * mi->scale;
  p[1] = ta->ta_Position[1] * mi->scale;
  p[2] = ta->ta_Position[2] * mi->scale;

  n = p[0] + turbulence(p) * mi->turbscale;
  n = trianglewave(n);

  t1 = 0.0;
  t2 = mi->t[0];
  t3 = mi->t[1];
  t4 = mi->t[2];

  if (n > t4) {

    ta->ta_Color[0] = mi->c3[0];
    ta->ta_Color[1] = mi->c3[1];
    ta->ta_Color[2] = mi->c3[2];

    return;

  }

  if (n >= t3) {

    s = (n - t3) / (t4 - t3);

    rs = mi->c2[0];
    gs = mi->c2[1];
    bs = mi->c2[2];

    re = mi->c3[0];
    ge = mi->c3[1];
    be = mi->c3[2];

    ta->ta_Color[0] = rs + ((re - rs) * s);
    ta->ta_Color[1] = gs + ((ge - gs) * s);
    ta->ta_Color[2] = bs + ((be - bs) * s);

    return;

  }

  if (n >= t2) {

    s = (n - t2) / (t3 - t2);

    rs = mi->c1[0];
    gs = mi->c1[1];
    bs = mi->c1[2];

    re = mi->c2[0];
    ge = mi->c2[1];
    be = mi->c2[2];

    ta->ta_Color[0] = rs + (re - rs) * s;
    ta->ta_Color[1] = gs + (ge - gs) * s;
    ta->ta_Color[2] = bs + (be - bs) * s;

    return;

  }

  if (n >= t1) {

    s = (n - t1) / (t2 - t1);

    rs = mi->c0[0];
    gs = mi->c0[1];
    bs = mi->c0[2];

    re = mi->c1[0];
    ge = mi->c1[1];
    be = mi->c1[2];

    ta->ta_Color[0] = rs + (re - rs) * s;
    ta->ta_Color[1] = gs + (ge - gs) * s;
    ta->ta_Color[2] = bs + (be - bs) * s;

    return;

  }

  ta->ta_Color[0] = mi->c0[0];
  ta->ta_Color[1] = mi->c0[1];
  ta->ta_Color[2] = mi->c0[2];

}

void brick(struct Texture *txt,struct TextureArg *ta)
{

  struct brickinfo *br;
  float bx, by, bz;
  float pnt[3];

  pnt[0] = ta->ta_Position[0];
  pnt[1] = ta->ta_Position[1];
  pnt[2] = ta->ta_Position[2];

  br = txt->tex_TextureData;

  by = pnt[1] / br->b_size[1];

  by -= floor(by);

  if (by < 0.0) by = by + 1.0;

  if (by <= br->m_size[1]) {

    ta->ta_Color[0] = br->m[0];
    ta->ta_Color[1] = br->m[1];
    ta->ta_Color[2] = br->m[2];

    return;

  }

  by = pnt[1] / br->b_size[1];

  by -= floor(by);
  by *= 0.5;

  if (by < 0.0) by = by + 1.0;

  bx = pnt[0] / br->b_size[0];
  bx -= floor(bx);

  if (bx < 0.0) bx = bx + 1.0;

  if (bx <= br->m_size[0]) {

     if (by <= 0.5) {

        ta->ta_Color[0] = br->m[0];
        ta->ta_Color[1] = br->m[1];
        ta->ta_Color[2] = br->m[2];

        return;

     }

  }

  bx = pnt[0] / br->b_size[0] + 0.5;
  bx -= floor(bx);

  if (bx < 0.0) bx = bx + 1.0;

  if (bx <= br->m_size[0]) {

     if (by > 0.5) {

        ta->ta_Color[0] = br->m[0];
        ta->ta_Color[1] = br->m[1];
        ta->ta_Color[2] = br->m[2];

        return;
     }

  }

  bz = pnt[2] / br->b_size[2];
  bz -= floor(bz);

  if (bz < 0.0) bz = bz + 1.0;

  if (bz <= br->m_size[2]) {
     if (by > 0.5) {

        ta->ta_Color[0] = br->m[0];
        ta->ta_Color[1] = br->m[1];
        ta->ta_Color[2] = br->m[2];

        return;
     }
  }

  bz = pnt[2] / br->b_size[2] + 0.5;
  bz -= floor(bz);

  if (bz < 0.0) bz = bz + 1.0;

  if (bz <= br->m_size[2]) {
     if (by <= 0.5) {

        ta->ta_Color[0] = br->m[0];
        ta->ta_Color[1] = br->m[1];
        ta->ta_Color[2] = br->m[2];

        return;
     }
  }

  ta->ta_Color[0] = br->b[0];
  ta->ta_Color[1] = br->b[1];
  ta->ta_Color[2] = br->b[2];

}

void wrinkle(struct Texture *txt,struct TextureArg *ta)
{

  struct wrinkleinfo *wi;
  float r[3];
  float d[3];
  float p[3];
  float t[3];
  float s;

  wi = txt->tex_TextureData;

  p[0] = (ta->ta_Position[0] + wi->offset[0]) * wi->scale;
  p[1] = (ta->ta_Position[1] + wi->offset[1]) * wi->scale;
  p[2] = (ta->ta_Position[2] + wi->offset[2]) * wi->scale;

  r[0] = r[1] = r[2] = 0.0;
  s = 1.0;

  while (s < wi->fbnd) {

    t[0] = p[0] * s;
    t[1] = p[1] * s;
    t[2] = p[2] * s;

    dnoise(t,d);

    r[0] += d[0];
    r[1] += d[1];
    r[2] += d[2];

    s *= wi->fscl;

  }

  ta->ta_Normal[0] = r[0] * wi->amt;
  ta->ta_Normal[1] = r[1] * wi->amt;
  ta->ta_Normal[2] = r[2] * wi->amt;

  normalize(ta->ta_Normal);

}

void wood1(struct Texture *txt,struct TextureArg *ta)
{

  struct wood1info *wi;

  short i;
  float t,o[3];

  wi = txt->tex_TextureData;

  o[0] = ta->ta_Position[0] + wi->offset[0];
  o[1] = ta->ta_Position[1] + wi->offset[1];
  o[2] = ta->ta_Position[2] + wi->offset[2];

  t = sqrt((o[0] * o[0]) + (o[2] * o[2])) * wi->ringspacing;

  t += (turbulence(o) * wi->turbscale);

  t = (sin(t) + 1.0) * 0.5; /* just sin(t)? */

  for (i = 0; i < wi->squeeze; i++) t = t * t;

  t = t * wi->thickscale;

  t = MCLAMP(t,0.0,1.0);

  ta->ta_Color[0] = wi->w[0] + (wi->r[0] - wi->w[0]) * t;
  ta->ta_Color[1] = wi->w[1] + (wi->r[1] - wi->w[1]) * t;
  ta->ta_Color[2] = wi->w[2] + (wi->r[2] - wi->w[2]) * t;

}

void wood2(struct Texture *txt,struct TextureArg *ta)
{

  struct wood2info *wi;
  short i;
  float t;

  float s[3];
  float p[3];

  wi = txt->tex_TextureData;

  p[0] = ta->ta_Position[0];
  p[1] = ta->ta_Position[1];
  p[2] = ta->ta_Position[2];

  s[0] = p[0] * wi->scale;
  s[1] = p[1] * wi->scale;
  s[2] = p[2] * wi->scale;

  t = (sqrt(p[0] * p[0] + p[2] * p[2])) * 0.03;
  t += turbulence(s) * wi->turbscale;
  t = (sin(t) + 1.0) * 0.5;

  for (i = 0; i < wi->order; i++) t = t * t;

  t = MCLAMP(t,0.0,1.0);

  ta->ta_Color[0] = wi->w[0] + (wi->r[0] - wi->w[0]) * t;
  ta->ta_Color[1] = wi->w[1] + (wi->r[1] - wi->w[1]) * t;
  ta->ta_Color[2] = wi->w[2] + (wi->r[2] - wi->w[2]) * t;

}

void snow(struct Texture *txt,struct TextureArg *ta)
{

  struct snowinfo *sn;
  float dot, bnd;

  sn = txt->tex_TextureData;

  dot = MDOT(ta->ta_Normal,sn->dir);
  if (dot > 0.0) return;

  bnd = sn->stickiness - 1.0;

  if ((dot <= bnd) || ((MGETRND * sn->depth) > (dot - bnd))) {

    ta->ta_Color[0] = sn->color[0];
    ta->ta_Color[1] = sn->color[1];
    ta->ta_Color[2] = sn->color[2];

    ta->ta_Opacity = 0.0;
    ta->ta_Smoothness = 0.0;
    ta->ta_Metalness = 0.0;

  }

}

void granite(struct Texture *txt,struct TextureArg *ta)
{

  struct graniteinfo *gr;
  short i;
  float n, f;
  float p[3],tmp[3];
  float fx4;

  gr = txt->tex_TextureData;

  p[0] = ta->ta_Position[0] * gr->scale;
  p[1] = ta->ta_Position[1] * gr->scale;
  p[2] = ta->ta_Position[2] * gr->scale;

  n = 0.0;
  f = 1.0;

  for (i = 0; i < gr->order; i++) {

    fx4 = f * 4.0;

    tmp[0] = p[0] * fx4;
    tmp[1] = p[1] * fx4;
    tmp[2] = p[2] * fx4;

    n += fabs(0.5 - noise3(tmp)) / f;
    f *= 2.0;

  }

  if (n > 1.0) n = 1.0;

  ta->ta_Color[0] = gr->v[0] + (gr->b[0] - gr->v[0]) * n;
  ta->ta_Color[1] = gr->v[1] + (gr->b[1] - gr->v[1]) * n;
  ta->ta_Color[2] = gr->v[2] + (gr->b[2] - gr->v[2]) * n;

}

void bumps(struct Texture *txt,struct TextureArg *ta)
{

  struct bumpinfo *bmp;
  float b[3];
  float pnt[3];

  bmp = txt->tex_TextureData;

  pnt[0] = (ta->ta_Position[0] + bmp->offset[0]) * bmp->scale;
  pnt[1] = (ta->ta_Position[1] + bmp->offset[1]) * bmp->scale;
  pnt[2] = (ta->ta_Position[2] + bmp->offset[2]) * bmp->scale;

  dnoise(b,pnt);

  ta->ta_Normal[0] = b[0] * bmp->amount;
  ta->ta_Normal[1] = b[1] * bmp->amount;
  ta->ta_Normal[2] = b[2] * bmp->amount;

  normalize(ta->ta_Normal);

}

void smokey(struct Texture *txt,struct TextureArg *ta)
{

  struct smokeyinfo *ci;
  float t;
  float p[3];

  ci = txt->tex_TextureData;

  p[0] = ta->ta_Position[0] * ci->scale;
  p[1] = ta->ta_Position[1] * ci->scale;
  p[2] = ta->ta_Position[2] * ci->scale;

  t = turbulence(p);

  if (t > 1.0) t = 1.0;

  ta->ta_Color[0] = ci->c[0] + (ci->b[0] - ci->c[0]) * t;
  ta->ta_Color[1] = ci->c[1] + (ci->b[1] - ci->c[1]) * t;
  ta->ta_Color[2] = ci->c[2] + (ci->b[2] - ci->c[2]) * t;

}

void swirls(struct Texture *txt,struct TextureArg *ta)
{

  struct swirlsinfo *swi;
  float n;
  float d[3], t1, t2, t3, t4;
  float p[3];

  swi = txt->tex_TextureData;

  p[0] = ta->ta_Position[0] * swi->scale;
  p[1] = ta->ta_Position[1] * swi->scale;
  p[2] = ta->ta_Position[2] * swi->scale;

  dturbulence(d,p);

  p[0] += d[0] * swi->turbscale;
  p[1] += d[1] * swi->turbscale;
  p[2] += d[2] * swi->turbscale;

  n = noise3(p);

  t1 = 0.0;
  t2 = swi->t[0];
  t3 = swi->t[1];
  t4 = swi->t[2];

  if (n > t4) {

    ta->ta_Color[0] = swi->c4[0];
    ta->ta_Color[1] = swi->c4[1];
    ta->ta_Color[2] = swi->c4[2];

    return;

  }

  if (n >= t3) {

    ta->ta_Color[0] = swi->c3[0] + ((swi->c4[0] - swi->c3[0]) * n);
    ta->ta_Color[1] = swi->c3[1] + ((swi->c4[1] - swi->c3[1]) * n);
    ta->ta_Color[2] = swi->c3[2] + ((swi->c4[2] - swi->c3[2]) * n);

    return;

  }

  if (n >= t2) {

    ta->ta_Color[0] = swi->c2[0] + ((swi->c3[0] - swi->c2[0]) * n);
    ta->ta_Color[1] = swi->c2[1] + ((swi->c3[1] - swi->c2[1]) * n);
    ta->ta_Color[2] = swi->c2[2] + ((swi->c3[2] - swi->c2[2]) * n);

    return;

  }

  if (n >= t1) {

    ta->ta_Color[0] = swi->c1[0] + ((swi->c2[0] - swi->c1[0]) * n);
    ta->ta_Color[1] = swi->c1[1] + ((swi->c2[1] - swi->c1[1]) * n);
    ta->ta_Color[2] = swi->c1[2] + ((swi->c2[2] - swi->c1[2]) * n);

    return;

  }

  ta->ta_Color[0] = swi->c1[0];
  ta->ta_Color[1] = swi->c1[1];
  ta->ta_Color[2] = swi->c1[2];

}

void spotty(struct Texture *txt,struct TextureArg *ta)
{

  struct spottyinfo *si;
  short i;
  float n;

  float s[3], d1, d2, d3;

  si = txt->tex_TextureData;

  s[0] = ta->ta_Position[0] * si->scale;
  s[1] = ta->ta_Position[1] * si->scale;
  s[2] = ta->ta_Position[2] * si->scale;

  n = noise3(s);

  for (i = 0; i < si->squeeze; i++) n = n * n;

  d1 = si->d1;
  d2 = si->d2;
  d3 = si->d3;

  if(n > d3) {

    ta->ta_Color[0] = si->d[0];
    ta->ta_Color[1] = si->d[1];
    ta->ta_Color[2] = si->d[2];

    return;

  }

  if(n > d2) {

    ta->ta_Color[0] = si->c[0];
    ta->ta_Color[1] = si->c[1];
    ta->ta_Color[2] = si->c[2];

    return;

  }

  if(n > d1) {

    ta->ta_Color[0] = si->b[0];
    ta->ta_Color[1] = si->b[1];
    ta->ta_Color[2] = si->b[2];

    return;

  }

  ta->ta_Color[0] = si->a[0];
  ta->ta_Color[1] = si->a[1];
  ta->ta_Color[2] = si->a[2];

}

void acid(struct Texture *txt,struct TextureArg *ta)
{

  struct acidinfo *ai;

  short i;
  float t,k;

  float s[3];

  ai = txt->tex_TextureData;

  s[0] = ta->ta_Position[0] * ai->scale;
  s[1] = ta->ta_Position[1] * ai->scale;
  s[2] = ta->ta_Position[2] * ai->scale;

  t = sin((
    (ta->ta_Position[0] * ai->scale2[0]) +
    (ta->ta_Position[1] * ai->scale2[1]) +
    (ta->ta_Position[2] * ai->scale2[2])
    ) + (turbulence(s) * ai->turbscale));

  for (i = 0; i < ai->order; i++) t = t * t;

  t = MCLAMP(t,0.0,1.0);

  if(t > ai->level) {

    ta->ta_Flags |= TAFLAG_CLIP;

  }
  else {

    k = ai->level * 0.30;
    if(t > k) {

      t = 1.0 - ((t - k) / (ai->level - k));

      ta->ta_Color[0] *= t;
      ta->ta_Color[1] *= t;
      ta->ta_Color[2] *= t;

    }

  }

}

void wire(struct Texture *txt,struct TextureArg *ta)
{

  struct wireinfo *wi;
  float t,tw,p[3];
  double dummy;

  wi = txt->tex_TextureData;

  p[0] = ta->ta_Position[0];
  p[1] = ta->ta_Position[1];
  p[2] = ta->ta_Position[2];

  if(p[0] < 0.0) p[0] = (-p[0]) + wi->spacing[0];
  if(p[1] < 0.0) p[1] = (-p[1]) + wi->spacing[1];
  if(p[2] < 0.0) p[2] = (-p[2]) + wi->spacing[2];

  tw = wi->width[0] + wi->spacing[0];

  t = modf(p[0]/tw,&dummy);
  t *= tw;

  if(t > wi->width[0]) {

    tw = wi->width[1] + wi->spacing[1];

    t = modf(p[1]/tw,&dummy);
    t *= tw;

    if(t > wi->width[1]) {

      tw = wi->width[2] + wi->spacing[2];

      t = modf(p[2]/tw,&dummy);
      t *= tw;

      if(t > wi->width[2]) {

        ta->ta_Color[0] = 0.0;
        ta->ta_Color[1] = 0.0;
        ta->ta_Color[2] = 0.0;
        ta->ta_Flags |= TAFLAG_CLIP;

      }

    }

  }

}
