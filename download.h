#include <stdio.h>
#include <string.h>//Strings
#include <stdlib.h>//Misc, mainly malloc
#include <unistd.h>//For chdir
#include <sys/stat.h>//For creating the dir
#include <stdbool.h>//For bool for the -v flag check
#include <curl/curl.h>

static size_t writeFile(char *data, size_t size, size_t nmemb, void *stream);


char **getfile(void){


    //Basic setup
    //Starts curl
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    //Curl's response.
    CURLcode rerr;
    //File we'll use
    FILE *dFile;
    //---------------
    char *url = URL; 
    char *dname = D_NAME;
    char *tdname = TD_NAME;//Temp dir
    char **arr = malloc(3 * sizeof(char *));
    char *vdname;//Version dir


    //Setting base URL
    curl_easy_setopt(curl, CURLOPT_URL, url);
    rerr = curl_easy_perform(curl);

    //Unelegant use of if statements for the verbose/debug mode but
    //but I'll keep it like this to avoid further trouble

    if(dflag == true){fprintf(stderr,"URL: %s\n", url);}
    //Getting Latest file name from redirect
    if(rerr == CURLE_OK){
        //Get Redirect
        int sc = 0;//size counter for later
        curl_easy_getinfo(curl, CURLINFO_REDIRECT_URL, &url);//Dunno what else to do
        if(dflag == true){fprintf(stderr, "Redirect URL:%s\n", url);}
        char *token = strtok(url, "/");  
        char **nurl = malloc(10 * sizeof(char *));//Allocates 10 pointers. 
                                                
        //Tokenize the URL
        for(int i = 0; token != NULL; i++){
            //Maybe I could've used strdup. Will keep that in mind for future uses
            nurl[i] = malloc(strlen(token) + 1);//Token size + null end char
            strcpy(nurl[i], token);
            token = strtok(NULL, "/");//Continue from where strtok left off
            if(dflag == true){fprintf(stderr,"Token %d is %s\n", i, nurl[i]);}
            //Total length of elements added up
            sc += strlen(nurl[i]);
        }  
        //Rebuilding url for the download of new version
        int msize = (sc + (2 * (strlen(URL))));
        char *murl = malloc(msize + 1);
        snprintf(murl, (msize + 1), "%s//%s/%s/%s/%s/download/%s/", nurl[0], nurl[1], nurl[2], nurl[3], nurl[4], nurl[6]);
        strcpy(url, murl); 

        int n2l;
        if(sflag == true){
            n2l = strlen(nurl[2]);
            nurl[2] = realloc(nurl[2], (n2l * 2));
            strcat(nurl[2], "-");
        }
        else if(sflag == false){
            n2l = strlen(nurl[2]);
            nurl[2] = realloc(nurl[2], (n2l * 4));
            strcat(nurl[2], "-linux-x64-");
        }
        int ns = strlen(nurl[2]) * 2, vs = strlen(nurl[6]), es = strlen(FILE_END);
        nurl[7] = malloc((ns + vs + es) + 1);
        nurl[8] = malloc((ns + vs + (es * 2)) + 1);


        //Making File name
        strcpy(nurl[7], nurl[2]);
        strcat(nurl[7], nurl[6]);
        //Info for later
        arr[2] = malloc(strlen(nurl[7]) + 1);
        strcpy(arr[2], nurl[7]);
        vdname = malloc(strlen(arr[2]) + 1);
        strcpy(vdname, arr[2]);
        //Continue
        if(sflag == true){
            strcat(nurl[7], FILE_END);
        }
        else if(sflag == false){
            strcat(nurl[7], FILE_END_FULL);
        }
        //Making SHA File name
        strcpy(nurl[8], nurl[7]);
        strcat(nurl[8], SHA_END);   
        msize = strlen(murl);
        nurl[9] = malloc(msize + 1);
        //Full URL Probably unnecessary
        strcpy(nurl[9], murl);


        if(vflag == true || dflag == true){
            fprintf(stderr, "FILE: %s\nSHA: %s\nDownload URL: %s\n",
            nurl[7], nurl[8], nurl[9]);
        }
        //Preparing
        ns = strlen(nurl[9]);
        vs = strlen(nurl[7]);
        es = strlen(nurl[8]);

        //Making needed folders
        //Move to /tmp-->dname-->vdname
        if(dflag == true){fprintf(stderr,"Making directories...\n");}
        STARTDIRMAKING: 
        if(chdir(tdname) == 0){
            if(chdir(dname) == 0){
                if(chdir(vdname) == 0){
                    goto DIRMADE;
                }else{goto MKVDNAME;}
            }else{goto MKDNAME;}
        }else{perror("Couldn't change to /tmp"); return arr;}

        MKDNAME: 
        if(mkdir(dname, 0755) == 0){
            if(dflag == true || vflag == true){fprintf(stderr,"Created %s\n",dname);}
            goto STARTDIRMAKING;
        }else{perror("Couldn't create folder"); return arr;}

        MKVDNAME: 
        if(mkdir(vdname, 0755) == 0){
            if(dflag == true || vflag == true){fprintf(stderr,"Created %s\n", vdname);}
            goto STARTDIRMAKING;
        }else{perror("Couldn't create folder"); return arr;}

        DIRMADE:
        //Setting url
        char turl[ns + vs + es + 1];
        strcpy(turl, nurl[9]);
        strcat(turl, nurl[7]);
        curl_easy_setopt(curl, CURLOPT_URL, turl);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        dFile = fopen(nurl[7], "wb");
        if(vflag == true || dflag == true){fprintf(stderr,"File will be downloaded from: %s \n", turl);}
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, dFile);
        curl_easy_perform(curl);

        fclose(dFile);
        if(vflag == true || dflag == true){fprintf(stderr, "File saved\n");}

        //Now for the other
        
        strcpy(turl, nurl[9]);
        strcat(turl, nurl[8]);
        curl_easy_setopt(curl, CURLOPT_URL, turl);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        dFile = fopen(nurl[8], "wb");
        if(vflag == true || dflag == true){fprintf(stderr,"File will be downloaded from: %s \n", turl);}
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFile);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, dFile);
        curl_easy_perform(curl);
        fclose(dFile);
        
        if(vflag == true || dflag == true){fprintf(stderr, "File saved\n");}

        //Last tidying up
        //Tidying up. Maybe some of the pointer were unnecessary but that was fun

        curl_easy_cleanup(curl);
        curl_global_cleanup();
        arr[0] = malloc(strlen(nurl[7]) + 1) ;       
        strcpy(arr[0], nurl[7]);
        arr[1] = malloc(strlen(nurl[8]) + 1);
        strcpy(arr[1], nurl[8]);

        for(int i = 0; i < 10; i++){
            free(nurl[i]);
            nurl[i] = NULL;
        }
        free(murl);
        murl = NULL;
        free(nurl);
        nurl = NULL;
        free(vdname);
        vdname = NULL;
        if(vflag == true || dflag == true){fprintf(stderr,"Done.\n");}
        return arr;
    }
    else{
        fprintf(stderr,"Failed to set URL\n");
        return arr;
    }
}

static size_t writeFile(char *data, size_t size, size_t nmemb, void *stream){
    size_t written = fwrite(data, size, nmemb, (FILE *)stream);
    return written;
}
