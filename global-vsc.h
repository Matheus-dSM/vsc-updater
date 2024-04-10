#include <stdbool.h>

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

struct returnlist{
    int returncode;
    char *filelist[2];//Since we are only dealing with two files, may change later
};

