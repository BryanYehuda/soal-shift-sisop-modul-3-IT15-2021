#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include<pthread.h>

void createDirectory(char *ext){
    int check =  mkdir(ext,0777);
}

void moveFile(char *location, char *namaFile, char *ext){
    int ret;
    char newname[2000];
    if(strcmp(ext, "unknown")==0){
        snprintf(newname, sizeof newname, "%s/%s", ext, namaFile);
    }else{
        snprintf(newname, sizeof newname, "%s/%s.%s", ext, namaFile, ext);
    }
    printf("%s %s\n", namaFile, newname);
    ret = rename(location, newname);
}

void processFiles(char *ext, char *location, char *namaFile){
    createDirectory(ext);
    moveFile(location, namaFile, ext);
}

char* getExt(char *namaFile){
    char *token ;
    char *str = namaFile;
    token = strtok(str, ".");
    // printf("%s\n", token);
    token = strtok(NULL, "");
    int jumlah;
    if(token == NULL){
        token = "unknown";
    }
    return token;
}

char* getFileName(char *namaFiles){
    char *token ;
    char *str = namaFiles;
    token = strtok(str, "/");
    int jumlah=0;
    char *namaFile[10];
    while( token != NULL ) {
      namaFile[jumlah] = token;
      token = strtok(NULL, "/");
      jumlah++;
   }
    return namaFile[jumlah-1];
}
pthread_t tid[50];
int main( int argc, char *argv[] )  {
    char *perintah = argv[1];
    char *file = "-f";
    char *directory = "-d";
    if(strcmp(perintah, file)==0){
        for(int i=2;i < argc ; i++){
            char *token, *fileName;
            token = getExt(argv[i]);
            fileName = getFileName(argv[i]);
            // printf("%s\n", token);
            // pthread_create(&(tid[i]), NULL, processFiles, (void*) token, argv[i], fileName);
            processFiles(token, argv[i], fileName);
        }
    }else if(strcmp(perintah, directory)==0){
        printf("ini sebuah directory\n");
    }
}