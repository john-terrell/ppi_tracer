/*
** Triangle.c
**
** 14 Sep 1992 - Created    - J. Terrell
**
*/
#include "Headers/Tracer.h"

/* tr_GetExtents() ; Finds the extents of a triangle and fills out the ext array */
void tr_GetExtents(struct prim_Triangle *tr,float *ext,struct Matrix44 *matrix)
{

  struct tvertex *t;
  float co[3];
  float xl,yl,zl,xh,yh,zh;
  short i;

  xl = yl = zl = INFINITY;
  xh = yh = zh = NEGINFINITY;

  for(i = 0; i < 3; i++) {

    switch(i) {

      case 0:
        t = (struct tvertex *)tr->tri_Vert1;
        break;

      case 1:
        t = (struct tvertex *)tr->tri_Vert2;
        break;

      case 2:
        t = (struct tvertex *)tr->tri_Vert3;
        break;

    }

    if(matrix) {

      vertmult(t->p,co,matrix);

    }
    else {

      co[0] = t->p[0];
      co[1] = t->p[1];
      co[2] = t->p[2];

    }

    if(co[0] > xh) xh = co[0];
    else if(co[0] < xl) xl = co[0];

    if(co[1] > yh) yh = co[1];
    else if(co[1] < yl) yl = co[1];

    if(co[2] > zh) zh = co[2];
    else if(co[2] < zl) zl = co[2];

  }

  ext[0] = xl;
  ext[1] = xh;
  ext[2] = yl;
  ext[3] = yh;
  ext[4] = zl;
  ext[5] = zh;

}

/* tr_IsVoxelTriangleIntersect() ; Determine if a voxel and triangle intersect. */
unsigned long tr_IsVoxelIntersect(struct prim_Triangle *tr,struct Voxel *vox,
  struct Object *obj)
{

  unsigned long contained1,contained2,contained3;
  unsigned long facecode;   /* Containment results */
  unsigned long intersect;
  struct tvertex v1,v2,v3,*p1,*p2,*p3;
  struct Ray edgeray;
  struct VoxObject vxo;
  BOOL reset;

  p1 = (struct tvertex *)tr->tri_Vert1;
  p2 = (struct tvertex *)tr->tri_Vert2;
  p3 = (struct tvertex *)tr->tri_Vert3;

  if(obj->obj_Local2World) {

    vertmult(p1->p,v1.p,obj->obj_Local2World);
    vertmult(p2->p,v2.p,obj->obj_Local2World);
    vertmult(p3->p,v3.p,obj->obj_Local2World);

    p1 = &v1;
    p2 = &v2;
    p3 = &v3;

  }

  /*
  ** Trivial Accept test. Test 1
  */
  contained1 = 0;
  contained2 = 0;
  contained3 = 0;

  if(p1->p[0] < vox->bounds[MINX]) contained1 |= 0x1;
  if(p2->p[0] < vox->bounds[MINX]) contained2 |= 0x1;
  if(p3->p[0] < vox->bounds[MINX]) contained3 |= 0x1;

  if(p1->p[0] > vox->bounds[MAXX]) contained1 |= 0x2;
  if(p2->p[0] > vox->bounds[MAXX]) contained2 |= 0x2;
  if(p3->p[0] > vox->bounds[MAXX]) contained3 |= 0x2;

  if(p1->p[1] < vox->bounds[MINY]) contained1 |= 0x4;
  if(p2->p[1] < vox->bounds[MINY]) contained2 |= 0x4;
  if(p3->p[1] < vox->bounds[MINY]) contained3 |= 0x4;

  if(p1->p[1] > vox->bounds[MAXY]) contained1 |= 0x8;
  if(p2->p[1] > vox->bounds[MAXY]) contained2 |= 0x8;
  if(p3->p[1] > vox->bounds[MAXY]) contained3 |= 0x8;

  if(p1->p[2] < vox->bounds[MINZ]) contained1 |= 0x10;
  if(p2->p[2] < vox->bounds[MINZ]) contained2 |= 0x10;
  if(p3->p[2] < vox->bounds[MINZ]) contained3 |= 0x10;

  if(p1->p[2] > vox->bounds[MAXZ]) contained1 |= 0x20;
  if(p2->p[2] > vox->bounds[MAXZ]) contained2 |= 0x20;
  if(p3->p[2] > vox->bounds[MAXZ]) contained3 |= 0x20;

  /*
  ** Trivial Accept Test.  (Triangle Completely inside voxel)
  */
  if(!contained1 && !contained2 && !contained3) return(0x0000FFFF);

