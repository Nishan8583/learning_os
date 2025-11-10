#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int main() {
  struct sockaddr_in server_info = {0};  // zero initializing in ours case
  server_info.sin_family = AF_INET;  // to ipv4
  server_info.sin_addr.s_addr = 0; // bind to all address
  server_info.sin_port = htons(8080);

  // creating a socket
  int server_fd = socket(AF_INET,SOCK_STREAM,0);
  if (server_fd == -1) {
    perror("server socket creation\n");
    goto early_cleanup;
  }

  // socket options to make that same port and address reusable
  int op = 1;
  if (setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&op,sizeof(op))== -1) {
    perror("main(): while setting up server options\n");
    goto err;
  }
 

  // lets bind the socket to the address
  if (bind(server_fd,(struct sockaddr*)(&server_info),sizeof(server_info))==-1) {
    perror("main(): binding failed\n");
    goto err;
  }


  // tell os to listen to that address
  if (listen(server_fd,0) == -1) {
    perror("main(): while trying to listen.\n");
    goto err;
  }

  
  while (1) {
    struct sockaddr_in client_info = {0};
    socklen_t addr_len = sizeof(client_info);

    int client = accept(server_fd, (struct sockaddr*)&client_info, &addr_len);
    if (client == -1) {
      perror("main(): while trying to accept connection\n");
      continue;
    }


    time_t now = time(NULL); // gets time in seconds since epoch
    struct tm *t = localtime(&now);  // we get more info on time

    char time_str[64] = {0};
    strftime(time_str,sizeof(time_str), "%Y-%m-%d %H:%M:%S\n",t);  // get user readable string

    int total_sent = send(client,time_str,strlen(time_str),0);
    if (total_sent == -1 ) {
      printf("main(): could not send message, total_sent response %d\n",total_sent);
    } 

    close(client);
  }




early_cleanup:
  printf("main(): early exit\n");
  exit(EXIT_FAILURE);
err:
  close(server_fd);
  exit(EXIT_FAILURE);
}
