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