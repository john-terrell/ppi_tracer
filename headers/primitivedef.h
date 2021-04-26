#ifndef INCLUDE_PRIMITIVEDEF_H
#define INCLUDE_PRIMITIVEDEF_H

/*
** Object structure that contains a list of primitives.
*/
struct Object {

  struct Full_Node obj_Node;
  char obj_Name[16];

  /*
  ** Matrix to multiply a world coord by to apply a texture
  */
  struct Matrix44 *obj_World2Texture;

  /*
  ** Matrix to multiply an object by to get its position.
  */
  struct Matrix44 *obj_Local2World;

  struct Min_List obj_PrimitiveList;    /* Primitive list. */
  struct Min_List obj_VertexList;       /* Vertex list. */

  short obj_Flags;

  float obj_SmoothingTension;       /* InterPhong smoothing tension. */

};

/*
** obj_Flags bit values
*/
#define OFLG_INSTANCE   (1 << 15)   /* This object is an instance */
#define OFLG_NOSMOOTH   (1 << 14)   /* Do not smooth this object */
#define OFLG_NORENDER   (1 << 12)   /* Do not render this object */

/*
** Standard primitive structure.
** This structure exists on both the object's primitive list and on any number
** of voxel lists.
*/
struct Primitive {

  struct Min_Node prm_Node;

  short prm_Flags;                /* primitive flags.  see below */

  void *prm_Attribute;
  void *prm_Object;               /* The object that owns this primitive */

};

/*
** Standard primitive type identifiers (lower eight bits of prm_Flags.)
*/
#define GET_PRIMTYPE(x) (x & 0xff)
#define PTYPE_TRIANGLE    0x00
#define PTYPE_SPHERE      0x01
#define PTYPE_PLANE       0x02

/*
** Primitive flag bit values (upper eight bits of prm_Flags.)
*/
#define PFLAG_GOODNORMAL  (1 << 15)
#define PFLAG_ALWAYSTEST  (1 << 10)  /* Put this guy on the always test list.*/
#define PFLAG_NOEXTENTS   (1 << 9)   /* Don't try to get extents on this primitive */
#define PFLAG_NORENDER    (1 << 8)   /* Don't render this primitive. */

/* PRIMITIVE STRUCTURES */

struct prim_Triangle {        /* 48 bytes long */

  struct Primitive tri_Prim;  /* 22 bytes */

  float *tri_Vert1;           /* 12 bytes */
  float *tri_Vert2;
  float *tri_Vert3;

  float tri_Normal[3];        /* 12 bytes */
  
  char tri_DominantAxis1;     /*  2 bytes */
  char tri_DominantAxis2;

};

/* Triangle vertex structure */
struct tvertex {

  float p[3]; /* Position */
  float n[3]; /* Normal   */

};

struct VertexNode {

  struct Min_Node vn_Node;
  struct tvertex vn_tvertex;

};

struct prim_Sphere {

  struct Primitive sph_Prim;
  float sph_origin[3];            /* sphere origin */
  float sph_radius;               /* sphere's true radius */
  float sph_rsquared;             /* squared radius (radius * radius) */

};

struct prim_Plane {

  struct Primitive pln_Prim;
  float pln_origin[3];            /* plane origin */
  float pln_normal[3];            /* plane normal */

};

#endif /* INCLUDE_PRIMITIVEDEF_H */
