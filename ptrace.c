/*
** PTrace.c
**
** 05 Aug 1992 - Created    - J. Terrell
*/
#include "headers/tracer.h"

void main(short argc,char **argv)
{

  unsigned long hrs,mins,secs,tsecs,totalsecs;

  if(argc < 2) {

    printf("USAGE: %s <scene file name>\n",argv[0]);
    exit(0);

  }

  pt_initialize();

  printf("Loading Scene...\r");

  if(pt_loadscene(argv[1]) == ERROR_OK) {

    oem_StartTimer();

    pt_init_camera(gl_env->Camera);

    printf("Preprocessing...\r");

    pt_buildvoxeltree();

    vs_SmoothVoxelList(&gl_env->RootVoxel);

    oem_StopTimer(&tsecs);

    totalsecs = tsecs;

    mins = tsecs / 60;
    secs = tsecs % 60;

    hrs  = mins / 60;
    mins = mins % 60;

    printf("Preprocessing Time: %ld:%02ld:%02ld\n",hrs,mins,secs);

    oem_StartTimer();

    pt_execute();

    oem_StopTimer(&tsecs);

    totalsecs += tsecs;

    mins = tsecs / 60;
    secs = tsecs % 60;

    hrs  = mins / 60;
    mins = mins % 60;

    printf("Rendering Time    : %ld:%02ld:%02ld     \n",hrs,mins,secs);

    tsecs = totalsecs;

    mins = tsecs / 60;
    secs = tsecs % 60;

    hrs  = mins / 60;
    mins = mins % 60;

    printf("Total Time        : %ld:%02ld:%02ld\n",hrs,mins,secs);

    pt_freevoxeltree();

  }

  pt_cleanup();
  
  exit(0);

}

void pt_execute(void)
{

  struct Ray ray;       /* Holds current ray info */
  float position[2];    /* holds x,y coord of pixel on image plane */
  struct AAInfo aa;

  /*
  ** Main trace loop
  */
  while(TRUE) {

    oem_raystart(&ray,position,&aa);

/*
    if(position[1] == 30 && position[0] == 100) {

      printf("Quack.\n");

    }
*/
    (gl_env->eyerayhandler) (&ray,&aa);

    pt_savepixel(position,ray.ci);

    if(oem_rayend()) break;

  }

}
