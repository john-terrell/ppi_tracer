#include "exec/types.h"
#include "exec/memory.h"
#include "intuition/intuition.h"
#include "functions.h"

#include "TRACER:tracer.h"

extern struct CustomRender *crmsg;

extern struct CustomTexture ctmsg;

extern struct MsgPort *ctrport;

extern struct WaveSource wavsrc[];

extern FLOAT sqrt(), pow(), fabs(), sin(), floor();

extern FLOAT getrnd(), unitvector();

extern FLOAT noise(), turbulence();

extern FLOAT cycloidal(), trianglewave();

VOID normtxt1(px, py, pz, nx, ny, nz, w)
FLOAT px, py, pz, *nx, *ny, *nz;
SHORT w;
{
   register struct Ripple *riptx;

   register FLOAT tmp1, tmp2, tmp3, frad;

   LONG lrad;
   FLOAT rx, ry, rz, len;

   riptx = crmsg->riptx;

   riptx = &riptx[w];

   rx = riptx->cx - px;
   ry = riptx->cy - py;
   rz = riptx->cz - pz;

   tmp1 = rx;
   tmp2 = ry;
   tmp3 = rz;

   len = unitvector(&rx, &ry, &rz);

   if (len == 0.0) return;

   tmp1 = tmp1 * tmp1;
   tmp2 = tmp2 * tmp2;
   tmp3 = tmp3 * tmp3;

   frad = sqrt(tmp1 + tmp2 + tmp3);

   tmp1 = riptx->wavelength;

   frad = frad + riptx->propagate * tmp1;

   if (tmp1 != 0.0) frad = frad / tmp1;

   tmp1 = riptx->damp;

   if (tmp1 != 1.0) tmp1 = pow(tmp1, frad);

   lrad = frad;

   if (lrad & 1) {
      rx = -rx;
      ry = -ry;
      rz = -rz;
   }

   frad = (frad - lrad) - 0.5;

   if (frad < 0.0) frad = -frad;

   frad = (0.5 - frad) * tmp1;

   frad = frad * riptx->amplitude;

   *nx = rx * frad;
   *ny = ry * frad;
   *nz = rz * frad;

   return;
}

VOID noiseB(px, py, pz, tx, ty, tz)
FLOAT px, py, pz, *tx, *ty, *tz;
{
   register SHORT w;
   register FLOAT l;

   FLOAT nx, ny, nz, rx, ry, rz;

   nx = 0.0;
   ny = 0.0;
   nz = 0.0;

   for (w = 0; w < NUMRIPTX - 1; w++) {
      normtxt1(px, py, pz, &rx, &ry, &rz, w);

      nx = nx + rx;
      ny = ny + ry;
      nz = nz + rz;
   }

   l = unitvector(&nx, &ny, &nz);

   if (l != 0.0) {
      *tx = nx;
      *ty = ny;
      *tz = nz;
   }
   else {
      *tx = 1.0;
      *ty = 0.0;
      *tz = 0.0;
   }

   return;
}

FLOAT noiseA(px, py, pz)
FLOAT px, py, pz;
{
   register FLOAT s;

   FLOAT tx, ty, tz;

   noiseB(px, py, pz, &tx, &ty, &tz);

   s = tx + ty + tz;

   s = s + 1.7321;

   s = s * 0.2886;

   return(s);
}

FLOAT turbulenceA(px, py, pz)
FLOAT px, py, pz;
{
   register FLOAT t, s;

   FLOAT tx, ty, tz;

   t = 0.0;

   s = 1.0;

   while (s < 64.0) {
      tx = px * s;
      ty = py * s;
      tz = pz * s;

      t = t + fabs(noiseA(tx, ty, tz) / s);

      s = s * 2.0;
   }

   return(t);
}

VOID normtxt2(px, py, pz, nx, ny, nz, w)
FLOAT px, py, pz, *nx, *ny, *nz;
SHORT w;
{
   register struct Bumpy *bumtx;

   register SHORT i;
   register FLOAT n;

   bumtx = crmsg->bumtx;

   bumtx = &bumtx[w];

   px = px * bumtx->displ + bumtx->ox;
   py = py * bumtx->displ + bumtx->oy;
   pz = pz * bumtx->displ + bumtx->oz;

   n = noiseA(px, py, pz) - 0.5;

   for (i = 0; i < bumtx->squeeze; i++) n = n * 0.5;

   n = n * bumtx->scale;

   *nx = n;
   *ny = n;
   *nz = n;

   return;
}

VOID normtxt3(px, py, pz, nx, ny, nz, w)
FLOAT px, py, pz, *nx, *ny, *nz;
SHORT w;
{
   register struct Rough *routx;

   register FLOAT nv;

   routx = crmsg->routx;

   routx = &routx[w];

   px = px + routx->ox;
   py = py + routx->oy;
   pz = pz + routx->oz;

   px = px * routx->scale;
   py = py * routx->scale;
   pz = pz * routx->scale;

   nv = noise(px, py, pz) - 0.5;
/*
   nv = getrnd() - 0.5;
*/
   nv = nv * routx->amount;

   *nx = nv;
   *ny = nv;
   *nz = nv;

   return;
}

VOID normtxt4(px, py, pz, nx, ny, nz, w)
FLOAT px, py, pz, *nx, *ny, *nz;
SHORT w;
{
   register struct Bumps *bmptx;

   FLOAT bx, by, bz;

   bmptx = crmsg->bmptx;

   bmptx = &bmptx[w];

   px = px + bmptx->xo;
   py = py + bmptx->yo;
   pz = pz + bmptx->zo;

   px = px * bmptx->scale;
   py = py * bmptx->scale;
   pz = pz * bmptx->scale;

   dnoise(&bx, &by, &bz, px, py, pz);

   bx = bx * bmptx->bamt;
   by = by * bmptx->bamt;
   bz = bz * bmptx->bamt;

   *nx = bx;
   *ny = by;
   *nz = bz;

   return;
}

VOID normtxt5(px, py, pz, nx, ny, nz, w)
FLOAT px, py, pz, *nx, *ny, *nz;
SHORT w;
{
   register struct Dented *dentx;

   register SHORT i;
   register FLOAT nv, sc;

   FLOAT dx, dy, dz;

   dentx = crmsg->dentx;

   dentx = &dentx[w];

   px = px + dentx->xo;
   py = py + dentx->yo;
   pz = pz + dentx->zo;

   px = px * dentx->scale;
   py = py * dentx->scale;
   pz = pz * dentx->scale;

   nv = 0.0;

   sc = 1.0;

   for (i = 0; i < dentx->freq; i++) {
      nv += noise(px * sc, py * sc, pz * sc) / sc;

      sc *= dentx->fscl;

      continue;
   }

   for (i = 0; i < dentx->order; i++) {
      nv = nv * nv;

      continue;
   }

   dnoise(&dx, &dy, &dz, px, py, pz);

   dx = dx * nv;
   dy = dy * nv;
   dz = dz * nv;

   dx = dx * dentx->amt;
   dy = dy * dentx->amt;
   dz = dz * dentx->amt;

   *nx = dx;
   *ny = dy;
   *nz = dz;

   return;
}

