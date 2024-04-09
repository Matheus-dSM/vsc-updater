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
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();
    CURLcode rerr;
    FILE *dFile;
    char *url = URL; 
    char *dname = D_NAME;
    char *tdname = TD_NAME;
    char **arr;
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
        int msize = (sc + (2 * (strlen(URL))));
        char *murl = malloc(msize + 1);
        snprintf(murl, (msize + 1), "%s//%s/%s/%s/%s/download/%s/", nurl[0], nurl[1], nurl[2], nurl[3], nurl[4], nurl[6]);
        strcpy(url, murl); 
        int ns = strlen(nurl[2]), vs = strlen(nurl[6]), es = strlen(FILE_END);
        nurl[7] = malloc((ns + vs + es) + 1);
        nurl[8] = malloc((ns + vs + (es = strlen(SHA_END))) + 1);
        //Making File name
        strcat(nurl[2], "-");
        strcpy(nurl[7], nurl[2]);
        strcat(nurl[7], nurl[6]);
        strcat(nurl[7], FILE_END);
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

        //Moving to tmp and creating a dir there. There may be a faster way of doing this
        //Assumes /tmp is created by default
        if(chdir(tdname) == 0){
            if(vflag == true || dflag == true){fprintf(stderr, "Moved to %s\n", tdname);}
            //Try to move first, if not then create
            if(chdir(dname) == 0){
                if(vflag == true || dflag == true){fprintf(stderr,"%s already created, moving to it\nContinuing...\n", dname);}
            }
            else if(mkdir(dname, 0755) == 0){
                if(vflag == true || dflag == true){fprintf(stderr, "Created %s\n", dname);}
                if(chdir(dname) == 0){
                    if(vflag == true || dflag == true){fprintf(stderr,"Moved to %s\nContinuing...\n", dname);}
                }
                else{
                    perror("Failed to move to new directory");
                    return arr;
                }
            }
            else{
                perror("Failed to create new directory");
                return arr;
            }
        }
        else{
            perror("Failed to move to /tmp");
            return arr;
        }

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
        
        //Closing the rest
        if(vflag == true || dflag == true){fprintf(stderr, "File saved\n");}
        curl_easy_cleanup(curl);
        curl_global_cleanup();

        //Last tidying up
        //Tidying up. Maybe some of the pointer were unnecessary but that was fun

        arr = malloc(2 * sizeof(char *));
        arr[0] = malloc(strlen(nurl[7]) + 1) ;       
        strcpy(arr[0], nurl[7]);
        arr[1] = malloc(strlen(nurl[8]) + 1);
        strcpy(arr[1], nurl[8]);
        free(murl);
        murl = NULL;
        for(int i = 0; i < 10; i++){
            free(nurl[i]);
            nurl[i] = NULL;
        }
        free(nurl);
        nurl = NULL;
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
