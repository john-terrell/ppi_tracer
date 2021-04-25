/*
** Voxel.c
**
** 15 Sep 1992 - Created    - J. Terrell
*/
#include "Headers/tracer.h"

struct Voxel *vx_CheckPointInVoxel(struct Voxel *start,float pnt[])
{

  struct Voxel *contain,*vox;

  if(!check_pointcontain(pnt,start->bounds)) return(NULL);

  if(start->PrimCount != -1) return(start);

  contain = NULL;

  vox = start->VoxelContents.lh_Head;
  while(vox->vox_Node.ln_Succ) {

    if(check_pointcontain(pnt,vox->bounds)) {

      contain = vx_CheckPointInVoxel(vox,pnt);
      break;

    }

    vox = vox->vox_Node.ln_Succ;

  }

  return(contain);

}

/* vx_CheckLineIntersect() ; Checks a line with a voxel.  Puts which side */
BOOL vx_CheckLineIntersect(struct Voxel *vx,struct Ray *ray,unsigned long *res,
  unsigned long facecodes)
{

  float t,ix,iy,iz,one_over_d;

  *res = 0;

  /*
  ** Check Z Planes.
  */
  if(ray->d[2] != 0.0 && (facecodes & 0x30)) {

    one_over_d = 1.0 / ray->d[2];

    if(facecodes & 0x10) {

      /* Check MIN Z Plane. */
      t = -((ray->o[2] - vx->bounds[MINZ]) * one_over_d);

      if(t >= 0.0 && t <= 1.0) {

        ix = ray->o[0] + (ray->d[0] * t);
        iy = ray->o[1] + (ray->d[1] * t);

        if(ix >= vx->bounds[MINX] && ix <= vx->bounds[MAXX] &&
           iy >= vx->bounds[MINY] && iy <= vx->bounds[MAXY]) {

          *res |= (1 << SOUTH);

        }

      }

    }

    if(facecodes & 0x20) {

      /* Check MAX Z Plane */
      t = -((ray->o[2] - vx->bounds[MAXZ]) * one_over_d);

      if(t >= 0.0 && t <= 1.0) {

        ix = ray->o[0] + (ray->d[0] * t);
        iy = ray->o[1] + (ray->d[1] * t);

        if(ix >= vx->bounds[MINX] && ix <= vx->bounds[MAXX] &&
           iy >= vx->bounds[MINY] && iy <= vx->bounds[MAXY]) {

          *res |= (1 << NORTH);

        }

      }

    }

  }


  /*
  ** Check X Planes.
  */
  if(ray->d[0] != 0.0 && (facecodes & 0x03)) {

    one_over_d = 1.0 / ray->d[0];

    if(facecodes & 0x1) {

      /* Check MIN X Plane. */
      t = -((ray->o[0] - vx->bounds[MINX]) * one_over_d);

      if(t >= 0.0 && t <= 1.0) {

        iy = ray->o[1] + (ray->d[1] * t);
        iz = ray->o[2] + (ray->d[2] * t);

        if(iy >= vx->bounds[MINY] && iy <= vx->bounds[MAXY] &&
           iz >= vx->bounds[MINZ] && iz <= vx->bounds[MAXZ]) {

          *res |= (1 << WEST);

        }

      }

    }

    if(facecodes & 0x2) {

      /* Check MAX X Plane. */
      t = -((ray->o[0] - vx->bounds[MAXX]) * one_over_d);

      if(t >= 0.0 && t <= 1.0) {

        iy = ray->o[1] + (ray->d[1] * t);
        iz = ray->o[2] + (ray->d[2] * t);

        if(iy >= vx->bounds[MINY] && iy <= vx->bounds[MAXY] &&
           iz >= vx->bounds[MINZ] && iz <= vx->bounds[MAXZ]) {

          *res |= (1 << EAST);

        }

      }

    }

  }

  /*
  ** Check Y Planes.
  */
  if(ray->d[1] != 0.0 && (facecodes & 0x0C)) {

    one_over_d = 1.0 / ray->d[1];

    if(facecodes & 0x4) {

      /* Check MIN Y Plane. */
      t = -((ray->o[1] - vx->bounds[MINY]) * one_over_d);

      if(t >= 0.0 && t <= 1.0) {

        ix = ray->o[0] + (ray->d[0] * t);
        iz = ray->o[2] + (ray->d[2] * t);

        if(ix >= vx->bounds[MINX] && ix <= vx->bounds[MAXX] &&
           iz >= vx->bounds[MINZ] && iz <= vx->bounds[MAXZ]) {

          *res |= (1 << DOWN);

        }

      }

    }

    if(facecodes & 0x8) {

      /* Check MAX Y Plane. */
      t = -((ray->o[1] - vx->bounds[MAXY]) * one_over_d);

      if(t >= 0.0 && t <= 1.0) {

        ix = ray->o[0] + (ray->d[0] * t);
        iz = ray->o[2] + (ray->d[2] * t);

        if(ix >= vx->bounds[MINX] && ix <= vx->bounds[MAXX] &&
           iz >= vx->bounds[MINZ] && iz <= vx->bounds[MAXZ]) {

          *res |= (1 << UP);

        }

      }

    }

  }

  if(*res)
    return(TRUE);
  else
    return(FALSE);

}

