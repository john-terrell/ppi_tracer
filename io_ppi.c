/*
** io_PPS.c
**
** 27 Oct 1992 - Created    - J. Terrell
*/
#include "Headers/Tracer.h"

void io_LoadScene_SIRES(FILE *file,short *error)
{

  struct parseinfo *pi;

  pi = oem_AllocMem(sizeof(struct parseinfo));

  *error = parsemain(file,pi);

  if(*error) printf("ERROR: %d, Line: %ld\n",*error,pi->cur_lineno);

  oem_FreeMem(pi,sizeof(struct parseinfo));

}

short parsemain(FILE *file,struct parseinfo *pi)
{

  short error;
  FILE *newfile;
  struct parseinfo *newpi;

  pi->cur_lineno = 1;

  pi->bufptr = pi->buffer;
  pi->bufpos = 1;
  pi->bufsize = 0;   /* Buffer is invalid */
  pi->lastblock = FALSE;

	while(get_token(file,pi) != END_OF_FILE) {

		switch(pi->cur_token) {

      case INCLUDE:   /* External File Load */

        get_token(file,pi);

        newfile = fopen(pi->cur_text,"r");
        if(!newfile) {

          return(ERROR_FILENOTFOUND);

        }

        newpi = oem_AllocMem(sizeof(struct parseinfo));
        if(!newpi) {

          fclose(newfile);
          return(ERROR_OUTOFMEMORY);

        }

        error = parsemain(newfile,newpi);

        oem_FreeMem(newpi,sizeof(struct parseinfo));

        fclose(newfile);

        if(error) return(error);

        break;

      case SIRES:     /* FILE Header */
        break;

      case CAMERA:    /* Camera Definition */

        error = parsecamera(file,pi);
        if(error) return(error);
        break;

      case LIGHT:     /* Lightsource Definition */

        error = parselight(file,pi);
        if(error) return(error);
        break;

      case ATTRIBUTE: /* Attribute Definition */

        error = parseattribute(file,pi);
        if(error) return(error);
        break;

      case OBJECT:    /* Object Definition */

        error = parseobject(file,pi);
        if(error) return(error);
        break;

      case CURRENTCAMERA:

        get_keyword(file,pi);

        if(!pt_SetCurrentCamera(pi->cur_text)) return(ERROR_UNKNOWNCAMERA);

        break;

      case TEXTURE:   /* Texture Definition */

        error = parsetexture(file,pi);
        if(error) return(error);
        break;

      case IMAGE:     /* Image Definition */

        error = parseimage(file,pi);
        if(error) return(error);
        break;

      case BACKDROPIMAGE:  /* Backdrop Definition */

        error = parsebackdrop(file,pi);
        if(error) return(error);
        break;

			default:
				break;


		}

	}

	return(ERROR_OK);

}

