#ifndef INCLUDE_MAPPINGDEF_H
#define INCLUDE_MAPPINGDEF_H

struct ImageBuffer {

  struct Min_Node im_Node;
  char im_Name[16];

  /*
  ** Dimensions in pixels.
  */
  long im_xsize;
  long im_ysize;

  /*
  ** Frame buffer data.
  */
  unsigned char *im_RedBuffer;
  unsigned char *im_GrnBuffer;
  unsigned char *im_BluBuffer;

};

struct Map {

  struct Min_Node map_node;

  short map_type;
  short map_flags;

  struct ImageBuffer *map_image;

};

#define MAPTYPE_FLAT      0
#define MAPTYPE_SPHERICAL 1
#define MAPTYPE_CYLINDER  2

#define MAPFLAG_REPEAT    (1 << 0)

struct FlatMap {

  struct Map header;

  /* map corner coordinates */
  float fm_c1[3];   /* Upper left, proceeding clockwise */
  float fm_c2[3];
  float fm_c3[3];
  float fm_c4[3];

  float fm_origin[3];
  float fm_normal[3];

};

struct SphericalMap {

  struct Map header;

  float sm_Center[3];

  /* Normalized Vectors */
  float sm_Sp[3];      /* Vector from sphere center to "North Pole" */
  float sm_Se[3];      /* Vector form sphere center to reference point on "Equator" */

};

#endif /* INCLUDE_MAPPINGDEF_H */
