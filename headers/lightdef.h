#ifndef INCLUDE_LIGHTDEF_H
#define INCLUDE_LIGHTDEF_H

struct LightSource {

  struct Full_Node ls_Node;

  char ls_Name[16];

  short ls_flags;        /* light source flags */

  float ls_position[3];   /* 3d position */
  float ls_direction[3];  /* Unit direction vector. (If a directional source) */
  float ls_color[3];      /* light source color */
  float ls_intensity;     /* light source intensity */
  float ls_radius;        /* light source radius. (for distributed shadow rays.) */
  float ls_angle;         /* light source divergence angle. */
  float ls_decay;         /* light source decay.  Value is the exponent for attenuation of light rays. 1/pow(d,decay) */
  float ls_dropoff;       /* Spot light dropoff value. Usable values (0 -> 350) */
  float ls_spotminangle;  /* cosine of radius (or MDOT(ls_direction,max ray angle ) */

  struct VoxObject *
        ls_shadowcache;   /* light source shadow cache */

};

/*
** ls_flags (Upper 8 bits)
*/
#define LSFLG_NOSHADOWS   (1 << 15)
#define LSFLG_NORENDER    (1 << 14)
#define LSFLG_DISABLED    (1 << 8)

/*
** ls_flags (Lower 8 bits)
*/
#define LSTYPE_POINT      0
#define LSTYPE_SPOT       1
#define LSTYPE_DISTANT    2
#define LSTYPE_SPHERICAL  3
#define LSTYPE_AMBIENT    4

#endif /* INCLUDE_LIGHTDEF_H */
