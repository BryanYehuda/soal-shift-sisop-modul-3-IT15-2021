# soal-shift-sisop-modul-3-IT15-2021
Ini adalah repository yang dibuat untuk menampung Jawaban Soal Shift Sisop Modul 3
# Soal 1
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
