#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "list_node.h"
#include "linked_list.h"
#include <pthread.h>

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
  unsigned char data[1500];
};

void set_tcp_header();
void set_server_address(struct sockaddr_in * server_address);
void bind_socket_to_address(int server_socket, struct sockaddr_in * server_address);
void listen_on_socket(int server_socket);
void print_tcp_header();
void * run_timer(struct pseudo_header tcp_header);

char ip_address[16] = "127.0.0.1\0";

int port_number = 20000;

struct pseudo_header tcp_header;

int connecting_port;

int main()
{
  int server_socket;
  int client_socket;
  struct sockaddr_in server_address;
  memset(&tcp_header, 0, 20);
  set_tcp_header();

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  server_socket = socket(PF_INET, SOCK_STREAM, 0);

  set_server_address(&server_address);

  bind_socket_to_address(server_socket, &server_address);

  listen_on_socket(server_socket);

  // get connecting socket port #
  struct sockaddr_storage addr;
  socklen_t len = sizeof(addr);

  /*---- Accept call creates a new socket for the incoming connection ----*/
  client_socket = accept(server_socket, (struct sockaddr *) &addr, &len);

  struct sockaddr_in *s = (struct sockaddr_in *) &addr;
  connecting_port = ntohs(s->sin_port);

  int handshake_counter = 0;
  while(handshake_counter < 2) {
    recv(client_socket, &tcp_header, 1520, 0);
    printf("Server received:\n");
    print_tcp_header();

    if (tcp_header.control_bits == 2 && handshake_counter == 0) {
      u_int32_t seq = tcp_header.seq_number;
      set_tcp_header();
      printf("Server received syn -> sending syn-ack\n");
      memset(&(tcp_header.control_bits), 18, 1);
      tcp_header.seq_number = seq + 1;
      write(client_socket, &tcp_header, 1520);
    } else if (tcp_header.control_bits == 2 && handshake_counter == 1) {
      u_int32_t seq = tcp_header.seq_number;
      set_tcp_header();
      printf("Server received ack -> handshake completed\n");
      memset(&(tcp_header.control_bits), 18, 1);
      tcp_header.seq_number = seq + 1;
    } else {
      printf("Error invalid flag received\n");
      close(client_socket);
      close(server_socket);
      exit(0);
    }
    handshake_counter++;
    printf("\n");
  }

  // Save image chunks to list
  struct list_node * head = NULL;
  unsigned char buffer[1500];
  FILE * fileptr;
  long filelen;

  fileptr = fopen("img.png", "rb");

  while (fread(buffer, 1, 1500, fileptr) > 0) {
    add_node(&head, buffer);
    memset(buffer, 0, 1500);
  }

  fclose(fileptr);

  // Start sending data
  while (head != NULL) {
    printf("Server sending file chunk\n");
    int seq_temp = tcp_header.ack_number;
    set_tcp_header();
    tcp_header.seq_number = seq_temp;

    memcpy(tcp_header.data, head->data, 1500);
    write(client_socket, &tcp_header, 1520);

    pthread_t thread;
    
    // wait for ack
    recv(client_socket, &tcp_header, 1520, 0);
    printf("Server received:\n");
    print_tcp_header();

    if (tcp_header.control_bits == 16) {
      printf("Server received acknowledgement -> sending next chunk\n");
      delete_front(&head);
    }
    else {
      printf("Error invalid flag received");
      break;
    }
  }

  // write fin to client
  set_tcp_header();
  tcp_header.control_bits = 1;
  printf("Sending fin\n");
  write(client_socket, &tcp_header, 1520);

  close(client_socket);

  close(server_socket);

  return 0;
}

void set_tcp_header()
{
  tcp_header.source_port = port_number;
  tcp_header.dest_port = connecting_port;;
  //tcp_header.seq_number = tcp_header.seq_number++;
  tcp_header.ack_number = 1;
  tcp_header.data_off_res_data = 0;
  tcp_header.control_bits = 0;
  tcp_header.window = 1500;
  tcp_header.checksum = 0xffff;
  tcp_header.u_pointer = 0;
  memset(&tcp_header.data, 0, 1500);
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
void listen_on_socket(int server_socket)
{
  int listen_status = listen(server_socket, 10);
  if (!listen_status) {
    printf("Listening Successful\n");
  } else {
    printf("Error: listening failed\n");
    exit(0);
  }
}

void print_tcp_header()
{
  printf("Source Port: %u\n", (uint)tcp_header.source_port);
  printf("Destination Port: %u\n", (uint)tcp_header.dest_port);
  printf("Sequence Number: %u\n", (uint)tcp_header.seq_number);
  printf("Acknowledgement Number: %u\n", (uint)tcp_header.ack_number);
  // data off / rec data all zeros for now
  printf("Data Offset: %u\n", (uint)tcp_header.data_off_res_data);
  printf("Reserved Data: %u\n", (uint)tcp_header.data_off_res_data);
  printf("Control Bits: %u\n", (uint)tcp_header.control_bits);
  printf("Window: %u\n", (uint)tcp_header.window);
  printf("Checksum: %u\n", (uint)tcp_header.checksum);
  printf("Urgent pointer: %u\n\n", (uint)tcp_header.u_pointer);
}

void * run_timer(struct pseudo_header tcp_header)
{

}
