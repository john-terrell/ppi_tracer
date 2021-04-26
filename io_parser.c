/*
** IO_Parser.c
**
** 09 Dec 1992 - Created    - J. Terrell
*/
#include "Headers/Tracer.h"

struct token token_list[] = {

  "}",              RIGHT_BRACE,
  "{",              LEFT_BRACE,
  "EOF",            EOF,

  "Include",        INCLUDE,

  /* Generic */
  "Type",           TYPE,
  "Position",       POSITION,
  "Direction",      LOOKAT,
  "Color",          COLOR,
  "LookAt",         LOOKAT,

  /* Scene Header */
  "SIRES",          SIRES,
  "version",        VERSION,
  "subversion",     SUBVERSION,

  /* Camera Info */
  "Camera",         CAMERA,
  "FOV",            FOV,
  "Tilt",           TILT,

  /* Light Info */
  "Light",          LIGHT,
  "Intensity",      INTENSITY,
  "Radius",         RADIUS,
  "Angle",          ANGLE,
  "Dropoff",        DROPOFF,
  "DISTANT",        DISTANT,
  "LS_DISTANT",     DISTANT,
  "POINT",          POINT,
  "LS_POINT",       POINT,
  "SPOT",           SPOT,
  "LS_SPOT",        SPOT,
  "AMBIENT",        AMBIENT,
  "LS_AMBIENT",     AMBIENT,
  "SPHERICAL",      SPHERICAL,
  "LS_SPHERICAL",   SPHERICAL,

  /* Attribute Info */
  "Attribute",      ATTRIBUTE,
  "Smoothness",     SMOOTHNESS,
  "Metalness",      METALNESS,
  "Transparency",   TRANSPARENCY,
  "Reflectivity",   REFLECTIVITY,
  "Refraction",     REFRACTION,
  "SIMPLE",         SIMPLE,
  "COMPLEX",        COMPLEX,
  "COLORTEXTURE",   COLORTEXTURE,
  "DISPLACEMENTTEXTURE",  DISPLACEMENTTEXTURE,
  "FuzzyReflections", FUZZYREFLECTIONS,
  "FuzzyTransmissions", FUZZYTRANSMISSIONS,

  /* Texture Info */
  "Wire",           WIRE,
  "Acid",           ACID,
  "Texture",        TEXTURE,
  "Scale",          SCALE,
  "TurbScale",      TURBSCALE,
  "Threshold",      THRESHOLD,
  "Marble2",        MARBLE2,
  "Marble3",        MARBLE3,
  "Swirls",         SWIRLS,
  "Spotty",         SPOTTY,
  "Wood2",          WOOD2,
  "Wood3",          WOOD3,
  "ClipMap",        CLIPMAP,
  "ColorMap",       COLORMAP,
  "BumpMap",        BUMPMAP,
  "WrapType",       WRAPTYPE,
  "FLAT",           FLAT,
  "CORNEROFFSET",   CORNEROFFSET,
  "LEVEL",          LEVEL,
  "SPACING",        SPACING,
  "WIDTH",          WIDTH,
  "SPHERICAL",      SPHERICAL,

  /* Object */
  "Object",         OBJECT,
  "Primitive",      PRIMITIVE,
  "NoSmoothing",    NOSMOOTHING,
  "InstanceOf",     INSTANCEOF,
  "InstanceTransform",  INSTANCEXFORM,
  "Translate",      TRANSLATE,

  "SPHERE",         SPHERE,
  "Origin",         ORIGIN,

  "PLANE",          PLANE,
  "Normal",         NORMAL,

  "POLYGON",        POLYGON,
  "Count",          COUNT,
  "VertexList",     VLIST,

  "CurrentCamera",  CURRENTCAMERA,

  "Image",          IMAGE,
  "SCULPT",         SCULPT,
  "FileName",       FILENAME,
  "RESOLUTION",     RESOLUTION,
  "REPEAT",         REPEAT,

  "BackDrop",       BACKDROPIMAGE,
  "OFFSET",         OFFSET,
  "SNOW",           SNOW,
  "DIRECTION",      DIRECTION,
  "DEPTH",          DEPTH,
  "STICKINESS",     STICKINESS,
  
  NULL,0L,

};

void ungetbufferchar(char c,struct parseinfo *pi)
{

  *pi->bufptr--;

  *pi->bufptr = c;
  pi->bufpos--;

}

char getbufferchar(struct parseinfo *pi,FILE *file)
{

  char retval;

  if(pi->bufpos > pi->bufsize) {

    if(pi->lastblock) return((char)EOF);
    else {

      pi->bufsize = fread(pi->buffer,1,BUFSIZE,file);

      pi->bufptr = pi->buffer;
      pi->bufpos = 1;

      if(pi->bufsize == 0) return((char)EOF);

      if(pi->bufsize != BUFSIZE) pi->lastblock = TRUE;
      else pi->lastblock = FALSE;

    }

  }

  retval = *pi->bufptr++;
  pi->bufpos++;

  return(retval);

}

short match_token(char *cur_text)
{

	short i, length,cur_token;
  char *ptr;

  length = 0;
  ptr = cur_text;

  while(*ptr++) length++;

  i = 0;
  while(TRUE) {

		if(strnicmp(cur_text, token_list[i].name, length) == 0) {

			cur_token = token_list[i].value;
			return(cur_token);

		}

		i++;

		if(!token_list[i].name) break;

	}

	return(UNKNOWN);

}

void get_keyword(FILE *yyin,struct parseinfo *pi)
{

  char c;
  short i;

start:

  while(TRUE) {

    c = getbufferchar(pi,yyin);

    if(c == '\n' || c == '\r') pi->cur_lineno++;

    if(!isspace(c)) {

      ungetbufferchar(c,pi);
      break;

    }
	  
	}

  i = 0;
  c = getbufferchar(pi,yyin);
  if(c == '\"') {     /* Quoted string */

    while(TRUE) {

      c = getbufferchar(pi,yyin);

      if(c == '\"') break;
      if(c == EOF) {

        strcpy(pi->cur_text,"EOF");
        break;

      }

      pi->cur_text[i++] = c;

    }

  }
  else {

    if(c == '/') {            /* Possible start of comment */

      c = getbufferchar(pi,yyin);
      if(c == '*') {          /* Its a comment alright! */

        while(TRUE) {

          c = getbufferchar(pi,yyin);

          if(c == EOF) {

            strcpy(pi->cur_text,"EOF");
            return;

          }

          if(c == '\n' || c == '\r') pi->cur_lineno++;
          if(c == '*') {      /* Possible end of comment */

            c = getbufferchar(pi,yyin);
            if(c == '/') goto start;

          }

        }

      }
      else {

        pi->cur_text[i++] = '/';
        pi->cur_text[i++] = c;

      }

    }

    pi->cur_text[i++] = c;

    while(TRUE) {

      c = getbufferchar(pi,yyin);

      if(c == '\n' || c == '\r') pi->cur_lineno++;
          
      if(isspace(c)) break;
      if(c == EOF) {

        strcpy(pi->cur_text,"EOF");
        break;

      }

      pi->cur_text[i++] = c;

    }

  }

  pi->cur_text[i] = 0;              /* null terminate */

}

float get_number(FILE *file,struct parseinfo *pi)
{

  get_keyword(file,pi);

  pi->cur_value = atof(pi->cur_text);

  return(pi->cur_value);

}

short get_token(FILE *yyin,struct parseinfo *pi)
{

  get_keyword(yyin,pi);

  pi->cur_token = match_token(pi->cur_text);

  return(pi->cur_token);

}
