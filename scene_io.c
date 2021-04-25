/*
** Scene_IO.c
**
** functions to load a scene from the host
**
** 17 Aug 1992 - Created          - J. Terrell
*/
#include "Headers/Tracer.h"

short pt_loadscene(char *scenefilename)
{

  FILE *file;
  short error;

  file = fopen(scenefilename,"r");
  if(file) {

    io_LoadScene_SIRES((void *)file,&error);

    fclose(file);

  }
  else {

    printf("Unable to open scene file '%s'.\n",scenefilename);

  }

  return(error);

}
