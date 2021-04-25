/*
** Math.c
**
** Three dimensional math routines.
**
** 17 Aug 1992 - Created          - J. Terrell
*/
#include "headers/tracer.h"

/* ray_reflect() ; finds the reflection vector */
void ray_reflect(float *V,float *N,float *R)
{

  float Vdotn;

  Vdotn = MDOT(V,N);

  R[0] = V[0] - (2.0 * Vdotn * N[0]);
  R[1] = V[1] - (2.0 * Vdotn * N[1]);
  R[2] = V[2] - (2.0 * Vdotn * N[2]);

}

short ray_refract(float *I,float *N,float *R,float eta)
{

  float n_dot_i;
  float root_arg;
  float l;

  n_dot_i = MNEGDOT(I,N);

  root_arg = 1.0 - ((eta * eta) * ((1.0 - (n_dot_i * n_dot_i))));

  /*
  ** Check for total internal reflection.  Precision check at end.
  */
  if(root_arg < 0.0) {

    R[0] = 0.0;
    R[1] = 0.0;
    R[2] = 0.0;

    return(1);

  }

  root_arg = (eta * n_dot_i) - sqrt(root_arg);

  R[0] = (root_arg * N[0]) - (eta * -I[0]);
  R[1] = (root_arg * N[1]) - (eta * -I[1]);
  R[2] = (root_arg * N[2]) - (eta * -I[2]);

  /*
  ** Normalize the refraction vector.
  */
  l = 1.0 / MLENGTH(R);

  R[0] *= l;
  R[1] *= l;
  R[2] *= l;

  return(0);

}

/* set_length() ; sets the given vector to the given length. */
void set_length(float *V,float len)
{

  float olen;
  float scaler;

  olen = MLENGTH(V);
  scaler = len/olen;

  V[0] *= scaler;
  V[1] *= scaler;
  V[2] *= scaler;

}

/* normalize() ; normalize the given vector */
float normalize(float *V)
{

  float l,d;

  d = MLENGTH(V);

  l = 1.0 / d;

  V[0] *= l;
  V[1] *= l;
  V[2] *= l;

  return(d);

}

/* mix() ; finds an interpolated color value */
void mix(float *color0,float *color1,float value,float *colorout)
{

  float one_minus_value;

  one_minus_value = 1.0 - value;

  colorout[0] = (one_minus_value * color0[0]) + (value * color1[0]);
  colorout[1] = (one_minus_value * color0[1]) + (value * color1[1]);
  colorout[2] = (one_minus_value * color0[2]) + (value * color1[2]);

}

void identity(struct Matrix44 *m)
{

  m->e[0][0] = 1.0; m->e[0][1] = 0.0; m->e[0][2] = 0.0; m->e[0][3] = 0.0;
  m->e[1][0] = 0.0; m->e[1][1] = 1.0; m->e[1][2] = 0.0; m->e[1][3] = 0.0;
  m->e[2][0] = 0.0; m->e[2][1] = 0.0; m->e[2][2] = 1.0; m->e[2][3] = 0.0;
  m->e[3][0] = 0.0; m->e[3][1] = 0.0; m->e[3][2] = 0.0; m->e[3][3] = 1.0;

}

void addtranslation(struct Matrix44 *m,float xlate[])
{

  struct Matrix44 t;

  identity(&t);

  t.e[3][0] = xlate[0];
  t.e[3][1] = xlate[1];
  t.e[3][2] = xlate[2];

  multiply(m,&t);

}

void multiply(struct Matrix44 *trafo,struct Matrix44 *m)
{

  struct Matrix44 trafo1;
  short i,j,k;

  trafo1 = *trafo;

  for(i = 0; i < 4; i++) {

    for(j = 0; j < 4; j++) {

      trafo->e[i][j] = 0.0;
      for(k = 0; k < 4; k++) {

        trafo->e[i][j] += trafo1.e[i][k] * m->e[k][j];

      }

    }

  }

}

void copymatrix(struct Matrix44 *trafo,struct Matrix44 *m)
{

  short i,j;

  for(i = 0; i < 4; i++) {

    for(j = 0; j < 4; j++) {

      m->e[i][j] = trafo->e[i][j];

    }

  }

}

