/*
** VoxelSmooth.c
**
** 20 Nov 1992 - Created    - J. Terrell
**
*/
#include "Headers/Tracer.h"

#define POP     (sptr++)
#define PUSH(x) ((*(--sptr)) = x)

short stack[256];
short *sptr;

void vs_SmoothVoxelList(struct Voxel *RootVoxel)
{

  struct Voxel *vox;

  sptr = &stack[255];

  if(RootVoxel->PrimCount != -1) return;

  vox = RootVoxel->VoxelContents.lh_Head;
  while(vox->vox_Node.ln_Succ) {

    PUSH(vox->ID);

    vs_SmoothVoxel(vox);

    POP;

    vox = vox->vox_Node.ln_Succ;

  }

}

void vs_SmoothVoxel(struct Voxel *vox)
{

  struct Voxel *n;
  short i,diff;

  for(i = 0; i < 6; i++) {

    n = vox->Neighbors[i];
    if(n) {

      /*
      ** See if the current neighbor needs smoothing.
      ** If the neighbor's level is less than the current one, and
      ** the neighbor has subvoxels, it needs to be smoothed.
      */
      if((n->Level < vox->Level) && (n->PrimCount == -1)) {

        diff = n->Level - vox->Level;

        vox->Neighbors[i] = vs_FindVoxelNeighbor(vox->Neighbors[i],i,diff);

      }

    }

  }

  /*
  ** If the current voxel has any subvoxels, recurse to smooth them.
  */
  if(vox->PrimCount == -1) {

    n = vox->VoxelContents.lh_Head;
    while(n->vox_Node.ln_Succ) {

      PUSH(n->ID);

      vs_SmoothVoxel(n);

      POP;

      n = n->vox_Node.ln_Succ;

    }

  }

}

struct Voxel *vs_FindVoxelNeighbor(struct Voxel *start,
  unsigned short voxdir,short leveldiff)
{

  unsigned short *ptr,dir;
  struct Voxel *currentvox;

  currentvox = start;
  ptr = (sptr + (leveldiff - 1));

  while(ptr >= sptr) {

    if(currentvox->PrimCount != -1) break;

    dir = *ptr--;

    switch(dir) {

      case UPPER_NORTHEAST:

        if(voxdir == EAST)  currentvox = MGETSUBVOX(currentvox,UPPER_NORTHWEST);
        else
        if(voxdir == NORTH) currentvox = MGETSUBVOX(currentvox,UPPER_SOUTHEAST);
        else
        if(voxdir == UP)    currentvox = MGETSUBVOX(currentvox,LOWER_NORTHEAST);
        break;

      case UPPER_NORTHWEST:

        if(voxdir == NORTH) currentvox = MGETSUBVOX(currentvox,UPPER_SOUTHWEST);
        else
        if(voxdir == WEST)  currentvox = MGETSUBVOX(currentvox,UPPER_NORTHEAST);
        else
        if(voxdir == UP)    currentvox = MGETSUBVOX(currentvox,LOWER_NORTHWEST);
        break;

      case UPPER_SOUTHEAST:

        if(voxdir == UP)    currentvox = MGETSUBVOX(currentvox,LOWER_SOUTHEAST);
        else
        if(voxdir == EAST)  currentvox = MGETSUBVOX(currentvox,UPPER_SOUTHWEST);
        else
        if(voxdir == SOUTH) currentvox = MGETSUBVOX(currentvox,UPPER_NORTHEAST);
        break;

      case UPPER_SOUTHWEST:

        if(voxdir == UP)    currentvox = MGETSUBVOX(currentvox,LOWER_SOUTHWEST);
        else
        if(voxdir == WEST)  currentvox = MGETSUBVOX(currentvox,UPPER_SOUTHEAST);
        else
        if(voxdir == SOUTH) currentvox = MGETSUBVOX(currentvox,UPPER_NORTHWEST);
        break;

      case LOWER_NORTHEAST:

        if(voxdir == DOWN)  currentvox = MGETSUBVOX(currentvox,UPPER_NORTHEAST);
        else
        if(voxdir == NORTH) currentvox = MGETSUBVOX(currentvox,LOWER_SOUTHEAST);
        else
        if(voxdir == EAST)  currentvox = MGETSUBVOX(currentvox,LOWER_NORTHWEST);
        break;

      case LOWER_NORTHWEST:

        if(voxdir == NORTH) currentvox = MGETSUBVOX(currentvox,LOWER_SOUTHWEST);
        else
        if(voxdir == WEST)  currentvox = MGETSUBVOX(currentvox,LOWER_NORTHEAST);
        else
        if(voxdir == DOWN)  currentvox = MGETSUBVOX(currentvox,UPPER_NORTHWEST);
        break;

      case LOWER_SOUTHEAST:

        if(voxdir == DOWN)  currentvox = MGETSUBVOX(currentvox,UPPER_SOUTHEAST);
        else
        if(voxdir == EAST)  currentvox = MGETSUBVOX(currentvox,LOWER_SOUTHWEST);
        else
        if(voxdir == SOUTH) currentvox = MGETSUBVOX(currentvox,LOWER_NORTHEAST);
        break;

      case LOWER_SOUTHWEST:

        if(voxdir == DOWN)  currentvox = MGETSUBVOX(currentvox,UPPER_SOUTHWEST);
        else
        if(voxdir == WEST)  currentvox = MGETSUBVOX(currentvox,LOWER_SOUTHEAST);
        else
        if(voxdir == SOUTH) currentvox = MGETSUBVOX(currentvox,LOWER_NORTHWEST);
        break;

    }

  }

  return(currentvox);

}

