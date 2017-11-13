#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <arpa/inet.h>

struct pseudo_header
{
    u_int16_t source_port;
    u_int16_t dest_port;
    u_int32_t seq_number;
    u_int32_t ack_number;
    u_int8_t  data_off_res_data;
    u_int8_t control_bits;
    u_int16_t window;
    u_int16_t checksum;
    u_int16_t u_pointer;
};

char ip_address[16] = "127.0.0.1\0";

int port_number = 20000;

struct pseudo_header tcp_header;

void set_tcp_header()
{
  tcp_header.source_port = port_number;
  tcp_header.dest_port = 0;
  tcp_header.seq_number = 0;
  tcp_header.ack_number = 0;
  tcp_header.data_off_res_data = 0;
  tcp_header.control_bits = 0;
  tcp_header.window = 0;
  tcp_header.checksum = 0xffff;
  tcp_header.u_pointer = 0;
}
/*---- Configure settings of the server address struct ----*/
void set_server_address(struct sockaddr_in * server_address)
{
  /* Address family = Internet */
  server_address->sin_family = AF_INET;
  /* Set IP address to localhost */
  server_address->sin_addr.s_addr = inet_addr(ip_address);
  /* Set port number, using htons function to use proper byte order */
  server_address->sin_port = htons(port_number);
}

/*---- Bind the address struct to the socket ----*/
void bind_socket_to_address(int server_socket, struct sockaddr_in * server_address)
{
  int bind_status = bind(server_socket, (struct sockaddr *) server_address, sizeof(*server_address));
  if (bind_status > -1) {
    printf("Bind Successful\n");
  } else {
    printf("Error: bind failed\n");
    exit(0);
  }
}

/*---- Listen on the socket, with 10 max connection requests queued ----*/
void listen_on_socket(int server_socket) {
  int listen_status = listen(server_socket, 10);
  if (!listen_status) {
    printf("Listening Successful\n");
  } else {
    printf("Error: listening failed\n");
    exit(0);
  }
}

/*---- Send message to the socket of the incoming connection ----*/
void send_return_message_to_client(int client_socket)
{
  char buffer[1024];
  time_t local_time = time(NULL);
  snprintf(buffer, sizeof(buffer), "%.24s\r\n", ctime(&local_time));
  write(client_socket, buffer, strlen(buffer));
}

int main()
{
  int server_socket;
  int client_socket;
  struct sockaddr_in server_address;
  set_tcp_header();

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  server_socket = socket(PF_INET, SOCK_STREAM, 0);

  set_server_address(&server_address);

  bind_socket_to_address(server_socket, &server_address);

  listen_on_socket(server_socket);

  while (1) {
    /*---- Accept call creates a new socket for the incoming connection ----*/
    client_socket = accept(server_socket, (struct sockaddr *) NULL, NULL);

    send_return_message_to_client(client_socket);

    close(client_socket);
  }

  close(server_socket);

  return 0;
}
