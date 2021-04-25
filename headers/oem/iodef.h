#ifndef INCLUDE_IODEF_H
#define INCLUDE_IODEF_H

#define BUFSIZE 8192

struct parseinfo {

  short cur_token;
  float cur_value;
  char  cur_text[256];

  long  cur_lineno;

  char buffer[BUFSIZE];
  char *bufptr;
  short bufpos;
  short bufsize;

  BOOL lastblock;
  BOOL eof;

};

struct token {

	char    *name;
	short   value;

};

/* punctuation */
#define UNKNOWN         -2
#define END_OF_FILE     -1

#define LEFT_BRACE      10
#define RIGHT_BRACE     11

/*
** Keywords
*/

/* Scene Header */
#define SIRES           100
#define VERSION         101
#define SUBVERSION      102

#define INCLUDE         110

/* Camera */
#define CAMERA          150
#define POSITION        151
#define LOOKAT          152
#define FOV             153
#define TILT            154

/* Lights */
#define LIGHT           180
#define TYPE            181
#define COLOR           182
#define RADIUS          183
#define ANGLE           184
#define INTENSITY       185
#define POINT           186
#define AMBIENT         187
#define SPOT            188
#define SPHERICAL       189
#define DISTANT         190
#define DROPOFF         191

/* Attributes */
#define ATTRIBUTE       220
#define SMOOTHNESS      221
#define METALNESS       222
#define TRANSPARENCY    223
#define REFLECTIVITY    224
#define REFRACTION      225
#define SIMPLE          226
#define COMPLEX         227
#define COLORTEXTURE    228
#define DISPLACEMENTTEXTURE 229
#define FUZZYTRANSMISSIONS  230
#define FUZZYREFLECTIONS    231

/* Objects */
#define OBJECT          300
#define PRIMITIVE       330
#define NOSMOOTHING     331
#define INSTANCEOF      332
#define INSTANCEXFORM   333

#define SPHERE          341
#define ORIGIN          342

#define PLANE           350
#define NORMAL          351

#define POLYGON         370
#define COUNT           371
#define VLIST           372

#define TRANSLATE       390

#define CURRENTCAMERA   500

#define TEXTURE         700
#define SCALE           701
#define TURBSCALE       702
#define THRESHOLD       703

#define SNOW            754
#define CLIPMAP         755
#define WIRE            756
#define ACID            757
#define SWIRLS          758
#define MARBLE2         759
#define MARBLE3         760
#define CHECKER         761
#define GRANITE         762
#define WAVES           763
#define COLORMAP        764
#define BUMPMAP         765
#define WOOD2           766
#define SPOTTY          767
#define WOOD3           768
#define LEVEL           769
#define WIDTH           770
#define SPACING         771

#define IMAGE           900
#define FILENAME        901

#define BACKDROPIMAGE   910
#define OFFSET          911

#define SCULPT          920
#define RESOLUTION      921

#define FLAT            960
#define WRAPTYPE        961
#define CORNEROFFSET    962
#define REPEAT          963

#define DEPTH           1000
#define STICKINESS      1001
#define DIRECTION       1002

/*
** Error Codes
*/
/* General */
#define ERROR_OK                0
#define ERROR_FILENOTFOUND      1
#define ERROR_OUTOFMEMORY       2
#define ERROR_SYNTAXERROR       3

/* Camera Specific */
#define ERROR_UNKNOWNCAMERA     100

/* Primitive Specific */
#define ERROR_BADPOLYGON        200

/* Object Specific */
#define ERROR_NOINSTANCESOURCE  305

/* Texture Specific */
#define ERROR_UNKNOWNTEXTURE    500

/* Attribute Specific */
#define ERROR_UNKNOWNATTRIBUTE  600

/* Image Specific */
#define ERROR_UNKNOWNIMAGE      700

#endif /* INCLUDE_IODEF_H */
