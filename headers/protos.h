#ifndef INCLUDE_PROTOS_H
#define INCLUDE_PROTOS_H

/* lightrender.c */
void pt_RenderLights(struct Ray *ray);

/* prefs.c */
short pt_loadprefs(void);

/* ptrace.c */
void pt_execute(void);

/* interphong.c */
void InterPhong(struct Object *obj,float *nestime,float *noriginal);

/* init.c */
void pt_initialize(void);
void pt_cleanup(void);

/* init_env.c */
short pt_init_environment(void);
void pt_cleanup_environment(void);
void pt_FreeObject(struct Object *obj);
void pt_FreeAttribute(struct Attribute *attr);

/* init_database.c */
short pt_init_database(void);
void pt_cleanup_database(void);
void pt_init_camera(struct Camera *cam);
BOOL pt_SetCurrentCamera(char *cameraname);

/* Fuzzy.c */
void fz_trace_ray(struct Ray *ray,struct Voxel *vox,float smoothness,long maxsamples);

/* Mapping.c */
struct ImageBuffer *im_FindImage(char *name);
short im_LoadRAWColorMap(char *filename,struct ImageBuffer *im,short xsize,short ysize);

void im_ProcessFlatMap(struct Texture *txt,struct TextureArg *);
void im_ProcessSphericalMap(struct Texture *txt,struct TextureArg *);
void im_ProcessEnvironmentMap(struct SphericalMap *,struct Ray *);
void getmapcolor(struct ImageBuffer *imap,float *uv,float *color);
void getmapnormal(struct ImageBuffer *imap,float *uv,float *norm);
BOOL getmapclip(struct ImageBuffer *imap,float *uvval);
float getmaptransparency(struct ImageBuffer *imap,float *uvval);
float getmapFVAL(struct ImageBuffer *imap,float u,float v);
void getmapRGB(struct ImageBuffer *imap,long x,long y,float *co);

/* framestore.c */
short pt_init_framestore(void);
void pt_cleanup_framestore(void);
void pt_savepixel(float *pos,float *value);

/* noise.c */
float noise3(float *x);

/* scene_io.c */
short pt_loadscene(char *);

/* List.c */
struct Min_Node *remtail(struct Min_List *list);
void addtail(struct Min_List *list,struct Min_Node *nd);
void addhead(struct Min_List *list,struct Min_Node *nd);
BOOL IsListEmpty(struct Min_List *l);
void newlist(struct Min_List *l);
void removenode(struct Min_Node *node);

/* math.c */
void ray_reflect(float *I,float *N,float *R);
short ray_refract(float *I,float *N,float *R,float eta);
float fresnel(float nt,float ni,float *V,float *N,float *R);
void set_length(float *V,float len);
float normalize(float *V);
void mix(float *color0,float *color1,float value,float *colorout);
void identity(struct Matrix44 *m);
void multiply(struct Matrix44 *trafo,struct Matrix44 *m);
void copymatrix(struct Matrix44 *trafo,struct Matrix44 *m);
void addtranslation(struct Matrix44 *m,float xlate[]);
void vertmult(float *pin,float *pout,struct Matrix44 *m);
void rotatearbitrary(float *v, float *axis, float *origin, float amt);
void perturbnormal(float *norm,float am);
BOOL check_pointcontain(float pnt[],float bounds[]);

/* lights.c */
void lsh_PointSourceHandler(struct Ray *ray,struct LightSource *ls,struct TextureArg *);
void lsh_DistantSourceHandler(struct Ray *ray,struct LightSource *ls,struct TextureArg *);
void lsh_AmbientSourceHandler(struct Ray *ray,struct LightSource *ls,struct TextureArg *);
void lsh_SpotSourceHandler(struct Ray *ray,struct LightSource *ls,struct TextureArg *);
void lsh_SphericalSourceHandler(struct Ray *ray,struct LightSource *ls,struct TextureArg *);
float soft_shadow(struct Ray *ray,struct LightSource *ls,struct Voxel *startvox);

/* eyerayhandlers.c */
void rayhandler_simple(struct Ray *ray,struct AAInfo *);
void rayhandler_stochastic_low(struct Ray *ray,struct AAInfo *);
void rayhandler_stochastic_medium(struct Ray *ray,struct AAInfo *);
void rayhandler_stochastic_high(struct Ray *ray,struct AAInfo *);

/* ray.c */
void pt_trace_fuzzyray(struct Ray *ray,struct Voxel *startvox);
void pt_trace_ray(struct Ray *ray,struct Voxel *startvox);
struct Ray *pt_InitRay(struct Ray *ray,short depth);