  /*
  ** Trivial Accept Test.  (Triangle Partially inside voxel)
  */
  if(!contained1 || !contained2 || !contained3) return(0xFFFFFFFF);

  /*
  ** Trivial Reject Test.
  */
  if(contained1 & contained2 & contained3) return(0x00000000);

  /*
  ** Triangle Edge Tests.
  */
  if(!(contained1 & contained2)) {

    /* Test edge 0 -> 1 */
    edgeray.o[0] = p1->p[0];
    edgeray.o[1] = p1->p[1];
    edgeray.o[2] = p1->p[2];

    edgeray.d[0] = p2->p[0] - p1->p[0];
    edgeray.d[1] = p2->p[1] - p1->p[1];
    edgeray.d[2] = p2->p[2] - p1->p[2];

    if(vx_CheckLineIntersect(vox,&edgeray,&intersect,(contained1 | contained2)))
      return(intersect);

  }

  if(!(contained2 & contained3)) {

    /* Test edge 1 -> 2 */
    edgeray.o[0] = p2->p[0];
    edgeray.o[1] = p2->p[1];
    edgeray.o[2] = p2->p[2];

    edgeray.d[0] = p3->p[0] - p2->p[0];
    edgeray.d[1] = p3->p[1] - p2->p[1];
    edgeray.d[2] = p3->p[2] - p2->p[2];

    if(vx_CheckLineIntersect(vox,&edgeray,&intersect,(contained2 | contained3)))
      return(intersect);

  }

  if(!(contained1 & contained3)) {

    /* Test edge 0 -> 2 */
    edgeray.o[0] = p1->p[0];
    edgeray.o[1] = p1->p[1];
    edgeray.o[2] = p1->p[2];

    edgeray.d[0] = p3->p[0] - p1->p[0];
    edgeray.d[1] = p3->p[1] - p1->p[1];
    edgeray.d[2] = p3->p[2] - p1->p[2];

    if(vx_CheckLineIntersect(vox,&edgeray,&intersect,(contained1 | contained3)))
      return(intersect);

  }

  /*
  ** Triangle edge test complete.  If we made it here, none of the triangle's edges
  ** intersected with a voxel boundary.  We must now execute the final test.
  */

  /*
  ** Voxel face diagonal intersection test.
  */
  vxo.vxo_Prim = (struct Primitive *) tr;
  vxo.vxo_Obj = obj;

  edgeray.maxt = 1.0;

  reset = TRUE;

  facecode = (contained1 ^ 0xFF) | (contained2 ^ 0xFF) | (contained3 ^ 0xFF);

  /* North Diagonal */
  if(facecode & 0x20) {

    edgeray.o[0] = vox->bounds[MINX];
    edgeray.o[1] = vox->bounds[MINY];
    edgeray.o[2] = vox->bounds[MAXZ];

    edgeray.d[0] = vox->bounds[MAXX] - edgeray.o[0];
    edgeray.d[1] = vox->bounds[MAXY] - edgeray.o[1];
    edgeray.d[2] = 0.0;

    if(tr_SimpleIsRayIntersect(&vxo,&edgeray)) return(0xFFFFFFFF);
    reset = FALSE;

  }

  /* South Diagonal */
  if(facecode & 0x10) {

    if(reset) {

      edgeray.o[0] = vox->bounds[MINX];
      edgeray.o[1] = vox->bounds[MINY];

      edgeray.d[0] = vox->bounds[MAXX] - edgeray.o[0];
      edgeray.d[1] = vox->bounds[MAXY] - edgeray.o[1];
      edgeray.d[2] = 0.0;

    }

    edgeray.o[2] = vox->bounds[MINZ];

    if(tr_SimpleIsRayIntersect(&vxo,&edgeray)) return(0xFFFFFFFF);

  }

  reset = TRUE;

  /* East Diagonal */
  if(facecode & 0x02) {

    edgeray.o[0] = vox->bounds[MAXX];
    edgeray.o[1] = vox->bounds[MINY];
    edgeray.o[2] = vox->bounds[MINZ];

    edgeray.d[0] = 0.0;
    edgeray.d[1] = vox->bounds[MAXY] - edgeray.o[1];
    edgeray.d[2] = vox->bounds[MAXZ] - edgeray.o[2];

    if(tr_SimpleIsRayIntersect(&vxo,&edgeray)) return(0xFFFFFFFF);

    reset = FALSE;

  }

