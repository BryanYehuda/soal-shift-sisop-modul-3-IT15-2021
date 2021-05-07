#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <stdlib.h>
#include <ctype.h>

int jumlah=1;
char *command;
char *file = "-f";
char *directory = "-d";
pthread_t tid[10000];

char* getExt(char *namaFile){
    char *token ;
    char *str = namaFile;
    // printf("namaFile = %s\n", str);
    char *ret;
    ret = strchr(str, '/');
    if(ret != NULL){
        // printf("%s\n", ret);
        if(ret[1]=='.'){
            token = "hidden";
            return token;
        }
    }
    token = strtok(str, ".");
    // printf("ini token = %s\n", token);
    token = strtok(NULL, "");
    if(token == NULL){
        token = "unknown";
        return token;
    }
    // printf("%s\n", token);
    // printf("Token = %s\n", token);
    for(int i = 0; token[i]; i++){
       token[i] = tolower(token[i]);
    }
    int jumlah;
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
char berhasil = '1';
void moveFile(char *location, char *namaFile, char *ext){
    int ret;
    char newname[2000];
    if(strcmp(ext, "unknown")==0 || strcmp(ext, "hidden")==0 ){
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
    }else if((strcmp(command, file)!=0)){
        if(ret != 0){
            berhasil = '0';
        }
    }
}
char *dicName;
void *processFiles(void *location){
    char *token, *fileName;
    char *fileLoc, fileLocCoba[2600], *location1;
    struct dirent *de;  // Pointer for directory entry
    
    // opendir() returns a pointer of DIR type. 
    fileLoc = (char *) location;
    location1 = (char *) location;
    DIR *dr = opendir(fileLoc);
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {

        snprintf(fileLocCoba, sizeof fileLocCoba, "%s", fileLoc);
        // printf("FileLoc = %s\n", fileLoc);
        token = getExt(location1);
        fileName = getFileName(location);
        // printf("token = %s filename = %s\n", token, fileName);
        // printf("%s\n", fileName);
        // for(int i = 0; token[i]; i++){
        //    token[i] = tolower(token[i]);
        // }
        createDirectory(token);
        moveFile(fileLocCoba, fileName, token);
    }
}
int banyakFile=0;
char *info[10000];
char infos[10000][10000];
int banyak=0;
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
            // printf("file %s\n", dp->d_name);
            // proceschsFiles(dp->d_name);
            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            char check[10000];
            snprintf(check, sizeof check, "%s", dp->d_name);
            if(check[0]=='.'){
                createDirectory("hidden");
                char moves[10000];
                snprintf(moves, sizeof moves, "hidden/%s", dp->d_name);
                rename(path, moves);
                // printf("hidden\n");
            }else{
                // printf("%s\n", path);
                char loc[1000];
                strcpy(loc, path);
                // printf("%d\n", banyak);
                snprintf(infos[banyak], sizeof loc, "%s", loc);
                // printf("isi = %s", infos[banyak]);
                banyak = banyak + 1;
                banyakFile++;
                // listFilesRecursively(path);
            }
            // strcpy(path, basePath);
            // strcat(path, "/");
            // strcat(path, dp->d_name);
            // printf("path = %s\n", path);
            // pthread_create(&(tid[banyakFile]), NULL, processFiles, (char*)path);
            // printf("processPath = %s\n", path);
            // printf("banyakFile = %d\n", banyakFile);
            // char loc[1000];
            // strcpy(loc, path);
            // // info[banyak] = loc;
            // // banyak++;
            // // printf("info %d = %s\n", banyakFile, info[2][1]);
            // //jagnan lupa join
            // // processFiles(path);
            // snprintf(infos[banyak], sizeof loc, "%s", loc);
            // // info[banyak] = loc;
            // // if(banyak >0 ){
            // //     printf("info %d = %s\n", banyakFile, infos[banyak-1]);
            // // }
            // // printf("info %d = %s\n", banyakFile, infos[banyak]);
            // banyak = banyak + 1;
            // banyakFile++;
            listFilesRecursively(path);
            // pthread_create(&(tid[banyakFile]), NULL, processFiles, (char*)path);
            // processFiles(path);
        }
    }
    // printf("info %d = %s\n", banyakFile, info[2]);
    closedir(dir);
}

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
        // printf("ini sebuah directory\n");
        dicName = argv[2];
        listFilesRecursively(argv[2]);
        // printf("%s\n", info[1]);
        // printf("banyak files = %d\n", banyakFile);
        // for(int i=0; i<banyakFile; i++){
        //     printf("%s\n", infos[i]);
        // }
        for(int i=0; i<banyakFile; i++){
            // printf("%s\n", infos[i]);
            pthread_create(&(tid[i]), NULL, processFiles, (char*)infos[i]);
        }
        for(int i=0;i < banyakFile ; i++){
            pthread_join(tid[i],NULL);
        }
        if(berhasil=='1'){
            printf("Direktori sukses disimpan!\n");
        }else{
            printf("Yah, gagal disimpan :(");
        }
    }else if(strcmp(perintah, "*")==0){
        // printf("%s\n", argv[1]);
        char *buf;
        buf=(char *)malloc(100*sizeof(char));
        getcwd(buf,100);
        // printf("\n %s \n",buf);
        listFilesRecursively(buf);
        for(int i=0; i<banyakFile; i++){
            // printf("%s\n", infos[i]);
        }
        // for(int i=0; i<banyakFile; i++){
        //     processFiles(infos[i]);
        // }
        for(int i=0; i<banyakFile; i++){
            pthread_create(&(tid[i]), NULL, processFiles, (char*)infos[i]);
        }
        for(int i=0;i < banyakFile ; i++){
            pthread_join(tid[i],NULL);
        }
    }
}