#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include "global-vsc.h"
#include "download.h"
#include "move.h"

//Files (I wonder if these names are bad...)
//download.h --> downloads files from url in url-vsc.h
//move.h --> decompress and move to a location and overall makes it feel native
//build.h --> if the rest went well, this will allow for allowing you to build from source  
//manager.h --> if we get this far, this will be for managing versions or othen adv actions

bool dflag = false;
bool vflag = false;
bool sdflag = false;


int main(int argc, char *argv[]){
    //Makes a list of flags to pass to other files
    //Don't know other way
    //Struct on url-vsc.h
    bool has_arg = false;
    bool sdflag = false;
    char *actpair[2];//REMINDER Free the associated malloc'd pointers later
    if(argc > 1){
        for(int i = 1; argv[i] != NULL; i++){
            int c;//TODO add a --help or -h flag
            if(strcmp(argv[i], "--verbose") == 0){
                c = 0;
            }
            else if(strcmp(argv[i], "--DEBUG") == 0){
                c = 1;
            }
            else if(strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0){
                c = 2;
            }
            else if(strcmp(argv[i], "--DEBUG=SKIP-DL") == 0){
                c = 3;
            }
            switch(c){
                case 0:
                    vflag = true;
                    break;
                case 1:
                    dflag = true;
                    break;
                case 2:
                    fprintf(stderr,"Program version: %s\n", VERSION);
                    return 0;
                    break;
                case 3:
                    fprintf(stderr,"Skipping download\n");
                    sdflag = true; 
                    dflag = true;
                default://???
                    break;
            }
            //just setting the layout for future features. Crude work. add more || later
            //Add a feature for -h or --help later
            if(strcmp(argv[i], "-d") == 0){
                //If target of action NULL return 1 and error message;
                if(argv[i+1] == NULL){
                    fprintf(stderr,"Error. No argument after action flag\n");
                    return 1;
                }
                else{
                    //For simplicty sake I'll allow one operation like that
                    actpair[0] = malloc(strlen("-d") + 1);
                    strcpy(actpair[0], "-d");
                    actpair[1] = malloc(strlen(argv[i + 1]) + 1);
                    strcpy(actpair[1], argv[i + 1]);
                    i++;
                    has_arg = true;
                }
            }
        }
    }
    if(dflag == true && actpair[0] != NULL){fprintf(stderr,"The pair is: %s and %s\n", actpair[0], actpair[1]);}
    //TODO ADD switch case for options, like to delete etc
    /*
    switch(actpair[0]){
        case X:
            do Z()
            ...
    }
    */
    //TODO remove most fprinft from other files and most of them here?
    if(sdflag == false){
        fprintf(stderr, "Downloading files...\n");
    }
    char **filearray = getfile();
    if(filearray == NULL){
        fprintf(stderr,"Failed to download files\n");
        return 1;
    }
    if(sdflag == false){
        fprintf(stderr, "Downloading files...\n");
        fprintf(stderr, "Done.\nFiles downloaded to /tmp\n"); 
    }
    fprintf(stderr, "Decompressing and moving...\n");
    if(move(filearray) != 0){
        fprintf(stderr, "Failed to execute changes\n");
        return 1;
    }
    fprintf(stderr, "Done.\n");
    //Freeing list flags
    //Possibly unnecessary loop
    if(has_arg == true){
        for(int i = 0; i < 2; i++){
            free(actpair[i]);
            actpair[i] = NULL;
        }
    }
    return 0;

}