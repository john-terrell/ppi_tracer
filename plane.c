/*
** Plane.c
**
** 07 Oct 1992 - Created    - J. Terrell
**
*/
#include "Headers/Tracer.h"

/* pl_IsVoxelIntersect() ; Determine if a voxel and plane intersect.

  DETAILS:

    - First see if the vertex on the plane is contained.
    - See if any voxel edge intersect's the plane.

    Unsigned long value returned will have the results of the intersection tests.

    0 = EAST Side Hit.
    1 = WEST Side Hit.
    2 = TOP Side Hit.
    3 = BOTTOM Side Hit.
    4 = NORTH Side Hit.
    5 = SOUTH Side Hit.

    6 = POSSIBLY Contained.  (A Vertex was contained but may still intersect.)
    7 = No intersection

*/
unsigned long pl_IsVoxelIntersect(struct prim_Plane *pl,struct Voxel *vox,
  struct Object *obj)
{

  struct Ray tr;
  struct VoxObject vxo;
  float *ptr;

  /*
  ** Plane origin containment check
  */
  ptr = pl->pln_origin;

  if(ptr[0] >= vox->bounds[MINX]) {

    if(ptr[0] <= vox->bounds[MAXX]) {

      if(ptr[1] >= vox->bounds[MINY]) {

        if(ptr[1] <= vox->bounds[MAXY]) {

          if(ptr[2] >= vox->bounds[MINZ]) {

            if(ptr[2] <= vox->bounds[MAXZ]) {

              return(6);

            }

          }

        }

      }

    }

  }

  /*
  ** Check each edge of the voxel to see if it intersects with the plane.
  */

  vxo.vxo_Prim = (struct Primitive *)&tr;
  vxo.vxo_Obj = obj;

  /* Test the vertical edges */

  tr.t = INFINITY;
  tr.maxt = 1.0;

  tr.o[0] = vox->bounds[MAXX];
  tr.o[1] = vox->bounds[MINY];
  tr.o[2] = vox->bounds[MAXZ];
  tr.d[0] = 0.0;
  tr.d[1] = vox->bounds[MAXY] - vox->bounds[MINY];
  tr.d[2] = 0.0;

  if(pl_IsRayIntersect(&vxo,&tr)) return(6);

  tr.o[0] = vox->bounds[MINX];
  tr.o[1] = vox->bounds[MINY];
  tr.o[2] = vox->bounds[MAXZ];
  tr.d[0] = 0.0;
  tr.d[1] = vox->bounds[MAXY] - vox->bounds[MINY];
  tr.d[2] = 0.0;

  if(pl_IsRayIntersect(&vxo,&tr)) return(6);



  tr.o[0] = vox->bounds[MAXX];
  tr.o[1] = vox->bounds[MINY];
  tr.o[2] = vox->bounds[MINZ];
  tr.d[0] = 0.0;
  tr.d[1] = vox->bounds[MAXY] - vox->bounds[MINY];
  tr.d[2] = 0.0;

  if(pl_IsRayIntersect(&vxo,&tr)) return(6);

  tr.o[0] = vox->bounds[MINX];
  tr.o[1] = vox->bounds[MINY];
  tr.o[2] = vox->bounds[MINZ];
  tr.d[0] = 0.0;
  tr.d[1] = vox->bounds[MAXY] - vox->bounds[MINY];
  tr.d[2] = 0.0;

  if(pl_IsRayIntersect(&vxo,&tr)) return(6);


  /* Test the horizontal edges */

  tr.t = INFINITY;
  tr.maxt = INFINITY;

  tr.o[0] = vox->bounds[MINX];
  tr.o[1] = vox->bounds[MINY];
  tr.o[2] = vox->bounds[MAXZ];
  tr.d[0] = vox->bounds[MAXX] - vox->bounds[MINX];
  tr.d[1] = 0.0;
  tr.d[2] = 0.0;

  if(pl_IsRayIntersect(&vxo,&tr)) return(6);

  tr.t = INFINITY;
  tr.maxt = INFINITY;

