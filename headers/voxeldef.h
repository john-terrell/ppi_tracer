#ifndef INCLUDE_VOXELDEF_H
#define INCLUDE_VOXELDEF_H

struct Voxel {

  struct Min_Node vox_Node;

  struct Min_List VoxelContents;  /* What this voxel contains. More voxels or primitives */
  struct Voxel *Neighbors[6]; /* direction cube for this voxel */


  /*
  NOTE:

    Neighbors pointers are ordered as follows:
    East  (Pos X)
    West  (Neg X)
    Up    (Pos Y)
    Down  (Neg Y)
    North (Pos Z)
    South (Neg Z)
  */

  float bounds[6];            /* mins first, then maxs */

  long PrimCount;             /* Primitive count for this voxel */
  long ID;                    /* ID of this voxel */
  long Level;                 /* Level of this voxel */

  short hitsides[2];          /* Voxel Hit Info for Last ray. */
  float hitparms[2];

};

/*

  NOTE:

  If PrimCount != -1, PrimList holds struct VoxObject, else it holds
  struct Voxel.

*/

/*
** VoxObject holds pointers to database primitives
*/
struct VoxObject {

  struct Min_Node vxo_Node;           /* Node on vox_PrimList below */

  struct Object
    *vxo_Obj;                     /* Pointer to an object that owns below primitive */

  struct Primitive
    *vxo_Prim;                    /* Pointer to a primitive */

};

/*
** Macro used to get a subvoxel by its ID value.
*/
#define MGETSUBVOX(v,id) (&((struct Voxel *)v->VoxelContents.lh_Head)[id])

/*
** Voxel bounds index values
*/
#define MINX 0
#define MINY 1
#define MINZ 2
#define MAXX 3
#define MAXY 4
#define MAXZ 5

/*
** Voxel neighbor index values
*/
#define EAST  0
#define WEST  1
#define UP    2
#define DOWN  3
#define NORTH 4
#define SOUTH 5

/*
** Subvoxel ordering index values
*/
#define UPPER_NORTHWEST   0
#define UPPER_NORTHEAST   1
#define UPPER_SOUTHEAST   2
#define UPPER_SOUTHWEST   3

#define LOWER_NORTHWEST   4
#define LOWER_NORTHEAST   5
#define LOWER_SOUTHEAST   6
#define LOWER_SOUTHWEST   7

/*
** Voxel Corner Identifiers.
*/
#define UPPERNORTHWEST    (100 + ((1 << MAXY) | (1 << MAXZ) | (1 << MINX)))
#define UPPERNORTHEAST    (100 + ((1 << MAXY) | (1 << MAXZ) | (1 << MAXX)))
#define UPPERSOUTHWEST    (100 + ((1 << MAXY) | (1 << MINZ) | (1 << MINX)))
#define UPPERSOUTHEAST    (100 + ((1 << MAXY) | (1 << MINZ) | (1 << MAXX)))

#define LOWERNORTHWEST    (100 + ((1 << MINY) | (1 << MAXZ) | (1 << MINX)))
#define LOWERNORTHEAST    (100 + ((1 << MINY) | (1 << MAXZ) | (1 << MAXX)))
#define LOWERSOUTHWEST    (100 + ((1 << MINY) | (1 << MINZ) | (1 << MINX)))
#define LOWERSOUTHEAST    (100 + ((1 << MINY) | (1 << MINZ) | (1 << MAXX)))

/*
** Voxel Edge Identifiers.
*/
#define NORTHEAST         (100 + ((1 << MAXZ) | (1 << MAXX)))
#define NORTHWEST         (100 + ((1 << MAXZ) | (1 << MINX)))
#define SOUTHEAST         (100 + ((1 << MINZ) | (1 << MAXX)))
#define SOUTHWEST         (100 + ((1 << MINZ) | (1 << MINX)))

#define WESTUP            (100 + ((1 << MAXY) | (1 << MINX)))
#define WESTDOWN          (100 + ((1 << MINY) | (1 << MINX)))
#define EASTUP            (100 + ((1 << MAXY) | (1 << MAXX)))
#define EASTDOWN          (100 + ((1 << MINY) | (1 << MAXX)))

#define NORTHUP           (100 + ((1 << MAXY) | (1 << MAXZ)))
#define NORTHDOWN         (100 + ((1 << MINY) | (1 << MAXZ)))
#define SOUTHUP           (100 + ((1 << MAXY) | (1 << MINZ)))
#define SOUTHDOWN         (100 + ((1 << MINY) | (1 << MINZ)))

#endif /* INCLUDE_VOXELDEF_H */