short parsecamera(FILE *file,struct parseinfo *pi)
{

  struct Camera *cam;

  cam = oem_AllocMem(sizeof(struct Camera));
  if(!cam) return(ERROR_OUTOFMEMORY);

  get_keyword(file,pi);
  strcpy(cam->cam_Name,pi->cur_text);

  cam->cam_Origin[0] = 0.0;
  cam->cam_Origin[1] = 0.0;
  cam->cam_Origin[2] = -1200.0;

  cam->cam_ViewPoint[0] = 0.0;
  cam->cam_ViewPoint[1] = 0.0;
  cam->cam_ViewPoint[2] = 0.0;

  cam->cam_FieldOfView = MRADIANS(60.0);
  cam->cam_Tilt = 0.0;

  cam->cam_Aperture = 0.0;
  cam->cam_FocalDistance = -1;

  addtail(&gl_env->CameraList,(struct Min_Node *)cam);

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case TYPE:

        get_keyword(file,pi);
        break;

      case POSITION:

        cam->cam_Origin[0] = get_number(file,pi);
        cam->cam_Origin[1] = get_number(file,pi);
        cam->cam_Origin[2] = get_number(file,pi);

        break;

      case LOOKAT:

        cam->cam_ViewPoint[0] = get_number(file,pi) - cam->cam_Origin[0];
        cam->cam_ViewPoint[1] = get_number(file,pi) - cam->cam_Origin[1];
        cam->cam_ViewPoint[2] = get_number(file,pi) - cam->cam_Origin[2];

        break;

      case FOV:

        cam->cam_FieldOfView = MRADIANS(get_number(file,pi));

        break;

      case TILT:

        cam->cam_Tilt = MRADIANS(get_number(file,pi));

        break;

      default:
        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

short parselight(FILE *file,struct parseinfo *pi)
{

  struct LightSource *ls;
  short done;

  ls = oem_AllocMem(sizeof(struct LightSource));
  if(!ls) return(ERROR_OUTOFMEMORY);

  ls->ls_position[0] = 0.0;
  ls->ls_position[1] = 0.0;
  ls->ls_position[2] = 0.0;

  ls->ls_direction[0] = 1.0;
  ls->ls_direction[1] = 0.0;
  ls->ls_direction[2] = 0.0;

  ls->ls_color[0] = 1.0;
  ls->ls_color[1] = 1.0;
  ls->ls_color[2] = 1.0;

  ls->ls_intensity = 0.2;
  ls->ls_flags = LSTYPE_AMBIENT;
  ls->ls_radius = 0.0;
  ls->ls_shadowcache = 0;
  ls->ls_intensity = 700.0;
  ls->ls_decay = 1.0;
  ls->ls_dropoff = 1.0;

  addtail(&gl_env->LightSourceList,(struct Min_Node *)ls);

  get_keyword(file,pi);

  done = FALSE;
  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case TYPE:

        get_token(file,pi);

        switch(pi->cur_token) {

          case POINT:
            ls->ls_flags = LSTYPE_POINT;
            break;

          case DISTANT:
            ls->ls_flags = LSTYPE_DISTANT;
            break;

          case SPHERICAL:
            ls->ls_flags = LSTYPE_SPHERICAL;
            break;

          case SPOT:
            ls->ls_flags = LSTYPE_SPOT;
            break;

          case AMBIENT:
            ls->ls_flags = LSTYPE_AMBIENT;
            break;

          default:
            return(ERROR_SYNTAXERROR);

        }

        break;


      case COLOR:

        ls->ls_color[0] = get_number(file,pi);
        ls->ls_color[1] = get_number(file,pi);
        ls->ls_color[2] = get_number(file,pi);

        break;

      case INTENSITY:

        ls->ls_intensity = get_number(file,pi);
        break;

      case POSITION:

        ls->ls_position[0] = get_number(file,pi);
        ls->ls_position[1] = get_number(file,pi);
        ls->ls_position[2] = get_number(file,pi);

        break;

      case ANGLE:

        ls->ls_angle = MRADIANS(get_number(file,pi));
        ls->ls_spotminangle = cos(ls->ls_angle * 0.5);

        break;

      case LOOKAT:

        ls->ls_direction[0] = get_number(file,pi) - ls->ls_position[0];
        ls->ls_direction[1] = get_number(file,pi) - ls->ls_position[1];
        ls->ls_direction[2] = get_number(file,pi) - ls->ls_position[2];

        normalize(ls->ls_direction);

        break;

      case DROPOFF:

        ls->ls_dropoff = get_number(file,pi);
        break;

      case RADIUS:

        ls->ls_radius = get_number(file,pi);
        break;

      default:

        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

    if(done) break;

  }

  return(ERROR_OK);

}

