# soal-shift-sisop-modul-3-IT15-2021
Ini adalah repository yang dibuat untuk menampung Jawaban Soal Shift Sisop Modul 3
# Soal 1
pada soal ini dibutuhkan client dan server yang nantinya digunakan untuk membuat request terhadap perintah yang dipilih. kemudian client akan melakukan register dan login. jika client memilih register maka akan diminta input id dan password yang nantinya dikirimkan ke server . jika client memilih login maka client akan melakukan input id dan password yang sesuai dengan data pada server. Server dapat menerima multiconection dan jika terdapat 2 request client atau lebih maka yang lain akan menunggu terlebih dahulu untuk diproses.

## Client 
# Library yang digunakan untuk soal ini:

#include <stdio.h> = untuk standard input-output

#include <sys/socket.h> = untuk menjalankan socket

#include <stdlib.h> = untuk fungsi umum

#include <netinet/in.h> = untuk alamat domain internet

#include <string.h> = untuk melakukan manipulasi string, misalnya strcmp()

#include <errno.h> = untuk memberikan tambahan error pada sistem

#include <unistd.h> = untuk melakukan system call fork()

#include <arpa/inet.h> = untuk operasi internet

#include <pthread.h> = untuk bisa menjalankan file c

#include <drent.h> = untuk operasi akses direktori

#include <ctype.h> = untuk mendeklarasikan serangkaian fungsi dan mengklasifikasikan karakter

#include <string.h> = untuk melakukan manipulasi string, misalnya strcmp()

pthread_t input, received;

```c
void *input_main();
int error(char *err)
{
    perror(err);
    exit(EXIT_FAILURE);
}

struct user
{
    char name[1000];
    char pwd[1000];
    char file[1000];
    char mode[1000];
    int is_auth;
    int socket;
} user_data;
```
struktur ini digunakan untuk Mendefinisikan port server dan Membuat struct untuk user

```c
int fileExist(char *fname)
{
    int found = 0;
    DIR *di;
    struct dirent *dir;
    di = opendir(SVR);
    while ((dir = readdir(di)) != NULL)
    {
        if (strcmp(dir->d_name, fname) == 0)
        {
            found = 1;
            break;
        }
    }
    closedir(di);
    return found;
}
```
struktur ini berfungsi untuk mengecek apakah file ada file yang disimpan
DIR *di untuk membuka path struct dirent *dir untuk entri direktori while dimana kita dapat membaca sesuatu dari direktori. Jika d_name adalah fname lalu break. apabila tidak ada masalah tutup direktori mengembalikan found

```c
int login(char id[], char password[])
{
    FILE *fp = fopen("akun.txt", "r");
    int is_auth = 0;
    char buffer[1024];
    while (fgets(buffer, 1024, fp) != NULL && is_auth == 0)
    {
        char file_id[1024], file_password[1024];
        char *token = strtok(buffer, ":");
        strcpy(file_id, token);
        token = strtok(NULL, "\n");
        strcpy(file_password, token);

        if (strcmp(id, file_id) == 0 && strcmp(password, file_password) == 0)
        {
            is_auth = 1;
        }
        else
        {
            is_auth = 0;
        }
    }
    fclose(fp);
    return is_auth;
```
stukrur ini dugunakan untuk mendefenisikan fungsi login pada server yang diakses dari client

```c
void *user_cetak(void *arg)
{
  if (strcmp(user_data.mode, "recvstrings") == 0)
  {
    int sock = *(int *)arg;
    char buffer[1024] = {0};
    while (1)
    {
      memset(buffer, 0, 1024);
      if (recv(sock, buffer, 1024, 0) > 1)
      {
        char buffer2[1024];
        strcpy(buffer2, buffer);
        char *token = strtok(buffer2, "\n");
        printf("%s", buffer);
      }
    }
  }
}
```
void *user_cetak untuk cetak input user. Variabel sock untuk menyimpan nilai deskriptor file socket server.
```c
int send_file(int socket, char *fname)
{
  char buffer[MAX_LENGTH] = {0};
  char fpath[MAX_LENGTH];
  strcpy(fpath, CLF);
  strcat(fpath, fname);
  FILE *file = fopen(fpath, "r");
  if (file == NULL)
  {
    printf("File %s not found.\n", fname);
    return -1;
  }
  bzero(buffer, MAX_LENGTH);
  int file_size;
  while ((file_size = fread(buffer, sizeof(char), MAX_LENGTH, file)) > 0)
  {
    if (send(socket, buffer, file_size, 0) < 0)
    {
      fprintf(stderr, "Failed to send file %s. (errno = %d)\n", fname, errno);
      break;
    }
    bzero(buffer, MAX_LENGTH);
  }
  fclose(file);
  return 0;
}

```
int send_file fungsi untuk mengirimkan file. file akan dibuka jika file == NULL akan menampilkan file tidak ada. Lalu akan dilakukan pengecekan pengiriman jika terjadi kesalahan akan menampilkan gagal mengirimkan file.

