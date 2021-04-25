/*
** Sphere.c
**
** 15 Sep 1992 - Created    - J. Terrell
**
*/
#include "Headers/tracer.h"

/* sp_GetExtents() ; Finds the extents of a sphere and fills out the ext array */
void sp_GetExtents(struct prim_Sphere *sp,float *ext,struct Matrix44 *matrix)
{

  float rad;

  rad = sp->sph_radius;

  ext[0] = sp->sph_origin[0] - rad;
  ext[1] = sp->sph_origin[0] + rad;

  ext[2] = sp->sph_origin[1] - rad;
  ext[3] = sp->sph_origin[1] + rad;

  ext[4] = sp->sph_origin[2] - rad;
  ext[5] = sp->sph_origin[2] + rad;

}

/*
  sp_CheckRayIntersect() ; Check for ray-sphere intersection.
  Ray must be normalized.

  This test is from Glassner, (pp. 39 - 44)

  returns TRUE or FALSE depending on intersection results.

*/
BOOL sp_IsRayIntersect(struct VoxObject *vxo,struct Ray *ray)
{

  float OC[3];
  float L2oc,tca,t2hc,t;
  struct prim_Sphere *sp;
  float *o,ori[3],pnt[3],pnt2[3];
  float radius_sq;

  if(vxo == ray->shootingvxo && !(ray->tdepth & 1)) return(FALSE);

  sp = (struct prim_Sphere *) vxo->vxo_Prim;

  if(vxo->vxo_Obj->obj_Local2World) {

    pnt[0] = sp->sph_origin[0] + sp->sph_radius;
    pnt[1] = sp->sph_origin[1];
    pnt[2] = sp->sph_origin[2];

    vertmult(sp->sph_origin,ori,vxo->vxo_Obj->obj_Local2World);
    vertmult(pnt,pnt2,vxo->vxo_Obj->obj_Local2World);

    pnt2[0] -= ori[0];
    pnt2[1] -= ori[1];
    pnt2[2] -= ori[2];

    radius_sq = MDOT(pnt2,pnt2);
    o = ori;

  }
  else {

    radius_sq = sp->sph_rsquared;
    o = sp->sph_origin;

  }

  OC[0] = o[0] - ray->o[0];
  OC[1] = o[1] - ray->o[1];
  OC[2] = o[2] - ray->o[2];

  L2oc = MDOT(OC,OC);
  tca  = MDOT(OC,ray->d);

  /*
  ** See if ray origin is outside sphere.
  */
  if(L2oc >= radius_sq && (vxo != ray->shootingvxo)) {

    if(tca < 0.0) return(FALSE);  /* Sphere behind ray. */

    /*
    ** get squared half chord distance.
    */
    t2hc = radius_sq - L2oc + (tca * tca);

    if(t2hc <= 0.0) return(FALSE); /* Ray misses sphere. */

    t = tca - sqrt(t2hc);

  }
  else {

    if(L2oc >= radius_sq) L2oc = radius_sq;

    t2hc = radius_sq - L2oc + (tca * tca);
    t = tca + sqrt(t2hc);

  }

  if(t > 0.0 && t < ray->t && t < ray->maxt) {

    ray->t = t;

    MRAYPOS(ray->o,ray->d,t,ray->ip);

    ray->norm[0] = ray->ip[0] - o[0];
    ray->norm[1] = ray->ip[1] - o[1];
    ray->norm[2] = ray->ip[2] - o[2];

    ray->flags &= ~RFLAG_NORMAL;
    ray->vxo = vxo;

    return(TRUE);

  }

  return(FALSE);

}

BOOL sp_BoundsCheck(register float *bounds,register float *origin)
{

  /*
  ** First check sphere origin containment.
  */
  if(origin[0] >= bounds[MINX]) {

    if(origin[0] <= bounds[MAXX]) {

      if(origin[1] >= bounds[MINY]) {

        if(origin[1] <= bounds[MAXY]) {

          if(origin[2] >= bounds[MINZ]) {

            if(origin[2] <= bounds[MAXZ]) {

              return(TRUE);

            }

          }

        }

      }

    }

  }

  return(FALSE);

}

unsigned long sp_IsVoxelIntersect(struct prim_Sphere *sp,struct Voxel *vox,
  struct Object *obj)
{

  float tst[3];
  float radius;

  if(sp_BoundsCheck(vox->bounds,sp->sph_origin)) return(6);

  radius = sp->sph_radius;

  tst[0] = sp->sph_origin[0] + radius;
  tst[1] = sp->sph_origin[1];
  tst[2] = sp->sph_origin[2];

  if(sp_BoundsCheck(vox->bounds,tst)) return(6);

  tst[0] = sp->sph_origin[0] - radius;
  tst[1] = sp->sph_origin[1];
  tst[2] = sp->sph_origin[2];

  if(sp_BoundsCheck(vox->bounds,tst)) return(6);

  tst[0] = sp->sph_origin[0];
  tst[1] = sp->sph_origin[1] + radius;
  tst[2] = sp->sph_origin[2];

  if(sp_BoundsCheck(vox->bounds,tst)) return(6);

  tst[0] = sp->sph_origin[0];
  tst[1] = sp->sph_origin[1] - radius;
  tst[2] = sp->sph_origin[2];

  if(sp_BoundsCheck(vox->bounds,tst)) return(6);

  tst[0] = sp->sph_origin[0];
  tst[1] = sp->sph_origin[1];
  tst[2] = sp->sph_origin[2] + radius;

  if(sp_BoundsCheck(vox->bounds,tst)) return(6);

  tst[0] = sp->sph_origin[0];
  tst[1] = sp->sph_origin[1];
  tst[2] = sp->sph_origin[2] - radius;

  if(sp_BoundsCheck(vox->bounds,tst)) return(6);

  return(7);

}