void vertmult(float *pin,float *pout,struct Matrix44 *m)
{

  float w;

  pout[0] = (pin[0] * m->e[0][0]) + (pin[1] * m->e[1][0]) + (pin[2] * m->e[2][0]) + m->e[3][0];

  pout[1] = (pin[0] * m->e[0][1]) + (pin[1] * m->e[1][1]) + (pin[2] * m->e[2][1]) + m->e[3][1];

  pout[2] = (pin[0] * m->e[0][2]) + (pin[1] * m->e[1][2]) + (pin[2] * m->e[2][2]) + m->e[3][2];

  w       = (pin[0] * m->e[0][3]) + (pin[1] * m->e[1][3]) + (pin[2] * m->e[2][3]) + m->e[3][3];

  if(w != 0.0) {

    pout[0] /= w;
    pout[1] /= w;
    pout[2] /= w;

  }
}

/* rotatearbitrary(); Rotate a point v, around an axis with origin by amt rads */
void rotatearbitrary(float *v, float *axis, float *origin, float amt)
{

   float u, l, dx, dy, dz, c, s, t;
   float cntx, cnty, cntz, x, y, z;

   dx = axis[0];
   dy = axis[1];
   dz = axis[2];

   if(origin) {

    cntx = origin[0];
    cnty = origin[1];
    cntz = origin[2];

   }
   else {

    cntx = 0.0;
    cnty = 0.0;
    cntz = 0.0;

   }

   x = v[0] - cntx;
   y = v[1] - cnty;
   z = v[2] - cntz;

   u = sqrt(dy * dy + dz * dz);
   l = sqrt(dx * dx + dy * dy + dz * dz);

   if (u > 0.0) {
      c = dz / u;
      s = dy / u;

      t = y * c - z * s;
      z = y * s + z * c;
      y = t;
   }

   if (l >= 0.0) {
      c = u / l;
      s = dx / l;

      t = x * c - z * s;
      z = x * s + z * c;
      x = t;
   }

   c = cos(amt);
   s = sin(amt);

   t = x * c - y * s;
   y = x * s + y * c;
   x = t;

   if (l >= 0.0) {

      c = u / l;
      s = dx / l;

      t = x * c + z * s;
      z = x * -s + z * c;
      x = t;

   }

   if (u >= 0.0) {

      c = dz / u;
      s = dy / u;

      t = y * c + z * s;
      z = y * -s + z * c;
      y = t;

   }

   v[0] = x + cntx;
   v[1] = y + cnty;
   v[2] = z + cntz;

}

void perturbnormal(float *norm,float am)
{

  float sc;

  sc = MGETRND * am;

  norm[0] += sc * ((2.0 * MGETRND) - 1.0);
  norm[1] += sc * ((2.0 * MGETRND) - 1.0);
  norm[2] += sc * ((2.0 * MGETRND) - 1.0);

  normalize(norm);

}

BOOL check_pointcontain(float pnt[],float bounds[])
{

  if(pnt[0] < bounds[MINX]) return(FALSE);
  if(pnt[0] > bounds[MAXX]) return(FALSE);
  if(pnt[1] < bounds[MINY]) return(FALSE);
  if(pnt[1] > bounds[MAXY]) return(FALSE);
  if(pnt[2] < bounds[MINZ]) return(FALSE);
  if(pnt[2] > bounds[MAXZ]) return(FALSE);

  return(TRUE);

}

/*
** Returns fresnel reflectance curve
*/
float fresnel(float nt,float ni,float *V,float *N,float *R)
{

  float thetai,thetat,eta;
  float F,r1,r2,p1,p2;
  float val,root_arg;

  thetai = MNEGDOT(V,N);
  eta = ni/nt;

  val =  1.0 - ( (eta * eta) * (1.0 - (thetai * thetai)) );
  if(val >= 0.0) {

    /*
    ** Make refraction vector
    */
    thetat = sqrt(val);

    if(R) {

      root_arg = (eta * thetai) - thetat;

      R[0] = (root_arg * N[0]) - (eta * -V[0]);
      R[1] = (root_arg * N[1]) - (eta * -V[1]);
      R[2] = (root_arg * N[2]) - (eta * -V[2]);

      normalize(R);

    }

    p1 = nt * thetai;
    p2 = ni * thetat;

    r1 = (p1 - p2)/(p1 + p2);

    p1 = ni * thetai;
    p2 = nt * thetat;

    r2 = (p1 - p2)/(p1 + p2);

    r1 *= r1;
    r2 *= r2;

    F = 0.5 * (r1 + r2);

  }
  else F = 1.0;

  return(F);

}