short parseattribute(FILE *file,struct parseinfo *pi)
{

  struct Attribute *attr;
  struct Texture *tex;
  struct TextureEntry *te;

  attr = oem_AllocMem(sizeof(struct Attribute));
  if(!attr) return(ERROR_OUTOFMEMORY);

  get_keyword(file,pi);

  strcpy(attr->attr_Name,pi->cur_text);

  attr->attr_Color[0] = 0.9882;
  attr->attr_Color[1] = 0.9020;
  attr->attr_Color[2] = 0.7882;

  attr->attr_Smoothness = 0.0;
  attr->attr_Metalness = 0.0;
  attr->attr_Transparency = 0.0;
  attr->attr_IndexOfRefr = 0.0;
  attr->attr_Reflectivity = 0.0;
  attr->attr_PhongExponent = 0.0;

  attr->attr_IlluminationModel = im_Strauss;

  newlist(&attr->attr_Shaders);
  
  attr->attr_Flags = 0;

  addtail(&gl_env->AttributeList,(struct Min_Node *)attr);

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case TYPE:
        break;

      case COLORTEXTURE:

        get_keyword(file,pi);

        tex = attr_FindTexture(pi->cur_text);
        if(tex) {

          te = oem_AllocMem(sizeof(struct TextureEntry));
          if(!te) return(ERROR_OUTOFMEMORY);

          te->te_Texture = tex;

          addtail(&attr->attr_Shaders,(struct Min_Node *)te);

        }
        else {

          return(ERROR_UNKNOWNTEXTURE);

        }

        break;

      case DISPLACEMENTTEXTURE:

        get_keyword(file,pi);

        tex = attr_FindTexture(pi->cur_text);
        if(tex) {

          te = oem_AllocMem(sizeof(struct TextureEntry));
          if(!te) return(ERROR_OUTOFMEMORY);

          te->te_Texture = tex;

          addtail(&attr->attr_Shaders,(struct Min_Node *)te);

        }
        else {

          return(ERROR_UNKNOWNTEXTURE);

        }

        break;

      case COLOR:

        attr->attr_Color[0] = get_number(file,pi);
        attr->attr_Color[1] = get_number(file,pi);
        attr->attr_Color[2] = get_number(file,pi);

        break;

      case SMOOTHNESS:

        attr->attr_Smoothness = get_number(file,pi);

        if(attr->attr_Smoothness > 0.0) {

          attr->attr_PhongExponent = (3.0 / (1.0 / attr->attr_Smoothness));

        }
        else {

          attr->attr_PhongExponent = 0.0;

        }

        break;

      case REFLECTIVITY:

        attr->attr_Reflectivity = get_number(file,pi);

        break;

      case METALNESS:

        attr->attr_Metalness = get_number(file,pi);

        break;

      case TRANSPARENCY:

        attr->attr_Transparency = get_number(file,pi);
        break;

      case REFRACTION:

        attr->attr_IndexOfRefr = get_number(file,pi);
        break;

      case FUZZYREFLECTIONS:

        attr->attr_Flags |= ATTRFLAG_FUZZYREFLECT;

        break;

      case FUZZYTRANSMISSIONS:

        attr->attr_Flags |= ATTRFLAG_FUZZYTRANS;

        break;

      default:
        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

short parsesphere(FILE *file,struct Object *obj,struct parseinfo *pi)
{

  struct prim_Sphere *sph;
  struct Attribute *attr;

  sph = oem_AllocMem(sizeof(struct prim_Sphere));
  if(!sph) return(ERROR_OUTOFMEMORY);

  sph->sph_Prim.prm_Flags = PTYPE_SPHERE;
  sph->sph_Prim.prm_Attribute =
    attr_findattribute(&gl_env->AttributeList,"EggShell White");

  sph->sph_Prim.prm_Object = obj;

  sph->sph_origin[0] = 0.0;
  sph->sph_origin[1] = 0.0;
  sph->sph_origin[2] = 0.0;
  sph->sph_radius = 200.0;
  sph->sph_rsquared = 200.0 * 200.0;

  addtail(&obj->obj_PrimitiveList,(struct Min_Node *)sph);

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case ORIGIN:

        sph->sph_origin[0] = get_number(file,pi);
        sph->sph_origin[1] = get_number(file,pi);
        sph->sph_origin[2] = get_number(file,pi);

        break;

      case RADIUS:

        sph->sph_radius = get_number(file,pi);
        sph->sph_rsquared = sph->sph_radius * sph->sph_radius;

        break;

      case ATTRIBUTE:

        get_keyword(file,pi);

        attr = attr_findattribute(&gl_env->AttributeList,pi->cur_text);
        if(attr) {

          sph->sph_Prim.prm_Attribute = attr;

        }
        else return(ERROR_UNKNOWNATTRIBUTE);

        break;

      default:

        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

short parseplane(FILE *file,struct Object *obj,struct parseinfo *pi)
{

  struct prim_Plane *pln;
  struct Attribute *attr;

  pln = oem_AllocMem(sizeof(struct prim_Plane));
  if(!pln) return(ERROR_OUTOFMEMORY);

  pln->pln_Prim.prm_Flags = PTYPE_PLANE | PFLAG_ALWAYSTEST | PFLAG_NOEXTENTS;
  pln->pln_Prim.prm_Attribute =
    attr_findattribute(&gl_env->AttributeList,"EggShell White");

  pln->pln_Prim.prm_Object = obj;

  pln->pln_origin[0] = 0.0;
  pln->pln_origin[1] = 0.0;
  pln->pln_origin[2] = 0.0;

  pln->pln_normal[0] = 0.0;
  pln->pln_normal[0] = 1.0;
  pln->pln_normal[0] = 0.0;

  addtail(&obj->obj_PrimitiveList,(struct Min_Node *)pln);

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case ORIGIN:

        pln->pln_origin[0] =get_number(file,pi);
        pln->pln_origin[1] =get_number(file,pi);
        pln->pln_origin[2] =get_number(file,pi);

        break;

      case NORMAL:

        pln->pln_normal[0] = get_number(file,pi);
        pln->pln_normal[1] = get_number(file,pi);
        pln->pln_normal[2] = get_number(file,pi);

        break;

      case ATTRIBUTE:

        get_keyword(file,pi);

        attr = attr_findattribute(&gl_env->AttributeList,pi->cur_text);
        if(attr) {

          pln->pln_Prim.prm_Attribute = attr;

        }
        else return(ERROR_UNKNOWNATTRIBUTE);

        break;

      default:

        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

short parsepolygon(FILE *file,struct Object *obj,struct parseinfo *pi)
{

  struct prim_Triangle *tri;
  struct VertexNode *tv;
  float coords[3];
  struct Attribute *attr;
  BOOL dt;

  tri = oem_AllocMem(sizeof(struct prim_Triangle));
  if(!tri) return(ERROR_OUTOFMEMORY);

  tri->tri_Prim.prm_Flags = PTYPE_TRIANGLE;
  tri->tri_Prim.prm_Attribute =
    attr_findattribute(&gl_env->AttributeList,"EggShell White");
  tri->tri_Prim.prm_Object = obj;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case COUNT:

        if(get_number(file,pi) != 3.0) {

          return(ERROR_BADPOLYGON);

        }

        break;

      case VLIST:

        if(get_token(file,pi) != LEFT_BRACE) return(ERROR_SYNTAXERROR);

        /* Vertex 1 */
        coords[0] = get_number(file,pi);
        coords[1] = get_number(file,pi);
        coords[2] = get_number(file,pi);

        tv = GetTvertex(obj,coords);
        if(!tv) return(ERROR_OUTOFMEMORY);

        tri->tri_Vert1 = tv->vn_tvertex.p;

        /* Vertex 2 */
        coords[0] = get_number(file,pi);
        coords[1] = get_number(file,pi);
        coords[2] = get_number(file,pi);

        tv = GetTvertex(obj,coords);
        if(!tv) return(ERROR_OUTOFMEMORY);

        tri->tri_Vert2 = tv->vn_tvertex.p;

        /* Vertex 3 */
        coords[0] = get_number(file,pi);
        coords[1] = get_number(file,pi);
        coords[2] = get_number(file,pi);

        tv = GetTvertex(obj,coords);
        if(!tv) return(ERROR_OUTOFMEMORY);

        tri->tri_Vert3 = tv->vn_tvertex.p;

        /*
        ** If this is a valid triangle, add it.
        */
        dt = io_CheckForDuplicateTriangle(obj,tri);
        if(!dt && tr_InitTriangle(tri)) {

          addtail(&obj->obj_PrimitiveList,(struct Min_Node *)tri);

        }
        else {

          oem_FreeMem(tri,sizeof(struct prim_Triangle));
          tri = NULL;

        }

        if(get_token(file,pi) != RIGHT_BRACE) return(ERROR_SYNTAXERROR);

        break;

      case ATTRIBUTE:

        get_keyword(file,pi);

        if(tri) {

          attr = attr_findattribute(&gl_env->AttributeList,pi->cur_text);
          if(attr) {

            tri->tri_Prim.prm_Attribute = attr;

          }
          else {

            printf("%s\n",pi->cur_text);
            return(ERROR_UNKNOWNATTRIBUTE);

          }


        }

        break;

      default:

        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

short parseprimitive(FILE *file,struct Object *obj,struct parseinfo *pi)
{

  short error;
  short done;

  done = FALSE;

  while(TRUE) {

    get_token(file,pi);

    switch(pi->cur_token) {

      case SPHERE:

        error = parsesphere(file,obj,pi);
        done = TRUE;
        break;

      case PLANE:

        error = parseplane(file,obj,pi);
        done = TRUE;
        break;

      case POLYGON:

        error = parsepolygon(file,obj,pi);
        done = TRUE;
        break;

    }

    if(done) break;

  }

  return(error);

}

short parseobject(FILE *file,struct parseinfo *pi)
{

  struct Object *obj,*iobj;
  short error;
  short c;
  float coords[3];

  obj = oem_AllocMem(sizeof(struct Object));
  if(!obj) return(ERROR_OUTOFMEMORY);

  obj->obj_World2Texture = NULL;
  obj->obj_Local2World = NULL;
  obj->obj_Flags = 0;
  obj->obj_SmoothingTension = 0.0;

  newlist(&obj->obj_PrimitiveList);
  newlist(&obj->obj_VertexList);

  addtail(&gl_env->ObjectList,(struct Min_Node *)obj);

  get_keyword(file,pi);

  strncpy(obj->obj_Name,pi->cur_text,15);

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case PRIMITIVE:

        error = parseprimitive(file,obj,pi);
        if(error) break;
        break;

      case NOSMOOTHING:

        obj->obj_Flags |= OFLG_NOSMOOTH;

        break;

      case INSTANCEXFORM:

        c = get_token(file,pi);
        if(c != LEFT_BRACE) {

          return(ERROR_SYNTAXERROR);

        }

        while(get_token(file,pi) != RIGHT_BRACE) {

          switch(pi->cur_token) {

            case TRANSLATE:

              coords[0] = get_number(file,pi);
              coords[1] = get_number(file,pi);
              coords[2] = get_number(file,pi);

              if(!obj->obj_Local2World) {

                obj->obj_Local2World = oem_AllocMem(sizeof(struct Matrix44));
                if(!obj->obj_Local2World) return(ERROR_OUTOFMEMORY);

                identity(obj->obj_Local2World);

              }

              addtranslation(obj->obj_Local2World,coords);

              break;

            default:
              return(ERROR_SYNTAXERROR);
              break;

          }

        }

        break;

      case INSTANCEOF:

        get_keyword(file,pi);

        iobj = findobject(pi->cur_text);
        if(iobj) {

          obj->obj_PrimitiveList.lh_Head = iobj->obj_PrimitiveList.lh_Head;
          obj->obj_PrimitiveList.lh_Tail = iobj->obj_PrimitiveList.lh_Tail;
          obj->obj_PrimitiveList.lh_TailPred = iobj->obj_PrimitiveList.lh_TailPred;

          obj->obj_Flags |= OFLG_INSTANCE;

        }
        else {

          return(ERROR_NOINSTANCESOURCE);

        }

        break;

    }

  }

  /*
  ** If object needs to be smoothed, do it.
  */
  if(error == ERROR_OK)

    if(!(obj->obj_Flags & OFLG_NOSMOOTH)) {

      sm_SmoothObject(obj);

    }

  return(error);

}

short parsesculptimage(FILE *file,struct ImageBuffer *ib,struct parseinfo *pi)
{

  char filename[108];
  short x,y;
  short fileok;
  short error;

  fileok = FALSE;
  x = 0; y = 0;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case FILENAME:

        get_token(file,pi);

        strcpy(filename,pi->cur_text);

        fileok = TRUE;

        break;

      case RESOLUTION:

        x = (short)get_number(file,pi);
        y = (short)get_number(file,pi);

        break;

      default:
        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);

        break;

    }

  }

  /*
  ** Ok, now load the filename.
  */
  if(fileok && x > 0 && y > 0) {

    error = im_LoadRAWColorMap(filename,ib,x,y);
    if(error != ERROR_OK) {

      return(error);

    }

  }
  else {

    return(ERROR_FILENOTFOUND);

  }

  addtail(&gl_env->ImageList,(struct Min_Node *)ib);

  return(ERROR_OK);

}

