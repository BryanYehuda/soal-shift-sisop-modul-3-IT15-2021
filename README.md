# soal-shift-sisop-modul-3-IT15-2021
## Laporan Pengerjaan Modul 3 Praktikum Sistem Operasi    

### Nama Anggota Kelompok:  
1. Bryan Yehuda Mannuel
2. Calvin Mannuel Simatupang
3. Ian Felix Jonathan Simanjuntak
4. Muhammad Nur Fauzan

# Soal 1
pada soal ini dibutuhkan client dan server yang nantinya akan digunakan untuk membuat request terhadap perintah yang dipilih. kemudian client akan melakukan register dan login. jika client memilih register maka akan diminta input id dan password yang nantinya dikirimkan ke server . jika client memilih login maka client akan melakukan input id dan password yang sesuai dengan data pada server. Server dapat menerima multiconection dan jika terdapat 2 request client atau lebih maka yang lain akan menunggu terlebih dahulu untuk diproses.

## Client 
### Library yang digunakan untuk soal ini:

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

```c
#define PORT 8080
#define MAX_LENGTH 1000
#define SVR "./FILES/"

pthread_t input, received;

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

Mendefinisikan port server
Membuat struct user yang terdiri atas nama ,password user, file

```c
void message(char input[])
{
    char buffer[1024];
    sprintf(buffer, "\n%s\n", input);
    send(user_data.socket, buffer, 1024, 0);
}
```
Fungsi untuk mengirimkan pesan ke client

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
}
```
Fungsi untuk melakukan login
file akun.txt akan dibuka dan dibaca disimpan dalam buffer ukr 1024 karakter. Lalu id dan password pada file akan dipisahkan. Jika id dan password sudah sesuai maka return ke 1.

```c
void regist(char id[], char password[])
{
    FILE *fp = fopen("akun.txt", "a"); 
    fprintf(fp, "%s:%s\n", id, password);
    fclose(fp);
}
```
void regist fungsi untuk melakukan register
File akun.txt akan dibuka lalu client akan menuliskan id dan password didalam file tersebut. Jika sudah maka akan mencetak id dan password

```c
int addtsv(int year)
{
    FILE *fp;
    fp = fopen("files.tsv", "a+");
    if (fp == NULL)
    {
        printf("File cannot be opened");
        return 1;
    }
    fprintf(fp, "\nFILES/%s\t%s\t%d", user_data.file, user_data.name, year);
    fclose(fp);
    return 0;
    }
    void catatlog(char argument[])
{
    FILE *fp;
    fp = fopen("running.log", "a+");
    fprintf(fp, argument);
    fclose(fp);
}
```
int addtsv fungsi untuk tambahkan file tsv
file "files.tsv akan dibuka a+ terbuka untuk membaca dan menulis. jika fp == NULL maka file tidak dapat dibuka.
void catatlog fungsi untuk mencetak log, pada running.log akan mencatat command add