/* intersect.c */
void pt_GetIntersection(struct Voxel *vox,struct Ray *ray);
void pt_CheckAlwaysTestList(struct Ray *ray);
struct Voxel *pt_GetInternalVoxelIntersection(struct Voxel *vox,struct Ray *ray);
struct Voxel *pt_GetNeighborVoxel(struct Voxel *vox,short hitvalue);

/* skyshaders.c */
void skyshader_simple(struct Ray *ray);
void skyshader_gradient(struct Ray *ray);
void skyshader_environmentmap(struct Ray *ray);

/* attributes.c */
void attr_InitAttributes(void);
void *attr_findattribute(struct Min_List *l,char *name);
void *attr_FindTexture(char *name);
void attr_InitEggShellWhite(void);

/* voxelbuild.c */
void vxb_InitVoxObjList(void);
void vxb_FreeVoxObjList(void);
struct VoxObject *vxb_GetFreeVoxObj(void);
void vxb_FreeVoxObj(struct VoxObject *vxo);
short pt_buildvoxeltree(void);
void pt_freevoxeltree(void);
short BuildRootVoxel(void);
short ProcessVoxel(struct Voxel *vox,unsigned short currentdepth);
void vx_PreprocessSubvoxels(struct Voxel *vox,struct Voxel *voxellist);
void FreeVoxelTree(struct Voxel *TopVoxel);

/* voxelsmooth.c */
void vs_SmoothVoxelList(struct Voxel *RootVoxel);
void vs_SmoothVoxel(struct Voxel *voxel);
struct Voxel *vs_FindVoxelNeighbor(struct Voxel *start,unsigned short voxdir,short leveldiff);
struct Voxel *vs_GetSubVoxel(struct Voxel *currentvox,long id);

/* triangle.c */
void tr_GetExtents(struct prim_Triangle *tr,float *ext,struct Matrix44 *matrix);
unsigned long tr_IsVoxelIntersect(struct prim_Triangle *tr,struct Voxel *vox,struct Object *obj);
BOOL tr_IsRayIntersect(struct VoxObject *,struct Ray *ray);
BOOL tr_SimpleIsRayIntersect(struct VoxObject *vxo,struct Ray *ray);
BOOL tr_InitTriangle(struct prim_Triangle *tr);

/* sphere.c */
void sp_GetExtents(struct prim_Sphere *sp,float *ext,struct Matrix44 *matrix);
BOOL sp_IsRayIntersect(struct VoxObject *,struct Ray *ray);
unsigned long sp_IsVoxelIntersect(struct prim_Sphere *sp,struct Voxel *vox,struct Object *obj);

/* plane.c */
unsigned long pl_IsVoxelIntersect(struct prim_Plane *pl,struct Voxel *vox,struct Object *obj);
BOOL pl_IsRayIntersect(struct VoxObject *,struct Ray *ray);

/* voxel.c */
struct Voxel *vx_CheckPointInVoxel(struct Voxel *start,float pnt[]);
BOOL vx_CheckLineIntersect(struct Voxel *vx,struct Ray *ray,unsigned long *,unsigned long);
BOOL vx_FindClosestHitPlane(float *minB,struct Ray *ray,struct VoxInt *vi);
BOOL vx_FindFarthestHitPlane(float bounds[],struct Ray *ray,struct VoxInt *vi,BOOL boundcheck);
BOOL vx_FindClosestHitPlane2(float bounds[],struct Ray *ray,struct VoxInt *vi,BOOL boundcheck);
BOOL check_voxelcontain(struct Voxel *vox,struct Ray *ray);
BOOL vx_IntersectVoxel(struct Voxel *,struct Ray *ray,BOOL bcheck);

/* shade_engine.c */
void sh_Shade_Ray(struct Ray *ray);
void sh_GetModifiedSurface(struct TextureArg *,struct Attribute *);
void sh_GetLightSourceContributions(struct Ray *ray,struct TextureArg *);
void sh_GetSecondaryRayContributions(struct Ray *ray,struct Attribute *,struct TextureArg *);
void sh_ApplyVolumeAttenuation(struct Ray *ray,struct Attribute *attr);

/* fsqrt.c */
void init_sqrt_tab(void);
float fsqrt(float f);

/* TextureTools.c */
float trianglewave(float val);
void make_waves(struct Texture *txt,struct TextureArg *);
void checkerboard(struct Texture *txt,struct TextureArg *);
void clouds(struct Texture *,struct Attribute *,struct Ray *);
void marble2(struct Texture *txt,struct TextureArg *);
void marble3(struct Texture *txt,struct TextureArg *);
void brick(struct Texture *txt,struct TextureArg *);
void wrinkle(struct Texture *txt,struct TextureArg *);
void wood1(struct Texture *txt,struct TextureArg *);
void wood2(struct Texture *txt,struct TextureArg *);
void wood3(struct Texture *txt,struct TextureArg *);
void snow(struct Texture *txt,struct TextureArg *);
void granite(struct Texture *txt,struct TextureArg *);
void bumps(struct Texture *txt,struct TextureArg *);
void clouds2(struct Texture *txt,struct TextureArg *);
void swirls(struct Texture *txt,struct TextureArg *);
void spotty(struct Texture *txt,struct TextureArg *);
void acid(struct Texture *txt,struct TextureArg *);
void wire(struct Texture *txt,struct TextureArg *);