  /* West Diagonal */
  if(facecode & 0x01) {

    if(reset) {

      edgeray.o[1] = vox->bounds[MINY];
      edgeray.o[2] = vox->bounds[MINZ];

      edgeray.d[0] = 0.0;
      edgeray.d[1] = vox->bounds[MAXY] - edgeray.o[1];
      edgeray.d[2] = vox->bounds[MAXZ] - edgeray.o[2];

    }

    edgeray.o[0] = vox->bounds[MINX];

    if(tr_SimpleIsRayIntersect(&vxo,&edgeray)) return(0xFFFFFFFF);

  }

  return(0x0);

}

BOOL tr_IsRayIntersect(struct VoxObject *vxo,struct Ray *ray)
{

  float u0,u1,u2,v0,v1,v2;
  float nd,t;
  float beta,alpha;
  float P[3];   /* Intersection point. */
  float tmp[3];
  float *norm,nrm[3],nrm2[3];

  struct prim_Triangle *tr;

  struct tvertex *tv0,*tv1,*tv2,p0,p1,p2;
  short i1,i2;

  if(ray->shootingvxo == vxo) return(FALSE);

  tr = (struct prim_Triangle *)vxo->vxo_Prim;

  tv0 = (struct tvertex *) tr->tri_Vert1;
  tv1 = (struct tvertex *) tr->tri_Vert2;
  tv2 = (struct tvertex *) tr->tri_Vert3;

  norm = tr->tri_Normal;

  i1 = tr->tri_DominantAxis1;
  i2 = tr->tri_DominantAxis2;

  if(vxo->vxo_Obj->obj_Local2World) {

    nrm[0] = tv0->p[0] + norm[0];
    nrm[1] = tv0->p[1] + norm[1];
    nrm[2] = tv0->p[2] + norm[2];

    vertmult(tv0->p,p0.p,vxo->vxo_Obj->obj_Local2World);
    vertmult(tv1->p,p1.p,vxo->vxo_Obj->obj_Local2World);
    vertmult(tv2->p,p2.p,vxo->vxo_Obj->obj_Local2World);
    vertmult(nrm,nrm2,vxo->vxo_Obj->obj_Local2World);

    nrm2[0] -= p0.p[0];
    nrm2[1] -= p0.p[1];
    nrm2[2] -= p0.p[2];

    tv0 = &p0;
    tv1 = &p1;
    tv2 = &p2;

    norm = nrm2;

  }
  
  /*
  ** See if ray and triangle are parallel.
  */
  if((nd = MDOT(ray->d,norm)) == 0.0) return(FALSE);

  /*
  ** Find ray parameter where intersection with containing plane occurs.
  */
  SUB(tv0->p,ray->o,tmp);
  t = MDOT(tmp,norm) / nd;

  /*
  ** ray parameter bounds check
  */
  if(t < 0.005)       return(FALSE);  /* Intersection is behind ray */
  if(t >= ray->t)     return(FALSE);  /* A closer intersection has already been found */
  if(t > ray->maxt)   return(FALSE);  /* Intersection beyond max allowed. */

  MRAYPOS(ray->o,ray->d,t,P); 

  u0 = P[i1] - tv0->p[i1];
  v0 = P[i2] - tv0->p[i2];

  u1 = tv1->p[i1] - tv0->p[i1];
  u2 = tv2->p[i1] - tv0->p[i1];
  v1 = tv1->p[i2] - tv0->p[i2];
  v2 = tv2->p[i2] - tv0->p[i2];

  if(u1 == 0.0) {

    beta = u0 / u2;

    if(beta < 0.0 || beta > 1.0) return(FALSE);

    alpha = (v0 - beta * v2) / v1;

  }
  else {

    beta = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);

    if(beta < 0.0 || beta > 1.0) return(FALSE);

    alpha = (u0 - beta * u2) / u1;

  }

  if(alpha < 0.0 || (alpha + beta) > 1.0) return(FALSE);

  ray->t = t;
  ray->vxo = vxo;
  ray->ip[0] = P[0];
  ray->ip[1] = P[1];
  ray->ip[2] = P[2];

  if(vxo->vxo_Obj->obj_Flags & OFLG_NOSMOOTH) {

    ray->norm[0] = norm[0];
    ray->norm[1] = norm[1];
    ray->norm[2] = norm[2];

    ray->flags |= RFLAG_NORMAL;

  }
  else {

    u2 = 1.0 - (alpha + beta);

    ray->norm[0] = (u2 * tv0->n[0]) + (alpha * tv1->n[0]) + (beta * tv2->n[0]);
    ray->norm[1] = (u2 * tv0->n[1]) + (alpha * tv1->n[1]) + (beta * tv2->n[1]);
    ray->norm[2] = (u2 * tv0->n[2]) + (alpha * tv1->n[2]) + (beta * tv2->n[2]);

    InterPhong(vxo->vxo_Obj,ray->norm,norm);

    ray->flags &= ~RFLAG_NORMAL;

  }

  return(TRUE);

}

