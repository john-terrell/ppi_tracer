#ifndef INCLUDE_ATTRDEF_H
#define INCLUDE_ATTRDEF_H

/*
** AttrDef.h
**
*/
struct Attribute {

  struct Full_Node attr_Node;

  char attr_Name[16];
  unsigned short attr_Flags;

  float attr_Color[3];        /* Primitive Color */
  float attr_Smoothness;      /* Diffuse (0.0) -> Specular (1.0) inclusive */
  float attr_Metalness;       /* Non Metallic (0.0) -> Metallic (1.0) inclusive */
  float attr_Transparency;    /* Opaque (0.0) -> Transparenct (1.0) inclusive */
  float attr_Reflectivity;    /* No reflections (0.0) -> Total mirror (1.0) */
  float attr_IndexOfRefr;     /* Index of refraction. */
  float attr_PhongExponent;   /* Phong exponent.  (3.0 / (1.0 - smoothness) */

  void (*attr_IlluminationModel)
    (struct TextureArg *,struct LightSource *,struct Ray *);

  struct Min_List attr_Shaders;
  
};

/*
** attr_Flags bit definitions
*/
#define ATTRFLAG_FUZZYREFLECT (1 << 15) /* Fuzzy reflections are on. */
#define ATTRFLAG_FUZZYTRANS   (1 << 14) /* Fuzzy transmissions are on. */

#endif /* INCLUDE_ATTRDEF_H */