VOID normtxt6(px, py, pz, nx, ny, nz, w)
FLOAT px, py, pz, *nx, *ny, *nz;
SHORT w;
{
   register struct Eroded *erotx;

   register FLOAT rx, ry, rz, s;

   FLOAT dx, dy, dz;

   erotx = crmsg->erotx;

   erotx = &erotx[w];

   px = px + erotx->xo;
   py = py + erotx->yo;
   pz = pz + erotx->zo;

   px = px * erotx->scale;
   py = py * erotx->scale;
   pz = pz * erotx->scale;

   rx = ry = rz = 0.0;

   s = 1.0;

   while (s < erotx->fbnd) {
      dnoise(&dx, &dy, &dz, px * s, py * s, pz * s);

      rx += fabs(dx) / s;
      ry += fabs(dy) / s;
      rz += fabs(dz) / s;

      s *= erotx->fscl;

      continue;
   }

   rx = rx * rx * rx;
   ry = ry * ry * ry;
   rz = rz * rz * rz;

   rx = rx * erotx->amt;
   ry = ry * erotx->amt;
   rz = rz * erotx->amt;

   *nx = rx;
   *ny = ry;
   *nz = rz;

   return;
}

VOID normtxt7(px, py, pz, nx, ny, nz, w)
FLOAT px, py, pz, *nx, *ny, *nz;
SHORT w;
{
   register struct Stucco *stutx;

   register SHORT i;
   register FLOAT nv;

   FLOAT bx, by, bz;

   stutx = crmsg->stutx;

   stutx = &stutx[w];

   px = px + stutx->xo;
   py = py + stutx->yo;
   pz = pz + stutx->zo;

   px = px * stutx->scale;
   py = py * stutx->scale;
   pz = pz * stutx->scale;

   nv = noise(px, py, pz);

   for (i = 0; i < stutx->order; i++) {
      nv = nv * nv;

      continue;
   }

   dnoise(&bx, &by, &bz, px, py, pz);

   bx = bx * nv;
   by = by * nv;
   bz = bz * nv;

   bx = bx * stutx->amt;
   by = by * stutx->amt;
   bz = bz * stutx->amt;

   *nx = bx;
   *ny = by;
   *nz = bz;

   return;
}

VOID normtxt8(px, py, pz, nx, ny, nz, w)
FLOAT px, py, pz, *nx, *ny, *nz;
SHORT w;
{
   register struct Wavy *wavtx;

   register SHORT i;
   register LONG lrad;
   register FLOAT frad, wave, damp;

   FLOAT tx, ty, tz, rx, ry, rz, ln;

   wavtx = crmsg->wavtx;

   wavtx = &wavtx[w];
/*
   px = px + wavtx->xo;
   py = py + wavtx->yo;
   pz = pz + wavtx->zo;
*/
   tx = ty = tz = 0.0;

   for (i = 0; i < wavtx->wcnt; i++) {
      rx = wavsrc[i].cx - px;
      ry = wavsrc[i].cy - py;
      rz = wavsrc[i].cz - pz;

      ln = rx * rx + ry * ry + rz * rz;

      if (ln < 1.0) continue;

      frad = sqrt(ln);

      rx /= frad;
      ry /= frad;
      rz /= frad;

      wave = wavtx->wave;

      if (wavtx->flg) wave = wavsrc[i].freq * wave;

      frad += wavtx->samp * wave;

      if (wave != 0.0) frad /= wave;

      damp = wavtx->damp;

      if (damp != 1.0) damp = pow(damp, frad);

      lrad = frad;

      if (lrad & 1) {
         rx = -rx;
         ry = -ry;
         rz = -rz;
      }

      frad = (frad - lrad) - 0.5;

      frad = (0.5 - fabs(frad)) * damp;

      frad *= wavtx->ampl;

      rx *= frad;
      ry *= frad;
      rz *= frad;

      tx += rx;
      ty += ry;
      tz += rz;
   }

   *nx = tx;
   *ny = ty;
   *nz = tz;

   return;
}

VOID normtxt9(px, py, pz, nx, ny, nz, w)
FLOAT px, py, pz, *nx, *ny, *nz;
SHORT w;
{
   register struct Wrinkle *writx;

   register FLOAT rx, ry, rz, s;

   FLOAT dx, dy, dz;

   writx = crmsg->writx;

   writx = &writx[w];

   px = px + writx->xo;
   py = py + writx->yo;
   pz = pz + writx->zo;

   px = px * writx->scale;
   py = py * writx->scale;
   pz = pz * writx->scale;

   rx = ry = rz = 0.0;

   s = 1.0;

   while (s < writx->fbnd) {
      dnoise(&dx, &dy, &dz, px * s, py * s, pz * s);

      rx += dx;
      ry += dy;
      rz += dz;

      s *= writx->fscl;

      continue;
   }

   rx = rx * writx->amt;
   ry = ry * writx->amt;
   rz = rz * writx->amt;

   *nx = rx;
   *ny = ry;
   *nz = rz;

   return;
}

VOID normtxt0(px, py, pz, nx, ny, nz, w)
FLOAT px, py, pz, *nx, *ny, *nz;
SHORT w;
{
   struct MsgPort *port;

   port = FindPort("NORMAL_TEXTURE");

   if (port == NULL) return;

   ctmsg.msg.mn_ReplyPort = ctrport;
   ctmsg.msg.mn_Length = sizeof(struct CustomTexture);

   ctmsg.px = px;
   ctmsg.py = py;
   ctmsg.pz = pz;

   ctmsg.nx = *nx;
   ctmsg.ny = *ny;
   ctmsg.nz = *nz;

   PutMsg(port, &ctmsg);

   WaitPort(ctrport);

   GetMsg(ctrport);

   *nx = ctmsg.nx;
   *ny = ctmsg.ny;
   *nz = ctmsg.nz;

   return;
}

VOID colrtxt1(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Spotty *spotx;

   register SHORT i;
   register FLOAT n;

   FLOAT sx, sy, sz, s1, e1, d1, d2, d3;

   spotx = crmsg->spotx;

   spotx = &spotx[w];

   sx = px * spotx->scale;
   sy = py * spotx->scale;
   sz = pz * spotx->scale;

   n = noiseA(sx, sy, sz);

   for (i = 0; i < spotx->squeeze; i++) n = n * n;

   s1 = 0.0;
   e1 = 1.0;

   d1 = spotx->d1;
   d2 = spotx->d2;
   d3 = spotx->d3;

   if (n >= s1 && n < d1) {
      *r = spotx->ar;
      *g = spotx->ag;
      *b = spotx->ab;
   } else

   if (n >= d1 && n < d2) {
      *r = spotx->br;
      *g = spotx->bg;
      *b = spotx->bb;
   } else

   if (n >= d2 && n < d3) {
      *r = spotx->cr;
      *g = spotx->cg;
      *b = spotx->cb;
   } else

   if (n >= d3 && n < e1) {
      *r = spotx->dr;
      *g = spotx->dg;
      *b = spotx->db;
   }

   return;
}

