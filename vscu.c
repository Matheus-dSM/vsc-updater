#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "global-vsc.h"
#include "download.h"
#include "move.h"

//Files (I wonder if these names are bad...)
//global-vsc.h --> functions and global vars
//download.h --> downloads files from url in url-vsc.h
//move.h --> decompress and move to a location and overall makes it feel native
//build.h --> if the rest went well, this will allow for allowing you to build from source  
//manager.h --> if we get this far, this will be for managing versions or othen adv actions

bool dflag = false;
bool vflag = false;
bool sdflag = false;
bool ssflag = false;


int main(int argc, char *argv[]){

    char *actpair[2];//REMINDER Free the associated malloc'd pointers later
    if(argc == 2){
        int c;
        if(strcmp(argv[1], "--verbose") == 0){
            vflag = true;
        }
        else if(strcmp(argv[1], "--DEBUG") == 0){
            dflag = true;
        }
        else if(strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0){
            fprintf(stderr,"Program version: %s\n", VERSION);
            return 0;
        }
        else if(strcmp(argv[1], "--DEBUG=SKIP-DL") == 0){
            dflag = true;
            sdflag = true;
        }
        else if(strcmp(argv[1], "--SKIP-SHA") == 0){
            fprintf(stderr, "Skipping sha256sum...\n");
            ssflag = true;
        }
        else if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0){
            goto HELP;
        }
        else if(strcmp(argv[1], "-D") == 0 ||
                strcmp(argv[1], "--DOWNLOAD") == 0 ||
                strcmp(argv[1], "--download") == 0){
            goto ASKCONT;
        }//ADD ONE FOR HELP LATER
    }
    else if(argc > 2){//Check this later, seems brolken
        goto PAIR;
    }
    else if(argc == 1){
        goto HELP;
    }

    //Asking confirm
    char ans[10];
    int rask;
    ASKCONT: 
    if(sdflag == false){
        fprintf(stderr,"Starting download of VSCodium, continue?");
        rask = ask();
        if(rask == 0){
            fprintf(stderr,"Continuing...\n");
            goto RUN;
        }
        else{
            return 0;
        }
    }
    else if(sdflag == true){
        fprintf(stderr,"Starting without download of VSCodium, continue?");
        rask = ask();
        if(rask == 0){
            fprintf(stderr,"Skipping download...\n");
            goto RUN;
        }
        else{
            return 0;
        }
    }

    RUN:
    //Running the program
    char **filearray = getfile();
    if(filearray == NULL){
        fprintf(stderr,"Failed to download files\n");
        return 1;
    }
    if(sdflag == false){
    fprintf(stderr, "Downloading files...\n");
    fprintf(stderr, "Done.\nFiles downloaded to /tmp\n"); 
    fprintf(stderr, "Decompressing and moving...\n");
    }
    int rMove = move(filearray);
    if(rMove == 1){
        fprintf(stderr, "Failed to execute changes\n");
        return 1;
    }
    else if(rMove == 2){
        fprintf(stderr,"Quitting.\n");
        return 1;
    }
    fprintf(stderr, "Done.\n");
    //Freeing list flags
    //Possibly unnecessary loop
    for(int i = 0; i < 4; i++){
        free(filearray[i]);
        filearray[i] = NULL;
    }
    free(filearray);
    return 0;


    PAIR:
    if(dflag == true && actpair[0] != NULL){fprintf(stderr,"The pair is: %s and %s\n", actpair[0], actpair[1]);}
    //ADD more about pair
    fprintf(stderr,"You inputed 2 args\n");
    for(int i = 0; i < 2; i++){
        free(actpair[i]);
        actpair[i] = NULL;
    }
    return 0;


    HELP:
    //Write things related to the -h or help command here
    fprintf(stderr,"no arg!!\n");
    return 0;
}