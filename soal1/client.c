#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define PORT 8080

int foldermaker()
{
    DIR *dp;
    struct dirent *ep;
    char path[100];

    printf("Enter path to list files: ");
    getcwd(path, 100);
    printf("%s\n", path);
    dp = opendir(path);

    if (dp != NULL) 
    {
        char *FILES= "FILES";
        int foldernya = 0;
        while ((ep = readdir (dp))) 
        {
            if(strncmp(ep->d_name, FILES, strlen(FILES)) == 0) printf("ada!\n");
            else 
            {
                foldernya++;
            }
        }

        if(foldernya != 0) 
        {
            mkdir("FILES", 0777);
        }
        (void) closedir (dp);

    } else perror ("tidak bisa dibuka");

    return 0;
}
  
int main(int argc, char const *argv[]) 
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "r";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    int status;
    if(fork()==0)
    {
        execlp("clear","clear",(char *)NULL);
    }
    while((wait(&status)>0));

    char command[64];
    char username[64];
    char password[64];
    char message[64] = {0};
    srandom(time(NULL));

    while(1)
    {
        puts("1. Login\n2. Register");
        printf("%s","Choices : ");
        
        //mendapatkan input command
        gets(command);
        send(sock,command,strlen(command),0);
        if(command[0]=='q')break;
        
        //mendapatkan username
        gets(username);
        send(sock,username,strlen(username),0);
        
        //mendapatkan password
        gets(password);
        send(sock,password,strlen(password),0);

        //mendapatkan feedback dari server
        read(sock,message,64);

        int status;
        if(fork()==0)
        {
            execlp("clear","clear",(char *)NULL);
        }
        while((wait(&status)>0));
        puts(message);

        if(strcmp(message,"login success")==0)
        {
            foldermaker();
        }

    }
    return 0;
}