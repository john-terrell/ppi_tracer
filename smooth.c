/*
** Smooth.c
**
** 11 Nov 1992 - Created    - J. Terrell
**
*/
#include "Headers/Tracer.h"

void sm_SmoothObject(struct Object *obj)
{

  struct VertexNode *vn;
  struct prim_Triangle *tr;
  struct tvertex *tv;

  tr = obj->obj_PrimitiveList.lh_Head;
  while(tr->tri_Prim.prm_Node.ln_Succ) {

    if(GET_PRIMTYPE(tr->tri_Prim.prm_Flags) == PTYPE_TRIANGLE) {

      tv = (struct tvertex *)tr->tri_Vert1;

      if(MDOT(tv->n,tr->tri_Normal) != 0.4226 ||
        (tv->n[0] == 0.0 && tv->n[1] == 0.0 && tv->n[2] == 0.0)) {

        tv->n[0] += tr->tri_Normal[0];
        tv->n[1] += tr->tri_Normal[1];
        tv->n[2] += tr->tri_Normal[2];

      }

      tv = (struct tvertex *)tr->tri_Vert2;

      if(MDOT(tv->n,tr->tri_Normal) != 0.4226 ||
        (tv->n[0] == 0.0 && tv->n[1] == 0.0 && tv->n[2] == 0.0)) {

        tv->n[0] += tr->tri_Normal[0];
        tv->n[1] += tr->tri_Normal[1];
        tv->n[2] += tr->tri_Normal[2];

      }

      tv = (struct tvertex *)tr->tri_Vert3;
      
      if(MDOT(tv->n,tr->tri_Normal) > 0.4226 ||
        (tv->n[0] == 0.0 && tv->n[1] == 0.0 && tv->n[2] == 0.0)) {

        tv->n[0] += tr->tri_Normal[0];
        tv->n[1] += tr->tri_Normal[1];
        tv->n[2] += tr->tri_Normal[2];

      }

    }

    tr = tr->tri_Prim.prm_Node.ln_Succ;

  }

  vn = obj->obj_VertexList.lh_Head;
  while(vn->vn_Node.ln_Succ) {

    normalize(vn->vn_tvertex.n);

    vn = vn->vn_Node.ln_Succ;

  }

}
