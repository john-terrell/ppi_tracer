#ifndef INCLUDE_SYSTEMDEF_H
#define INCLUDE_SYSTEMDEF_H

/*
** System specific include file
*/
#define BOOL  short

#ifndef NULL
#define NULL  0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifdef BOLOGNA
#define INFINITY 1.797693134862316E+308
#define NEGINFINITY 2.225073858507201E-308
#define EPSILON  2.2204460492503131E-16
#endif

#define EPSILON 1.19209290E-07F
#define	NEGINFINITY	1.17549435E-38F
#define INFINITY 3.40282347E+38F

#endif /* INCLUDE_SYSTEMDEF_H */
