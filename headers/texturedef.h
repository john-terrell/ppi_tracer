#ifndef INCLUDE_TEXTUREDEF_H
#define INCLUDE_TEXTUREDEF_H

/*
** Texture Information. (Exists on global texture list.)
*/
struct Texture {

  struct Full_Node tex_Node;

  char tex_Name[16];

  short tex_Type;
  short tex_Flags;

  void (*tex_TextureFunc)(struct Texture *,struct TextureArg *);
  void *tex_TextureData;
  unsigned long tex_TextureDataSize;

};

/*
** Texture entry on attribute shader lists.
*/
struct TextureEntry {

  struct Full_Node te_Node;

  char te_Name[24];

  struct Texture *te_Texture;

};

/*
** Texture argument structure.
*/
struct TextureArg {

  unsigned short ta_Flags;

  float ta_Position[3];       /* Position in texture space. */
  float ta_Normal[3];         /* Normal in texture space. */
  float ta_Color[3];
  float ta_Opacity;
  float ta_Smoothness;
  float ta_Metalness;

  void (*ta_IlluminationModel)
    (struct TextureArg *,struct LightSource *,struct Ray *);

  float ta_OutputColor[3];    /* Final color after illumination model */

};

/*
** ta_Flags bit values.
*/
#define TAFLAG_CLIP (1 << 15) /* Clip this position */

/*
** tex_Type values
*/
#define TXTYPE_COLOR  1   /* Algorithmic Color Texture */
#define TXTYPE_NORM   2   /* Algorithmic Normal Texture */

#define TXTYPE_IMAP   5   /* Image Map */
#define TXTYPE_BMAP   6   /* Bump Map  */
#define TXTYPE_CMAP   7   /* Clip Map  */
#define TXTYPE_TMAP   8   /* Transparency Map */

#define TXTYPE_POST   20  /* Post Texture */

/*
** tex_Flags bit definitions
*/
#define TXFLG_DISABLED  (1 << 15)       /* This texture is disabled */

/* Structure defining info about waves. */
struct wave {

  float center[3];
  float wavelength;
  float phase;
  float amp;
  float damp;

};

struct waveinfo {

  short wavecount;
  void *firstwave;    /* Pointer to an array of waves */

};

/* Structure defining info about a checkerboard texture */
struct checkerinfo {

  float xscale,yscale,zscale;

  float color0[3];
  float color1[3];

};

/* Structure defining info about a cloudy texture. */
struct cloudinfo {

  float background[3],cloud[3];

  float xscale,yscale;
  float xmul,ymul;
  float stepval;
  float offset;
  float maxits;
  float threshold;
  float density;

};

/* Structure defining info about a marble2 texture */
struct marble2info {

  float scale;
  float scale2[3];
  float turbscale;
  float order;

  float b[3];
  float v[3];

};

/* Structure defining info about a marble3 texture */
struct marble3info {

  float scale;
  float turbscale;

  float t[3];

  float c0[3];
  float c1[3];
  float c2[3];
  float c3[3];

};

struct brickinfo {

  float m_size[3];
  float b_size[3];

  float b[3];
  float m[3];

};

struct wrinkleinfo {

  float offset[3];
  float scale;
  float fbnd;
  float fscl;
  float amt;

};

struct wood1info {

  float offset[3];
  float ringspacing;
  float turbscale;
  float squeeze;
  float thickscale;

  float w[3];
  float r[3];

};

struct wood2info {

  float scale;
  float turbscale;
  float order;

  float w[3];
  float r[3];

};

struct wood3info {

  float scale;
  float turbscale;

  float t[3];

  float c1[3];
  float c2[3];
  float c3[3];
  float c4[3];

};

struct snowinfo {

  float depth;
  float stickiness;
  float dir[3];
  float color[3];

};

struct graniteinfo {

  float scale;
  short order;

  float v[3];   /* Vein Color */
  float b[3];   /* Background color */

};

struct bumpinfo {

  float scale;
  float amount;
  float offset[3];

};

struct smokeyinfo {

  float scale;

  float c[3];   /* Cloud color */
  float b[3];   /* Background color */

};

struct swirlsinfo {

  float scale;
  float turbscale;

  float t[3];

  float c1[3];
  float c2[3];
  float c3[3];
  float c4[3];

};

struct spottyinfo {

  float scale;
  float squeeze;
  float d1,d2,d3;

  float a[3],b[3],c[3],d[3];

};

struct acidinfo {

  float scale;
  float scale2[3];
  float turbscale;
  float order;
  float level;

};

struct wireinfo {

  float width[3];
  float spacing[3];

};

#endif /* INCLUDE_TEXTUREDEF_H */
