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
    //Making target dir ---NOTE---I forgot I could have used snprintf...
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
    if(ssflag == false){

        //Reading local sha
        int size = 256;
        FILE *shafile;
        shafile = fopen(filearray[1], "r");
        if(shafile == NULL){
            fprintf(stderr,"Couldn't open sha256 file for reading\n");
            return 1;
        }
        char *shafileresp = malloc(size);
        fread(shafileresp, sizeof(char), size, shafile);
        shafileresp = strtok(shafileresp, " ");


        //Calling your local sha256sum.
        char *sha = "sha256sum ";
        char *shacmd = malloc(strlen(sha) + strlen(filearray[0]) + 1);
        char buffer[size];
        char *sharesp = malloc(size);
        sharesp[0] = '\0';//Gotta do this to fix broken characters
        strcpy(shacmd, sha);
        strcat(shacmd, filearray[0]);
        //Doing it
        FILE *pipe = popen(shacmd, "r");
        if(!pipe){
            fprintf(stderr, "Couldn't open pipe\n");
            return 1;
        }
        while(fgets(buffer, sizeof(buffer), pipe) != NULL){
            strcat(sharesp, buffer);
        }
        sharesp = strtok(sharesp, " ");


        if(dflag == true){fprintf(stderr,"READ FROM FILE:%s\nREAD FROM RESPONSE:%s\n",
                                                            shafileresp, sharesp);}
        if(strcmp(sharesp, shafileresp) == 0){
            if(dflag == true){fprintf(stderr,"sha256 match.\nContinuing...\n");}
        }
        else{
            fprintf(stderr,"sha256 did not match. Quitting.\n");
            return 1;
        }
        free(shacmd);
        free(sharesp);
        free(shafileresp);
        fclose(shafile);
        pclose(pipe);
    }
    //Removing the SHA either way
    if(remove(filearray[1]) == 0){
        if(dflag == true){fprintf(stderr,"Deleted %s\n", filearray[1]);}
    }
    else{
        perror("Failed to delete sha256 file");
        return 1;
    }

    //Extracting
    if(extract(filearray[0]) == 0){
        if(dflag == true){fprintf(stderr,"Extracted successfully\n");}
    }
    else{//It exits anyway, but just making sure
        return 1;
    }

    if(remove(filearray[0]) == 0){
        if(dflag == true){fprintf(stderr,"Deleted %s\n", filearray[0]);}
    }
    //Getting your username
    char *uname = getlogin();
    if(uname == NULL){
        perror("Failed to get user's name");
        return 1;
    }
    if(dflag == true){fprintf(stderr,"USERNAME:%s", uname);}

    //Making folder in home, asking for confirm
    fprintf(stderr,"A folder will be created on your home directory. Continue?");
    int rask = ask();
    if(rask == 1){
        return 2;//Return this (2) if quit but no errors
    }
    //Create folder on X place, or default?
    fprintf(stderr,"Create on home?");
    rask = ask();
    if(rask == 1){
        //IF respond no
        goto PICKFOLDER;
    }
    else if(rask == 0){
        //If respon yes, just continue
    }
    ASKHID:
    //Hidden folder or not?
    fprintf(stderr,"Make if a hidden folder?");
    rask = ask();
    if(rask == 0){
        //IF yes
    }
    else if(rask == 1){
        //If no...
    }
    
    closedir(wdir);
    free(tdir);
    return 0;

    PICKFOLDER: 


    goto ASKHID;
}