VOID colrtxt2(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Marble *martx;

   register SHORT i;
   register FLOAT t;

   SHORT vr, vg, vb, br, bg, bb;
   FLOAT ox, oy, oz;

   martx = crmsg->martx;

   martx = &martx[w];

   ox = px + martx->ox;
   oy = py + martx->oy;
   oz = pz + martx->oz;

   t = ox * martx->xscale;

   t = t + turbulenceA(ox, oy, oz) * martx->turbscale;

   t = sin(t);

   for (i = 0; i < martx->squeeze; i++) t = t * t;

   if (t < 0.0) t = 0.0;
   else
   if (t > 1.0) t = 1.0;

   br = martx->br;
   bg = martx->bg;
   bb = martx->bb;

   vr = martx->vr;
   vg = martx->vg;
   vb = martx->vb;

   *r = br + (vr - br) * t;
   *g = bg + (vg - bg) * t;
   *b = bb + (vb - bb) * t;

   return;
}

VOID colrtxt3(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Wood *wootx;

   register SHORT i;
   register FLOAT t;

   SHORT wr, wg, wb, rr, rg, rb;
   FLOAT ox, oy, oz;

   wootx = crmsg->wootx;

   wootx = &wootx[w];

   ox = px + wootx->ox;
   oy = py + wootx->oy;
   oz = pz + wootx->oz;

   t = sqrt(ox * ox + oz * oz);

   t = t * wootx->ringspacing;

   t = t + turbulenceA(ox, oy, oz) * wootx->turbscale;

   t = (sin(t) + 1.0) * 0.5; /* just sin(t)? */

   for (i = 0; i < wootx->squeeze; i++) t = t * t;

   t = t * wootx->thickscale;

   if (t < 0.0) t = 0.0;
   else
   if (t > 1.0) t = 1.0;

   wr = wootx->wr;
   wg = wootx->wg;
   wb = wootx->wb;

   rr = wootx->rr;
   rg = wootx->rg;
   rb = wootx->rb;

   *r = wr + (rr - wr) * t;
   *g = wg + (rg - wg) * t;
   *b = wb + (rb - wb) * t;

   return;
}

VOID colrtxt4(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Brick *britx;

   register FLOAT bx, by, bz;

   LONG ix, iy, iz;

   britx = crmsg->britx;

   britx = &britx[w];

   if (britx->bheight == 0.0) return;

   if (britx->bwidth == 0.0) return;

   if (britx->bdepth == 0.0) return;

   by = py / britx->bheight;

   iy = by;

   by = by - iy;

   if (by < 0.0) by = by + 1.0;

   if (by <= britx->mheight) {
      *r = britx->mr;
      *g = britx->mg;
      *b = britx->mb;

      return;
   }

   by = py / britx->bheight;

   by = by * 0.5;

   iy = by;

   by = by - iy;

   if (by < 0.0) by = by + 1.0;

   bx = px / britx->bwidth;

   ix = bx;

   bx = bx - ix;

   if (bx < 0.0) bx = bx + 1.0;

   if (bx <= britx->mwidth) {
      if (by <= 0.5) {
         *r = britx->mr;
         *g = britx->mg;
         *b = britx->mb;

         return;
      }
   }

   bx = px / britx->bwidth + 0.5;

   ix = bx;

   bx = bx - ix;

   if (bx < 0.0) bx = bx + 1.0;

   if (bx <= britx->mwidth) {
      if (by > 0.5) {
         *r = britx->mr;
         *g = britx->mg;
         *b = britx->mb;

         return;
      }
   }

   bz = pz / britx->bdepth;

   iz = bz;

   bz = bz - iz;

   if (bz < 0.0) bz = bz + 1.0;

   if (bz <= britx->mdepth) {
      if (by > 0.5) {
         *r = britx->mr;
         *g = britx->mg;
         *b = britx->mb;

         return;
      }
   }

   bz = pz / britx->bdepth + 0.5;

   iz = bz;

   bz = bz - iz;

   if (bz < 0.0) bz = bz + 1.0;

   if (bz <= britx->mdepth) {
      if (by <= 0.5) {
         *r = britx->mr;
         *g = britx->mg;
         *b = britx->mb;

         return;
      }
   }

   *r = britx->br;
   *g = britx->bg;
   *b = britx->bb;

   return;
}

VOID colrtxt5(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Checker *chetx;

   register LONG ix, iy, iz;
   register FLOAT val, tmp;

   chetx = crmsg->chetx;

   chetx = &chetx[w];

   ix = 0;
   iy = 0;
   iz = 0;

   if (chetx->xs != 0.0) {
      val = px / chetx->xs;

      if (val < 0.0) {
         tmp = 1.0 - val;

         val = tmp;
      }

      ix = val;
   }

   if (chetx->ys != 0.0) {
      val = py / chetx->ys;

      if (val < 0.0) {
         tmp = 1.0 - val;

         val = tmp;
      }

      iy = val;
   }

   if (chetx->zs != 0.0) {
      val = pz / chetx->zs;

      if (val < 0.0) {
         tmp = 1.0 - val;

         val = tmp;
      }

      iz = val;
   }

   if (ix + iy + iz & 1) {
      *r = chetx->cr;
      *g = chetx->cg;
      *b = chetx->cb;
   }
   else {
      *r = chetx->br;
      *g = chetx->bg;
      *b = chetx->bb;
   }

   return;
}

VOID colrtxt6(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Blend *bletx;

   register FLOAT val, scale, coord;

   SHORT sr, sg, sb, er, eg, eb;

   bletx = crmsg->bletx;

   bletx = &bletx[w];

   scale = bletx->end - bletx->start;

   if (scale == 0.0) scale = 1.0;

   if (bletx->dx == 1.0) coord = px;
   else
   if (bletx->dy == 1.0) coord = py;
   else
   if (bletx->dz == 1.0) coord = pz;

   val = (coord - bletx->start) / scale;

   if (val < 0.0) val = 0.0;
   else
   if (val > 1.0) val = 1.0;

   sr = bletx->sr;
   sg = bletx->sg;
   sb = bletx->sb;

   er = bletx->er;
   eg = bletx->eg;
   eb = bletx->eb;

   *r = sr + (er - sr) * val;
   *g = sg + (eg - sg) * val;
   *b = sb + (eb - sb) * val;

   return;
}

VOID colrtxt7(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Terrain *tertx;

   register SHORT ca;
   register FLOAT bdis, sub, lev;

   SHORT cr, cg, cb;
   FLOAT num, den;

   tertx = crmsg->tertx;

   tertx = &tertx[w];

   cr = *r;
   cg = *g;
   cb = *b;

   ca = 255.0 * (getrnd() * tertx->coldim);

   if (py > tertx->alev) {
      cr = tertx->sr;
      cg = tertx->sg;
      cb = tertx->sb;

      cr -= ca;
      cg -= ca;
      cb -= ca;
   } else

   if (py <= tertx->alev && py > tertx->blev) {
      cr = tertx->ar;
      cg = tertx->ag;
      cb = tertx->ab;

      cr -= ca;
      cg -= ca;
      cb -= ca;

      sub = fabs(tertx->alev - tertx->blev) * tertx->aper;

      lev = tertx->blev + sub;

      if (py <= lev) goto nosdrop;

      num = fabs(py - lev);

      den = fabs(tertx->alev - lev);

      if (den < FERR) goto nosdrop;

      bdis = num / den;

      if (getrnd() < bdis) {
         cr = tertx->sr;
         cg = tertx->sg;
         cb = tertx->sb;

         cr -= ca;
         cg -= ca;
         cb -= ca;
      }

      nosdrop: ;
   } else

   if (py <= tertx->blev && py > tertx->end) {
      cr = tertx->br;
      cg = tertx->bg;
      cb = tertx->bb;

      cr -= ca;
      cg -= ca;
      cb -= ca;

      sub = fabs(tertx->blev - tertx->end) * tertx->bper;

      lev = tertx->end + sub;

      if (py <= lev) goto noadrop;

      num = fabs(py - lev);

      den = fabs(tertx->blev - lev);

      if (den < FERR) goto noadrop;

      bdis = num / den;

      if (getrnd() < bdis) {
         cr = tertx->ar;
         cg = tertx->ag;
         cb = tertx->ab;

         cr -= ca;
         cg -= ca;
         cb -= ca;
      }

      noadrop: ;
   } else

   if (py <= tertx->end) {
      cr = tertx->er;
      cg = tertx->eg;
      cb = tertx->eb;

      cr -= ca;
      cg -= ca;
      cb -= ca;
   }

   *r = cr < 0 ? 0 : cr;
   *g = cg < 0 ? 0 : cg;
   *b = cb < 0 ? 0 : cb;

   return;
}

