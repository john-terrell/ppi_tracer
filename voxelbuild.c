 /*
** VoxelBuild.c
**
** Functions to build a voxel tree.
**
** 19 Aug 1992 - Created    - J. Terrell
**
*/
#include "headers/tracer.h"

struct Min_List vxb_UnusedVoxObjList;
long vxb_UnusedCount;

void vxb_InitVoxObjList(void)
{

  newlist(&vxb_UnusedVoxObjList);
  vxb_UnusedCount = 0;

}

void vxb_FreeVoxObjList(void)
{

  struct VoxObject *vxo;

  while(!IsListEmpty(&vxb_UnusedVoxObjList)) {

    vxo = (struct VoxObject *)remtail(&vxb_UnusedVoxObjList);

    oem_FreeMem(vxo,sizeof(struct VoxObject));

  }

  vxb_UnusedCount = 0;

}

struct VoxObject *vxb_GetFreeVoxObj(void)
{

  struct VoxObject *vxo;

  if(vxb_UnusedCount > 0) {

    vxo = (struct VoxObject *)remtail(&vxb_UnusedVoxObjList);
    vxb_UnusedCount--;

  }
  else {

    vxo = oem_AllocMem(sizeof(struct VoxObject));

  }

  return(vxo);

}

void vxb_FreeVoxObj(struct VoxObject *vxo)
{

  addtail(&vxb_UnusedVoxObjList,(struct Min_Node *)vxo);
  vxb_UnusedCount++;

}

/* pt_buildvoxeltree() ; Entry function to build the database's voxel tree. */
short pt_buildvoxeltree(void)
{

  short error;

  /*
  ** Build the first root voxel that encompasses the entire scene.
  */
  error = BuildRootVoxel();
  if(error == ERROR_OK) {

    /*
    ** Get a pointer to the Root voxel and call the recursive function that
    ** will build the tree.  (the '1' parameter is the current voxel depth.)
    */

    vxb_InitVoxObjList();

    error = ProcessVoxel(&gl_env->RootVoxel,1);

    vxb_FreeVoxObjList();

    gl_env->CameraVoxel = vx_CheckPointInVoxel(&gl_env->RootVoxel,
      gl_env->Camera->cam_Origin);

    if(!gl_env->CameraVoxel) gl_env->CameraVoxel = &gl_env->RootVoxel;

    if(IsListEmpty(&gl_env->AlwaysTestList)) gl_env->flags |= ENVFLAG_ATLEMPTY;

  }

  return(error);

}

/* pt_freevoxeltree() ; Frees the entire voxel tree. */
void pt_freevoxeltree(void)
{

  FreeVoxelTree(&gl_env->RootVoxel);

}

