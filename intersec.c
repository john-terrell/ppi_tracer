/*
** Intersect.c
**
** Functions to handle ray-object intersections
**
** 18 Aug 1992 - Created    - J. Terrell
*/
#include "headers/tracer.h"

/* pt_GetIntersection() ; Get an object intersection using vox. */
void pt_GetIntersection(struct Voxel *vox,struct Ray *ray)
{

  struct VoxObject *vxo;
  struct Voxel *newvox;

  float oldraymaxt;
  short functionindex;

  /*
  ** Intersect the ray with whatever is inside this voxel.
  */
  if(vox->PrimCount < 0) {        /* This voxel contains more voxels */

    if(newvox = pt_GetInternalVoxelIntersection(vox,ray)) {

      pt_GetIntersection(newvox,ray);

    }
    else {

      printf("Voxel ERROR: No intersection in internal voxels.\n");
      ray->vxo = 0;
      return;

    }

  }
  else {                          /* This voxel contains primitives */

    oldraymaxt = ray->maxt;
    if(vox->hitparms[1] < ray->maxt) ray->maxt = vox->hitparms[1];

    vxo = vox->VoxelContents.lh_Head;
    while(vxo->vxo_Node.ln_Succ) {

      functionindex = GET_PRIMTYPE(((struct Primitive *)vxo->vxo_Prim)->prm_Flags);
      if(IsPrimitiveRayIntersect[functionindex](vxo,ray)) {

        ray->primvox = vox;
        ray->vxo = vxo;

      }

      vxo = (struct VoxObject *)vxo->vxo_Node.ln_Succ;

    }

    ray->maxt = oldraymaxt;

    if(!ray->vxo) { /* No intersections found in voxel, find where ray went next. */

      newvox = pt_GetNeighborVoxel(vox,vox->hitsides[1]);
      if(newvox) {

        vx_IntersectVoxel(newvox,ray,FALSE);
        pt_GetIntersection(newvox,ray);

      }

    }

  }

}

/* pt_CheckAlwaysHitList() ; Checks primitives that are on the always hit list. */
void pt_CheckAlwaysTestList(struct Ray *ray)
{

  struct VoxObject *vxo;
  struct Primitive *prim;
  short functionindex;

  vxo = gl_env->AlwaysTestList.lh_Head;
  while(vxo->vxo_Node.ln_Succ) {

    prim = vxo->vxo_Prim;

    functionindex = GET_PRIMTYPE(prim->prm_Flags);
    if(IsPrimitiveRayIntersect[functionindex](vxo,ray)) {

      ray->primvox = 0;
      ray->vxo = vxo;

    }

    vxo = vxo->vxo_Node.ln_Succ;

  }

}

long westside[] = { UPPER_NORTHWEST, UPPER_SOUTHWEST, LOWER_NORTHWEST,LOWER_SOUTHWEST ,-1};
long eastside[] = { UPPER_NORTHEAST, UPPER_SOUTHEAST, LOWER_NORTHEAST,LOWER_SOUTHEAST ,-1};
long northside[]= { UPPER_NORTHWEST, UPPER_NORTHEAST, LOWER_NORTHWEST,LOWER_NORTHEAST ,-1};
long southside[]= { UPPER_SOUTHWEST, UPPER_SOUTHEAST, LOWER_SOUTHWEST,LOWER_SOUTHEAST ,-1};
long upperlevel[] = { UPPER_NORTHWEST,UPPER_NORTHEAST,UPPER_SOUTHWEST,UPPER_SOUTHEAST ,-1};
long lowerlevel[] = { LOWER_NORTHWEST,LOWER_NORTHEAST,LOWER_SOUTHWEST,LOWER_SOUTHEAST ,-1};
long all[] = { UPPER_NORTHWEST,UPPER_NORTHEAST,UPPER_SOUTHWEST,UPPER_SOUTHEAST ,LOWER_NORTHWEST,LOWER_NORTHEAST,LOWER_SOUTHWEST,LOWER_SOUTHEAST ,-1};

struct Voxel *pt_GetInternalVoxelIntersection(struct Voxel *vox,struct Ray *ray)
{

  float t_test;
  struct Voxel *vox_start,*newvox;
  long *vcheck,i;

  switch(vox->hitsides[0]) {

    case -1:   vcheck = all; break;
    case MINX: vcheck = westside; break;
    case MAXX: vcheck = eastside; break;
    case MINY: vcheck = lowerlevel; break;
    case MAXY: vcheck = upperlevel; break;
    case MINZ: vcheck = southside; break;
    case MAXZ: vcheck = northside; break;
    default:
      printf("Voxel ERROR: No voxel entrance detected.\n");
      return(NULL);

  }

  t_test = INFINITY;
  vox_start = 0;

  i = 0;
  while(vcheck[i] != -1) {

    newvox = MGETSUBVOX(vox,vcheck[i]);
    if(vx_IntersectVoxel(newvox,ray,TRUE)) {

      if(newvox->hitparms[1] < t_test) {

        t_test = newvox->hitparms[1];
        vox_start = newvox;

      }

    }

    i++;

  }

  return(vox_start);

}

