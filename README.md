# soal-shift-sisop-modul-3-IT15-2021
Ini adalah repository yang dibuat untuk menampung Jawaban Soal Shift Sisop Modul 3

# Soal 3
pertama kami membuat kode yang didalamnya akan memisahkan setiap perintah yang ada dari terminal, kodenya terlihat seperti ini
```c
pthread_t tid[10000];
char *file = "-f";
char *directory = "-d";
int main( int argc, char *argv[] )  {
    char *perintah = argv[1];
    command = argv[1];
    if(strcmp(perintah, file)==0){
        //menampung case -f
    }else if(strcmp(perintah, directory)==0){
        //menampung case -d
    }else if(strcmp(perintah, "*")==0){
        //menampung case current working directory
    }
}
```
Pada soal, dengan memasukkan input "-f" maka user dapat memasukkan banyak file sesuai dengan keinginannya. Files yang akan dimasukkan oleh user merupakan letak serta nama file tersebut. kami menggunakan argc untuk mengetahui banyak input dari user, dan menggunakan perulangan for untuk memproses setiap file inputan user. Unutk setiap input yang dibuat oleh user akan digunakan sebuah thread, dimana thread ini akan memanggil fungsi processFiles. Setelah kita mendaftarkan thread, selanjutnya kita join thread tersebut dengan menggunakan thread join. Berikut adalah implementasinya
```c
if(strcmp(perintah, file)==0){
    for(int i=2;i < argc ; i++){
        pthread_create(&(tid[i]), NULL, processFiles, (char*)argv[i]);
    }
    for(int i=2;i < argc ; i++){
        pthread_join(tid[i],NULL);
    }
}
```  
  


Pada soal, dengan memasukkan input "-d" maka user dapat mengkategorikan sebuah directory. Ide yang muncul saat melihat case ini adalah menggunakan kode untuk melakukan traverse secara rekursif di sebuah directory, untuk itu kami menggunakan kode template listFilesRecursivly pada modul 2 dengan beberapa perubahan didalamnya. Perubahan yang kami tambahkan adalah, untuk setiap files yang sudah di traverse akan disimpan dalam sebuah array (akan dijelaskan lebih lanjut pada penjelasan fungsi traversefilesrecursivly). setelah itu, untuk masing masing file yang ada dalam array akan kami process menggunakan thread dan memanggil fungsi processfiles. setelah membuat thread, maka semua thread nya akan dijoin. Berikut adalah implementasi nya
```c
else if(strcmp(perintah, directory)==0){
    dicName = argv[2];
    listFilesRecursively(argv[2]);
    for(int i=0; i<banyakFile; i++){

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
}
```  
  
    

Pada soal, kita dapat mengkategorikan current directory dengan menggunakan perintah "\*". Hal ini sama dengan kasus "-d" sebelumnya, hanya saja dengan perintah ini kita hanya perlu mengirim current working directory ke fungsi listFilesRecursively. Berikkut adalah implementasi kode yang kami buat
```c
else if(strcmp(perintah, "*")==0){
    char *buf;
    buf=(char *)malloc(100*sizeof(char));
    getcwd(buf,100);
    listFilesRecursively(buf);
    for(int i=0; i<banyakFile; i++){

        pthread_create(&(tid[i]), NULL, processFiles, (char*)infos[i]);
    }
    for(int i=0;i < banyakFile ; i++){
        pthread_join(tid[i],NULL);
    }
}
```
dapat kita lihat pada kode tersebut, kita mendapatkan current working directory dengan memanggil getcwd dan memparsing hasilnya ke variabel buf. setelah itu kita traverse directorynya.  
  
