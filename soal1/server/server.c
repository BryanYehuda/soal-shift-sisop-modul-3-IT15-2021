#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>

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

struct orang
{
    char name[1000];
    char pwd[1000];
    char file[1000];
    char mode[1000];
    int is_auth;
    int socket;
} orang_data;

void message(char input[])
{
    char buffer[1024];
    sprintf(buffer, "\n%s\n", input);
    send(orang_data.socket, buffer, 1024, 0);
}

int login(char id[], char password[])
{
    FILE *fp = fopen("akun.txt", "r");
    int is_auth = 0;
    char buffer[1024];
    while (fgets(buffer, 1024, fp) != NULL && is_auth == 0)
    {
        char file_id[1024], file_password[1024];
        //Memisahkan id dan password pada file
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

void regist(char id[], char password[])
{
    FILE *fp = fopen("akun.txt", "a");
    fprintf(fp, "%s:%s\n", id, password);
    fclose(fp);
}

int addtsv(int year)
{
    FILE *fp;
    fp = fopen("files.tsv", "a+");
    if (fp == NULL)
    {
        printf("File cannot be opened");
        return 1;
    }
    fprintf(fp, "\nFILES/%s\t%s\t%d", orang_data.file, orang_data.name, year);
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

int receive_file(int socket, char *fname)
{
    char buffer[MAX_LENGTH] = {0};
    char fpath[MAX_LENGTH];
    strcpy(fpath, SVR);
    strcat(fpath, fname);
    FILE *file_masuk = fopen(fpath, "wb");
    if (file_masuk == NULL)
    {
        printf("File %s, Cannot be made in the server.\n", fname);
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
                error("Writing on the server failed.");
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
                fprintf(stderr, "failed = %d\n", errno);
                exit(1);
            }
        }
        printf("Accepting file from client!\n");
    }
    fclose(file_masuk);
    printf("File %s from client accepted!\n", orang_data.file);
    char msg[1000], publisher[1000], path[1000], log[1000];
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    int year = local->tm_year + 1900;

    strcpy(publisher, orang_data.name);
    strcpy(path, orang_data.file);
    sprintf(msg, "Publisher: %s\nTahun Publikasi: %d\nFilepath: Client/%s", publisher, year, path);
    message(msg);
    addtsv(year);
    sprintf(log, "Added : %s (%s:%s)\n", orang_data.file, orang_data.name, orang_data.pwd);
    catatlog(log);                         
    strcpy(orang_data.mode, "recvstrings");
}

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
        printf("File %s missing.\n", fname);
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

void download(int accept_sockfd, char *fname)
{
    char buffer[MAX_LENGTH];
    if (fileExist(fname))
    {
        printf("File %s Terdapat diserver\n", fname);
        char msg[1024];
        if (send_file(accept_sockfd, fname) == 0)
        {
            printf("File has been sent!\n");
            strcpy(orang_data.mode, "recvstrings");
        }
        else
        {
            printf("File failed to be send\n");
            strcpy(orang_data.mode, "recvstrings");
        }
    }
    else
    {
        char msg[1024];
        sprintf(msg, "File %s not on the server", fname);
        message(msg);
        strcpy(orang_data.mode, "recvstrings");
    }
}

void addfile()
{
    if (fileExist(orang_data.file))
    { 
        message("File already on server\n");
        strcpy(orang_data.mode, "recvstrings");
    }
    else
    { 
        message("File not on server, Preparing to receive file!");
        strcpy(orang_data.mode, "recvimage");
        receive_file(orang_data.socket, orang_data.file);
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
                //printf("%s",line);
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
    sprintf(newName, "old-%s", orang_data.file);
    if (rename(orang_data.file, newName) == 0)
    {
        message("File deleted");
        printf("File %s has been deleted.", orang_data.file);
        chdir("../"); 
        sprintf(name, "Delete : %s (%s:%s)", orang_data.file, orang_data.name, orang_data.pwd);
        catatlog(name);
        sprintf(db, "FILES/%s", orang_data.file);
        deletedb(db);
    }
    else
    {
        printf("File %s Failed to be deleted.", orang_data.file);
        message("Files failed to be deleted");
    }
}

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
    memset(orang_data.file, 0, 1000);
    strcpy(orang_data.file, fname);
}

void loginsukses()
{
    char msg[1024], buffer[1024];
    message("\e[1;1H\e[2J");
    printf("\norang %s Has logged in.\n", orang_data.name);
    message("Login Success!");
    while (strcmp(buffer, "exit") != 0 || strcmp(orang_data.mode, "recvstrings") == 0)
    {
        message("Input Command: ");
        read(orang_data.socket, buffer, 1024);
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
            download(orang_data.socket, orang_data.file);
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
            find(orang_data.file);
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

void *input_main()
{
    char buffer[1024];
    while (1)
    {
        if (orang_data.is_auth == 0)
        { 
            message("1. Login\n"
                    "2. Register\n"
                    "Choices: ");

            read(orang_data.socket, buffer, 1024); 
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
                read(orang_data.socket, id, 1024);

                message("Password: ");
                read(orang_data.socket, password, 1024);

                orang_data.is_auth = login(id, password); 
                if (orang_data.is_auth == 0)
                {
                    message("\e[1;1H\e[2J");
                    message("Login Failed id/password is wrong!");
                    printf("Login Failed id/password is wrong!\n");
                    break;
                }
                else if (orang_data.is_auth == 1)
                {
                    strcpy(orang_data.name, id);
                    strcpy(orang_data.pwd, password);
                    strcpy(orang_data.mode, "recvstrings");
                    loginsukses();
                }
            }
            if (strcmp(buffer, "register") == 0 || strcmp(buffer, "2") == 0)
            {
                char id[1024];
                char password[1024];
                message("\e[1;1H\e[2J");
                message("Id: ");
                read(orang_data.socket, id, 1024);
				
                message("Password: ");
                read(orang_data.socket, password, 1024);

                regist(id, password);

                char msg[1024];
                message("\e[1;1H\e[2J");
                printf("orang %s has registered.", id);
                sprintf(msg, "Register orang %s Success... (Press Anything to Continue)", id);
                message(msg);
                break;
            }
        }
        else if (orang_data.is_auth == 1)
        {
            loginsukses();
        }
    }
}

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
        printf("Connected to client with socket: %d\n", clientsocket);
        orang_data.socket = clientsocket;
    }
    orang_data.is_auth = 0

    pthread_create(&input, NULL, &input_main, 0);
    while (1)
    {
        if (pthread_join(input, NULL) == 0)
        {
            pthread_create(&input, NULL, &input_main, 0);
        }
    }
}