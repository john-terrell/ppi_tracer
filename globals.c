/*
** Globals.c
**
** Holds definitions for all global data.
**
** 17 Aug 1992 - Created          - J. Terrell
*/
#include "headers/tracer.h"

struct trace_env *gl_env;
struct OemInfo *gl_oem;

float softshadow_circledata[19][2] = {

  0.750,0.433,
  0.000,0.866,
  -0.750,0.433,
  -0.750,-0.433,
  0.000,-0.866,
  0.750,-0.433,
  0.000,0.000,

  0.750,0.000,
  0.375,0.650,
  -0.375,0.650,
  -0.750,0.000,
  -0.375,-0.650,
  0.375,-0.650,

  0.375,0.216,
  0.000,0.433,
  -0.375,0.217,
  -0.375,-0.216,
  0.000,-0.433,
  0.375,-0.217,

};

