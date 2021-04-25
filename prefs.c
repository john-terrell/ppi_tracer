/*
** Prefs.c
** 
** Preferences loader.
*/
#include "Headers/Tracer.h"

char breakstring[] = " \n\t";

short parseprefcommand(char *ptr)
{

  long res;

  char *cmd,*args;

  cmd = strtok(ptr,breakstring);

  if(stricmp(cmd,"Resolution") == 0) {

    args = strtok(NULL,breakstring);
    
    if(args) {

      sscanf(args,"%ld",&res);
      gl_oem->oem_HorzRes = res;

    }
    else 
      return(1);  

    args = strtok(NULL,breakstring);
    if(args) {

      sscanf(args,"%ld",&res);
      gl_oem->oem_VertRes = res;    

    }
    else 
      return(1);

  }
  else 
  if(stricmp(cmd,"DisplayAspectRatio") == 0) {

    args = strtok(NULL,breakstring);
    
    if(stricmp(args,"NTSC") == 0) {

      gl_oem->oem_DisplayAspectRatio = 4.0/3.0;  

    }
    else
    if(stricmp(args,"HDTV") == 0) {

      gl_oem->oem_DisplayAspectRatio = 16.0/9.0;  
      
    }
    else
    if(stricmp(args,"PANAVISION") == 0) {

      gl_oem->oem_DisplayAspectRatio = 2.35;  
      
    }
    else
    if(stricmp(args,"35MM") == 0) {

      gl_oem->oem_DisplayAspectRatio = 5.0/3.0;  
      
    }
    else return(1);

  }
  else
  if(stricmp(cmd,"ImageAspectRatio") == 0) {

    args = strtok(NULL,breakstring);
    
    if(stricmp(args,"NTSC") == 0) {

      gl_oem->oem_DisplayAspectRatio = 4.0/3.0;  

    }
    else
    if(stricmp(args,"HDTV") == 0) {

      gl_oem->oem_DisplayAspectRatio = 16.0/9.0;  
      
    }
    else
    if(stricmp(args,"PANAVISION") == 0) {

      gl_oem->oem_DisplayAspectRatio = 2.35;  
      
    }
    else
    if(stricmp(args,"35MM") == 0) {

      gl_oem->oem_DisplayAspectRatio = 5.0/3.0;  
      
    }
    else return(1);

  }
  else {

    printf("** Unrecognized command in config file. **\n"); 
  
    return(1);

  }

  return(0);

}

short pt_loadprefs(void)
{

  FILE *pfile;
  char buffer[256],*ptr;

  pfile = fopen("s:PTrace.Prefs","r");
  if(!pfile) {
    
    printf("** Config File Not Found. **\n");
    return(0);

  }

  ptr = fgets(buffer,sizeof(buffer),pfile);  

  while(ptr) {

    if(parseprefcommand(ptr)) {

      fclose(pfile);
      return(1);

    }

    ptr = fgets(buffer,sizeof(buffer),pfile);

  }

  fclose(pfile);

  return(0);

}
