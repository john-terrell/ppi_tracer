/*
** io_Textures.c
**
** 09 Dec 1992 - Created    - J. Terrell
*/
#include "Headers/Tracer.h"

short parsetexture(FILE *file,struct parseinfo *pi)
{

  struct Texture *tex;
  short error;
  short done;

  tex = oem_AllocMem(sizeof(struct Texture));
  if(!tex) return(ERROR_OUTOFMEMORY);

  get_keyword(file,pi);

  strcpy(tex->tex_Name,pi->cur_text);

  tex->tex_TextureData = 0;
  tex->tex_TextureFunc = 0;
  tex->tex_TextureDataSize = 0;

  addtail(&gl_env->TextureList,(struct Min_Node *)tex);

  done = FALSE;
  error = ERROR_OK;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case TYPE:

        get_token(file,pi);
        switch(pi->cur_token) {

          case COLORMAP:

            tex->tex_Type = TXTYPE_IMAP;
            error = parsemap(file,tex,pi);
            done = TRUE;
            break;

          case BUMPMAP:

            tex->tex_Type = TXTYPE_BMAP;
            error = parsemap(file,tex,pi);
            done = TRUE;
            break;

          case CLIPMAP:

            tex->tex_Type = TXTYPE_CMAP;
            error = parsemap(file,tex,pi);
            done = TRUE;
            break;

          case MARBLE2:

            error = parsemarble2texture(file,tex,pi);
            done = TRUE;
            break;

          case MARBLE3:

            error = parsemarble3texture(file,tex,pi);
            done = TRUE;
            break;

          case WOOD2:

            error = parsewood2texture(file,tex,pi);
            done = TRUE;
            break;

          case WOOD3:

            error = parsewood3texture(file,tex,pi);
            done = TRUE;
            break;

          case SWIRLS:

            error = parseswirlstexture(file,tex,pi);
            done = TRUE;
            break;

          case SPOTTY:

            error = parsespottytexture(file,tex,pi);
            done = TRUE;
            break;

          case ACID:

            error = parseacidtexture(file,tex,pi);
            done = TRUE;
            break;

          case WIRE:

            error = parsewiretexture(file,tex,pi);
            done = TRUE;
            break;

          case SNOW:

            error = parsesnowtexture(file,tex,pi);
            done = TRUE;
            break;

          default:
            return(ERROR_SYNTAXERROR);

        }

        break;

      default:

        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

    if(done) break;

  }

  return(error);

}

