#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>

int jumlah=1;
char *command;
char *file = "-f";
char *directory = "-d";
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
    // printf("%s %s %s\n", location, namaFile, newname);
    ret = rename(location, newname);
    if(strcmp(command, file)==0){
        if(ret == 0) {
            printf("File %d : Berhasil Dikategorikan\n", jumlah);
        } else {
            printf("File %d : Sad, gagal :(\n", jumlah);
        }
        jumlah++;
    }
}
char *dicName;
void *processFiles(void *location){
    char *token, *fileName;
    char *fileLoc, fileLocCoba[2600];
    fileLoc = (char *) location;
    if(strcmp(command, directory)==0){
        snprintf(fileLocCoba, sizeof fileLocCoba, "%s/%s", dicName,fileLoc);
    }else{
        snprintf(fileLocCoba, sizeof fileLocCoba, "%s", fileLoc);
    }
    token = getExt(location);
    fileName = getFileName(location);
    printf("%s %s\n", token, fileName);
    createDirectory(token);
    moveFile(fileLocCoba, fileName, token);
}
void listFilesRecursively(char *basePath)
{
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            printf("%s\n", dp->d_name);
            processFiles(dp->d_name);
            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            listFilesRecursively(path);
        }
    }

    closedir(dir);
}
pthread_t tid[100];
int main( int argc, char *argv[] )  {
    char *perintah = argv[1];
    command = argv[1];
    if(strcmp(perintah, file)==0){
        for(int i=2;i < argc ; i++){
            // printf("%s\n", token);
            pthread_create(&(tid[i]), NULL, processFiles, (char*)argv[i]);
            // processFiles(token, argv[i], fileName)
        }
        for(int i=2;i < argc ; i++){
            pthread_join(tid[i],NULL);
        }
    }else if(strcmp(perintah, directory)==0){
        printf("ini sebuah directory\n");
        dicName = argv[2];
        listFilesRecursively(argv[2]);
    }
}