short parseimage(FILE *file,struct parseinfo *pi)
{

  struct ImageBuffer *ib;
  short error;
  short done;

  ib = oem_AllocMem(sizeof(struct ImageBuffer));
  if(!ib) return(ERROR_OUTOFMEMORY);

  error = ERROR_OK;

  get_keyword(file,pi);

  strcpy(ib->im_Name,pi->cur_text);

  ib->im_xsize = 0;
  ib->im_ysize = 0;

  ib->im_RedBuffer = 0;
  ib->im_GrnBuffer = 0;
  ib->im_BluBuffer = 0;

  done = FALSE;
  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case TYPE:
        break;

      case SCULPT:

        error = parsesculptimage(file,ib,pi);
        done = TRUE;
        break;

      default:

        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

    if(done) break;

  }

  return(error);

}

short parsebackdrop(FILE *file,struct parseinfo *pi)
{

  struct ImageBuffer *ib;

  while(get_token(file,pi) != RIGHT_BRACE) {

    switch(pi->cur_token) {

      case IMAGE:

        get_token(file,pi);

        ib = im_FindImage(pi->cur_text);
        if(!ib) return(ERROR_UNKNOWNIMAGE);

        gl_env->BackDropImage = ib;
        gl_env->backdrop_shader = (short (*)())bd_BackdropImage;

        break;

      default:

        if(pi->cur_token != LEFT_BRACE) return(ERROR_SYNTAXERROR);
        break;

    }

  }

  return(ERROR_OK);

}

