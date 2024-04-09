#include <stdio.h>
#include <string.h>//Strings
#include <stdlib.h>//Misc, mainly malloc
#include <unistd.h>//For chdir
#include <sys/stat.h>//For creating the dir
#include <stdbool.h>//For bool for the -v flag check
#include <limits.h>//For PATH_MAX
#include <dirent.h>//For using directories


int move(char **filearray){
    //Getting the current directory and changing it accordingly
    char cdir[PATH_MAX];//Current dir
    char *tdir = malloc((strlen(TD_NAME) + strlen(D_NAME) + strlen("/")) + 1);// target dir
    if(getcwd(cdir, sizeof(cdir)) != NULL){
        if(dflag == true || vflag == true){fprintf(stderr, "Current directory:%s\n", cdir);}
    }
    else{
        perror("Failed to acquire current directory\n");
        return 1;
    }
    //Checking if same dir
    strcpy(tdir, TD_NAME);
    strcat(tdir, "/");
    strcat(tdir, D_NAME);
    if(dflag == true){fprintf(stderr, "CURRENT:%s\nTARGET:%s\n",cdir, tdir);}
    if(strcmp(cdir, tdir) != 0){
        if(dflag == true || vflag == true){fprintf(stderr, "Changing directory to: %s\n", tdir);}
    }
    else{
        if(dflag == true || vflag == true){fprintf(stderr, "Already on directory %s\n", tdir);}
    }
    //Getting contents of dir
    DIR *wdir;//Working directory
    struct dirent *entry;
    wdir = opendir(tdir);
    if(wdir == NULL){
        fprintf(stderr,"Failed to read contents of directory\n");
        return 1;
    }
    
    //There may be a better way of doing this
    int mc = 0;
    for(;(entry = readdir(wdir)) != NULL;){
        if(strcmp(entry->d_name, ".") == 0 || 
           strcmp(entry->d_name, "..") == 0){
            continue;//If this jump to end of for loop
        }
        if(strcmp(entry->d_name, filearray[0]) == 0 ||
           strcmp(entry->d_name, filearray[1]) == 0){
            if(dflag == true){fprintf(stderr,"MATCHFOUND\n");}
            mc++;
        }
    }
    if(mc != 2){
        fprintf(stderr,"Couldn't find files to extract\n");
        return 1;
    }
    
    closedir(wdir);
    free(tdir);
    return 0;
}
