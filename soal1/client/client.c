#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
#include <dirent.h>
#include <ctype.h>

#define MAX_LENGTH 1024
#define PORT 8080
#define CLF "./"

pthread_t input, cetak;

struct user
{
  int is_auth;
  int socket;
  char file[1000];
  char input[1000];
  char mode[1000];
} user_data;

int error(char *err)
{
  perror(err);
  exit(EXIT_FAILURE);
}

int fileExist(char *fname)
{
  int found = 0;
  DIR *di;
  struct dirent *dir;
  di = opendir(CLF);
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

void download(int client_sock, char *fname)
{
  char buffer[MAX_LENGTH];
  bzero(buffer, MAX_LENGTH);
  if (fileExist(fname))
  {
    printf("There are files with the same name");
    strcpy(user_data.mode, "recvstrings");
  }
  else
  {
    receive_file(client_sock, fname);
  }
}

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