struct VertexNode *GetTvertex(struct Object *obj,float *coords)
{

  struct VertexNode *tv;

  tv = obj->obj_VertexList.lh_Head;
  while(tv->vn_Node.ln_Succ) {

    if(coords[0] == tv->vn_tvertex.p[0] &&
       coords[1] == tv->vn_tvertex.p[1] &&
       coords[2] == tv->vn_tvertex.p[2]) return(tv);

    tv = tv->vn_Node.ln_Succ;

  }

  tv = oem_AllocMem(sizeof(struct VertexNode));
  if(!tv) return(NULL);

  tv->vn_tvertex.p[0] = coords[0];
  tv->vn_tvertex.p[1] = coords[1];
  tv->vn_tvertex.p[2] = coords[2];

  tv->vn_tvertex.n[0] = 0.0;
  tv->vn_tvertex.n[1] = 0.0;
  tv->vn_tvertex.n[2] = 0.0;

  addtail(&obj->obj_VertexList,(struct Min_Node *)tv);

  return(tv);

}

BOOL io_CheckForDuplicateTriangle(struct Object *obj,struct prim_Triangle *tr)
{

  struct prim_Triangle *tst;

  tst = obj->obj_PrimitiveList.lh_Head;
  while(tst->tri_Prim.prm_Node.ln_Succ) {

    if(GET_PRIMTYPE(tst->tri_Prim.prm_Flags) == PTYPE_TRIANGLE) {

      if(
        ((tr->tri_Vert1 == tst->tri_Vert1) ||
         (tr->tri_Vert1 == tst->tri_Vert2) ||
         (tr->tri_Vert1 == tst->tri_Vert3)) &&
        ((tr->tri_Vert2 == tst->tri_Vert1) ||
         (tr->tri_Vert2 == tst->tri_Vert2) ||
         (tr->tri_Vert2 == tst->tri_Vert3)) &&
        ((tr->tri_Vert3 == tst->tri_Vert1) ||
         (tr->tri_Vert3 == tst->tri_Vert2) ||
         (tr->tri_Vert3 == tst->tri_Vert3))) {

          return(TRUE);

        }

    }

    tst = tst->tri_Prim.prm_Node.ln_Succ;

  }

  return(FALSE);

}

struct Object *findobject(char *name)
{

  struct Object *obj;

  obj = gl_env->ObjectList.lh_Head;
  while(obj->obj_Node.ln_Succ) {

    if(strcmp(name,obj->obj_Name) == 0) return(obj);

    obj = obj->obj_Node.ln_Succ;

  }

  return(0);

}