  tr.o[0] = vox->bounds[MINX];
  tr.o[1] = vox->bounds[MAXY];
  tr.o[2] = vox->bounds[MAXZ];
  tr.d[0] = vox->bounds[MAXX] - vox->bounds[MINX];
  tr.d[1] = 0.0;
  tr.d[2] = 0.0;

  if(pl_IsRayIntersect(&vxo,&tr)) return(6);

  tr.t = INFINITY;
  tr.maxt = INFINITY;

  tr.o[0] = vox->bounds[MINX];
  tr.o[1] = vox->bounds[MINY];
  tr.o[2] = vox->bounds[MINZ];
  tr.d[0] = vox->bounds[MAXX] - vox->bounds[MINX];
  tr.d[1] = 0.0;
  tr.d[2] = 0.0;

  if(pl_IsRayIntersect(&vxo,&tr)) return(6);

  tr.t = INFINITY;
  tr.maxt = INFINITY;

  tr.o[0] = vox->bounds[MINX];
  tr.o[1] = vox->bounds[MAXY];
  tr.o[2] = vox->bounds[MINZ];
  tr.d[0] = vox->bounds[MAXX] - vox->bounds[MINX];
  tr.d[1] = 0.0;
  tr.d[2] = 0.0;

  if(pl_IsRayIntersect(&vxo,&tr)) return(6);

  /* Test the edges going back. */

  tr.t = INFINITY;
  tr.maxt = INFINITY;

  tr.o[0] = vox->bounds[MINX];
  tr.o[1] = vox->bounds[MINY];
  tr.o[2] = vox->bounds[MINZ];
  tr.d[0] = 0.0;
  tr.d[1] = 0.0;
  tr.d[2] = vox->bounds[MAXZ] - vox->bounds[MINZ];

  if(pl_IsRayIntersect(&vxo,&tr)) return(6);

  tr.t = INFINITY;
  tr.maxt = INFINITY;

  tr.o[0] = vox->bounds[MAXX];
  tr.o[1] = vox->bounds[MINY];
  tr.o[2] = vox->bounds[MINZ];
  tr.d[0] = 0.0;
  tr.d[1] = 0.0;
  tr.d[2] = vox->bounds[MAXZ] - vox->bounds[MINZ];

  if(pl_IsRayIntersect(&vxo,&tr)) return(6);

  tr.o[0] = vox->bounds[MINX];
  tr.o[1] = vox->bounds[MAXY];
  tr.o[2] = vox->bounds[MINZ];
  tr.d[0] = 0.0;
  tr.d[1] = 0.0;
  tr.d[2] = vox->bounds[MAXZ] - vox->bounds[MINZ];

  if(pl_IsRayIntersect(&vxo,&tr)) return(6);

  tr.t = INFINITY;
  tr.maxt = INFINITY;

  tr.o[0] = vox->bounds[MAXX];
  tr.o[1] = vox->bounds[MAXY];
  tr.o[2] = vox->bounds[MINZ];
  tr.d[0] = 0.0;
  tr.d[1] = 0.0;
  tr.d[2] = vox->bounds[MAXZ] - vox->bounds[MINZ];

  if(pl_IsRayIntersect(&vxo,&tr)) return(6);

  return(7);

}

BOOL pl_IsRayIntersect(struct VoxObject *vxo,struct Ray *ray)
{

  float p[3];
  float t;

  struct prim_Plane *pl;

  if(vxo == ray->shootingvxo) return(FALSE);

  pl = (struct prim_Plane *)vxo->vxo_Prim;

  SUB(pl->pln_origin,ray->o,p);

  t = MDOT(p,pl->pln_normal) / MDOT(ray->d,pl->pln_normal);
  if(t > 0.005 && t < ray->t && t < ray->maxt) {

    ray->t = t;

    MRAYPOS(ray->o,ray->d,t,ray->ip);

    ray->norm[0] = pl->pln_normal[0];
    ray->norm[1] = pl->pln_normal[1];
    ray->norm[2] = pl->pln_normal[2];

    ray->flags |= RFLAG_NORMAL;
    ray->vxo = vxo;

    return(TRUE);

  }

  return(FALSE);

}
