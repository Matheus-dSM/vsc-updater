#include <stdbool.h>
#include <archive.h>
#include <stdio.h>
#include <ctype.h> // For toupper
#include <archive_entry.h>

//TODO Maybe make these not hardcoded?
#define URL "https://github.com/VSCodium/vscodium/releases/latest/"
#define FILE_END "-src.tar.gz"
#define SHA_END ".sha256"
#define D_NAME "vscu"
#define TD_NAME "/tmp"
#define VERSION "1.0.0"//Just an example

extern bool vflag;
extern bool dflag;
extern bool sdflag;
extern bool ssflag;


static int copy_data(struct archive *archRead, struct archive *archWrite){
    int response;
    const void *buffer;
    size_t size;
    la_int64_t offset;
    for (;;){
        response = archive_read_data_block(archRead, &buffer, &size, &offset);
        if(response == ARCHIVE_EOF){//If end of file
            return (ARCHIVE_EOF);
        }
        if(response < ARCHIVE_OK){//Warn if error
            return (response);
        }
        response = archive_write_data_block(archWrite, buffer, size, offset);
        if(response < ARCHIVE_OK){//Warn if error
            fprintf(stderr, "%s\n", archive_error_string(archWrite));
            return (response);
        }
    }
}

static int extract(const char *filename){
    struct archive *reader;
    struct archive *writer;
    struct archive_entry *entry;
    int flags;
    int response;

    //What to keep from extracted files
    flags = ARCHIVE_EXTRACT_TIME;
    flags |= ARCHIVE_EXTRACT_PERM;
    flags |= ARCHIVE_EXTRACT_ACL;
    flags |= ARCHIVE_EXTRACT_FFLAGS;

    //Initialize reader and set options
    reader = archive_read_new();
    archive_read_support_format_all(reader);
    archive_read_support_filter_all(reader);
    //Initialize writer and set option
    writer = archive_write_disk_new();
    archive_write_disk_set_options(writer, flags);
    archive_write_disk_set_standard_lookup(writer);
    //Open archive, quit if fail
    if((response = archive_read_open_filename(reader, filename, 10240))){
        exit(1);
    }
    //Loop through entries
    for(;;){
        //Read next entry in archive
        response = archive_read_next_header(reader, &entry);
        if(response == ARCHIVE_EOF){//Stop if archive end
            break;
        }
        if(response < ARCHIVE_OK){//Warn if error
            fprintf(stderr, "%s\n", archive_error_string(reader));
        }
        if(response < ARCHIVE_WARN){//Quit if big error
            exit(1);
        }
        //Write what was read
        response = archive_write_header(writer, entry);
        if(response < ARCHIVE_OK){//Warn if error
            fprintf(stderr, "%s\n", archive_error_string(writer));
        }
        //If no error
        else if(archive_entry_size(entry) > 0){//Check if it has data
            response = copy_data(reader, writer);//Exchange data between them reader-->writer
            if(response < ARCHIVE_OK){//Warn if error
                fprintf(stderr, "%s\n", archive_error_string(writer));
            }
            if(response < ARCHIVE_WARN){//Quit if big error
                exit(1);
            }
        }
    }
    //After done, last touches to what is being written
    response = archive_write_finish_entry(writer);
        if(response < ARCHIVE_OK){//Warn if error
            fprintf(stderr, "%s\n", archive_error_string(writer));
        }
        if(response < ARCHIVE_WARN){//Quit if big error
            exit(1);
        }
    //Closing and freeing them
    archive_read_close(reader);
    archive_read_free(reader);
    archive_write_close(writer);
    archive_write_free(writer);
    return 0;
}
void strup(char *chptr){
    for(int i = 0; chptr[i] != '\0'; i++){
        chptr[i] = toupper(chptr[i]);
    }
}
int ask(void){
    ASKCONT: 
    char ans[10];
    int fail = 0;
    if(fail == 0){
        fprintf(stderr," [Y]es/[N]o: ");
    }
    if(fail == 3){
        fprintf(stderr,"Too many failed attempts. Quitting...\n");
        return 1;
    }
    scanf("%s", &ans);
    strup(ans);
    if(strcmp(ans, "NO") == 0 || strcmp(ans, "N") == 0 || 
       strcmp(ans, "QUIT") == 0 || strcmp(ans, "Q") == 0 ||
       strcmp(ans, "CLOSE") == 0 || strcmp(ans, "C") == 0){
        fprintf(stderr,"Closing...\n");
        return 1;
    }
    else if(strcmp(ans, "YES") == 0 || strcmp(ans, "Y") == 0){
        return 0;
    }
    else{
        fprintf(stderr,"Response not recognized.\n" \
        "To continue, type Y or Yes.\n"\
        "To cancel, type N or No.\n");
        fail++;
        goto ASKCONT;
    }
}