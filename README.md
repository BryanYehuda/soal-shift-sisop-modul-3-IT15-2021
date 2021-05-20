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
    // printf("namaFile = %s\n", str);
    char *ret;
    ret = strrchr(str, '/'); 
    if(ret != NULL){
        // printf("%s\n", ret);
        if(ret[1]=='.'){
            token = "hidden";
            return token;
        }
    }
    // if(str[0]=='.'){
    //     token = "hidden";
    //     return token;
    // }
    token = strtok(str, ".");
    // printf("ini token1 = %s\n", token);
    token = strtok(NULL, "");
    // printf("ini token1 = %s\n", token);
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
```