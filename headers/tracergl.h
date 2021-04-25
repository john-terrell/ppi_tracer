#ifndef INCLUDE_TRACERGLOBALS_H
#define INCLUDE_TRACERGLOBALS_H

extern struct trace_env *gl_env;
extern struct OemInfo *gl_oem;
extern void (*gl_handle_rayintersectresult[])();
extern short (*IsPrimitiveRayIntersect[])(struct VoxObject *,struct Ray *);
extern unsigned long (*IsPrimitiveVoxelIntersect[])(void *,struct Voxel *,struct Object *);
extern void (*GetPrimitiveExtents[])(void *,float *,struct Matrix44 *);
extern void (*LightSourceHandler[])(struct Ray *,struct LightSource *,struct TextureArg *);
extern float softshadow_circledata[19][2];

#endif /* INCLUDE_TRACERGLOBALS_H */
