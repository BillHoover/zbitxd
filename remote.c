#include <arpa/inet.h>
#include <complex.h>
#include <errno.h>
#include <fcntl.h>
#include <fftw3.h>
#include <math.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "sdr.h"
#include "sdr_ui.h"

static int welcome_socket = -1, data_socket = -1;
#define MAX_DATA 1000
static char incoming_data[MAX_DATA];
static int incoming_ptr;

#define MAX_LINE 30

void remote_start() {
  char buffer[MAX_DATA];
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  welcome_socket = socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(8081);
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  /* Bind the address struct to the socket */
  bind(welcome_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

  /* Listen on the socket, with 5 max connection requests queued */
  if (listen(welcome_socket, 5) != 0)
    printf("telnet listen() Error\n");
  incoming_ptr = 0;
}

void remote_send(char *m) { send(data_socket, m, strlen(m), 0); }

void remote_init() {
  remote_send("\033[1;1H"); // goto 1,1
  remote_send("\033[r");    // clear the scrollable area
  remote_send("\033[2J");   // clear the screen
  remote_send("\033[25;1r");

  remote_send(VER_STR);
  remote_send("\r\n");
}

void remote_write(char *message) {
  if (data_socket < 0)
    return;

  int e = send(data_socket, message, strlen(message), 0);
  if (e >= 0)
    return;
  close(data_socket);
  data_socket = -1;
}

void remote_slice() {
  struct sockaddr_storage server_storage;
  socklen_t addr_size;
  int e, len;
  char buffer[1024];

  if (data_socket == -1) {
    addr_size = sizeof server_storage;
    e = accept(welcome_socket, (struct sockaddr *)&server_storage, &addr_size);
    if (e == -1)
      return;
    puts("Accepted telnet connection\n");
    incoming_ptr = 0;
    data_socket = e;
    fcntl(data_socket, F_SETFL, fcntl(data_socket, F_GETFL) | O_NONBLOCK);

    // init the console
    remote_init();
  } else {
    // this section was changed by W9JES
    len = recv(data_socket, buffer, sizeof(buffer), 0);
    if (len > 0) {
      buffer[len] =
          '\0'; // Ensure the buffer is null-terminated. Changed by W9JES
      printf("Received on remote : [%s]\n", buffer);
      // Strip off the last \r or \n
      buffer[strcspn(buffer, "\r\n")] = '\0';
      if (buffer[0] == '?') {
        char response[2000];
        get_field_value_by_label(buffer + 1, response);
        strcat(response, "\n");
        remote_write(response);
      } else if (strlen(buffer)) {
        remote_execute(buffer);
      }
    } else if (len == 0) {
      // The recv function returned 0 -> the client closed the connection. Added
      // by w9JES
      puts("Client closed the connection. Restarting to listen...");
      close(data_socket);
      data_socket = -1;
    } else {
      // An error occurred, check if it's EAGAIN or EWOULDBLOCK to keep the
      // connection open. Modifed by W9JES
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        return; // No data available right now, try again later
      // For other errors, close the socket.
      puts("Connection error. Dropping the connection...");
      close(data_socket);
      data_socket = -1;
    }
  }
}