struct Voxel *pt_GetNeighborVoxel(struct Voxel *vox,short hitvalue)
{

  struct Voxel *newvox;

  newvox = 0;

  switch(hitvalue) {

    case MINX: newvox = vox->Neighbors[WEST];  break;
    case MAXX: newvox = vox->Neighbors[EAST];  break;
    case MINY: newvox = vox->Neighbors[DOWN];  break;
    case MAXY: newvox = vox->Neighbors[UP];    break;
    case MINZ: newvox = vox->Neighbors[SOUTH]; break;
    case MAXZ: newvox = vox->Neighbors[NORTH]; break;

    /* Voxel Corner Cases */
    case UPPERNORTHWEST:

      if(vox->Neighbors[NORTH]) {

        newvox = vox->Neighbors[NORTH]->Neighbors[UP];
        if(newvox) {

          newvox = newvox->Neighbors[WEST];

        }

      }

      break;

    case UPPERNORTHEAST:

      if(vox->Neighbors[NORTH]) {

        newvox = vox->Neighbors[NORTH]->Neighbors[UP];
        if(newvox) {

          newvox = newvox->Neighbors[EAST];

        }

      }

      break;

    case UPPERSOUTHWEST:

      if(vox->Neighbors[SOUTH]) {

        newvox = vox->Neighbors[SOUTH]->Neighbors[UP];
        if(newvox) {

          newvox = newvox->Neighbors[WEST];

        }

      }

      break;

    case UPPERSOUTHEAST:

      if(vox->Neighbors[SOUTH]) {

        newvox = vox->Neighbors[SOUTH]->Neighbors[UP];
        if(newvox) {

          newvox = newvox->Neighbors[EAST];

        }

      }

      break;

    case LOWERNORTHWEST:

      if(vox->Neighbors[NORTH]) {

        newvox = vox->Neighbors[NORTH]->Neighbors[DOWN];
        if(newvox) {

          newvox = newvox->Neighbors[WEST];

        }

      }

      break;

    case LOWERNORTHEAST:

      if(vox->Neighbors[NORTH]) {

        newvox = vox->Neighbors[NORTH]->Neighbors[DOWN];
        if(newvox) {

          newvox = newvox->Neighbors[EAST];

        }

      }

      break;

    case LOWERSOUTHWEST:

      if(vox->Neighbors[SOUTH]) {

        newvox = vox->Neighbors[SOUTH]->Neighbors[DOWN];
        if(newvox) {

          newvox = newvox->Neighbors[WEST];

        }

      }

      break;

    case LOWERSOUTHEAST:

      if(vox->Neighbors[SOUTH]) {

        newvox = vox->Neighbors[SOUTH]->Neighbors[DOWN];
        if(newvox) {

          newvox = newvox->Neighbors[EAST];

        }

      }

      break;

    /* Voxel Edge Cases */
    case NORTHWEST:

      if(vox->Neighbors[NORTH]) {

        newvox = vox->Neighbors[NORTH]->Neighbors[WEST];

      }

      break;

    case NORTHEAST:

      if(vox->Neighbors[NORTH]) {

        newvox = vox->Neighbors[NORTH]->Neighbors[EAST];

      }

      break;

    case SOUTHWEST:

      if(vox->Neighbors[SOUTH]) {

        newvox = vox->Neighbors[SOUTH]->Neighbors[WEST];

      }

      break;

    case SOUTHEAST:

      if(vox->Neighbors[SOUTH]) {

        newvox = vox->Neighbors[SOUTH]->Neighbors[EAST];

      }

      break;

    case WESTUP:

      if(vox->Neighbors[WEST]) {

        newvox = vox->Neighbors[WEST]->Neighbors[UP];

      }

      break;

    case WESTDOWN:

      if(vox->Neighbors[WEST]) {

        newvox = vox->Neighbors[WEST]->Neighbors[DOWN];

      }

      break;

    case EASTUP:

      if(vox->Neighbors[EAST]) {

        newvox = vox->Neighbors[EAST]->Neighbors[UP];

      }

      break;

    case EASTDOWN:

      if(vox->Neighbors[EAST]) {

        newvox = vox->Neighbors[EAST]->Neighbors[DOWN];

      }

      break;

    case NORTHUP:

      if(vox->Neighbors[NORTH]) {

        newvox = vox->Neighbors[NORTH]->Neighbors[UP];

      }

      break;

    case NORTHDOWN:

      if(vox->Neighbors[NORTH]) {

        newvox = vox->Neighbors[NORTH]->Neighbors[DOWN];

      }

      break;

    case SOUTHUP:

      if(vox->Neighbors[SOUTH]) {

        newvox = vox->Neighbors[SOUTH]->Neighbors[UP];

      }

      break;

    case SOUTHDOWN:

      if(vox->Neighbors[SOUTH]) {

        newvox = vox->Neighbors[SOUTH]->Neighbors[DOWN];

      }

      break;

    default:
      printf("Illegal hit value for voxel traversal.\n");
      printf("Hitsides: %d\n",hitvalue);

      newvox = NULL;
      break;

  }

  return(newvox);

}