/* BuildRootVoxel() ; Function to make the scene encompassing root voxel. */
short BuildRootVoxel(void)
{

  struct Voxel *vox;
  struct VoxObject *voxobj;
  struct Object *obj;
  struct Primitive *prim;

  short functionindex;

  float extents[6];
  short error;

  error = ERROR_OK;

  /*
  ** Get the "Root" voxel
  */
  vox = &gl_env->RootVoxel;

  /*
  ** "Root" voxel initialization
  */
  newlist(&vox->VoxelContents);

  vox->Neighbors[EAST] = NULL;
  vox->Neighbors[WEST] = NULL;
  vox->Neighbors[UP] = NULL;
  vox->Neighbors[DOWN] = NULL;
  vox->Neighbors[NORTH] = NULL;
  vox->Neighbors[SOUTH] = NULL;

  vox->bounds[MINX] = vox->bounds[MINY] = vox->bounds[MINZ] = INFINITY;
  vox->bounds[MAXX] = vox->bounds[MAXY] = vox->bounds[MAXZ] = NEGINFINITY;

  vox->PrimCount = 0;

  /*
  ** Find all objects in database.
  */
  obj = (struct Object *)gl_env->ObjectList.lh_Head;
  while(obj->obj_Node.ln_Succ) {

    /*
    ** Find all primitives in this object
    */

    if(!(obj->obj_Flags & OFLG_NORENDER)) {

      prim = (struct Primitive *)obj->obj_PrimitiveList.lh_Head;
      while(prim->prm_Node.ln_Succ) {

        /*
        ** Get the index into the primitive specific extent calculation function,
        ** then call it.
        */

        if(!(prim->prm_Flags & PFLAG_NOEXTENTS)) {

          functionindex = GET_PRIMTYPE(prim->prm_Flags);
          GetPrimitiveExtents[functionindex](prim,extents,obj->obj_Local2World);

          /*
          ** Check primitive extents against current voxel extents.
          */
          if(extents[0] < vox->bounds[MINX]) vox->bounds[MINX] = extents[0];
          if(extents[1] > vox->bounds[MAXX]) vox->bounds[MAXX] = extents[1];
          if(extents[2] < vox->bounds[MINY]) vox->bounds[MINY] = extents[2];
          if(extents[3] > vox->bounds[MAXY]) vox->bounds[MAXY] = extents[3];
          if(extents[4] < vox->bounds[MINZ]) vox->bounds[MINZ] = extents[4];
          if(extents[5] > vox->bounds[MAXZ]) vox->bounds[MAXZ] = extents[5];

          /*
          ** Make a voxel entry for this primitive.
          */
          voxobj = oem_AllocMem(sizeof(struct VoxObject));
          if(!voxobj) {

            error = ERROR_NOMEM;
            goto done;

          }

          voxobj->vxo_Obj = obj;
          voxobj->vxo_Prim = prim;
  

          /*
          ** Add this primitive to the voxel's content list.
          */
          addtail(&vox->VoxelContents,(struct Min_Node *)voxobj);

          /*
          ** Increment the primitive count for this voxel
          */
          vox->PrimCount++;

        }
        
        /*
        ** See if the ALWAYS test bit is set.
        */
        if(prim->prm_Flags & PFLAG_ALWAYSTEST) {

          voxobj = oem_AllocMem(sizeof(struct VoxObject));
          if(!voxobj) {

            error = ERROR_NOMEM;
            goto done;

          }

          voxobj->vxo_Obj = obj;
          voxobj->vxo_Prim = prim;

          /*
          ** Add this primitive to the voxel's content list.
          */
          addtail(&gl_env->AlwaysTestList,(struct Min_Node *)voxobj);

        }

        
        /*
        ** Next primitive...
        */
        prim = (struct Primitive *)prim->prm_Node.ln_Succ;

      }

    }

    /*
    ** Next object...
    */
    obj = (struct Object *)obj->obj_Node.ln_Succ;

  }

  /*
  ** Increase the root voxel's volume by 1% to compensate for objects that
  ** may lie exactly on the bounds of the voxel.
  */
  if(vox->PrimCount > 0) {

    vox->bounds[0] *= 1.01;
    vox->bounds[1] *= 1.01;
    vox->bounds[2] *= 1.01;
    vox->bounds[3] *= 1.01;
    vox->bounds[4] *= 1.01;
    vox->bounds[5] *= 1.01;

  }

done:

  /*
  ** If an error occurred, free the voxel tree and bug out.
  */
  if(error != ERROR_OK) {

    FreeVoxelTree(vox);

  }

  return(error);

}

/* ProcessVoxel() ; Recursive function that checks to see if a voxel's primitive
                    count exceeds the max allowed or the maximum voxel depth has
                    been reached, if either is true , the recursion is terminated,
                    otherwise, the voxel is subdivided down into eight subvoxels.
                    Each primitive in the parent voxel is tested against each
                    subvoxel to determine which primitives are contained within
                    each subvoxel.  After containment testing, this function is
                    called again (recursively) to process each subvoxel. */