BOOL vx_IntersectVoxel(struct Voxel *vox,struct Ray *ray,BOOL bcheck)
{

  short i;

  float tst[3];
  float t1,t2;
	float nearmaxt;             /* Nearest of the far planes. */
	float farmint;              /* Farthest of the near planes. */
	float farval,nearval;       /* Parameters for the nearest and farthest planes. */
	short nearplane,farplane;   /* near and far plane ids */
	short n,f;                  /* Temporary near and far values. */
  short samehitplane[3],samehitvals;

  /*
  ** Check the voxel planes on each axis.
  */
	nearmaxt = INFINITY;
	farmint = NEGINFINITY;
	nearplane = -1;
	farplane = -1;
  f = -1;

  samehitvals = 0;

	for(i = 0; i < 3; i++) {

    if(ray->d[i] != 0.0) {

      t1 = (vox->bounds[MINX + i] - ray->o[i]) * ray->rd[i];
      t2 = (vox->bounds[MAXX + i] - ray->o[i]) * ray->rd[i];
			
      if(t2 > t1) { /* MAX is farther that MIN */

        if(t2 < 0.0) return(FALSE);	/* Both planes behind ray */

				farval = t2;
				nearval = t1;
	
				n = MINX + i;
				f = MAXX + i;
				
			}
			else {				/* MIN is farther than MAX */

				if(t1 < 0.0) return(FALSE);	/* Both planes behind ray */

				farval = t1;
				nearval = t2;

				n = MAXX + i;
				f = MINX + i;

			}
	
      if(farval == nearmaxt) {

        samehitvals++;  
        samehitplane[samehitvals] = f;

      }
      else {

  			if(farval < nearmaxt) {
					
          samehitplane[0] = f;	
          samehitvals = 0;
	  			nearmaxt = farval;
		  		farplane = f;

			  }

      }

			if(nearval > farmint) {

				farmint  = nearval;
				nearplane = n;

			}

    }

  }

  /*
  ** If f is negative one, the ray has a zero length direction vector.
  */
  if(f == -1) return(FALSE);

  /*
  ** Test to see if the ray went through a voxel edge or corner.
  ** If samehitvals == 1, the ray went through an edge, if samehitvals == 2,
  ** the ray went through a corner.
  */
  if(samehitvals > 0) {

    farplane = 0;
    for(i = 0; i <= samehitvals; i++) {

      farplane |= (1 << samehitplane[i]);

    }

    farplane += 100;

    bcheck = FALSE;

  }

  /*
  ** Find Near Plane.
  */
  vox->hitsides[0] = nearplane;
  vox->hitparms[0] = farmint;

  /*
  ** Find Far Plane.
  */
  vox->hitsides[1] = farplane;
  vox->hitparms[1] = nearmaxt;

  if(!bcheck) return(TRUE);


  /*
  ** Check to see if the intersection with the farthest voxel plane is actually
  ** inside the voxel bounds.
  */
  if(farplane > 2) farplane -= 3;

  if(farplane == 0) {

    tst[1] = ray->o[1] + (ray->d[1] * nearmaxt);
    tst[2] = ray->o[2] + (ray->d[2] * nearmaxt);

    if(tst[1] < vox->bounds[MINY] || tst[1] > vox->bounds[MAXY]) return(FALSE);
    if(tst[2] < vox->bounds[MINZ] || tst[2] > vox->bounds[MAXZ]) return(FALSE);

  }
  else 
  if(farplane == 1) {

    tst[0] = ray->o[0] + (ray->d[0] * nearmaxt);
    tst[2] = ray->o[2] + (ray->d[2] * nearmaxt);

    if(tst[0] < vox->bounds[MINX] || tst[0] > vox->bounds[MAXX]) return(FALSE);
    if(tst[2] < vox->bounds[MINZ] || tst[2] > vox->bounds[MAXZ]) return(FALSE);

  }
  else 
  if(farplane == 2) {

    tst[0] = ray->o[0] + (ray->d[0] * nearmaxt);
    tst[1] = ray->o[1] + (ray->d[1] * nearmaxt);

    if(tst[0] < vox->bounds[MINX] || tst[0] > vox->bounds[MAXX]) return(FALSE);
    if(tst[1] < vox->bounds[MINY] || tst[1] > vox->bounds[MAXY]) return(FALSE);

  }

  return(TRUE);

}
