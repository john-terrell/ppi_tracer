#ifndef INCLUDE_CAMERA_H
#define INCLUDE_CAMERA_H

struct Camera {

  struct Full_Node cam_Node;

  char cam_Name[16];

  /*
  ** View parameters (Given)
  */
  float cam_Origin[3];
  float cam_ViewPoint[3];
  float cam_FieldOfView;
  float cam_Tilt;

  /*
  ** View vectors (Calculated)
  */
  float cam_ViewDir[3];
  float cam_ViewUp[3];
  float cam_ViewRight[3];

  /*
  ** Coordinate of center of viewplane (Calculated)
  */
  float cam_ViewPlaneCenter[3];
  float cam_ViewPlaneUpperLeft[3];

  /*
  ** Physical Camera lens info.
  */
  float cam_Aperture;
  float cam_LensI[3];
  float cam_LensJ[3];
  float cam_FocalDistance;

};

#endif /* INCLUDE_CAMERA_H */