short ProcessVoxel(struct Voxel *vox,unsigned short voxdepth)
{

  struct VoxObject *vxo,*newvxo;
  struct Object *obj;
  struct Primitive *prim;
  struct Voxel *svox,*voxellist;
  unsigned long intersect,k,voxtest;
  short voxnum,index,error;

  error = ERROR_OK;

  /*
  ** See if this voxel's count exceeds the max allowed and terminate the recursive
  ** process if so.
  */
  if(vox->PrimCount <= gl_env->maxvoxelprims || voxdepth > gl_env->maxvoxeldepth) {

    return(error);

  }

  vox->PrimCount = -1L;
  vox->Level = voxdepth;

  /*
  ** Allocate the subvoxels
  */
  voxellist = oem_AllocMem((sizeof(struct Voxel) << 3));
  if(!voxellist) {

    error = ERROR_NOMEM;
    goto done;

  }

  /*
  ** Preprocess the subvoxels.  (Setup neighbor pointers and bounds.)
  */
  vx_PreprocessSubvoxels(vox,voxellist);

  /*
  ** Intersection tests.
  */
  while(!IsListEmpty(&vox->VoxelContents)) {

    /*
    ** First remove the primitive from the voxel's list.
    */
    vxo = (struct VoxObject *)remtail(&vox->VoxelContents);

    prim = vxo->vxo_Prim;
    obj = vxo->vxo_Obj;


    index = GET_PRIMTYPE(prim->prm_Flags);

    /*
    ** Determine which voxels the primitive intersects.
    ** voxtest holds intersect information. one bit for each
    ** voxel that was intersected by the current prim.
    */
    voxtest = 0;
    voxnum = 0;

    do {

      /*
      ** If this primitive hasn't intersected this voxel yet, check it.
      */
      if(!(voxtest & (1 << voxnum))) {

        svox = &voxellist[voxnum];

        intersect = IsPrimitiveVoxelIntersect[index](prim,svox,obj);
        if(intersect) {

          voxtest |= (1 << voxnum);

          /*
          ** If the primitive penetrated the side of the voxel, it must also
          ** intersect the neighbor on that side, so mark it as such.
          */
          if(intersect == 0x0000FFFF) { /* Trivial Accept */

            break;

          }
          else if(intersect != 0xFFFFFFFF) {

            for(k = 0; k < 6; k++) {

              if(intersect & (1 << k)) {

                switch(k) {

                  case EAST:
                    if(voxnum == 0) voxtest |= (1 << 1);
                    if(voxnum == 3) voxtest |= (1 << 2);
                    if(voxnum == 4) voxtest |= (1 << 5);
                    if(voxnum == 7) voxtest |= (1 << 6);
                    break;

                  case WEST:
                    if(voxnum == 1) voxtest |= (1 << 0);
                    if(voxnum == 2) voxtest |= (1 << 3);
                    if(voxnum == 5) voxtest |= (1 << 4);
                    if(voxnum == 6) voxtest |= (1 << 7);
                    break;

                  case SOUTH:
                    if(voxnum == 0) voxtest |= (1 << 3);
                    if(voxnum == 1) voxtest |= (1 << 2);
                    if(voxnum == 4) voxtest |= (1 << 7);
                    if(voxnum == 5) voxtest |= (1 << 6);
                    break;

                  case NORTH:
                    if(voxnum == 3) voxtest |= (1 << 0);
                    if(voxnum == 2) voxtest |= (1 << 1);
                    if(voxnum == 7) voxtest |= (1 << 4);
                    if(voxnum == 6) voxtest |= (1 << 5);
                    break;

                  case DOWN:
                    if(voxnum == 0) voxtest |= (1 << 4);
                    if(voxnum == 1) voxtest |= (1 << 5);
                    if(voxnum == 2) voxtest |= (1 << 6);
                    if(voxnum == 3) voxtest |= (1 << 7);
                    break;

                  case UP:
                    if(voxnum == 4) voxtest |= (1 << 0);
                    if(voxnum == 5) voxtest |= (1 << 1);
                    if(voxnum == 6) voxtest |= (1 << 2);
                    if(voxnum == 7) voxtest |= (1 << 3);
                    break;

                }

              }

            }

          }

        }

      }

      voxnum++;

    } while(voxnum < 8);

    /*
    ** See which voxels were intersected by the current primitive.
    */
    for(voxnum = 0; voxnum < 8; voxnum++) {

      if(voxtest & (1 << voxnum)) { /* It intersected. */

        svox = &voxellist[voxnum];

        newvxo = vxb_GetFreeVoxObj();
        if(!newvxo) {

          error = ERROR_NOMEM;
          goto done;

        }

        newvxo->vxo_Prim = prim;
        newvxo->vxo_Obj = obj;

        addtail(&svox->VoxelContents,(struct Min_Node *)newvxo);
        svox->PrimCount++;

      }

    }

    vxb_FreeVoxObj(vxo);

  }

  /*
  ** Add each subvoxel to the parent voxel's content list and process using
  ** recursion.
  */
  voxnum = 0;
  do {

    addtail(&vox->VoxelContents,(struct Min_Node *)&voxellist[voxnum]);

    error = ProcessVoxel(&voxellist[voxnum],voxdepth + 1);

    voxnum++;

  } while(voxnum < 8 && !error);

done:

  /*
  ** If an error occurred, free the voxels.
  */
  if(error) {

    if(voxellist) oem_FreeMem(voxellist,(sizeof(struct Voxel) << 3));

  }

  return(error);

}