BOOL tr_SimpleIsRayIntersect(struct VoxObject *vxo,struct Ray *ray)
{

  struct prim_Triangle *tr;
  struct tvertex *tv0,*tv1,*tv2,p0,p1,p2;
  float u0,u1,u2,v0,v1,v2;
  float nd,t;
  float beta,alpha;
  float tmp[3],P[3];

  tr = (struct prim_Triangle *)vxo->vxo_Prim;

  if(vxo->vxo_Obj->obj_Local2World) {

    vertmult(tr->tri_Vert1,p0.p,vxo->vxo_Obj->obj_Local2World);
    vertmult(tr->tri_Vert2,p1.p,vxo->vxo_Obj->obj_Local2World);
    vertmult(tr->tri_Vert3,p2.p,vxo->vxo_Obj->obj_Local2World);

    tv0 = &p0;
    tv1 = &p1;
    tv2 = &p2;

  }
  else {

    tv0 = (struct tvertex *) tr->tri_Vert1;
    tv1 = (struct tvertex *) tr->tri_Vert2;
    tv2 = (struct tvertex *) tr->tri_Vert3;

  }

  /*
  ** See if ray and triangle are parallel.
  */
  if((nd = MDOT(ray->d,tr->tri_Normal)) == 0.0) return(FALSE);

  /*
  ** Find ray parameter where intersection with containing plane occurs.
  */
  SUB(tv0->p,ray->o,tmp);
  t = MDOT(tmp,tr->tri_Normal) / nd;

  /*
  ** ray parameter bounds check
  */
  if(t < 0.0 || t > ray->maxt) return(FALSE);  /* Intersection is behind ray */

  /*
  ** Get intersection point.
  */
  MRAYPOS(ray->o,ray->d,t,P);

  u0 = P[tr->tri_DominantAxis1] - tv0->p[tr->tri_DominantAxis1];
  u1 = tv1->p[tr->tri_DominantAxis1] - tv0->p[tr->tri_DominantAxis1];
  u2 = tv2->p[tr->tri_DominantAxis1] - tv0->p[tr->tri_DominantAxis1];

  v0 = P[tr->tri_DominantAxis2] - tv0->p[tr->tri_DominantAxis2];
  v1 = tv1->p[tr->tri_DominantAxis2] - tv0->p[tr->tri_DominantAxis2];
  v2 = tv2->p[tr->tri_DominantAxis2] - tv0->p[tr->tri_DominantAxis2];

  if(u1 == 0.0) {

    beta = u0/u2;

    if(beta < 0.0 || beta > 1.0) return(FALSE);

    alpha = (v0 - beta * v2) / v1;

  }
  else {

    beta = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);

    if(beta < 0.0 || beta > 1.0) return(FALSE);

    alpha = (u0 - beta * u2) / u1;

  }

  if(alpha < 0.0 || (alpha + beta) > 1.0) return(FALSE);

  return(TRUE);

}

BOOL tr_InitTriangle(struct prim_Triangle *tr)
{

  float v1[3],v2[3];

  SUB(tr->tri_Vert2,tr->tri_Vert1,v1);
  SUB(tr->tri_Vert3,tr->tri_Vert1,v2);

  MCROSS(v1,v2,tr->tri_Normal);

  if(tr->tri_Normal[0] == 0.0 &&
     tr->tri_Normal[1] == 0.0 &&
     tr->tri_Normal[2] == 0.0) {

    return(FALSE);

  }

  normalize(tr->tri_Normal);

  tr->tri_DominantAxis1 = 1;
  tr->tri_DominantAxis2 = 2;

  if(fabs(tr->tri_Normal[1]) >= fabs(tr->tri_Normal[0]) &&
    fabs(tr->tri_Normal[1]) >= fabs(tr->tri_Normal[2])) {

    tr->tri_DominantAxis1 = 0;
    tr->tri_DominantAxis2 = 2;

  }
  else
  if(fabs(tr->tri_Normal[2]) >= fabs(tr->tri_Normal[0]) &&
    fabs(tr->tri_Normal[2]) >= fabs(tr->tri_Normal[1])) {

    tr->tri_DominantAxis1 = 0;
    tr->tri_DominantAxis2 = 1;

  }

  tr->tri_Prim.prm_Flags = PTYPE_TRIANGLE;

  return(TRUE);

}