```c
int receive_file(int socket, char *fname)
{
  pthread_cancel(cetak);
  char buffer[MAX_LENGTH] = {0};
  char fpath[MAX_LENGTH];
  strcpy(fpath, CLF);
  strcat(fpath, fname);
  FILE *file_masuk = fopen(fpath, "wb");
  if (file_masuk == NULL)
  {
    printf("File %s, Cannot be made in client.\n", fname);
  }
  else
  {
    bzero(buffer, MAX_LENGTH);
    int file_size = 0;
    while ((file_size = recv(socket, buffer, MAX_LENGTH, 0)) > 0)
    {
      int write_size = fwrite(buffer, sizeof(char), file_size, file_masuk);
      if (write_size < file_size)
      {
        error("Failed to write file.");
      }
      bzero(buffer, MAX_LENGTH);
      if (file_size == 0 || file_size != MAX_LENGTH)
      {
        break;
      }
    }
    if (file_size < 0)
    {
      if (errno == EAGAIN)
      {
        printf("Timeout.\n");
      }
      else
      {
        fprintf(stderr, "Failure = %d\n", errno);
        exit(1);
      }
    }
    printf("Downloading file from server!\n");
  }
  fclose(file_masuk);
  printf("File %s has been downloaded!\n", user_data.file);
  strcpy(user_data.mode, "recvstrings"); //Set mode ke input strings
  pthread_create(&cetak, NULL, &user_cetak, (void *)&user_data.socket);
}
```
int receive_file fungsi untuk menerima file
file masuk akan dibuka dengan perintah write byte lalu jika file tidak ada maka akan menampilkan File, Tidak dapat dibuat di client. Dimana ukuran file sudah didefinisikan jika tidak sesuai maka akan menampilkan gagal menulis file dan dilakukan pengecekan jika ukuran file sudah sesuai maka file akan didownload dari server apabila sudah didownload akan menampilkan file berhasil didownloads dan file akan ditutup.

```c
void *user_input(void *arg)
{
  while (strcmp(user_data.mode, "recvstrings") == 0)
  {
    char buffer[1024] = {0};
    bzero(buffer, MAX_LENGTH);
    fgets(buffer, MAX_LENGTH, stdin);
    buffer[strcspn(buffer, "\n")] = 0;
  
    send(user_data.socket, buffer, MAX_LENGTH, 0);

    char cmd_line[MAX_LENGTH];
    strcpy(cmd_line, buffer);
    char *cmd = strtok(cmd_line, " "); 

    for (int i = 0; cmd[i]; i++)
    { 
      cmd[i] = tolower(cmd[i]);
    }

    if (strcmp("add", cmd) == 0)
    { 
      strcpy(user_data.mode, "recvimage");
      char *fname;
      cmd = strtok(NULL, " "); 
      fname = cmd;
      strcpy(user_data.file, fname);
      if (!fileExist(fname))
      { 
        printf("File %s missing.\n", fname);
        continue;
      }
      if (send_file(user_data.socket, fname) == 0)
      {
        printf("File has been sent\n");
        strcpy(user_data.mode, "recvstrings");
      }
      else
      {
        printf("File failed to be sent\n");
        strcpy(user_data.mode, "recvstrings");
      }
    }
    else if (strcmp("download", cmd) == 0)
    { 
      strcpy(user_data.mode, "downimage");
      char *fname;
      cmd = strtok(NULL, " ");
      fname = cmd;
      strcpy(user_data.file, fname);
      download(user_data.socket, fname);
    }
  }
  if (strcmp(user_data.mode, "recvimage") == 0)
  {
    if (send_file(user_data.socket, user_data.file) == 0)
    {
      printf("File has been sent\n");
      strcpy(user_data.mode, "recvstrings");
    }
    else
    {
      printf("File failed to be sent\n");
      strcpy(user_data.mode, "recvstrings");
    }
  }
}
```
void *user_input fungsi untuk inputan user dimana Client akan menginput pilihan login dan register. lalu client akan menginputkan id dan passwordnya. Dari data tersebut nantinya akan dikirim ke server. Jika sudah register maka client dapat login. Lalu client dapat menginputkan command add untuk menambahkan file jika file tidak ada akan menampilkan file tidak ada. send(user_data.socket, buffer, MAX_LENGTH, 0); untuk mengirimkan input ke server. cmd[i] = tolower(cmd[i]); untuk merubah input ke lowecase.