VOID colrtxt8(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Bozo *boztx;

   register FLOAT n;

   SHORT cr, cg, cb;
   FLOAT d1, d2, d3, d4, d5;

   boztx = crmsg->boztx;

   boztx = &boztx[w];

   px = px * boztx->scale;
   py = py * boztx->scale;
   pz = pz * boztx->scale;

   n = noise(px, py, pz);

   d1 = 0.0;
   d2 = boztx->d1;
   d3 = boztx->d2;
   d4 = boztx->d3;
   d5 = 1.0;

   if (n >= d1 && n <= d2) {
      cr = boztx->r1;
      cg = boztx->g1;
      cb = boztx->b1;
   } else

   if (n >= d2 && n <= d3) {
      cr = boztx->r2;
      cg = boztx->g2;
      cb = boztx->b2;
   } else

   if (n >= d3 && n <= d4) {
      cr = boztx->r3;
      cg = boztx->g3;
      cb = boztx->b3;
   } else

   if (n >= d4 && n <= d5) {
      cr = boztx->r4;
      cg = boztx->g4;
      cb = boztx->b4;
   }

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID colrtxt9(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Cloudy *clotx;

   register FLOAT t;

   SHORT rs, gs, bs, re, ge, be, cr, cg, cb;

   clotx = crmsg->clotx;

   clotx = &clotx[w];

   px = px * clotx->scale;
   py = py * clotx->scale;
   pz = pz * clotx->scale;

   t = turbulence(px, py, pz);

   if (t > 1.0) t = 1.0;

   rs = clotx->cr;
   gs = clotx->cg;
   bs = clotx->cb;

   re = clotx->br;
   ge = clotx->bg;
   be = clotx->bb;

   cr = rs = (re - rs) * t;
   cg = gs = (ge - gs) * t;
   cb = bs = (be - bs) * t;

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID colrtxt10(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Dots *dottx;

   register FLOAT dx, dy, dz, ln;

   SHORT cr, cg, cb;
   FLOAT lx, ux, ly, uy, lz, uz, cx, cy, cz;

   dottx = crmsg->dottx;

   dottx = &dottx[w];

   px = px * dottx->scale;
   py = py * dottx->scale;
   pz = pz * dottx->scale;

   lx = floor(px / dottx->space) * dottx->space;

   ux = lx + dottx->space;

   ly = floor(py / dottx->space) * dottx->space;

   uy = ly + dottx->space;

   lz = floor(pz / dottx->space) * dottx->space;

   uz = lz + dottx->space;

   cx = lx + (ux - lx) / 2.0;
   cy = ly + (uy - ly) / 2.0;
   cz = lz + (uz - lz) / 2.0;

   dx = px - cx;
   dy = py - cy;
   dz = pz - cz;

   ln = sqrt(dx * dx + dy * dy + dz * dz);

   if (ln <= dottx->size) {
      cr = dottx->dr;
      cg = dottx->dg;
      cb = dottx->db;
   }
   else {
      cr = dottx->br;
      cg = dottx->bg;
      cb = dottx->bb;
   }

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID colrtxt11(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Fire1 *fi1tx;

   register FLOAT l, t, v, m;

   SHORT rs, gs, bs, re, ge, be, cr, cg, cb;
   FLOAT sx, sy, sz, dx, dy, dz, t1, t2, t3, t4;

   fi1tx = crmsg->fi1tx;

   fi1tx = &fi1tx[w];

   sx = px * fi1tx->scale;
   sy = py * fi1tx->scale;
   sz = pz * fi1tx->scale;

   dx = px - fi1tx->cx;
   dy = py - fi1tx->cy;
   dz = pz - fi1tx->cz;

   l = sqrt(dx * dx + dy * dy + dz * dz);

   if (l > fi1tx->turbstart) {
      t = turbulence(sx, sy, sz);

      if (t > 1.0) t = 1.0;
   }
   else {
      t = 0.0;
   }

   v = l + t * fi1tx->turbscale;

   t1 = 0.0;
   t2 = fi1tx->t2;
   t3 = fi1tx->t3;
   t4 = fi1tx->t4;

   if (v >= t1 && v <= t2) {
      m = (v - t1) / (t2 - t1);

      rs = fi1tx->r1;
      gs = fi1tx->g1;
      bs = fi1tx->b1;

      re = fi1tx->r2;
      ge = fi1tx->g2;
      be = fi1tx->b2;

      cr = rs + (re - rs) * m;
      cg = gs + (ge - gs) * m;
      cb = bs + (be - bs) * m;
   } else

   if (v >= t2 && v <= t3) {
      m = (v - t2) / (t3 - t2);

      rs = fi1tx->r2;
      gs = fi1tx->g2;
      bs = fi1tx->b2;

      re = fi1tx->r3;
      ge = fi1tx->g3;
      be = fi1tx->b3;

      cr = rs + (re - rs) * m;
      cg = gs + (ge - gs) * m;
      cb = bs + (be - bs) * m;
   } else

   if (v >= t3 && v <= t4) {
      m = (v - t3) / (t4 - t3);

      rs = fi1tx->r3;
      gs = fi1tx->g3;
      bs = fi1tx->b3;

      re = fi1tx->r4;
      ge = fi1tx->g4;
      be = fi1tx->b4;

      cr = rs + (re - rs) * m;
      cg = gs + (ge - gs) * m;
      cb = bs + (be - bs) * m;
   } else

   if (v >= t4) {
      cr = fi1tx->r4;
      cg = fi1tx->g4;
      cb = fi1tx->b4;
   }

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID colrtxt12(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Fire2 *fi2tx;

   register FLOAT n, d, t, l, v, m;

   SHORT rs, gs, bs, re, ge, be, cr, cg, cb;
   FLOAT sx, sy, sz, tx, ty, tz, dx, dy, dz, t1, t2, t3, t4;

   fi2tx = crmsg->fi2tx;

   fi2tx = &fi2tx[w];

   sx = px * fi2tx->scale;
   sy = py * fi2tx->scale;
   sz = pz * fi2tx->scale;

   tx = fi2tx->nx * (px - fi2tx->ox);
   ty = fi2tx->ny * (py - fi2tx->oy);
   tz = fi2tx->nz * (pz - fi2tx->oz);

   n = tx + ty + tz;

   tx = fi2tx->nx * fi2tx->nx;
   ty = fi2tx->ny * fi2tx->ny;
   tz = fi2tx->nz * fi2tx->nz;

   d = tx + ty + tz;

   t = -(n / d);

   tx = px + fi2tx->nx * t;
   ty = py + fi2tx->ny * t;
   tz = pz + fi2tx->nz * t;

   dx = tx - px;
   dy = ty - py;
   dz = tz - pz;

   l = sqrt(dx * dx + dy * dy + dz * dz);

   if (l > fi2tx->turbstart) {
      t = turbulence(sx, sy, sz);

      if (t > 1.0) t = 1.0;
   }
   else {
      t = 0.0;
   }

   v = l + t * fi2tx->turbscale;

   t1 = 0.0;
   t2 = fi2tx->t2;
   t3 = fi2tx->t3;
   t4 = fi2tx->t4;

   if (v >= t1 && v <= t2) {
      m = (v - t1) / (t2 - t1);

      rs = fi2tx->r1;
      gs = fi2tx->g1;
      bs = fi2tx->b1;

      re = fi2tx->r2;
      ge = fi2tx->g2;
      be = fi2tx->b2;

      cr = rs + (re - rs) * m;
      cg = gs + (ge - gs) * m;
      cb = bs + (be - bs) * m;
   } else

   if (v >= t2 && v <= t3) {
      m = (v - t2) / (t3 - t2);

      rs = fi2tx->r2;
      gs = fi2tx->g2;
      bs = fi2tx->b2;

      re = fi2tx->r3;
      ge = fi2tx->g3;
      be = fi2tx->b3;

      cr = rs + (re - rs) * m;
      cg = gs + (ge - gs) * m;
      cb = bs + (be - bs) * m;
   } else

   if (v >= t3 && v <= t4) {
      m = (v - t3) / (t4 - t3);

      rs = fi2tx->r3;
      gs = fi2tx->g3;
      bs = fi2tx->b3;

      re = fi2tx->r4;
      ge = fi2tx->g4;
      be = fi2tx->b4;

      cr = rs + (re - rs) * m;
      cg = gs + (ge - gs) * m;
      cb = bs + (be - bs) * m;
   } else

   if (v >= t4) {
      cr = fi2tx->r4;
      cg = fi2tx->g4;
      cb = fi2tx->b4;
   }

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID colrtxt13(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Granite *gratx;

   register SHORT i;
   register FLOAT n, f, t;

   SHORT rs, gs, bs, re, ge, be, cr, cg, cb;

   gratx = crmsg->gratx;

   gratx = &gratx[w];

   px = px * gratx->scale;
   py = py * gratx->scale;
   pz = pz * gratx->scale;

   n = 0.0;

   f = 1.0;

   for (i = 0; i < 6; i++) {
      t = 0.5 - noise(px * 4.0 * f, py * 4.0 * f, pz * 4.0 * f);

      n += fabs(t) / f;

      f *= 2.0;

      continue;
   }

   if (n > 1.0) n = 1.0;

   rs = gratx->vr;
   gs = gratx->vg;
   bs = gratx->vb;

   re = gratx->br;
   ge = gratx->bg;
   be = gratx->bb;

   cr = rs + (re - rs) * n;
   cg = gs + (ge - gs) * n;
   cb = bs + (be - bs) * n;

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID colrtxt14(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Grid *gritx;

   register SHORT gr;

   SHORT cr, cg, cb;
   LONG lx, ly, lz, spc, siz;

   gritx = crmsg->gritx;

   gritx = &gritx[w];

   px = px * gritx->scale;
   py = py * gritx->scale;
   pz = pz * gritx->scale;

   lx = fabs(px + 1000000.0);
   ly = fabs(py + 1000000.0);
   lz = fabs(pz + 1000000.0);

   spc = gritx->space;

   siz = gritx->size;

   gr = 0;

   if (lx % spc < siz) gr = 1;
   else
   if (ly % spc < siz) gr = 1;
   else
   if (lz % spc < siz) gr = 1;

   if (gr) {
      cr = gritx->gr;
      cg = gritx->gg;
      cb = gritx->gb;
   }
   else {
      cr = gritx->br;
      cg = gritx->bg;
      cb = gritx->bb;
   }

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID colrtxt15(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Marble2 *ma2tx;

   register SHORT i;
   register FLOAT t;

   SHORT rs, gs, bs, re, ge, be, cr, cg, cb;
   FLOAT sx, sy, sz;

   ma2tx = crmsg->ma2tx;

   ma2tx = &ma2tx[w];

   sx = px * ma2tx->scale;
   sy = py * ma2tx->scale;
   sz = pz * ma2tx->scale;

   t = px * ma2tx->xs + py * ma2tx->ys + pz * ma2tx->zs;

   t = t + turbulence(sx, sy, sz) * ma2tx->turbscale;

   t = sin(t);

   for (i = 0; i < ma2tx->order; i++) t = t * t;

   if (t < 0.0) t = 0.0;
   else
   if (t > 1.0) t = 1.0;

   rs = ma2tx->br;
   gs = ma2tx->bg;
   bs = ma2tx->bb;

   re = ma2tx->vr;
   ge = ma2tx->vg;
   be = ma2tx->vb;

   cr = rs + (re - rs) * t;
   cg = gs + (ge - gs) * t;
   cb = bs + (be - bs) * t;

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID colrtxt16(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Marble3 *ma3tx;

   register FLOAT n, s;

   SHORT cr, cg, cb, rs, gs, bs, re, ge, be;
   FLOAT t1, t2, t3, t4;

   ma3tx = crmsg->ma3tx;

   ma3tx = &ma3tx[w];

   px = px * ma3tx->scale;
   py = py * ma3tx->scale;
   pz = pz * ma3tx->scale;

   n = px + turbulence(px, py, pz) * ma3tx->turbscale;

   n = trianglewave(n);

   t1 = 0.0;
   t2 = ma3tx->t1;
   t3 = ma3tx->t2;
   t4 = ma3tx->t3;

   if (n < t1) {
      cr = ma3tx->r1;
      cg = ma3tx->g1;
      cb = ma3tx->b1;
   } else

   if (n >= t1 && n <= t2) {
      s = (n - t1) / (t2 - t1);

      rs = ma3tx->r1;
      gs = ma3tx->g1;
      bs = ma3tx->b1;

      re = ma3tx->r2;
      ge = ma3tx->g2;
      be = ma3tx->b2;

      cr = rs + (re - rs) * s;
      cg = gs + (ge - gs) * s;
      cb = bs + (be - bs) * s;
   } else

   if (n >= t2 && n <= t3) {
      s = (n - t2) / (t3 - t2);

      rs = ma3tx->r2;
      gs = ma3tx->g2;
      bs = ma3tx->b2;

      re = ma3tx->r3;
      ge = ma3tx->g3;
      be = ma3tx->b3;

      cr = rs + (re - rs) * s;
      cg = gs + (ge - gs) * s;
      cb = bs + (be - bs) * s;
   } else

   if (n >= t3 && n <= t4) {
      s = (n - t3) / (t4 - t3);

      rs = ma3tx->r3;
      gs = ma3tx->g3;
      bs = ma3tx->b3;

      re = ma3tx->r4;
      ge = ma3tx->g4;
      be = ma3tx->b4;

      cr = rs + (re - rs) * s;
      cg = gs + (ge - gs) * s;
      cb = bs + (be - bs) * s;
   } else

   if (n > t4) {
      cr = ma3tx->r4;
      cg = ma3tx->g4;
      cb = ma3tx->b4;
   }

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID colrtxt17(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct RGBCube *cubtx;

   register FLOAT xs, ys, zs;

   SHORT cr, cg, cb;

   cubtx = crmsg->cubtx;

   cubtx = &cubtx[w];

   xs = (px - cubtx->xl) / (cubtx->xh - cubtx->xl);
   ys = (py - cubtx->yl) / (cubtx->yh - cubtx->yl);
   zs = (pz - cubtx->zl) / (cubtx->zh - cubtx->zl);

   if (xs < 0.0) xs = 0.0;
   else
   if (xs > 1.0) xs = 1.0;

   if (ys < 0.0) ys = 0.0;
   else
   if (ys > 1.0) ys = 1.0;

   if (zs < 0.0) zs = 0.0;
   else
   if (zs > 1.0) zs = 1.0;

   cr = xs * 255.0;
   cg = ys * 255.0;
   cb = zs * 255.0;

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID colrtxt18(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Slabs *slatx;

   register FLOAT n, d, t, l;

   SHORT cr, cg, cb;
   FLOAT tx, ty, tz, dx, dy, dz;
   FLOAT t1, t2, t3, t4, t5, t6, t7;

   slatx = crmsg->slatx;

   slatx = &slatx[w];
/*
   px = px * slatx->scale;
   py = py * slatx->scale;
   pz = pz * slatx->scale;
*/
   tx = slatx->nx * (px - 0.0);
   ty = slatx->ny * (py - 0.0);
   tz = slatx->nz * (pz - 0.0);

   n = tx + ty + tz;

   tx = slatx->nx * slatx->nx;
   ty = slatx->ny * slatx->ny;
   tz = slatx->nz * slatx->nz;

   d = tx + ty + tz;

   t = -(n / d);

   tx = px + slatx->nx * t;
   ty = py + slatx->ny * t;
   tz = pz + slatx->nz * t;

   dx = tx - px;
   dy = ty - py;
   dz = tz - pz;

   l = sqrt(dx * dx + dy * dy + dz * dz);

   l += slatx->start;

   t1 = 0.0;
   t2 = slatx->t12;
   t3 = slatx->t23;
   t4 = slatx->t34;
   t5 = slatx->t45;
   t6 = slatx->t56;
   t7 = slatx->t61;

   while (l > t7) l -= (t7 - t1);

   if (l >= t1 && l <= t2) {
      cr = slatx->r1;
      cg = slatx->g1;
      cb = slatx->b1;
   } else

   if (l >= t2 && l <= t3) {
      cr = slatx->r2;
      cg = slatx->g2;
      cb = slatx->b2;
   } else

   if (l >= t3 && l <= t4) {
      cr = slatx->r3;
      cg = slatx->g3;
      cb = slatx->b3;
   } else

   if (l >= t4 && l <= t5) {
      cr = slatx->r4;
      cg = slatx->g4;
      cb = slatx->b4;
   } else

   if (l >= t5 && l <= t6) {
      cr = slatx->r5;
      cg = slatx->g5;
      cb = slatx->b5;
   } else

   if (l >= t6 && l <= t7) {
      cr = slatx->r6;
      cg = slatx->g6;
      cb = slatx->b6;
   }

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID colrtxt19(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Spotted *spttx;

   register SHORT i;
   register FLOAT n;

   SHORT rs, gs, bs, re, ge, be, cr, cg, cb;

   spttx = crmsg->spttx;

   spttx = &spttx[w];

   px = px * spttx->scale;
   py = py * spttx->scale;
   pz = pz * spttx->scale;

   n = noise(px, py, pz) * spttx->space;

   for (i = 0; i < spttx->order; i++) n = n * n;

   rs = spttx->br;
   gs = spttx->bg;
   bs = spttx->bb;

   re = spttx->sr;
   ge = spttx->sg;
   be = spttx->sb;

   cr = rs + (re - rs) * n;
   cg = gs + (ge - gs) * n;
   cb = bs + (be - bs) * n;

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID colrtxt20(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Swirls *switx;

   register FLOAT n;

   SHORT rs, gs, bs, re, ge, be, cr, cg, cb;
   FLOAT dx, dy, dz, t1, t2, t3, t4;

   switx = crmsg->switx;

   switx = &switx[w];

   px = px * switx->scale;
   py = py * switx->scale;
   pz = pz * switx->scale;

   dturbulence(&dx, &dy, &dz, px, py, pz);

   px = px + dx * switx->turbscale;
   py = py + dy * switx->turbscale;
   pz = pz + dz * switx->turbscale;

   n = noise(px, py, pz);

   t1 = 0.0;
   t2 = switx->t1;
   t3 = switx->t2;
   t4 = switx->t3;

   if (n >= t1 && n <= t2) {
      rs = switx->r1;
      gs = switx->g1;
      bs = switx->b1;

      re = switx->r2;
      ge = switx->g2;
      be = switx->b2;

      cr = rs + (re - rs) * n;
      cg = gs + (ge - gs) * n;
      cb = bs + (be - bs) * n;
   } else

   if (n >= t2 && n <= t3) {
      rs = switx->r2;
      gs = switx->g2;
      bs = switx->b2;

      re = switx->r3;
      ge = switx->g3;
      be = switx->b3;

      cr = rs + (re - rs) * n;
      cg = gs + (ge - gs) * n;
      cb = bs + (be - bs) * n;
   } else

   if (n >= t3 && n <= t4) {
      rs = switx->r3;
      gs = switx->g3;
      bs = switx->b3;

      re = switx->r4;
      ge = switx->g4;
      be = switx->b4;

      cr = rs + (re - rs) * n;
      cg = gs + (ge - gs) * n;
      cb = bs + (be - bs) * n;
   }

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID colrtxt21(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Wood2 *wo2tx;

   register SHORT i;
   register FLOAT t;

   SHORT rs, gs, bs, re, ge, be, cr, cg, cb;
   FLOAT sx, sy, sz;

   wo2tx = crmsg->wo2tx;

   wo2tx = &wo2tx[w];

   sx = px * wo2tx->scale;
   sy = py * wo2tx->scale;
   sz = pz * wo2tx->scale;

   t = sqrt(px * px + pz * pz);

   t = t * 0.03;

   t = t + turbulence(sx, sy, sz) * wo2tx->turbscale;

   t = (sin(t) + 1.0) / 2.0;

   for (i = 0; i < wo2tx->order; i++) t = t * t;

   t = t * 1.0;

   if (t < 0.0) t = 0.0;
   else
   if (t > 1.0) t = 1.0;

   rs = wo2tx->wr;
   gs = wo2tx->wg;
   bs = wo2tx->wb;

   re = wo2tx->rr;
   ge = wo2tx->rg;
   be = wo2tx->rb;

   cr = rs + (re - rs) * t;
   cg = gs + (ge - gs) * t;
   cb = bs + (be - bs) * t;

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID colrtxt22(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   register struct Wood3 *wo3tx;

   register FLOAT fx, fz, n, s;

   SHORT cr, cg, cb, rs, gs, bs, re, ge, be;
   FLOAT dx, dy, dz, t1, t2, t3, t4;

   wo3tx = crmsg->wo3tx;

   wo3tx = &wo3tx[w];

   px = px * wo3tx->scale;
   py = py * wo3tx->scale;
   pz = pz * wo3tx->scale;

   dturbulence(&dx, &dy, &dz, px, py, pz);

   fx = cycloidal((px + dx) * wo3tx->turbscale);
   fz = cycloidal((pz + dz) * wo3tx->turbscale);

   fx = fx + px;
   fz = fz + pz;

   n = sqrt(fx * fx + fz * fz);

   n = trianglewave(n);

   t1 = 0.0;
   t2 = wo3tx->t1;
   t3 = wo3tx->t2;
   t4 = wo3tx->t3;

   if (n < t1) {
      cr = wo3tx->r1;
      cg = wo3tx->g1;
      cb = wo3tx->b1;
   } else

   if (n >= t1 && n <= t2) {
      s = (n - t1) / (t2 - t1);

      rs = wo3tx->r1;
      gs = wo3tx->g1;
      bs = wo3tx->b1;

      re = wo3tx->r2;
      ge = wo3tx->g2;
      be = wo3tx->b2;

      cr = rs + (re - rs) * s;
      cg = gs + (ge - gs) * s;
      cb = bs + (be - bs) * s;
   } else

   if (n >= t2 && n <= t3) {
      s = (n - t2) / (t3 - t2);

      rs = wo3tx->r2;
      gs = wo3tx->g2;
      bs = wo3tx->b2;

      re = wo3tx->r3;
      ge = wo3tx->g3;
      be = wo3tx->b3;

      cr = rs + (re - rs) * s;
      cg = gs + (ge - gs) * s;
      cb = bs + (be - bs) * s;
   } else

   if (n >= t3 && n <= t4) {
      s = (n - t3) / (t4 - t3);

      rs = wo3tx->r3;
      gs = wo3tx->g3;
      bs = wo3tx->b3;

      re = wo3tx->r4;
      ge = wo3tx->g4;
      be = wo3tx->b4;

      cr = rs + (re - rs) * s;
      cg = gs + (ge - gs) * s;
      cb = bs + (be - bs) * s;
   } else

   if (n > t4) {
      cr = wo3tx->r4;
      cg = wo3tx->g4;
      cb = wo3tx->b4;
   }

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID colrtxt0(r, g, b, px, py, pz, w)
UBYTE *r, *g, *b;
FLOAT px, py, pz;
SHORT w;
{
   struct MsgPort *port;

   port = FindPort("COLOR_TEXTURE");

   if (port == NULL) return;

   ctmsg.msg.mn_ReplyPort = ctrport;
   ctmsg.msg.mn_Length = sizeof(struct CustomTexture);

   ctmsg.px = px;
   ctmsg.py = py;
   ctmsg.pz = pz;

   ctmsg.cr = *r;
   ctmsg.cg = *g;
   ctmsg.cb = *b;

   PutMsg(port, &ctmsg);

   WaitPort(ctrport);

   GetMsg(ctrport);

   *r = ctmsg.cr;
   *g = ctmsg.cg;
   *b = ctmsg.cb;

   return;
}

VOID posttxt1(r, g, b, nx, ny, nz, w)
UBYTE *r, *g, *b;
FLOAT nx, ny, nz;
SHORT w;
{
   register struct Snowy *snotx;

   register FLOAT dot, bnd, thr;

   snotx = crmsg->snotx;

   snotx = &snotx[w];

   dot = nx * snotx->dx + ny * snotx->dy + nz * snotx->dz;

   if (dot > 0.0) return;

   bnd = snotx->stickiness - 1.0;

   if (dot <= bnd) {
      *r = snotx->cr;
      *g = snotx->cg;
      *b = snotx->cb;

      return;
   }

   thr = getrnd() * snotx->depth;

   if (thr > dot - bnd) {
      *r = snotx->cr;
      *g = snotx->cg;
      *b = snotx->cb;
   }

   return;
}

VOID posttxt2(r, g, b, nx, ny, nz, w)
UBYTE *r, *g, *b;
FLOAT nx, ny, nz;
SHORT w;
{
   register struct Fruity *frutx;

   register FLOAT scl, amt;

   SHORT rs, gs, bs, re, ge, be;
   FLOAT cr, cg, cb;

   frutx = crmsg->frutx;

   frutx = &frutx[w];

   cr = cg = cb = 0.0;

   scl = (nx + 1.0) / 2.0;

   amt = fabs(nx) * 0.75;

   rs = frutx->r1;
   gs = frutx->g1;
   bs = frutx->b1;

   re = frutx->r2;
   ge = frutx->g2;
   be = frutx->b2;

   cr += (rs + (re - rs) * scl) * amt;
   cg += (gs + (ge - gs) * scl) * amt;
   cb += (bs + (be - bs) * scl) * amt;

   scl = (ny + 1.0) / 2.0;

   amt = fabs(ny) * 0.75;

   rs = frutx->r3;
   gs = frutx->g3;
   bs = frutx->b3;

   re = frutx->r4;
   ge = frutx->g4;
   be = frutx->b4;

   cr += (rs + (re - rs) * scl) * amt;
   cg += (gs + (ge - gs) * scl) * amt;
   cb += (bs + (be - bs) * scl) * amt;

   scl = (nz + 1.0) / 2.0;

   amt = fabs(nz) * 0.75;

   rs = frutx->r5;
   gs = frutx->g5;
   bs = frutx->b5;

   re = frutx->r6;
   ge = frutx->g6;
   be = frutx->b6;

   cr += (rs + (re - rs) * scl) * amt;
   cg += (gs + (ge - gs) * scl) * amt;
   cb += (bs + (be - bs) * scl) * amt;

   if (cr > 255.0) cr = 255.0;
   if (cg > 255.0) cg = 255.0;
   if (cb > 255.0) cb = 255.0;

   *r = cr;
   *g = cg;
   *b = cb;

   return;
}

VOID posttxt3(r, g, b, nx, ny, nz, w)
UBYTE *r, *g, *b;
FLOAT nx, ny, nz;
SHORT w;
{
   register struct Glow *glotx;

   register FLOAT dx, dy, dz, ln;

   FLOAT dot, cr, cg, cb;

   glotx = crmsg->glotx;

   glotx = &glotx[w];

   dx = glotx->xp - 0.0;
   dy = glotx->yp - 0.0;
   dz = glotx->zp - 0.0;

   ln = sqrt(dx * dx + dy * dy + dz * dz);

   if (ln > 1.0) {
      dx /= ln;
      dy /= ln;
      dz /= ln;
   }
   else {
      dx = 0.0;
      dy = 0.0;
      dz = 0.0;
   }

   dot = dx * nx + dy * ny + dz * nz;

   if (dot >= 0.0 && dot <= 1.0) {
      cr = *r;
      cg = *g;
      cb = *b;

      cr = cr + (glotx->gr - cr) * dot;
      cg = cg + (glotx->gg - cg) * dot;
      cb = cb + (glotx->gb - cb) * dot;

      *r = cr;
      *g = cg;
      *b = cb;
   }

   return;
}

VOID posttxt0(r, g, b, nx, ny, nz, w)
UBYTE *r, *g, *b;
FLOAT nx, ny, nz;
SHORT w;
{
   struct MsgPort *port;

   port = FindPort("POST_TEXTURE");

   if (port == NULL) return;

   ctmsg.msg.mn_ReplyPort = ctrport;
   ctmsg.msg.mn_Length = sizeof(struct CustomTexture);

   ctmsg.nx = nx;
   ctmsg.ny = ny;
   ctmsg.nz = nz;

   ctmsg.cr = *r;
   ctmsg.cg = *g;
   ctmsg.cb = *b;

   PutMsg(port, &ctmsg);

   WaitPort(ctrport);

   GetMsg(ctrport);

   *r = ctmsg.cr;
   *g = ctmsg.cg;
   *b = ctmsg.cb;

   return;
}

VOID normalizetexture(p, px, py, pz)
struct Primitive *p;
FLOAT *px, *py, *pz;
{
   register FLOAT sr, ta, tb;

   struct Object *obj;

   FLOAT hx, hy, hz, vx, vy, vz, nx, ny, nz;
   FLOAT sx, cx, sy, cy, sz, cz, xu, yu, ru;

   if (p->obj < 0) return;

   obj = crmsg->objects;

   obj = &obj[p->obj];

   if (!(obj->flags & LINKTXT)) return;

   *px -= obj->ox;
   *py -= obj->oy;
   *pz -= obj->oz;

   hx = obj->hx;
   hy = obj->hy;
   hz = obj->hz;

   vx = obj->vx;
   vy = obj->vy;
   vz = obj->vz;

   nx = hy * vz - vy * hz;
   ny = hz * vx - vz * hx;
   nz = hx * vy - vx * hy;

   unitvector(&nx, &ny, &nz);

   sx = 0.0;
   cx = 1.0;

   sr = sqrt(ny * ny + nz * nz);

   if (sr < FERR) goto skipdiv1;

   sx = -ny / sr;
   cx = -nz / sr;

   skipdiv1:

   sy = -nx;
   cy = sr;

   xu = vx * cy + vy * -sx * sy + vz * -cx * sy;

   yu = vy * cx + vz * -sx;

   ru = sqrt(xu * xu + yu * yu);

   if (ru < FERR) return;

   sz = xu / ru;
   cz = yu / ru;

   if (sr < FERR) goto skipdiv2;

   ta = *py * cx - *pz * sx;
   tb = *py * sx + *pz * cx;

   *py = ta;
   *pz = tb;

   skipdiv2:

   ta = *px * cy - *pz * sy;
   tb = *px * sy + *pz * cy;

   *px = ta;
   *pz = tb;

   ta = *px * cz - *py * sz;
   tb = *px * sz + *py * cz;

   *px = ta;
   *py = tb;

   return;
}

VOID calccolrtxt(p, px, py, pz, cr, cg, cb)
struct Primitive *p;
FLOAT px, py, pz, *cr, *cg, *cb;
{
   register LONG flg;

   UBYTE r, g, b;

   SHORT tn;
   FLOAT rx, ry, rz;

   flg = p->ctexture;

   tn = p->tnum;

   rx = px;
   ry = py;
   rz = pz;

   reversetransform(&rx, &ry, &rz, 0);

   normalizetexture(p, &rx, &ry, &rz);

   r = *cr * 255.0;
   g = *cg * 255.0;
   b = *cb * 255.0;

   if (flg == SPOTTY) {
      colrtxt1(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == MARBLE) {
      colrtxt2(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == WOOD) {
      colrtxt3(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == BRICK) {
      colrtxt4(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == CHECKER) {
      colrtxt5(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == BLEND) {
      colrtxt6(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == TERRAIN) {
      colrtxt7(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == BOZO) {
      colrtxt8(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == CLOUDY) {
      colrtxt9(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == DOTS) {
      colrtxt10(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == FIRE1) {
      colrtxt11(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == FIRE2) {
      colrtxt12(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == GRANITE) {
      colrtxt13(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == GRID) {
      colrtxt14(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == MARBLE2) {
      colrtxt15(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == MARBLE3) {
      colrtxt16(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == RGBCUBE) {
      colrtxt17(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == SLABS) {
      colrtxt18(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == SPOTTED) {
      colrtxt19(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == SWIRLS) {
      colrtxt20(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == WOOD2) {
      colrtxt21(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == WOOD3) {
      colrtxt22(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == CUSTTXT) {
      colrtxt0(&r, &g, &b, rx, ry, rz, tn);
   }

   *cr = r * INVSHPG;
   *cg = g * INVSHPG;
   *cb = b * INVSHPG;

   return;
}

VOID calcnormtxt(p, px, py, pz, nx, ny, nz)
struct Primitive *p;
FLOAT px, py, pz, *nx, *ny, *nz;
{
   register LONG flg;

   SHORT tn;
   FLOAT rx, ry, rz;

   flg = p->ntexture;

   tn = p->tnum;

   rx = px;
   ry = py;
   rz = pz;

   reversetransform(&rx, &ry, &rz, 0);

   normalizetexture(p, &rx, &ry, &rz);

   *nx = 0.0;
   *ny = 0.0;
   *nz = 0.0;

   if (flg == RIPPLE) {
      if (tn > 0) tn += NUMRIPTX - 1;

      normtxt1(rx, ry, rz, nx, ny, nz, tn);
   } else

   if (flg == BUMPY) {
      normtxt2(rx, ry, rz, nx, ny, nz, tn);
   } else

   if (flg == ROUGH) {
      normtxt3(rx, ry, rz, nx, ny, nz, tn);
   } else

   if (flg == BUMPS) {
      normtxt4(rx, ry, rz, nx, ny, nz, tn);
   } else

   if (flg == DENTED) {
      normtxt5(rx, ry, rz, nx, ny, nz, tn);
   } else

   if (flg == ERODED) {
      normtxt6(rx, ry, rz, nx, ny, nz, tn);
   } else

   if (flg == STUCCO) {
      normtxt7(rx, ry, rz, nx, ny, nz, tn);
   } else

   if (flg == WAVY) {
      normtxt8(rx, ry, rz, nx, ny, nz, tn);
   } else

   if (flg == WRINKLE) {
      normtxt9(rx, ry, rz, nx, ny, nz, tn);
   } else

   if (flg == CUSTTXT) {
      normtxt0(rx, ry, rz, nx, ny, nz, tn);
   }

   return;
}

VOID calcposttxt(p, nx, ny, nz, cr, cg, cb)
struct Primitive *p;
FLOAT nx, ny, nz, *cr, *cg, *cb;
{
   register LONG flg;

   UBYTE r, g, b;

   SHORT tn;
   FLOAT rx, ry, rz;

   flg = p->ptexture;

   tn = p->tnum;

   rx = nx;
   ry = ny;
   rz = nz;

   reversetransform(&rx, &ry, &rz, 1);

   r = *cr * 255.0;
   g = *cg * 255.0;
   b = *cb * 255.0;

   if (flg == SNOWY) {
      posttxt1(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == FRUITY) {
      posttxt2(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == GLOW) {
      posttxt3(&r, &g, &b, rx, ry, rz, tn);
   } else

   if (flg == CUSTTXT) {
      posttxt0(&r, &g, &b, rx, ry, rz, tn);
   }

   *cr = r * INVSHPG;
   *cg = g * INVSHPG;
   *cb = b * INVSHPG;

   return;
}