dalam fungsi diatas, kita juga dapat melihat beberapa fungsi seperti processFiles, listFilesRecursively dll. Berikut adalah implementasi dari kode kode diatas  
- processFiles
```c
void *processFiles(void *location){
    char *token, *fileName;
    char *fileLoc, fileLocCoba[2600], *location1;
    struct dirent *de;  // Pointer for directory entry

    fileLoc = (char *) location;
    location1 = (char *) location;
    DIR *dr = opendir(fileLoc);
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        snprintf(fileLocCoba, sizeof fileLocCoba, "%s", fileLoc);
        token = getExt(location1);
        fileName = getFileName(location);
        createDirectory(token);
        moveFile(fileLocCoba, fileName, token);
    }
}
```
pada fungsi processFiles diatas, kita menerima sebuah parameter yang merupakan sebuah lokasi files yang isinya tergantung dengan inputan dari user. pada fungsi ini, semua lokasi files akan dicek terlebih dahulu apakah merupakan sebuah directory atau tidak, jika tidak sebuah directory maka hal pertama yang kami lakukan ambil ext dengan fungsi getExt dan simpan di variabel token, setelah itu ambil filename dengan fungsi getFileName dan simpan didalam variabel fileName. untuk membuat directory, panggil fungsi createDirectory, dan untuk memindahkan file panggil fungsi moveFile. berikut adalah implementasi dari kode kode fungsi diatas  

- getExt
```c
char* getExt(char *namaFile){
    char *token ;
    char *str = namaFile;
    char *ret;
    ret = strrchr(str, '/'); 
    if(ret != NULL){
        if(ret[1]=='.'){
            token = "hidden";
            return token;
        }
    }
    token = strtok(str, ".");
    token = strtok(NULL, "");
    if(token == NULL){
        token = "unknown";
        return token;
    }
    for(int i = 0; token[i]; i++){
       token[i] = tolower(token[i]);
    }
    int jumlah;
    return token;
}
```
pada fungsi diatas pertama kali kita akan memisahkan tanda "/" dari lokasi file menggunakan `strrchr` setelah itu akan dilakukan pengecekan apakah ada tanda titik dipaling pertama dari namafile. jika ada maka itu merupakan kategori hidden, setelah itu kita cari titik paling pertama muncul setelah nama file untuk dijadikan sebuah extensi. extensi yang diambil adalah mulai dari titik pertama muncul hingga ke titik terakhir muncul. Setalah itu, semua ext yang telah didapat akan dibuat kedalam huruf kecil lalu nilainya di return  

  
- getFileName
```c
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
```
untuk mendapatkan nama files, kami menggunakan `strtok` untuk memisahkan semua tanda "/" pada lokasi files, setelah itu kami return nilai dari 2 terakhir yang muncul.  
  
  
- createDirectory
```c
void createDirectory(char *ext){
    int check =  mkdir(ext,0777);
}
```
setelah mendapatkan ext ataupun token dari lokasi file yang diinputkan user, maka kami membuat directory berdasarkan ext tersebut dengan menggunakan perintah `mkdir`.  
  
    
- moveFile
```c
char berhasil = '1';
void moveFile(char *location, char *namaFile, char *ext){
    int ret;
    char newname[2000];
    if(strcmp(ext, "unknown")==0 || strcmp(ext, "hidden")==0 ){
        snprintf(newname, sizeof newname, "%s/%s", ext, namaFile);
    }else{
        snprintf(newname, sizeof newname, "%s/%s.%s", ext, namaFile, ext);
    }
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
```
pada fungsi ini, kita harus menyusun sedemikian rupa dari lokasi files yang dikirim user, nama file yang sudah kita dapat serta ext yang juga sudah kita tentukan agar menjadi suatu lokasi baru dari file tersebut. Setelah kita berhasil menyusun lokasi baru tersebut, maka kita gunakan perintah `rename` dengan parameter pertama adalah lokasi file terkini dan parameter kedua adalah lokasi kemana kita ingin memindahkan file tersebut. Unutk case "-f" setiap file yang dipindahkan akan dicek berhasil dipindahkan atau belum. Untuk kasus "-d" kami menggunakan suatu parameter global yang bernama `berhasil` yang akan dirubah nilainya menjadi 0 jika terjadi kegagalan dalam memindahkan file. Nah, nilai dari berhasil ini yang nanti akan dieavluasi di fungsi "-d"  
  
    
- listFilesRecursively
```c
char infos[10000][10000];
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
            }else{
                char loc[1000];
                strcpy(loc, path);
                snprintf(infos[banyak], sizeof loc, "%s", loc);
                banyak = banyak + 1;
                banyakFile++;
            }
            listFilesRecursively(path);
        }
    }
    closedir(dir);
}
```
kode ini merupakan kode template yang terdapat dalam module2. Perbadaan yang kami lakukan adalah, untuk setiap path yang dicek akan dimasukkan terlebih dahulu kedalam array infos.