```c
int main(int argc, char const *argv[])
{
  struct sockaddr_in address;
  int sock = 0, valread;
  struct sockaddr_in serv_addr;
  char buffer[1024] = {0};

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("\n Socket creation error \n");
    return -1;
  }

  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
  {
    printf("\nInvalid address/ Address not supported \n");
    return -1;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("\nConnection Failed \n");
    return -1;
  }
  else
  {
    user_data.socket = sock;
    printf("Connected to server with the address %d\n", sock);
  }
  strcpy(user_data.mode, "recvstrings"); 

  pthread_create(&cetak, NULL, &user_cetak, (void *)&sock);
  pthread_create(&input, NULL, &user_input, (void *)&sock);
  while (1)
  { 
    if (pthread_join(input, NULL) == 0)
    {
      pthread_create(&input, NULL, &user_input, (void *)&sock);
    }
  }
  if (strcmp(user_data.mode, "recvstrings") == 0)
  {
    pthread_join(cetak, NULL);
  }
  else
  {
    pthread_exit(&cetak);
  }
}

```
Pada fungsi main, dalam fungsi main menggunakan parameter. parameter pertama bertipe data int (argc). Parameter argc berfungsi untuk menunjukkan banyaknya parameter yang digunakan dalam eksekusi program dan yang kedua bertipe data char consts *argv[]). Parameter argv yang akan menyimpan parameter - parameter yang digunakan dalam eksekusi program.
Variabel serv_addr memiliki beberapa field. Field sin_family harus diset menjadi konstanta simbolic AF_INET. Pada field sin_port harus diisikan dengan port. Agar portabilitas program terjamin maka digunakan fungsi htons() untuk mengubah format biner komputer host ke standar network dalam jenis bilangan short. Fungsi ini akan menyesuaikan byte data sehingga tidak terjadi kesalahan membaca antara host maupun jaringan.
Pada fungsi inet_pton() mengubah alamat IP yang dapat dibaca menjadi format IPv6 32bit atau 128bit IPv6 yang dikemas. Disini akan melakukan pengecekan error jika terjadi kesalahan pada alamat maka akan menampilkan Invalid address/ Address not supported.
Pada fungsi connect() akan menghubungkan proses client terhubung dengan server. fungsi ini akan mngembalikan nilai berupa file deskriptor file baru. Argumen kedua adalah penunjuk alamat client dari koneksi. Dan argumen ketiga adalah ukuran dari server_addr. Disini akan melakukan pengecekan error jika terjadi kesalahan pada alamat maka akan menampilkan Connection Failed.

## Server

Library yang digunakan :
#include <stdio.h> = untuk standard input-output

#include <sys/socket.h> = untuk menjalankan socket

#include <stdlib.h> = untuk fungsi umum

#include <netinet/in.h> = untuk alamat domain internet

#include <string.h> = untuk melakukan manipulasi string, misalnya strcmp()

#include <errno.h> = untuk memberikan tambahan error pada sistem

#include <unistd.h> = untuk melakukan system call fork()

#include <arpa/inet.h> = untuk operasi internet

#include <pthread.h> = untuk bisa menjalankan file c

#include <drent.h> = untuk operasi akses direktori

#include <ctype.h> = untuk mendeklarasikan serangkaian fungsi dan mengklasifikasikan karakter

#include <string.h> = untuk melakukan manipulasi string, misalnya strcmp()