void vx_PreprocessSubvoxels(struct Voxel *vx,struct Voxel *vl)
{

  struct Voxel *svox;
  long vlevel;

  vlevel = vx->Level + 1;

  /*
  ** Subvoxel ZERO Preprocess.
  */
  svox = &vl[UPPER_NORTHWEST];

  newlist(&svox->VoxelContents);
  svox->ID = UPPER_NORTHWEST;
  svox->Level = vlevel;

  svox->Neighbors[EAST]   = &vl[UPPER_NORTHEAST];
  svox->Neighbors[WEST]   = vx->Neighbors[WEST];
  svox->Neighbors[UP]     = vx->Neighbors[UP];
  svox->Neighbors[DOWN]   = &vl[LOWER_NORTHWEST];
  svox->Neighbors[NORTH]  = vx->Neighbors[NORTH];
  svox->Neighbors[SOUTH]  = &vl[UPPER_SOUTHWEST];

  svox->bounds[MINX] = vx->bounds[MINX];
  svox->bounds[MAXY] = vx->bounds[MAXY];
  svox->bounds[MAXZ] = vx->bounds[MAXZ];

  svox->bounds[MAXX] =
    vx->bounds[MINX] + ((vx->bounds[MAXX] - vx->bounds[MINX]) * 0.5);

  svox->bounds[MINY] =
    vx->bounds[MINY] + ((vx->bounds[MAXY] - vx->bounds[MINY]) * 0.5);

  svox->bounds[MINZ] =
    vx->bounds[MINZ] + ((vx->bounds[MAXZ] - vx->bounds[MINZ]) * 0.5);

  svox->PrimCount = 0;

  /*
  ** Subvoxel ONE Preprocess.
  */
  svox = &vl[UPPER_NORTHEAST];

  newlist(&svox->VoxelContents);
  svox->ID = UPPER_NORTHEAST;
  svox->Level = vlevel;

  svox->Neighbors[EAST]   = vx->Neighbors[EAST];
  svox->Neighbors[WEST]   = &vl[UPPER_NORTHWEST];
  svox->Neighbors[UP]     = vx->Neighbors[UP];
  svox->Neighbors[DOWN]   = &vl[LOWER_NORTHEAST];
  svox->Neighbors[NORTH]  = vx->Neighbors[NORTH];
  svox->Neighbors[SOUTH]  = &vl[UPPER_SOUTHEAST];

  svox->bounds[MINX] = vl[UPPER_NORTHWEST].bounds[MAXX];
  svox->bounds[MAXX] = vx->bounds[MAXX];
  svox->bounds[MINY] = vl[UPPER_NORTHWEST].bounds[MINY];
  svox->bounds[MAXY] = vl[UPPER_NORTHWEST].bounds[MAXY];
  svox->bounds[MINZ] = vl[UPPER_NORTHWEST].bounds[MINZ];
  svox->bounds[MAXZ] = vl[UPPER_NORTHWEST].bounds[MAXZ];
  svox->PrimCount = 0;

  /*
  ** Subvoxel TWO Preprocess.
  */
  svox = &vl[UPPER_SOUTHEAST];

  newlist(&svox->VoxelContents);
  svox->ID = UPPER_SOUTHEAST;
  svox->Level = vlevel;

  svox->Neighbors[EAST]   = vx->Neighbors[EAST];
  svox->Neighbors[WEST]   = &vl[UPPER_SOUTHWEST];
  svox->Neighbors[UP]     = vx->Neighbors[UP];
  svox->Neighbors[DOWN]   = &vl[LOWER_SOUTHEAST];
  svox->Neighbors[NORTH]  = &vl[UPPER_NORTHEAST];
  svox->Neighbors[SOUTH]  = vx->Neighbors[SOUTH];

  svox->bounds[MINX] = vl[UPPER_NORTHEAST].bounds[MINX];
  svox->bounds[MAXX] = vl[UPPER_NORTHEAST].bounds[MAXX];
  svox->bounds[MINY] = vl[UPPER_NORTHEAST].bounds[MINY];
  svox->bounds[MAXY] = vl[UPPER_NORTHEAST].bounds[MAXY];
  svox->bounds[MINZ] = vx->bounds[MINZ];
  svox->bounds[MAXZ] = vl[UPPER_NORTHEAST].bounds[MINZ];
  svox->PrimCount = 0;

  /*
  ** Subvoxel THREE Preprocess.
  */
  svox = &vl[UPPER_SOUTHWEST];

  newlist(&svox->VoxelContents);
  svox->ID = UPPER_SOUTHWEST;
  svox->Level = vlevel;

  svox->Neighbors[EAST]   = &vl[UPPER_SOUTHEAST];
  svox->Neighbors[WEST]   = vx->Neighbors[WEST];
  svox->Neighbors[UP]     = vx->Neighbors[UP];
  svox->Neighbors[DOWN]   = &vl[LOWER_SOUTHWEST];
  svox->Neighbors[NORTH]  = &vl[UPPER_NORTHWEST];
  svox->Neighbors[SOUTH]  = vx->Neighbors[SOUTH];

  svox->bounds[MINX] = vl[UPPER_NORTHWEST].bounds[MINX];
  svox->bounds[MAXX] = vl[UPPER_NORTHWEST].bounds[MAXX];
  svox->bounds[MINY] = vl[UPPER_NORTHWEST].bounds[MINY];
  svox->bounds[MAXY] = vl[UPPER_NORTHWEST].bounds[MAXY];
  svox->bounds[MINZ] = vx->bounds[MINZ];
  svox->bounds[MAXZ] = vl[UPPER_NORTHWEST].bounds[MINZ];
  svox->PrimCount = 0;

  /*
  ** Subvoxel FOUR Preprocess.
  */
  svox = &vl[LOWER_NORTHWEST];

  newlist(&svox->VoxelContents);
  svox->ID = LOWER_NORTHWEST;
  svox->Level = vlevel;

  svox->Neighbors[EAST]   = &vl[LOWER_NORTHEAST];
  svox->Neighbors[WEST]   = vx->Neighbors[WEST];
  svox->Neighbors[UP]     = &vl[UPPER_NORTHWEST];
  svox->Neighbors[DOWN]   = vx->Neighbors[DOWN];
  svox->Neighbors[NORTH]  = vx->Neighbors[NORTH];
  svox->Neighbors[SOUTH]  = &vl[LOWER_SOUTHWEST];

  svox->bounds[MINX] = vl[UPPER_NORTHWEST].bounds[MINX];
  svox->bounds[MAXX] = vl[UPPER_NORTHWEST].bounds[MAXX];
  svox->bounds[MINY] = vx->bounds[MINY];
  svox->bounds[MAXY] = vl[UPPER_NORTHWEST].bounds[MINY];
  svox->bounds[MINZ] = vl[UPPER_NORTHWEST].bounds[MINZ];
  svox->bounds[MAXZ] = vl[UPPER_NORTHWEST].bounds[MAXZ];
  svox->PrimCount = 0;

  /*
  ** Subvoxel FIVE Preprocess.
  */
  svox = &vl[LOWER_NORTHEAST];

  newlist(&svox->VoxelContents);
  svox->ID = LOWER_NORTHEAST;
  svox->Level = vlevel;

  svox->Neighbors[EAST]   = vx->Neighbors[EAST];
  svox->Neighbors[WEST]   = &vl[LOWER_NORTHWEST];
  svox->Neighbors[UP]     = &vl[UPPER_NORTHEAST];
  svox->Neighbors[DOWN]   = vx->Neighbors[DOWN];
  svox->Neighbors[NORTH]  = vx->Neighbors[NORTH];
  svox->Neighbors[SOUTH]  = &vl[LOWER_SOUTHEAST];

  svox->bounds[MINX] = vl[UPPER_NORTHEAST].bounds[MINX];
  svox->bounds[MAXX] = vl[UPPER_NORTHEAST].bounds[MAXX];
  svox->bounds[MINY] = vx->bounds[MINY];
  svox->bounds[MAXY] = vl[UPPER_NORTHEAST].bounds[MINY];
  svox->bounds[MINZ] = vl[UPPER_NORTHEAST].bounds[MINZ];
  svox->bounds[MAXZ] = vl[UPPER_NORTHEAST].bounds[MAXZ];
  svox->PrimCount = 0;

  /*
  ** Subvoxel SIX Preprocess.
  */
  svox = &vl[LOWER_SOUTHEAST];

  newlist(&svox->VoxelContents);
  svox->ID = LOWER_SOUTHEAST;
  svox->Level = vlevel;

  svox->Neighbors[EAST]   = vx->Neighbors[EAST];
  svox->Neighbors[WEST]   = &vl[LOWER_SOUTHWEST];
  svox->Neighbors[UP]     = &vl[UPPER_SOUTHEAST];
  svox->Neighbors[DOWN]   = vx->Neighbors[DOWN];
  svox->Neighbors[NORTH]  = &vl[LOWER_NORTHEAST];
  svox->Neighbors[SOUTH]  = vx->Neighbors[SOUTH];

  svox->bounds[MINX] = vl[UPPER_SOUTHEAST].bounds[MINX];
  svox->bounds[MAXX] = vl[UPPER_SOUTHEAST].bounds[MAXX];
  svox->bounds[MINY] = vx->bounds[MINY];
  svox->bounds[MAXY] = vl[UPPER_SOUTHEAST].bounds[MINY];
  svox->bounds[MINZ] = vl[UPPER_SOUTHEAST].bounds[MINZ];
  svox->bounds[MAXZ] = vl[UPPER_SOUTHEAST].bounds[MAXZ];
  svox->PrimCount = 0;

  /*
  ** Subvoxel SEVEN Preprocess.
  */
  svox = &vl[LOWER_SOUTHWEST];

  newlist(&svox->VoxelContents);
  svox->ID = LOWER_SOUTHWEST;
  svox->Level = vlevel;

  svox->Neighbors[EAST]   = &vl[LOWER_SOUTHEAST];
  svox->Neighbors[WEST]   = vx->Neighbors[WEST];
  svox->Neighbors[UP]     = &vl[UPPER_SOUTHWEST];
  svox->Neighbors[DOWN]   = vx->Neighbors[DOWN];
  svox->Neighbors[NORTH]  = &vl[LOWER_NORTHWEST];
  svox->Neighbors[SOUTH]  = vx->Neighbors[SOUTH];

  svox->bounds[MINX] = vl[UPPER_SOUTHWEST].bounds[MINX];
  svox->bounds[MAXX] = vl[UPPER_SOUTHWEST].bounds[MAXX];
  svox->bounds[MINY] = vx->bounds[MINY];
  svox->bounds[MAXY] = vl[UPPER_SOUTHWEST].bounds[MINY];
  svox->bounds[MINZ] = vl[UPPER_SOUTHWEST].bounds[MINZ];
  svox->bounds[MAXZ] = vl[UPPER_SOUTHWEST].bounds[MAXZ];
  svox->PrimCount = 0;

}

void FreeVoxelTree(struct Voxel *TopVoxel)
{

  struct Voxel *svox;
  struct VoxObject *vxo;

  if(TopVoxel->PrimCount != -1) {

    while(!IsListEmpty(&TopVoxel->VoxelContents)) {

      vxo = (struct VoxObject *)remtail(&TopVoxel->VoxelContents);
      oem_FreeMem(vxo,sizeof(struct VoxObject));

    }

  }
  else {

    svox = TopVoxel->VoxelContents.lh_Head;
    while(svox->vox_Node.ln_Succ) {

      FreeVoxelTree(svox);

      svox = svox->vox_Node.ln_Succ;

    }

    oem_FreeMem(TopVoxel->VoxelContents.lh_Head,sizeof(struct Voxel) << 3);

  }

}
