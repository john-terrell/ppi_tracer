/*
** function_tables.c
**
** 04 Aug 1992 - Created    - J. Terrell
**
*/
#include "Headers/tracer.h"

/*
** Functions that determine if a ray intersects a primitive
*/
BOOL (*IsPrimitiveRayIntersect[])(struct VoxObject *,struct Ray *) = {

  tr_IsRayIntersect,
  sp_IsRayIntersect,
  pl_IsRayIntersect,

};

/*
** Functions that determine if a primitive intersects a voxel
*/
unsigned long (*IsPrimitiveVoxelIntersect[])() = {

  tr_IsVoxelIntersect,
  sp_IsVoxelIntersect,
  pl_IsVoxelIntersect,

};

/*
** Functions that return the extents of a primitive.
*/
void (*GetPrimitiveExtents[])() = {

  tr_GetExtents,
  sp_GetExtents,
  0L,

};

/*
** Functions that handle light sources.
*/
void (*LightSourceHandler[])() = {

  lsh_PointSourceHandler,
  lsh_SpotSourceHandler,
  lsh_DistantSourceHandler,
  lsh_SphericalSourceHandler,
  lsh_AmbientSourceHandler,

};
