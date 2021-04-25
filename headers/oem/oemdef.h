#ifndef INCLUDE_OEMDEF_H
#define INCLUDE_OEMDEF_H

struct OemInfo {

  /*
  ** Frame buffer info
  */
  short oem_HorzRes;
  short oem_VertRes;

  /*
  ** Crop window coords. [0,1]
  ** Region of screen to render.
  */
  float oem_CropWindow[4];

  /*
  ** aspect ratios
  **
  **  4/3   NTSC/PAL
  **  5/3   35mm Film
  ** 16/9   HDTV
  ** 2.35/1 PANAVISION
  **
  */
  float oem_ImageAspectRatio;     /* Aspect ratio of image */
  float oem_DisplayAspectRatio;   /* Aspect ratio of display device */

  /*
  ** Horizontal and Vertical frame coordinates.
  */
  float oem_StartH,oem_StartV;
  float oem_EndH,oem_EndV;
  float oem_CurrentH,oem_CurrentV;
  float oem_StepH,oem_StepV;

  /*
  ** Horizontal and Vertical pixel coordinates.
  */
  float oem_StartPx,oem_StartPy;
  float oem_EndPx,oem_EndPy;
  float oem_CurrentPx,oem_CurrentPy;

#ifdef PSRT

  struct TreeNode *oem_CurrentRayTree;
  struct TreeNode *oem_CurrentTreeNode;

#endif

};

#endif /* INCLUDE_OEMDEF_H */
