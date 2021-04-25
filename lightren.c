/*
** LightRender.c
**
** 13-Jan-93 - Created    - J. Terrell
**
*/
#include "Headers/Tracer.h"

void pt_RenderLights(struct Ray *ray)
{

  struct LightSource *ls;
  short ltype;
  float OC[3],radius;
  float P[3],D[3],ldist,tca,L2oc;
  float intensity;

  ls = gl_env->LightSourceList.lh_Head;
  while(ls->ls_Node.ln_Succ) {

    ltype = ls->ls_flags & 0xFF;

    if((ltype == LSTYPE_POINT || ltype == LSTYPE_SPHERICAL) && 
      !(ls->ls_flags & (LSFLG_NORENDER | LSFLG_DISABLED))) {

      SUB(ls->ls_position,ray->o,OC);      
      L2oc = MDOT(OC,OC);

      if(MDOT(ray->d,OC) > 0.0) {
  
        radius = ls->ls_radius;
        radius *= radius;

        if(L2oc < radius) {

          ray->ci[0] = ls->ls_color[0];
          ray->ci[1] = ls->ls_color[1];
          ray->ci[2] = ls->ls_color[2];

          break;

        }

        tca = MDOT(OC,ray->d);

        if(tca > 0.0 || (ray->depth == 1)) {

          if((tca > ray->t) && ray->vxo) {

            D[0] = ls->ls_position[0] - ray->ip[0];
            D[1] = ls->ls_position[1] - ray->ip[1];
            D[2] = ls->ls_position[2] - ray->ip[2];

          }
          else {
    
            MRAYPOS(ray->o,ray->d,tca,P);
      
            D[0] = ls->ls_position[0] - P[0];
            D[1] = ls->ls_position[1] - P[1];
            D[2] = ls->ls_position[2] - P[2];

          }
  
          ldist = MLENGTH(D);
        
          if(ldist < ls->ls_radius) {
  
            ray->ci[0] += ls->ls_color[0];
            ray->ci[1] += ls->ls_color[1];
            ray->ci[2] += ls->ls_color[2];

          }
          else {

            ldist -= ls->ls_radius;

            intensity = (ls->ls_intensity * (1.0 / pow(ldist,(ls->ls_decay + 1))));
  
            ray->ci[0] += ls->ls_color[0] * intensity;
            ray->ci[1] += ls->ls_color[1] * intensity;
            ray->ci[2] += ls->ls_color[2] * intensity;

          }

        }

      }

    }

    ls = ls->ls_Node.ln_Succ;

  }  

  ray->ci[0] = MCLAMP(ray->ci[0],0.0,1.0);
  ray->ci[1] = MCLAMP(ray->ci[1],0.0,1.0);
  ray->ci[2] = MCLAMP(ray->ci[2],0.0,1.0);

}