#include <time.h> = untuk manipulasi date and time

    
# Soal 2
## Soal 2a
pada soal 2a ini kita akan memasukan matriks 4 x 3 dan juga 3 x 6 lalu dimasukan ke dalam fungsi perkalian matriks dengan code sebagai berikut
```c
void* perkalianMatriks(void *arg)
{
    
    for (int i = iter ; i < (iter + 1) ; i++){
        for(int j = 0; j < 6; j++){
            for(int k =0; k < 4; k++){
                matrikC[i][j] += matrikA[i][k] * matrikB[k][j];         
            }
        }   
    }
    iter++;
}
```
setelah itu hasil perkalian tersebut akan dimasukan ke dalam shared memory lalu diprint.
```c
printf("\nHasil :\n");
for(int i =0; i < 4; i++){
    for(int j = 0; j < 6; j++){
        matriksShare[i][j] =  matrikC[i][j];
        printf("%d\t", matriksShare[i][j]);
    }
    printf("\n");
}
```
## Soal 2b
sama seperti 2a kita juga akan memasukan matriks, disini kita akan memasukan matriks 4 x 6 lalu akan dibandingkan dengan matriks hasil perkalian soal 2a menggunakan shared memory. karena kita akan melakukan proses secara pararel untuk setiap element matriks maka kita akan menggunakan trhead dan juga struct node untuk memparsing iterasi.
```c
pthread_t thread[24];
key_t key = 1234;
int (*matriksShare)[10];
int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
matriksShare = shmat(shmid, 0, 0);

node mynode;

for (int c = 0; c < 4; c++){
	for (int d = 0; d < 6; d++){
		mynode.f=c;
		mynode.h=d;
		mynode.o=matriksShare[c][d];
		// printf("%d %d\n", matrikD[c][d], mynode.o);
		pthread_create(&(thread[c * 6 + d]),NULL,&factorial,(void*) &mynode);
		pthread_join(thread[c * 6 + d],NULL);
	}
}
```
setelah itu akan dimasukan ke dalam fungsi factorial sebagai berikut. nilai dari node f, h, dan o akan dimasukan kedalam int a1, a2, dan a3.
```c
void *factorial(void *a){
	node *args = (node *) a;
	int a1=args->f;
	int a2=args->h;
	int a3=args->o;
	long long fact=1, fact2 = 1;
	int ma = matrikD[a1][a2];

    if(a3==0 || ma==0){
		matrikE[a1][a2] = 0;
	}
	else if (ma > a3)
	{
		for (int i = 1; i <= a3; ++i) {
            fact *= i;
        }
		matrikE[a1][a2] = fact;
	}
	else
	{
		int m = (a3-ma);
		for (int i = 1; i <= a3; ++i) {
            fact *= i;
        }
		for (int i = 1; i <= m; ++i) {
            fact2 *= i;
        }
		matrikE[a1][a2] = fact/fact2;
	}	
}
```
lalu diprint.
```c
printf("\nHasil perhitungan dengan matrix baru :\n");
for (int c = 0; c < 4; c++){
	for (int d = 0; d < 6; d++){
		printf("%d\t", matrikE[c][d]);
	}
	printf("\n");
}
```

## Soal 2c
pada soal 2c kita akan membuat sebuah program untuk Inter-process communication dengan melihat 5 proses teratas pada sistem kita menggunakan pipe. untuk codenya sudah ada di file github.

## Hasil Run
**soal 2a**  
![image](https://user-images.githubusercontent.com/73151866/118959043-26a77200-b98c-11eb-9011-7041760d6a8d.png)  

**soal 2b**  
![image](https://user-images.githubusercontent.com/73151866/118960144-2f4c7800-b98d-11eb-8e29-3c87dc54cdd7.png)  

**soal 2c**  
hasil run program  
![image](https://user-images.githubusercontent.com/73151866/118960330-60c54380-b98d-11eb-8959-07df35d013b7.png)  

hasil run command di terminal  
![image](https://user-images.githubusercontent.com/73151866/118960453-82bec600-b98d-11eb-8d57-f070d93f3832.png)  

## Kendala yang Dihadapi
- Kami bingung bagaimana cara untuk memparsing iterasi untuk digunakan di fungsi factorial.
- Ketika input terlalu besar maka output tergadang menjadi tidak sesuai dengan yang diharapkan.

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

## hasil Run
kami membuat 2 buah file yang akan dikategorikan  

![image_2021-05-20_151556](https://user-images.githubusercontent.com/50267676/118943926-46379e00-b97e-11eb-950b-da8fb1e2c8dd.png)  
dan berikut adalah hasil run untuk case 1  

![Soal3Ian2](https://user-images.githubusercontent.com/50267676/118945010-44220f00-b97f-11eb-8eef-5b10a9e193c8.png)  

Selanjutnya kita mencoba untuk test case 2 `("-d")`  

![Soal3Ian3](https://user-images.githubusercontent.com/50267676/118945598-cc081900-b97f-11eb-9f2c-433ad7c54349.PNG)  
dan berikut adalah hasil run untuk test case 2
![Soal3Ian5](https://user-images.githubusercontent.com/50267676/118946038-3ae57200-b980-11eb-9fd8-90c8d6f6dfaa.PNG)
![Soal3Ian4](https://user-images.githubusercontent.com/50267676/118946045-3c169f00-b980-11eb-8277-c0f25906725f.png)  
  
Selanjutnya kita mencoba untuk test case 3 `("\*")`
![Soal3Ian7](https://user-images.githubusercontent.com/50267676/118946730-e1ca0e00-b980-11eb-8863-d9b7d0e7a2d8.PNG)
![Soal3Ian6](https://user-images.githubusercontent.com/50267676/118946735-e393d180-b980-11eb-8e75-9759ec178c9e.png)

## Kesulitan yang dihadapi
1. Contoh test case yang diberikan sangat sedikit sehingga tidak tahu mana yang benar setalah menjalankan program untuk mengkategorikan directory soal3
