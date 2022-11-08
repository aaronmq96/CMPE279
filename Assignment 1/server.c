// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#define PORT 80

int main(int argc, char const *argv[])
{
  int server_fd, new_socket, valread;
  pid_t f_pid;
  struct passwd *pwd;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};
  char *hello = "Hello from server";

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

  // Forcefully attaching socket to the port 80
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

  // code
  pid_t forked = fork();

  if (forked < 0)
  {
    perror("Failed in creating child process");
    exit(EXIT_FAILURE);
  }
  else if (forked > 0)
  {
    int stat = 0;
    while ((wait(&stat)) > 0)
      ;
  }
  else
  {
    printf("Child process : %d\n", forked);

    pwd = getpwnam("nobody");

    printf("UID of nobody: %ld\n", (long)pwd->pw_uid);

    if (setuid(pwd->pw_uid) != 0)
    {
      perror("Could not drop privileges for child process failed");
      exit(EXIT_FAILURE);
    }

    printf("Dropped privileges for child process\n");

    valread = read(new_socket, buffer, 1024);
    printf("%d %s\n", valread, buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");
  }
  return 0;
}
