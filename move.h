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
    int size = strlen(TD_NAME) + strlen(D_NAME);
    size += strlen("/") * 2;
    size += strlen(filearray[2]);
    char *tdir = malloc(size + 1);// target dir
    if(getcwd(cdir, sizeof(cdir)) != NULL){
        if(dflag == true || vflag == true){fprintf(stderr, "Current directory:%s\n", cdir);}
    }
    else{
        perror("Failed to acquire current directory\n");
        return 1;
    }
    //Making target dir
    strcpy(tdir, TD_NAME);
    strcat(tdir, "/");
    strcat(tdir, D_NAME);
    strcat(tdir, "/");
    strcat(tdir, filearray[2]);

    if(dflag == true){fprintf(stderr, "CURRENT:%s\nTARGET:%s\n",cdir, tdir);}
    
    //Check if same dir as target dir
    if(strcmp(cdir, tdir) != 0){//If not same, change
        if(dflag == true || vflag == true){fprintf(stderr, "Changing directory to:%s\n", tdir);}
        if(chdir(tdir) == 0){//Changes dir
            if(dflag == true || vflag == true){fprintf(stderr, "Directory changed.\n");}
        }
        else{perror("Couldn't change directory"); return 1;}
    } //If same, continue
    else{if(dflag == true || vflag == true){fprintf(stderr, "Already on directory %s\n", tdir);}}


    //Getting contents of 
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
    //Check sum
    //If fail, return 1 and warn
    //If pass, delete SHA file and extract archive.
    //After that, make file in user home, use .. and move it to a folder in user
    //Do the rest
    closedir(wdir);
    free(tdir);
    return 0;
}
