#include <stdio.h>
#include <fcntl.h> //for rename at 2
#include <string.h>//Strings
#include <stdlib.h>//Misc, mainly malloc
#include <unistd.h>//For chdir
#include <sys/stat.h>//For creating the dir, and chmod
#include <stdbool.h>//For bool for the -v flag check
#include <limits.h>//For PATH_MAX
#include <dirent.h>//For using directories


int move(char **filearray){
    //Getting the current directory and changing it accordingly
    bool folderpick;
    int cmdresp;
    char *command = malloc(300);//Excessive, I know
    char seldir[PATH_MAX];
    char cdir[PATH_MAX];//Current dir
    int size = strlen(TD_NAME) + strlen(D_NAME);
    size += strlen("/") * 2;
    size += strlen(filearray[2]) * 2;
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
    if(dflag == true){fprintf(stderr,"USERNAME:%s\n", uname);}

    //Making folder in home, asking for confirm
    fprintf(stderr,"Changes will be made to your home directory. Continue? ");
    int rask;
    rask = ask();
    if(rask == 1 || rask == 2){
        return 2;//Return this (2) if quit but no errors
    }

    ASKCREATE:
    char *userhome = malloc(strlen("/home/") + (strlen("/") * 4) 
                    + strlen(uname) + (strlen(filearray[2]) * 3) + 1 );
    strcpy(userhome, "/home/");
    strcat(userhome, uname);
    bool hide_folder;
    //Create folder on X place, or default?
    fprintf(stderr,"Create on home? ");
    rask = ask();
    if(rask == 1){
        //IF respond no
        fprintf(stderr,"Do you wish to select a save folder? ");
        rask = ask();
        if(rask == 0){
            goto PICKFOLDER;
        }
        else{
            fprintf(stderr,"Closing...\n");
            return 2;
        }
    }
    else if(rask == 0){
        //If respon yes, just continue

    }
    else if(rask == 2){
        //If quit/close
        return 2;
    }
    //Hidden folder or not?
    fprintf(stderr,"Make it a hidden folder? ");
    rask = ask();
    if(rask == 0){
        //IF yes
        hide_folder = true;
    }
    else if(rask == 1){
        //If no...
        hide_folder = false;
    }
    //Create folder
    char *finaldir = malloc((strlen(filearray[2]) * 2) + 1);
    if(hide_folder == true){
        strcpy(finaldir,".VSC-Updater");
    }
    else if(hide_folder == false){
        strcpy(finaldir,"VSC-Updater");
    }
    //change to home
    if(chdir(userhome) == 0){
        if(dflag == true){fprintf(stderr,"Changed to %s\n", userhome);}
    }
    else{
        perror("Couldn't change to user's home");
        return 1;
    }
    //Create a folder for VSCU if not already
    int chdirr = chdir(finaldir);
    if(chdirr != 0){
        if(mkdir(finaldir, 0700) == 0){
            if(dflag == true){fprintf(stderr,"Directory %s created\n", finaldir);}
        }
        else{
            perror("Failed to create directory at home");
            return 1;
        }
    }
    else if(chdirr == 0){
        if(dflag == true){fprintf(stderr,"Directory %s already created, moved to it\n", finaldir);}
    }
    //Return to /tmp
    if(chdir(tdir) == 00){
        if(dflag == true){fprintf(stderr,"Changed to %s\n", tdir);}
    }
    else{
        perror("Couldn't change to directory where extracted files are at");
        return 1;
    }
    //Move the extracted folder to there
    strcat(userhome, "/");
    strcat(userhome, finaldir);
    strcat(userhome, "/");
    snprintf(command, 300, "mv %s %s", tdir, userhome);
    cmdresp = system(command);
    if(cmdresp == 0){
        if(dflag == true){fprintf(stderr,
        "Moved %s to %s\n", tdir, userhome
        );}
    }
    else{
        fprintf(stderr,"Failed to move folders.\n");
        return 1;
    }

    MAKENATIVE:
    fprintf(stderr,"\n\nA bash script will be created on %s\n"\
    "This is done to avoid use of root privileges in this project.\n"\
    "Move it to wherever your PATH will read from.\n"
    "If you used --source, this may be useless.\n\n", userhome);
    if(chdir(userhome) == 0){
        if(dflag == true){fprintf(stderr,"Moved to %s\n", userhome);}
    }
    else{
        perror("Failed to move to folder");
        return 1;
    }
    char vscname[10];
    fprintf(stderr,"How do you wish to name the script? ");
    rask = ask();
    if(rask == 0){
        fprintf(stderr,"Maximum of 10 characters: ");
        scanf("%s", &vscname);
    }
    else if(rask == 1){
        strcpy(vscname, "vscodium");
        fprintf(stderr,"The name default to %s\n", vscname);
    }
    else if(rask == 2){
        fprintf(stderr,"Canceling...\n");
        return 1;
    }
    FILE *bashscript = fopen(vscname, "w");
    if(bashscript == NULL){
        fprintf(stderr, "Failed to open file for writing\n");
        return 1;
    }
    //Create file
    char *scriptcontent = malloc(1000);
    if(folderpick == false){
        strcat(userhome, filearray[2]);
        strcat(userhome, "/");
    }
    snprintf(scriptcontent, 1000, "#!/bin/bash\n"
                                   "\n"
                                   "PDIR=\"%s\"\n"
                                   "\n"
                                   "exec \"${PDIR}/./codium\" \"$@\"", userhome);
    fprintf(bashscript, "%s", scriptcontent);
    
    //Chmod to make it executable

    mode_t mode = S_IXUSR | S_IRUSR | S_IWUSR;//Only user can exec, read and write
    if(chmod(vscname, mode) == 0){
        if(dflag== true){fprintf(stderr,"%s is now executable\n", vscname);}
    }
    else{
        perror("Couldn't run chmod on the file");
        return 1;
    }


    //That's all?
    fclose(bashscript);
    closedir(wdir);
    free(tdir);
    free(finaldir);
    free(userhome);
    free(command);
    free(scriptcontent);
    return 0;

    PICKFOLDER: 
    strcat(userhome, "/");
    fprintf(stderr,"Ensure the folder already exists, then type the desired save location.\n" \
    "The base location is: %s", userhome);// -> /home/USER/
    scanf("%s", seldir);
    strcat(userhome, seldir); //-->/home/USER/FOLDER
    strcat(userhome, "/");
    strcat(userhome, filearray[2]);
    folderpick = true;
    fprintf(stderr,"The final location will be: %s \nContinue? ", userhome);
    rask = ask();
    if(rask == 0){
        fprintf(stderr,"Continuing...");
    }
    else if(rask == 1){
        fprintf(stderr,"Restarting...\n");
        goto ASKCREATE;
    }
    else{
        fprintf(stderr,"Quitting.\n");
        return 2;
    }
    snprintf(command, 300, "mv %s %s", tdir, userhome);
    cmdresp = system(command);
    if(cmdresp == 0){
        if(dflag == true){fprintf(stderr,
        "Moved %s to %s\n", tdir, userhome
        );}
    }
    else{
        fprintf(stderr,"Failed to move folders.\n");
        return 1;
    }
    goto MAKENATIVE;
    return 1;
}
