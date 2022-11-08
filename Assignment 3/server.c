// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    pid_t f_pid;
    struct passwd* pwd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    if(argc == 1){

      // Creating socket file descriptor
      if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
      {
          perror("socket failed");
          exit(EXIT_FAILURE);
      }

      if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                     &opt, sizeof(opt)))
      {
          perror("setsockopt");
          exit(EXIT_FAILURE);
      }
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = INADDR_ANY;
      address.sin_port = htons(PORT);

      if (bind(server_fd, (struct sockaddr *)&address,
               sizeof(address)) < 0)
      {
          perror("bind failed");
          exit(EXIT_FAILURE);
      }

      if (listen(server_fd, 3) < 0)
      {
          perror("listen");
          exit(EXIT_FAILURE);
      }

      if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                               (socklen_t *)&addrlen)) < 0)
      {
          perror("accept");
          exit(EXIT_FAILURE);
      }

      pid_t child_id = fork();

      if (child_id < 0)
      {
        perror("Fork failed");
        exit(EXIT_FAILURE);
      }

      else if (child_id == 0)
      {
        char socket_string[12];
        sprintf(socket_string, "%d", new_socket);

        char * args[] = {"./server", socket_string, NULL};
        if(execvp(args[0], args) < 0){
          perror("exec");
          exit(EXIT_FAILURE);
        }
      }

      else
      {
        int status = 0;
        while ((wait(&status)) > 0);
      }

    }

    else
    {
      pwd = getpwnam("nobody");

      if (setuid(pwd->pw_uid) != 0)
      {
        perror("Dropping privileges for child process failed");
        exit(EXIT_FAILURE);
      }

      new_socket = atoi(argv[1]);
      valread = read(new_socket, buffer, 1024);

      if (valread < 0)
      {
        perror("read failed");
        exit(EXIT_FAILURE);
      }

      printf("Read %d bytes: %s\n", valread, buffer);
      send(new_socket, hello, strlen(hello), 0);
      printf("Hello message sent\n");
    }

    return 0;
  }
