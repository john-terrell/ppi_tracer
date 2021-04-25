/*
** Init_Database.c
**
** functions to init the database
**
** 17 Aug 1992 - Created          - J. Terrell
*/
#include "headers/tracer.h"

extern struct OemInfo *gl_oem;

short pt_init_database(void)
{

  struct Camera *cam;

  cam = gl_env->Camera;

  cam->cam_Origin[0] = 0.0;
  cam->cam_Origin[1] = 0.0;
  cam->cam_Origin[2] = 1000.0;

  cam->cam_ViewPoint[0] = 0.0;
  cam->cam_ViewPoint[1] = 0.0;
  cam->cam_ViewPoint[2] = 0.0;

  cam->cam_FieldOfView = MRADIANS(60.0);
  cam->cam_Tilt = 0.0;

  attr_InitAttributes();

  return(0);

}

void pt_cleanup_database(void)
{

}

void pt_init_camera(struct Camera *cam)
{

  float dist;

  /*
  ** make assumed view up.
  */
  cam->cam_ViewUp[0] = 0.0;
  cam->cam_ViewUp[1] = 1.0;
  cam->cam_ViewUp[2] = 0.0;

  /*
  ** make view dir.
  */
  cam->cam_ViewDir[0] = cam->cam_ViewPoint[0] - cam->cam_Origin[0];
  cam->cam_ViewDir[1] = cam->cam_ViewPoint[1] - cam->cam_Origin[1];
  cam->cam_ViewDir[2] = cam->cam_ViewPoint[2] - cam->cam_Origin[2];

  /*
  ** make view right
  */
  MCROSS(cam->cam_ViewUp,cam->cam_ViewDir,cam->cam_ViewRight);

  /*
  ** make actual view up.
  */
  MCROSS(cam->cam_ViewDir,cam->cam_ViewRight,cam->cam_ViewUp);

  /*
  ** Find center of image plane.
  */

  /* Find distance from camera to center of viewplane. */
  dist = (gl_oem->oem_ImageAspectRatio * 0.5) /
    tan(cam->cam_FieldOfView * 0.5);

  set_length(cam->cam_ViewDir,dist);

  MRAYPOS(cam->cam_Origin,cam->cam_ViewDir,1.0,cam->cam_ViewPlaneCenter);

  /*
  ** Ok, now rotate the viewup and viewright vectors around viewdir to account for
  ** tilt.
  */
  if(cam->cam_Tilt != 0.0) {

    rotatearbitrary(cam->cam_ViewUp,cam->cam_ViewDir,NULL,cam->cam_Tilt);
    rotatearbitrary(cam->cam_ViewRight,cam->cam_ViewDir,NULL,cam->cam_Tilt);

  }

  set_length(cam->cam_ViewRight,gl_oem->oem_ImageAspectRatio);
  set_length(cam->cam_ViewUp,1.0);

  MRAYPOS(cam->cam_ViewPlaneCenter,cam->cam_ViewRight,
    -0.5,cam->cam_ViewPlaneUpperLeft);

  cam->cam_ViewPlaneUpperLeft[0] += (cam->cam_ViewUp[0] * 0.5);
  cam->cam_ViewPlaneUpperLeft[1] += (cam->cam_ViewUp[1] * 0.5);
  cam->cam_ViewPlaneUpperLeft[2] += (cam->cam_ViewUp[2] * 0.5);

  cam->cam_ViewDir[0] = cam->cam_ViewPoint[0] - cam->cam_ViewPlaneCenter[0];
  cam->cam_ViewDir[1] = cam->cam_ViewPoint[1] - cam->cam_ViewPlaneCenter[1];
  cam->cam_ViewDir[2] = cam->cam_ViewPoint[2] - cam->cam_ViewPlaneCenter[2];

  /*
  ** initialize the lens information.
  */
  if(cam->cam_FocalDistance == -1)
    cam->cam_FocalDistance = MLENGTH(cam->cam_ViewDir);

  cam->cam_Aperture = 0.0;

  cam->cam_LensJ[0] = cam->cam_ViewUp[0];
  cam->cam_LensJ[1] = cam->cam_ViewUp[1];
  cam->cam_LensJ[2] = cam->cam_ViewUp[2];

  MCROSS(cam->cam_ViewUp,cam->cam_ViewDir,cam->cam_LensI);

  normalize(cam->cam_LensJ);
  normalize(cam->cam_LensI);

  cam->cam_LensI[0] *= cam->cam_Aperture;
  cam->cam_LensI[1] *= cam->cam_Aperture;
  cam->cam_LensI[2] *= cam->cam_Aperture;

  cam->cam_LensJ[0] *= cam->cam_Aperture;
  cam->cam_LensJ[1] *= cam->cam_Aperture;
  cam->cam_LensJ[2] *= cam->cam_Aperture;

  if(cam->cam_Aperture > 0.0 && !(gl_env->flags & ENVFLAG_NODEPTH)) {

    gl_env->fuzzyrayhandler = pt_trace_fuzzyray;

  }
  else {

    gl_env->fuzzyrayhandler = pt_trace_ray;

  }
}

BOOL pt_SetCurrentCamera(char *cameraname)
{

  struct Camera *cam;

  cam = gl_env->CameraList.lh_Head;
  while(cam->cam_Node.ln_Succ) {

    if(strcmp(cameraname,cam->cam_Name) == 0) {

      gl_env->Camera = cam;
      return(TRUE);

    }

    cam = cam->cam_Node.ln_Succ;

  }

  return(FALSE);

}
