/*
** Attributes.c
**
** 12 Oct 1992 - Created    - J. Terrell
**
*/
#include "Headers/Tracer.h"

void attr_InitAttributes(void)
{

  attr_InitEggShellWhite();

}

void *attr_findattribute(struct Min_List *l,char *name)
{

  struct Attribute *node,*fnode;
  short srclen;
  char *ptr,*fptr;
  short match;
  short i;

  /* Find length of source string */
  ptr = name;
  srclen = 0;
  while(*ptr++) srclen++;

  fnode = 0;
  node = l->lh_Head;

  while(node->attr_Node.ln_Succ) {

    fptr = node->attr_Name;

    i = 0;
    match = 1;

    ptr = name;
    while(i < srclen) {

      if(*fptr++ != *ptr++) { match = 0; break; }
      i++;

    }

    if(match) {

      fnode = node;
      break;

    }

    node = node->attr_Node.ln_Succ;

  }

  return(fnode);

}

void *attr_FindTexture(char *name)
{

  struct Texture *tex;

  tex = gl_env->TextureList.lh_Head;
  while(tex->tex_Node.ln_Succ) {

    if(strcmp(name,tex->tex_Name) == 0) return(tex);

    tex = tex->tex_Node.ln_Succ;

  }

  return(0L);

}

void attr_InitEggShellWhite(void)
{

  struct Attribute *attr;

  attr = oem_AllocMem(sizeof(struct Attribute));
  if(attr) {

    strcpy(attr->attr_Name,"EggShell White");

    attr->attr_Color[0] = 0.9882;
    attr->attr_Color[1] = 0.9020;
    attr->attr_Color[2] = 0.7882;

    attr->attr_Smoothness = 0.0;
    attr->attr_Metalness = 0.0;
    attr->attr_Transparency = 0.0;
    attr->attr_Reflectivity = 0.0;
    attr->attr_IndexOfRefr = 0.0;
    attr->attr_PhongExponent = 0.0;
    attr->attr_IlluminationModel = im_Strauss;

    newlist(&attr->attr_Shaders);

    attr->attr_Flags = 0;

    addtail(&gl_env->AttributeList,(struct Min_Node *)attr);

  }

}
