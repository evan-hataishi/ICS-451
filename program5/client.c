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
  unsigned char data[1500];
};

void set_tcp_header();
void print_tcp_header();

char ip_address[16] = "127.0.0.1\0";

int server_port = 20000;

struct pseudo_header tcp_header;

int main()
{
  int server_socket;
  struct sockaddr_in server_address;
  memset(&tcp_header, 0, 20);
  set_tcp_header();

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  server_socket = socket(PF_INET, SOCK_STREAM, 0);

  if (server_socket < 0) {
    perror("ERROR opening socket");
    exit(1);
  }

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(server_port);
  server_address.sin_addr.s_addr = inet_addr(ip_address);

  int client_socket = connect(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
  if (client_socket < 0) {
    perror("ERROR connecting");
    exit(1);
  }

  memset(&(tcp_header.control_bits), 2, 1);
  write(server_socket, &tcp_header, 20);

  while (1) {
    if (recv(server_socket, &tcp_header, 1520, 0) > -1) {
      printf("Client Received:\n");
      print_tcp_header();

      if (tcp_header.control_bits == 18) {
        printf("Client received syn-ack -> sending ack\n");
        int seq_temp = tcp_header.ack_number;
        set_tcp_header();
        tcp_header.seq_number = seq_temp;
        tcp_header.ack_number = 1;
        tcp_header.control_bits = 2;
        write(server_socket, &tcp_header, 20);
        break;
      } else {
        printf("Error invalid flag received\n");
      }
    }
    printf("\n");
  }

  // begin exchange
  FILE * file = fopen("output.png", "wb");
  while (1) {
    if (recv(server_socket, &tcp_header, 1520, 0) > -1) {
      printf("Client Received chunk:\n");
      print_tcp_header();

      if (tcp_header.control_bits == 1) {
        printf("Received fin\n");
        break;
      }

      if (fwrite(tcp_header.data, 1, 1500, file) != 1500) {
        printf("error\n");
        exit(1);
      }

      int seq_temp = tcp_header.ack_number;
      set_tcp_header();
      tcp_header.seq_number = seq_temp;
      tcp_header.ack_number = 1501;
      // set ack
      tcp_header.control_bits = 16;
      print_tcp_header();
      write(server_socket, &tcp_header, 1520);
    }
    printf("\n");
  }

  fclose(file);
  close(client_socket);

  close(server_socket);

  return 0;
}

void set_tcp_header()
{
  tcp_header.source_port = tcp_header.dest_port;
  tcp_header.dest_port = server_port;
  tcp_header.seq_number = 0;
  tcp_header.ack_number = 0;
  tcp_header.data_off_res_data = 0;
  tcp_header.control_bits = 0;
  tcp_header.window = 1500;
  tcp_header.checksum = 0xffff;
  tcp_header.u_pointer = 0;
  memset(&tcp_header.data, 0, 1500);
}

void print_tcp_header()
{
  printf("Size: %u\n", (uint)sizeof(tcp_header));
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
  printf("Urgent pointer: %u\n", (uint)tcp_header.u_pointer);
}
