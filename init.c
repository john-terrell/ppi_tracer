/*
** Init.c
**
** Contains software initialization and cleanup routines.
**
** 17 Aug 1992 - Created          - J. Terrell
*/
#include "headers/tracer.h"

void pt_initialize(void)
{

  pt_init_oem();

  pt_init_environment();
  pt_init_database();
  pt_init_framestore();

}

void pt_cleanup(void)
{

  pt_cleanup_framestore();
  pt_cleanup_database();
  pt_cleanup_environment();

  pt_cleanup_oem();

}