```c
int receive_file(int socket, char *fname)
{
    char buffer[MAX_LENGTH] = {0};
    char fpath[MAX_LENGTH];
    strcpy(fpath, SVR);
    strcat(fpath, fname);
    FILE *file_masuk = fopen(fpath, "wb");
    if (file_masuk == NULL)
    {
        printf("File %s, cannot be made in the server\n", fname);
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
                error("Failed to write on the server");
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
                printf("Timeout\n");
            }
            else
            {
                fprintf(stderr, "Failure = %d\n", errno);
                exit(1);
            }
        }
        printf("Receiving file from client\n");
    }
    fclose(file_masuk);
    printf("File %s from client successfully accepted\n", user_data.file);
    char msg[1000], publisher[1000], path[1000], log[1000];
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    int year = local->tm_year + 1900;

    strcpy(publisher, user_data.name);
    strcpy(path, user_data.file);
    sprintf(msg, "Publisher: %s\nTahun Publikasi: %d\nFilepath: Client/%s", publisher, year, path);
    message(msg);
    addtsv(year);
    sprintf(log, "Add : %s (%s:%s)\n", user_data.file, user_data.name, user_data.pwd);
    catatlog(log);                         
    strcpy(user_data.mode, "recvstrings"); 
}
```
receive_file fungsi untuk menerima file. Server akan menerima file, ukuran file, dan isi dari file tersebut. Dimana akan dilakukan pengecekan jika file gagal ditulis maka akan error menampilkan "gagal menulis file pada server". Jika errno == EAGAIN EAGAIN yang berarti tidak ada data yang tersedia maka akan menampilkan waktu habis. Apabila berhasil maka akan server akan menerima file dan menampilkan menerima file dari client dan file ditutup.
output akan ditampilkan ke client berupa Nama, Publisher, Tahun publishing, Ekstensi File, Filepath.

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
int send_file(int socket, char *fname)
{
    char buffer[MAX_LENGTH] = {0};
    char fpath[MAX_LENGTH];
    strcpy(fpath, SVR);
    strcat(fpath, fname);
    FILE *file = fopen(fpath, "r");
    if (file == NULL)
    {
        printf("File %s missing\n", fname);
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
int fileExist fungsi untuk mengecek apakah file ada pada server.
int send_file fungsi untuk mengirim file. File akan dibuka r yang artinya buka file teks untuk membaca. Jika file == NULL maka akan menampilkan file tidak ada.
menginisiasi variabel buffer dengan fungsi bzero(). Variabel buffer akan mengambil data dari socket dengan fungsi fread. Selanjutnya akan dilakukan pengecekan fungsi send akan mengirimkan file melalui socket. Jika tidak sesuai maka gagal mengirimkan file.

```c
void download(int accept_sockfd, char *fname)
{
    char buffer[MAX_LENGTH];
    if (fileExist(fname))
    {
        printf("File %s is not on the server\n", fname);
        char msg[1024];
        if (send_file(accept_sockfd, fname) == 0)
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
    else
    {
        char msg[1024];
        sprintf(msg, "File %s is not on the server", fname);
        message(msg);
        strcpy(user_data.mode, "recvstrings");
    }
}

```
void download fungsi untuk mendownload files
jika file sudah diserver maka akan mengirimkan pesan ke server dan client dan jika file sudah dikirimkan ke client maka akan menampilkan file berhasil dikirimkan jika tidak maka akan menampilkan file gagal dikirimkan
Apabila file tidak terdapat diserver maka akan menampilkan file tidak ada diserver
void addfile()

```c
void addfile()
{
    if (fileExist(user_data.file))
    {
        message("File is already on the server\n");
        strcpy(user_data.mode, "recvstrings");
    }
    else
    {
        message("File is not on the server, Preparing to receive file");
        strcpy(user_data.mode, "recvimage");
        receive_file(user_data.socket, user_data.file);
    }
}
void deletedb(char word[])
{ 
    FILE *fp;
    char line[1024];
    char *buffer;
    char *ptr;

    buffer = (char *)malloc(1000 * sizeof(char));
    memset(buffer, 0, 1000 * sizeof(char));
    ptr = buffer;

    fp = fopen("files.tsv", "r");
    if (fp != NULL)
    {
        while (!feof(fp))
        {
            fgets(line, 1000, fp);
            if (strstr(line, word) == NULL)
            {
                strcpy(ptr, line);
                ptr += strlen(line);
            }
        }
        fclose(fp);

        fp = fopen("files.tsv", "w");
        fprintf(fp, "%s", buffer);
        fclose(fp);
    }
    else
    {
        printf("File failed to be opened");
    }
}
void deletefile()
{
    char newName[1000], name[1000], db[1000];
    chdir("FILES"); 
    sprintf(newName, "old-%s", user_data.file);
    if (rename(user_data.file, newName) == 0)
    {
        message("File successfully deleted");
        printf("File %s has been deleted", user_data.file);
        chdir("../"); 
        sprintf(name, "Delete : %s (%s:%s)", user_data.file, user_data.name, user_data.pwd);
        catatlog(name); 
        sprintf(db, "FILES/%s", user_data.file);
        deletedb(db);
    }
    else
    {
        printf("File %s failed to be deleted", user_data.file);
        message("File failed to be deleted");
    }
}

```
void addfile fungsi untuk menambahkan file keserver
void deletedb fungsi untuk menghapus data di file.tsv. server akan memanggil fungsi deletedb
voud deletfiles fungsi untuk menghapus file dimana client dapat menghapus file yang tersimpan di server. Jika file yang dibuang ternyata file yang penting, maka file hanya akan diganti namanya menjadi ‘old-NamaFile.ekstensi’. Ketika file telah diubah namanya, maka row dari file tersebut di file.tsv akan terhapus. catatlog(name) akan mencatat di log. Apabila file gagal dihapus maka akan menampilkan "Gagal menghapus files"

```c
void seeallfile()
{ 
    FILE *fp = fopen("files.tsv", "r");
    if (!fp)
    {
        printf("Can't open file\n");
    }
    else
    {
        char buffer[1024], buff[1024];
        char path[1024], publisher[1024];
        char getname2[1024], publisher2[1024], year[1024], ext[1024], path2[1024];
        int row = 0;
        int column = 0;

        while (fgets(buffer, 1024, fp))
        {
            column = 0;
            row++;

            if (row == 1) 
                continue;

            char *value = strtok(buffer, "\t");
            while (value != NULL)
            {
                if (column == 0)
                {
                    sprintf(path, "%s", value);
                }
                else if (column == 1)
                {
                    sprintf(publisher, "%s", value);
                }
                else if (column == 2)
                {
                    char *getname = strtok(path, "FILES/");
                    sprintf(getname2, "Nama: %s", getname);
                    message(getname2);
                    sprintf(publisher2, "Publisher: %s", publisher);
                    message(publisher2);
                    sprintf(year, "Tahun Publishing: %s", value);
                    message(year);
                    if (!strrchr(getname, '.'))
                    {
                        message("File Extension: No Extensions");
                    }
                    else
                    {
                        sprintf(ext, "File Extension: %s", strrchr(getname, '.') + 1);
                        message(ext);
                    }
                    sprintf(path2, "Filepath: %s", path);
                    message(path2);
                }
                value = strtok(NULL, "\t");
                column++;
            }
            message(" ");
        }
        fclose(fp);
    }
}
void find(char *word)
{ 
    char line[1024];
    char buffer[1024];
    FILE *fp = fopen("files.tsv", "r");
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (strstr(line, word) == NULL)
        {
            message(line);
        }
    }
    fclose(fp);
}

void setfile(char cmd[])
{ 
    char *fname;
    cmd = strtok(NULL, " "); 
    fname = cmd;
    memset(user_data.file, 0, 1000);
    strcpy(user_data.file, fname);
}


```
void seeallfile() fungsi untuk melihat isi files.tsv. Dimana files.tsv akan dibuka jika tidak dapat dibuka akan menampilkan "Can't open file\n".
Client dapat melihat semua isi files.tsv dengan memanggil suatu perintah yang bernama see. Dan output tersebut akan keluar dengan format yang telah ditentukan pada soal.
void find yang nantinya client melakukan pencarian maka server akan memanggil fungsi find
void setfile fungsi untuk split input dan set struct file dimana jika client menginput maka fungsi ini akan dipanggil server

```c
void loginsukses()
{
    char msg[1024], buffer[1024];
    message("\e[1;1H\e[2J");
    printf("\nUser %s Has successfully logged in.\n", user_data.name);
    message("Login Success!");
    while (strcmp(buffer, "exit") != 0 || strcmp(user_data.mode, "recvstrings") == 0)
    {
        message("Input Command: ");
        read(user_data.socket, buffer, 1024);
        char cmd_line[MAX_LENGTH];
        strcpy(cmd_line, buffer);
        char *cmd = strtok(cmd_line, " ");
        for (int i = 0; cmd[i]; i++)
        {
            cmd[i] = tolower(cmd[i]);
        }
        if (strcmp(cmd, "add") == 0)
        {
            setfile(cmd);
            addfile(); 
        }
        else if (strcmp(cmd, "download") == 0)
        { 
            setfile(cmd);
            download(user_data.socket, user_data.file);
        }
        else if (strcmp(cmd, "delete") == 0)
        { 
            setfile(cmd);
            deletefile();
        }
        else if (strcmp(cmd, "see") == 0)
        { 
            seeallfile();
        }
        else if (strcmp(cmd, "find") == 0)
        {
            setfile(cmd);
            find(user_data.file);
        }
        else if (strcmp(cmd, "clear") == 0)
        {
            message("\e[1;1H\e[2J");
        }
        else
        {
            printf("\nArgument Invalid!");
            message("Argument Invalid!");
        }
    }
}

```
void loginsukses fungsi login sukses apabila client dapat login dan akan menampilkan telah berhasil login dan pesan login berhasil
Server akan menampilkan pesan input command. Jika client command add makan akan menambahkan file addfile file akan ditambahkan diserver. Jika client command download Server akan mencari file yang ingin client download kemudian mengirim file beserta ukuran dan isinya ke client. Server harus melihat dari files.tsv untuk melakukan pengecekan apakah file tersebut valid. Jika tidak valid, maka mengirimkan pesan error balik ke client. Jika berhasil, file akan dikirim dan akan diterima ke client di folder client tersebut. Client juga dapat menghapus file dnegan command delete yang nantinya fungsi deletefile() akan dipanggil. Lalu Client dapat melihat semua isi files.tsv dengan command see maka fungsi seeallfile() yang telah dibuat akan dipanggil. Agar client dapat melakukan pencarian dengan command finf maka fungsi find() akan dipanggil untuk dapat dijalankan. Jika command tidak sesuai maka akan menampilkan "Argument Invalid".

```c
void *input_main()
{
    char buffer[1024];
    while (1)
    {
        if (user_data.is_auth == 0)
        { 
            message("1. Login\n"
                    "2. Register\n"
                    "Choices: ");

            read(user_data.socket, buffer, 1024); 
            for (int i = 0; buffer[i]; i++)
            { 
                buffer[i] = tolower(buffer[i]);
            }
            if (strcmp(buffer, "login") == 0 || strcmp(buffer, "1") == 0)
            {
                char id[1024];
                char password[1024];
                message("\e[1;1H\e[2J");
                message("Id: ");
                read(user_data.socket, id, 1024);

                message("Password: ");
                read(user_data.socket, password, 1024);

                user_data.is_auth = login(id, password); 
                if (user_data.is_auth == 0)
                { 
                    message("\e[1;1H\e[2J");
                    message("Login failed id/password is wrong!");
                    printf("Login failed id/password is wrong!\n");
                    break;
                }
                else if (user_data.is_auth == 1)
                {
                    strcpy(user_data.name, id);
                    strcpy(user_data.pwd, password);
                    strcpy(user_data.mode, "recvstrings");
                    loginsukses();
                }
            }
            if (strcmp(buffer, "register") == 0 || strcmp(buffer, "2") == 0)
            {
                char id[1024];
                char password[1024];
                message("\e[1;1H\e[2J");
                message("Id: ");
                read(user_data.socket, id, 1024);

                message("Password: ");
                read(user_data.socket, password, 1024);

                regist(id, password); 

                char msg[1024];
                message("\e[1;1H\e[2J");
                printf("User %s is registered", id);
                sprintf(msg, "User %s Register Completed... (Press Anything to Continue)", id);
                message(msg);
                break;
            }
        }
        else if (user_data.is_auth == 1)
        { 
            loginsukses();
        }
    }
}

```
pada fungsi input main ini akan menampilkan menu pilihan berupa login dan register client diminta untuk memilih dan menginputkan jika user memilih atau mengetik login maka akan menampilkan pesan ke client berupa id dan password lalu server akan mendapatkan data id dan password client. Apabila terjadi kesalahan pada client dan server maka akan menampilkan pesan "Login gagal id/password salah!". Jika berhasil maka memanggil fungsi loginsukses(). Agar client dapat login maka client register terlebih dahulu. Apabila client memilih register atau mnegetik 2 maka client akan menginputkan id dan password lalu server akan mendapatkan data dari client dan fungsi regist akan dipanggil. Apabila client berhasil untuk registrasi maka akan menampilkan user telah terdaftar.

```c
int main()
{
    int server_fd, clientsocket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Server is running\nMaking folder FILES...\n");
        int check;
        char *dirname = "FILES";
        check = mkdir(dirname, 0777);

        if (!check)
        {
            printf("Folder has been made\n");
            printf("Waiting connection from client.....\n");
        }
        else
        {
            printf("Folder failed to be made\n");
            exit(1);
        }
    }

    if (listen(server_fd, 1) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((clientsocket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Connected with client with the address: %d\n", clientsocket);
        user_data.socket = clientsocket;
    }
    user_data.is_auth = 0;

    pthread_create(&input, NULL, &input_main, 0);
    while (1)
    { 
        if (pthread_join(input, NULL) == 0)
        {
            pthread_create(&input, NULL, &input_main, 0);
        }
    }
}
```
Pada fungsi main, fungsi socket memiliki 3 parameter dan mengembalikan nilai integer, nilai integer inilah yang akan digunakan sebagai identifier file descriptor untuk melakukan IPC. Parameter pertama kita gunakan AF_INET, ini menunjukan bahwa kita akan membuat aplikasi yang berbasis pada IPv4. Parameter kedua kita akan menggunakan SOCK_STREAM, ini menunjukan bahwa kita akan membuat aplikasi yang menggunakan protocol TCP yaitu koneksi yang datanya ditransmisikan secara reliable atau bersifat connection oriented, koneksinya terbuat dulu baru data bisa dikirim dan diterima. Parameter ketiga adalah kita menggunakan angka 0, kita gunakan 0 agar penerapan protocol yang digunakan dapat diserahkan pengaturannya pada sistem operasi.
Variabel server_ad memiliki beberapa field. Field sin_family harus diset menjadi konstanta simbolic AF_INET. Pada field sin_port harus diisikan dengan port. Agar portabilitas program terjamin maka digunakan fungsi htons() untuk mengubah format biner komputer host ke standar network dalam jenis bilangan short. Fungsi ini akan menyesuaikan byte data sehingga tidak terjadi kesalahan membaca antara host maupun jaringan.
fungsi bind() digunakan untuk mengikat antara informasi alamat IP yang digunakan untuk tujuan pengiriman data dengan file descriptor dari socket yang telah dibuat. Parameter pertama yang digunakan adalah integer hasil dari pembuatan fungsi socket sebelumnya. Parameter kedua berisi informasi yang akan diikat dengan file descriptor socket, informasi tersebut dibuat dalam satu struct yaitu struct sockaddr. Parameter ketiga berisi ukuran dari struct sockaddr yang telah dibuat. Apabila server bisa dijalankan maka akan menampilkan "Server berhasil dijalankan\nMembuat folder FILES...\n"
fungsi listen digunakan untuk membuat socket siap menerima connection dengan fungsi accept nantinya. Parameter pertama yang digunakan adalah integer hasil dari pembuatan fungsi socket sebelumnya. Parameter kedua diisi jumlah antrian yang bisa socket terima disini yang digunakan adalah 1, karena kita hanya akan membuat iterative server.
fungsi accept ini menerima dari antrian koneksi yang masuk pada server, membuat file descriptor baru sebagai media komunikasi yang akan digunakan pada pertukaran data nantinya dan merujuk pada socket yang telah dibuat sebelumnya. Parameter pertama yang digunakan adalah integer hasil dari pembuatan fungsi socket sebelumnya. Parameter kedua adalah adalah pointer ke struct sockaddr. Pointer ini berisi informasi mengenai peer yang terkoneksi dengan server. Parameter ketiga berisi ukuran dari pointer sockaddr yang menampung informasi client, nilai ini harus berbentuk pointer structure dari ukuran struct addr. Apabila client sudah terhubung dengan server maka akan menampilkan "Terhubung dengan klien yang beralamat:".

## Screenshot Hasil Run :
![messageImage_1621781470998](https://user-images.githubusercontent.com/71550384/119268947-60dc7200-bc1f-11eb-8221-8ec0679d58b0.jpg)
![messageImage_1621781259424](https://user-images.githubusercontent.com/71550384/119268950-620d9f00-bc1f-11eb-906f-7dcc1bebfaa9.jpg)
![messageImage_1621781223578](https://user-images.githubusercontent.com/71550384/119268951-62a63580-bc1f-11eb-901e-75ebf61fc064.jpg)
![messageImage_1621781193780](https://user-images.githubusercontent.com/71550384/119268952-633ecc00-bc1f-11eb-863c-00f958caa888.jpg)
![messageImage_1621781176223](https://user-images.githubusercontent.com/71550384/119268953-633ecc00-bc1f-11eb-9427-38d77e7f5b8c.jpg)
![messageImage_1621781486361](https://user-images.githubusercontent.com/71550384/119268954-63d76280-bc1f-11eb-9368-11b327977c9f.jpg)




# Soal 2
## Soal 2A
pada soal 2A ini kita akan memasukan matriks 4 x 3 dan juga 3 x 6 lalu dimasukan ke dalam fungsi perkalian matriks dengan code sebagai berikut
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
## Soal 2B
sama seperti 2A kita juga akan memasukan matriks, disini kita akan memasukan matriks 4 x 6 lalu akan dibandingkan dengan matriks hasil perkalian soal 2A menggunakan shared memory. karena kita akan melakukan proses secara pararel untuk setiap element matriks maka kita akan menggunakan trhead dan juga struct node untuk memparsing iterasi.
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

## Soal 2C
pada soal 2C kita akan membuat sebuah program untuk Inter-process communication dengan melihat 5 proses teratas pada sistem kita menggunakan pipe. untuk codenya sudah ada di file github.

## Hasil Run
**soal 2A**  
![image](https://user-images.githubusercontent.com/73151866/118959043-26a77200-b98c-11eb-9011-7041760d6a8d.png)  

**soal 2B**  
![image](https://user-images.githubusercontent.com/73151866/118960144-2f4c7800-b98d-11eb-8e29-3c87dc54cdd7.png)  

**soal 2C**  
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

## Hasil Run
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

## Kendala yang Dihadapi
1. Contoh test case yang diberikan sangat sedikit sehingga tidak tahu mana yang benar setalah menjalankan program untuk mengkategorikan directory soal3
