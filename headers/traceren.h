#ifndef INCLUDE_TRACERENV_H
#define INCLUDE_TRACERENV_H

struct trace_env {

  unsigned long flags;                    /* Raytrace flags */

  /*
  ** Rendering camera information.
  */
  struct Camera *Camera;                  /* The scene's camera, if none is loaded. */
  struct Camera DefaultCamera;            /* The scene's default camera */

  /*
  ** Default voxel information.
  */
  struct Voxel *CameraVoxel;              /* Voxel the camera is in. */
  struct Voxel RootVoxel;                 /* The scene encompassing root voxel */

  struct SphericalMap
    EnvironmentMap;                       /* Global environment map */

  /*
  ** Database lists
  */
  struct Min_List CameraList;                 /* List of possible cameras. */
  struct Min_List ImageList;                  /* List of Internal Images that can be used as maps. */
  struct Min_List ObjectList;                 /* Database object list. */
  struct Min_List LightSourceList;            /* Database light source list. */
  struct Min_List AttributeList;              /* Database attribute list. */
  struct Min_List TextureList;                /* Database texture list. */
  struct Min_List AlwaysTestList;             /* List of Primitives to always test. */

  /*
  ** Voxel build control information.
  */
  long maxvoxelprims;                     /* Max voxel primitive count. */
  long maxvoxeldepth;                     /* Max voxel subdivision. */

  /*
  ** Trace control information.
  */
  long maxtracedepth;                     /* Max trace recursion depth */

  long maxsamples_depthoffield;           /* Max depth of field ray samples. */
  long maxsamples_reflection;             /* Max reflection samples. */
  long maxsamples_transmission;           /* Max transmission samples. */

  /*
  ** Backdrop Image Information.
  */
  struct ImageBuffer *BackDropImage;

  /*
  ** World Information.
  */
  float ZenithColor[3];                   /* Sky color at zenith */
  float HorizonColor[3];                  /* Sky color at horizon */
  float AZenithColor[3];                  /* Sky color at antizenith */
  float BackDropColor[3];                 /* Color for plain backgrounds */
  float HazeColor[3];                     /* Global haze color */
  float HazeDistance;                     /* Distance where hazeout occurs. */

  /*
  ** Patchable rendering functions
  */
  short (*backdrop_shader)(struct Ray *); /* Shades "eye" rays that enter sky */
  void  (*sky_shader)(struct Ray *);      /* Shades rays that enter sky */
  void  (*eyerayhandler)(struct Ray *,struct AAInfo *); /* Called to process an eye ray. */
  void  (*fuzzyrayhandler)(struct Ray *,struct Voxel *);

};

/*
** flags bit values.
*/
#define ENVFLAG_HAZEENABLED   (1 << 31)   /* Haze is turned on. */
#define ENVFLAG_ATLEMPTY      (1 << 13)   /* Always test list is empty. */

#define ENVFLAG_NOSHADOWS     (1 << 26)   /* Shadows are turned off. */
#define ENVFLAG_NOREFLECTIONS (1 << 25)   /* Reflections are turned off. */
#define ENVFLAG_NOREFRACTIONS (1 << 24)   /* Refractions are turned off. */
#define ENVFLAG_NOFUZZYREFL   (1 << 23)   /* Fuzzy Reflections are off. */
#define ENVFLAG_NOFUZZYREFR   (1 << 22)   /* Fuzzy Refractions are off. */
#define ENVFLAG_NOSOFTSHADOWS (1 << 21)   /* Softshadows are off. */
#define ENVFLAG_NODEPTH       (1 << 20)   /* Depth of field is turned off. */

#endif /* INCLUDE_TRACERENV_H */
