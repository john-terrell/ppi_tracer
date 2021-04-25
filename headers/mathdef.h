#ifndef INCLUDE_MATHDEF_H
#define INCLUDE_MATHDEF_H

struct Ray {

  short flags;

  float o[3];             /* Origin */
  float d[3];             /* Direction */
  float rd[3];            /* Reciprocal of ray direction vector components. */
  float maxt;             /* Max value along the ray->t is allowed to be */
  float index;            /* Index of refraction of material this ray is traveling through */

  float ci[3];            /* Output color of ray */
  float norm[3];          /* Normal at intersection point. */

  float d_dot_norm;       /* Dot product of reversed ray->d and ray->norm */
  float t;                /* Ray parameter value where first intersection occurs. */
  float ip[3];            /* Intersection point of closest hit. */

  /*
  ** Surface parameters at ray intersection point.
  */
  float rz;               /* Calculated transmittance */
  float rs;               /* Calculated specular component. */

  struct VoxObject
    *vxo;                 /* Closest vxo hit by ray. */

  struct VoxObject
    *shootingvxo;         /* Voxel object that shot this ray. */

  struct Voxel
    *primvox;             /* Voxel the closest intersection was in */

  short depth;            /* This ray's depth. */
  short tdepth;           /* This ray's transparency depth */

};

#define RFLAG_NORMAL  (1 << 0)  /* The ->norm field is normalized */
#define RFLAG_NOHIT   (1 << 1)  /* No intersection at intersection point. */
                                /* Used for shading... */
struct Matrix44 {

  float e[4][4];

};

/* PI */
#ifndef PI
#define PI 3.1415926535897932384626433832795028841971693993751
#endif

#define PIDIV180 (PI / 180.0)

/* Conversion Macros */
#define MRADIANS(x) (PIDIV180 * x)

/* Dot Product Macro */
#define MDOT(x,y) ((x[0] * y[0]) + (x[1] * y[1]) + (x[2] * y[2]))

/* Dot Product Macro that reverses the first vector */
#define MNEGDOT(x,y) ((-x[0] * y[0]) + (-x[1] * y[1]) + (-x[2] * y[2]))

/* Cross Product Macro */
#define MCROSS(a,b,c) { \
  c[0] = (a[1] * b[2]) - (a[2] * b[1]); \
  c[1] = (a[2] * b[0]) - (a[0] * b[2]); \
  c[2] = (a[0] * b[1]) - (a[1] * b[0]); }

/* Min and Max finders */
#ifndef MAX
#define MAX(x,y) (x > y ? x : y)
#endif

#ifndef MIN
#define MIN(x,y) (x > y ? y : x)
#endif

#define MIN3(a,b,c) (((a < b) && (a < c)) ? a : ((b < c) ? b : c))
#define MAX3(a,b,c) (((a > b) && (a > c)) ? a : ((b > c) ? b : c))

/* Returns the index of the largest value, 0, 1 or 2 */
#define MAX3INDEX(a,b,c) ((a > b) && (a > c)) ? 0 : ((b > c) ? 1 : 2)


#define SUB(A,B,C) { \
  C[0] = A[0] - B[0]; \
  C[1] = A[1] - B[1]; \
  C[2] = A[2] - B[2]; } \

#define SIGN3(A) ((A[0] < 0) ? 4L:0L | (A[1] < 0) ? 2L:0L | (A[2] < 0) ? 1L:0L)

#define LERP(A,B,C) ((B + A) * (C - B))

/* Macro that finds a 3d position given a ray and a parameter */
#define MRAYPOS(o,d,t,p) { \
  p[0] = o[0] + (d[0] * t); \
  p[1] = o[1] + (d[1] * t); \
  p[2] = o[2] + (d[2] * t); }

/* Macro to swap two variables without temporary storage. */
#define MSWAP(a,b) { a ^=b; b ^= a; a ^= b; }

/* Macro that returns the length of a vector */
#define MLENGTH(V)  sqrt(MDOT(V,V))

/* Macro that clamps a value between bounds. */
#define MCLAMP(a,min,max) (a < min ? min : (a > max ? max : a))

/* Macro that gets a random number.  (Machine dependent.) */
#define MGETRND ((float)drand48())

/* Macro to set vector component values. */
#define MSETVEC(k,a,b,c) { k[0] = a; k[1] = b; k[2] = c; }

#define MSIGN(a) (a < 0 ? -1 : (a > 0 ? 1: 0))

#endif /* INCLUDE_MATHDEF_H */