/* Shadows.c */
BOOL pt_FilterShadowRay(struct Ray *ray,struct Ray *sray,struct LightSource *ls);
void pt_GetShadowIntersection(struct Voxel *vox,struct Ray *ray);
void pt_ShadowCheckAlwaysTestList(struct Ray *ray);
short pt_CheckShadowCache(struct Ray *ray,struct LightSource *ls);

/* Smooth.c */
void sm_SmoothObject(struct Object *obj);

/* io_Parser.c */
void ungetbufferchar(char c,struct parseinfo *pi);
char getbufferchar(struct parseinfo *pi,FILE *file);
short match_token(char *cur_text);
void get_keyword(FILE *yyin,struct parseinfo *pi);
float get_number(FILE *file,struct parseinfo *pi);
short get_token(FILE *yyin,struct parseinfo *pi);

/* io_PPI.c */
void io_LoadScene_SIRES(FILE *file,short *error);
short parsemain(FILE *file,struct parseinfo *pi);

short parsecamera(FILE *file,struct parseinfo *pi);
short parselight(FILE *file,struct parseinfo *pi);
short parseattribute(FILE *file,struct parseinfo *pi);
short parsesphere(FILE *file,struct Object *obj,struct parseinfo *pi);
short parseplane(FILE *file,struct Object *obj,struct parseinfo *pi);
short parsepolygon(FILE *file,struct Object *obj,struct parseinfo *pi);
short parseprimitive(FILE *file,struct Object *obj,struct parseinfo *pi);
short parseobject(FILE *file,struct parseinfo *pi);
short parsesculptimage(FILE *file,struct ImageBuffer *ib,struct parseinfo *pi);
short parseimage(FILE *file,struct parseinfo *pi);
short parsebackdrop(FILE *file,struct parseinfo *pi);

struct VertexNode *GetTvertex(struct Object *obj,float *coords);
BOOL io_CheckForDuplicateTriangle(struct Object *obj,struct prim_Triangle *tr);
struct Object *findobject(char *name);

/* io_Textures.c */
short parsetexture(FILE *file,struct parseinfo *pi);
short parseflatmap(FILE *file,struct Texture *tex,struct parseinfo *pi);
short parsesphericalmap(FILE *file,struct Texture *tex,struct parseinfo *pi);
short parsemap(FILE *file,struct Texture *tex,struct parseinfo *pi);
short parsewood2texture(FILE *file,struct Texture *tex,struct parseinfo *pi);
short parsewood3texture(FILE *file,struct Texture *tex,struct parseinfo *pi);
short parsemarble2texture(FILE *file,struct Texture *tex,struct parseinfo *pi);
short parsemarble3texture(FILE *file,struct Texture *tex,struct parseinfo *pi);
short parseswirlstexture(FILE *file,struct Texture *tex,struct parseinfo *pi);
short parsespottytexture(FILE *file,struct Texture *tex,struct parseinfo *pi);
short parseacidtexture(FILE *file,struct Texture *tex,struct parseinfo *pi);
short parsewiretexture(FILE *file,struct Texture *tex,struct parseinfo *pi);
short parsesnowtexture(FILE *file,struct Texture *tex,struct parseinfo *pi);

/* BackDrop.c */
void bd_BackdropImage(struct Ray *ray);

/* ILLUMINATION MODEL FUNCTIONS. */
void im_Strauss(struct TextureArg *,struct LightSource *ls,struct Ray *sray);
void im_GetSurfaceZR(struct TextureArg *,struct Ray *ray,struct Ray *reflect);

/* oem.c (FUNCTIONS THAT MAY NEED TO BE PORTED.) */
short pt_init_oem(void);
void pt_cleanup_oem(void);
void oem_debugout(char *message);
void oem_raystart(struct Ray *ray,float *position,struct AAInfo *);
BOOL oem_rayend(void);
void oem_debugout(char *message);
void oem_initframebuffer(void);
void *oem_AllocMem(unsigned long size);
void oem_FreeMem(void *ptr,unsigned long size);
void oem_setpixel(short,short,unsigned long val);
void oem_savebuffer(void);
void oem_StartTimer(void);
void oem_StopTimer(unsigned long *secs);
void oem_clearmemory(unsigned char *buffer,unsigned long size);

#endif /* INCLUDE_PROTOS_H */