short parseflatmap(FILE *file,struct Texture *tex,struct parseinfo *pi)
{

  struct FlatMap *map;
  float *ptr;
  short cornernum;

  tex->tex_TextureFunc = im_ProcessFlatMap;
  tex->tex_TextureData = 0;
  tex->tex_TextureDataSize = sizeof(struct FlatMap);

  map = oem_AllocMem(tex->tex_TextureDataSize);
  if(!map) {

    tex->tex_TextureDataSize = 0;
    return(ERROR_OUTOFMEMORY);

  }

  tex->tex_TextureData = map;
  tex->tex_Flags = 0;

  map->header.map_type = MAPTYPE_FLAT;
  map->header.map_flags = 0;
  map->header.map_image = 0;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case REPEAT:

        map->header.map_flags |= MAPFLAG_REPEAT;
        break;

      case IMAGE:

        get_keyword(file,pi);

        map->header.map_image = im_FindImage(pi->cur_text);
        if(!map->header.map_image) return(ERROR_UNKNOWNIMAGE);

        break;

      case ORIGIN:

        map->fm_origin[0] = get_number(file,pi);
        map->fm_origin[1] = get_number(file,pi);
        map->fm_origin[2] = get_number(file,pi);

        break;

      case NORMAL:

        map->fm_normal[0] = get_number(file,pi);
        map->fm_normal[1] = get_number(file,pi);
        map->fm_normal[2] = get_number(file,pi);

        break;

      case CORNEROFFSET:

        cornernum = (short)get_number(file,pi);

        if(cornernum < 1 || cornernum > 4) return(ERROR_SYNTAXERROR);

        if(cornernum == 1) ptr = map->fm_c1;
        if(cornernum == 2) ptr = map->fm_c2;
        if(cornernum == 3) ptr = map->fm_c3;
        if(cornernum == 4) ptr = map->fm_c4;

        ptr[0] = get_number(file,pi) + map->fm_origin[0];
        ptr[1] = get_number(file,pi) + map->fm_origin[1];
        ptr[2] = get_number(file,pi) + map->fm_origin[2];

        break;

      default:

        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

short parsesphericalmap(FILE *file,struct Texture *tex,struct parseinfo *pi)
{

  struct SphericalMap *map;

  tex->tex_TextureFunc = im_ProcessSphericalMap;
  tex->tex_TextureData = 0;
  tex->tex_TextureDataSize = sizeof(struct SphericalMap);

  map = oem_AllocMem(tex->tex_TextureDataSize);
  if(!map) {

    tex->tex_TextureDataSize = 0;
    return(ERROR_OUTOFMEMORY);

  }

  tex->tex_TextureData = map;
  tex->tex_Flags = 0;

  map->header.map_type = MAPTYPE_SPHERICAL;

  map->sm_Center[0] = 0.0;
  map->sm_Center[1] = 0.0;
  map->sm_Center[2] = 0.0;

  map->sm_Sp[0] = 0.0;
  map->sm_Sp[1] = 1.0;
  map->sm_Sp[2] = 0.0;

  map->sm_Se[0] = 0.0;
  map->sm_Se[0] = 0.0;
  map->sm_Se[0] = 1.0;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case IMAGE:

        get_keyword(file,pi);

        map->header.map_image = im_FindImage(pi->cur_text);
        if(!map->header.map_image) return(ERROR_UNKNOWNIMAGE);

        break;

      default:

        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

short parsemap(FILE *file,struct Texture *tex,struct parseinfo *pi)
{

  short done,error;

  done = FALSE;
  error = ERROR_OK;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case WRAPTYPE:

        get_token(file,pi);

        switch(pi->cur_token) {

          case FLAT:

            error = parseflatmap(file,tex,pi);
            done = TRUE;
            break;

          case SPHERICAL:

            error = parsesphericalmap(file,tex,pi);
            done = TRUE;
            break;

          default:
            return(ERROR_SYNTAXERROR);
            break;

        }

        break;

      default:
        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

    if(done) break;

  }

  return(error);

}

short parsewood2texture(FILE *file,struct Texture *tex,struct parseinfo *pi)
{

  struct wood2info *wi;

  tex->tex_Type = TXTYPE_COLOR;
  tex->tex_TextureFunc = wood2;
  tex->tex_TextureData = 0;
  tex->tex_TextureDataSize = sizeof(struct wood2info);

  wi = oem_AllocMem(tex->tex_TextureDataSize);
  if(!wi) {

    tex->tex_TextureDataSize = 0;
    return(ERROR_OUTOFMEMORY);

  }

  tex->tex_TextureData = wi;
  tex->tex_Flags = 0;

  wi->scale = 0.02;
  wi->turbscale = 6.0;
  wi->order = 1.0;

  wi->w[0] = 0.600;
  wi->w[1] = 0.400;
  wi->w[2] = 0.133;

  wi->r[0] = 0.333;
  wi->r[1] = 0.200;
  wi->r[2] = 0.000;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case SCALE:

        wi->scale = get_number(file,pi);

        break;

      case TURBSCALE:

        wi->turbscale = get_number(file,pi);

        break;

      default:
        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

short parsewood3texture(FILE *file,struct Texture *tex,struct parseinfo *pi)
{

  struct wood3info *wi;

  tex->tex_Type = TXTYPE_COLOR;
  tex->tex_TextureFunc = NULL;
  tex->tex_TextureData = 0;
  tex->tex_TextureDataSize = sizeof(struct wood3info);

  wi = oem_AllocMem(tex->tex_TextureDataSize);
  if(!wi) {

    tex->tex_TextureDataSize = 0;
    return(ERROR_OUTOFMEMORY);

  }

  tex->tex_TextureData = wi;
  tex->tex_Flags = 0;

  wi->scale = 0.02;
  wi->turbscale = 3.0;

  wi->c1[0] = 0.6000;
  wi->c1[1] = 0.4000;
  wi->c1[2] = 0.1333;

  wi->c2[0] = 0.5215;
  wi->c2[1] = 0.3450;
  wi->c2[2] = 0.1019;

  wi->c3[0] = 0.3333;
  wi->c3[1] = 0.2000;
  wi->c3[2] = 0.0000;

  wi->c4[0] = 0.3529;
  wi->c4[1] = 0.2235;
  wi->c4[2] = 0.0352;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case SCALE:

        wi->scale = get_number(file,pi);

        break;

      case TURBSCALE:

        wi->turbscale = get_number(file,pi);

        break;

      default:
        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

short parsemarble2texture(FILE *file,struct Texture *tex,struct parseinfo *pi)
{

  struct marble2info *mi;
  short colorcount;
  float *ptr;

  tex->tex_TextureFunc = marble2;
  tex->tex_TextureData = 0;
  tex->tex_TextureDataSize = sizeof(struct marble2info);

  mi = oem_AllocMem(tex->tex_TextureDataSize);
  if(!mi) {

    tex->tex_TextureDataSize = 0;
    return(ERROR_OUTOFMEMORY);

  }

  tex->tex_TextureData = mi;
  tex->tex_Flags = 0;
  tex->tex_Type = TXTYPE_COLOR;

  mi->scale = 0.002;
  mi->turbscale = 4.8;
  mi->order = 2.0;
  mi->scale2[0] = 0.00004;
  mi->scale2[1] = 0.0;
  mi->scale2[2] = 0.0;

  mi->b[0] = 0.7843;
  mi->b[1] = 0.7843;
  mi->b[2] = 0.7843;

  mi->v[0] = 0.1960;
  mi->v[1] = 0.2745;
  mi->v[2] = 0.7843;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case SCALE:

        mi->scale = get_number(file,pi);

        break;

      case TURBSCALE:

        mi->turbscale = get_number(file,pi);

        break;

      case COLOR:

        ptr = 0;

        colorcount = (short)get_number(file,pi);

        if(colorcount > 1 || colorcount < 0) return(ERROR_SYNTAXERROR);

        if(colorcount == 0) ptr = mi->b;
        if(colorcount == 1) ptr = mi->v;

        ptr[0] = get_number(file,pi);
        ptr[1] = get_number(file,pi);
        ptr[2] = get_number(file,pi);

        break;

      default:
        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

short parsemarble3texture(FILE *file,struct Texture *tex,struct parseinfo *pi)
{

  struct marble3info *mi;
  short colorcount;
  float *ptr;

  tex->tex_TextureFunc = marble3;
  tex->tex_TextureData = 0;
  tex->tex_TextureDataSize = sizeof(struct marble3info);

  mi = oem_AllocMem(tex->tex_TextureDataSize);
  if(!mi) {

    tex->tex_TextureDataSize = 0;
    return(ERROR_OUTOFMEMORY);

  }

  tex->tex_TextureData = mi;
  tex->tex_Flags = 0;
  tex->tex_Type = TXTYPE_COLOR;

  mi->scale = 0.002;
  mi->turbscale = 1.0;
  mi->t[0] = 0.6;
  mi->t[1] = 0.8;
  mi->t[2] = 0.0;

  mi->c0[0] = 0.7843;
  mi->c0[1] = 0.7843;
  mi->c0[2] = 0.7843;

  mi->c1[0] = 0.3922;
  mi->c1[1] = 0.7843;
  mi->c1[2] = 0.7843;

  mi->c2[0] = 0.0392;
  mi->c2[1] = 0.0980;
  mi->c2[2] = 0.5490;

  mi->c3[0] = 0.0000;
  mi->c3[1] = 0.1961;
  mi->c3[2] = 0.7843;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case SCALE:

        mi->scale = get_number(file,pi);

        break;

      case TURBSCALE:

        mi->turbscale = get_number(file,pi);

        break;

      case THRESHOLD:

        mi->t[0] = get_number(file,pi);
        mi->t[1] = get_number(file,pi);
        mi->t[2] = get_number(file,pi);

        break;

      case COLOR:

        ptr = 0;

        colorcount = (short)get_number(file,pi);

        if(colorcount > 3 || colorcount < 0) return(ERROR_SYNTAXERROR);

        if(colorcount == 0) ptr = mi->c0;
        if(colorcount == 1) ptr = mi->c1;
        if(colorcount == 2) ptr = mi->c2;
        if(colorcount == 3) ptr = mi->c3;

        ptr[0] = get_number(file,pi);
        ptr[1] = get_number(file,pi);
        ptr[2] = get_number(file,pi);

        break;

      default:
        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

short parsesnowtexture(FILE *file,struct Texture *tex,struct parseinfo *pi)
{

  struct snowinfo *si;

  tex->tex_TextureFunc = snow;
  tex->tex_TextureData = 0;
  tex->tex_TextureDataSize = sizeof(struct snowinfo);

  si = oem_AllocMem(tex->tex_TextureDataSize);
  if(!si) {

    tex->tex_TextureDataSize = 0;
    return(ERROR_OUTOFMEMORY);

  }

  tex->tex_TextureData = si;
  tex->tex_Flags = 0;
  tex->tex_Type = TXTYPE_POST;

  si->depth = 0.3;
  si->stickiness = 0.5;
  si->color[0] = 0.9;
  si->color[1] = 0.9;
  si->color[2] = 0.9;
  si->dir[0] = 0.0;
  si->dir[1] = -1.0;
  si->dir[2] = 0.0;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case DEPTH:

        si->depth = get_number(file,pi);
        break;

      case STICKINESS:

        si->stickiness = get_number(file,pi);
        break;

      case LOOKAT:

        si->dir[0] = get_number(file,pi);
        si->dir[1] = get_number(file,pi);
        si->dir[2] = get_number(file,pi);

        break;

      case COLOR:

        si->color[0] = get_number(file,pi);
        si->color[1] = get_number(file,pi);
        si->color[2] = get_number(file,pi);

        break;

      default:
        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

short parseswirlstexture(FILE *file,struct Texture *tex,struct parseinfo *pi)
{

  struct swirlsinfo *si;
  short colorcount;
  float *ptr;

  tex->tex_TextureFunc = swirls;
  tex->tex_TextureData = 0;
  tex->tex_TextureDataSize = sizeof(struct swirlsinfo);

  si = oem_AllocMem(tex->tex_TextureDataSize);
  if(!si) {

    tex->tex_TextureDataSize = 0;
    return(ERROR_OUTOFMEMORY);

  }

  tex->tex_TextureData = si;
  tex->tex_Flags = 0;
  tex->tex_Type = TXTYPE_COLOR;

  si->scale = 0.002;
  si->turbscale = 1.0;
  si->t[0] = 0.3;
  si->t[1] = 0.6;
  si->t[2] = 0.8;

  si->c1[0] = 0.2000;
  si->c1[1] = 0.2000;
  si->c1[2] = 1.0000;

  si->c2[0] = 0.5000;
  si->c2[1] = 0.5000;
  si->c2[2] = 1.0000;

  si->c3[0] = 0.8000;
  si->c3[1] = 0.8000;
  si->c3[2] = 1.0000;

  si->c4[0] = 1.0000;
  si->c4[1] = 1.0000;
  si->c4[2] = 1.0000;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case SCALE:

        si->scale = get_number(file,pi);

        break;

      case TURBSCALE:

        si->turbscale = get_number(file,pi);

        break;

      case THRESHOLD:

        si->t[0] = get_number(file,pi);
        si->t[1] = get_number(file,pi);
        si->t[2] = get_number(file,pi);

        break;

      case COLOR:

        ptr = 0;

        colorcount = (short)get_number(file,pi);

        if(colorcount > 3 || colorcount < 0) return(ERROR_SYNTAXERROR);

        if(colorcount == 0) ptr = si->c1;
        if(colorcount == 1) ptr = si->c2;
        if(colorcount == 2) ptr = si->c3;
        if(colorcount == 3) ptr = si->c4;

        ptr[0] = get_number(file,pi);
        ptr[1] = get_number(file,pi);
        ptr[2] = get_number(file,pi);

        break;

      default:
        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

short parsespottytexture(FILE *file,struct Texture *tex,struct parseinfo *pi)
{

  struct spottyinfo *si;
  short colorcount;
  float *ptr;

  tex->tex_TextureFunc = spotty;
  tex->tex_TextureData = 0;
  tex->tex_TextureDataSize = sizeof(struct spottyinfo);

  si = oem_AllocMem(tex->tex_TextureDataSize);
  if(!si) {

    tex->tex_TextureDataSize = 0;
    return(ERROR_OUTOFMEMORY);

  }

  tex->tex_TextureData = si;
  tex->tex_Flags = 0;
  tex->tex_Type = TXTYPE_COLOR;

  si->scale = 0.002;
  si->d1 = 0.25;
  si->d2 = 0.50;
  si->d3 = 0.75;

  si->a[0] = 0.510;
  si->a[1] = 0.510;
  si->a[2] = 0.510;

  si->b[0] = 0.510;
  si->b[1] = 0.000;
  si->b[2] = 0.000;

  si->c[0] = 0.000;
  si->c[1] = 0.510;
  si->c[2] = 0.000;

  si->d[0] = 0.000;
  si->d[1] = 0.000;
  si->d[2] = 0.510;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case SCALE:

        si->scale = get_number(file,pi);

        break;

      case THRESHOLD:

        si->d1 = get_number(file,pi);
        si->d2 = get_number(file,pi);
        si->d3 = get_number(file,pi);

        break;

      case COLOR:

        ptr = 0;

        colorcount = (short)get_number(file,pi);

        if(colorcount > 3 || colorcount < 0) return(ERROR_SYNTAXERROR);

        if(colorcount == 0) ptr = si->a;
        if(colorcount == 1) ptr = si->b;
        if(colorcount == 2) ptr = si->c;
        if(colorcount == 3) ptr = si->d;

        ptr[0] = get_number(file,pi);
        ptr[1] = get_number(file,pi);
        ptr[2] = get_number(file,pi);

        break;

      default:
        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

short parseacidtexture(FILE *file,struct Texture *tex,struct parseinfo *pi)
{

  struct acidinfo *ai;

  tex->tex_TextureFunc = acid;
  tex->tex_TextureData = 0;
  tex->tex_TextureDataSize = sizeof(struct acidinfo);

  ai = oem_AllocMem(tex->tex_TextureDataSize);
  if(!ai) {

    tex->tex_TextureDataSize = 0;
    return(ERROR_OUTOFMEMORY);

  }

  tex->tex_TextureData = ai;
  tex->tex_Flags = 0;
  tex->tex_Type = TXTYPE_COLOR;

  ai->scale = 0.002;
  ai->turbscale = 4.8;
  ai->order = 2.0;
  ai->scale2[0] = 0.00004;
  ai->scale2[1] = 0.0;
  ai->scale2[2] = 0.0;

  ai->level = 0.75;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case SCALE:

        ai->scale = get_number(file,pi);

        break;

      case TURBSCALE:

        ai->turbscale = get_number(file,pi);

        break;

      case LEVEL:

        ai->level = get_number(file,pi);

        break;

      default:
        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

short parsewiretexture(FILE *file,struct Texture *tex,struct parseinfo *pi)
{

  struct wireinfo *wi;

  tex->tex_TextureFunc = wire;
  tex->tex_TextureData = 0;
  tex->tex_TextureDataSize = sizeof(struct wireinfo);

  wi = oem_AllocMem(tex->tex_TextureDataSize);
  if(!wi) {

    tex->tex_TextureDataSize = 0;
    return(ERROR_OUTOFMEMORY);

  }

  tex->tex_TextureData = wi;
  tex->tex_Flags = 0;
  tex->tex_Type = TXTYPE_COLOR;

  wi->width[0] = 0.0;
  wi->width[1] = 25.0;
  wi->width[2] = 25.0;

  wi->spacing[0] = 200.0;
  wi->spacing[1] = 200.0;
  wi->spacing[2] = 200.0;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      default:
        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}
