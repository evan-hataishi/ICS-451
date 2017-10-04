#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

int main()
{
  int server_socket;
  int client_socket;
  char buffer[1024];
  struct sockaddr_in server_address;
  time_t local_time;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  server_socket = socket(PF_INET, SOCK_STREAM, 0);

  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  server_address.sin_family = AF_INET;
  /* Set IP address to localhost */
  server_address.sin_addr.s_addr = INADDR_ANY;
  /* Set port number, using htons function to use proper byte order */
  server_address.sin_port = htons(20000);

  /*---- Bind the address struct to the socket ----*/
  int bind_status = bind(server_socket, (struct sockaddr *) &server_address, sizeof(server_address));
  if (bind_status > -1) {
    printf("Bind Successful\n");
  } else {
    printf("Error: bind failed\n");
    return 0;
  }
  /*---- Listen on the socket, with 5 max connection requests queued ----*/
  int listen_status = listen(server_socket, 10);
  if (listen(server_socket, 10) == 0) {
    printf("Listening Successful\n");
  } else {
    printf("Error: listening failed\n");
    return 0;
  }

  /*---- Accept call creates a new socket for the incoming connection ----*/
  client_socket = accept(server_socket, (struct sockaddr *) NULL, NULL);

  /*---- Send message to the socket of the incoming connection ----*/
  local_time = time(NULL);
  snprintf(buffer, sizeof(buffer), "%.24s\r\n", ctime(&local_time));
  write(client_socket, buffer, strlen(buffer));

  close(client_socket);
  close(server_socket);

  return 0;
}
