#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

/*******************************************/
/* Define global variables                 */
/*******************************************/
#define CLIENT_SOCK_PATH "/tmp/power_data.sock"
#define SERVER_SOCK_PATH "/tmp/gui_control.sock"
#define BUFF_SIZE 4
#define TEST_RUN_LENGTH 10

/*******************************************/
/* Create data structure for system return */
/* values                                  */
/*******************************************/
struct rf_data{
    uint16_t data;
    uint16_t angle;
};

int main(void){
    /***************************************/
    /* Define integers that will be used   */
    /* for the struct length, socket file  */
    /* descriptors, and return values from */
    /* socket fuctions; a struct for the   */
    /* server socket, and a buffer.        */
    /***************************************/
    int len, len_client, len_server;
    int client_sock, server_sock;
    int rc,bytes_rec;
    int backlog = 5; //This will tell you how many times it will accept connections before bailing out.
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    bool run = true;

    /**************************************/
    /* Create test variables              */
    /**************************************/
    struct rf_data test_data;
    int counter = 0;
    int num_angles = 360 / TEST_RUN_LENGTH;

    /**************************************/
    /* Prefill all values to zero         */
    /**************************************/
    memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));

    /**************************************/
    /* Create the client UNIX domain      */
    /* stream socket                      */
    /**************************************/
    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock < 0){
        printf("SOCKET ERROR: %d\n", client_sock);
        exit(1);
    }

    /***************************************/
    /* Set up the UNIX sockaddr structure  */
    /* by using AF_UNIX for the family and */
    /* giving it a filepath to bind to.    */
    /*                                     */
    /* Unlink the file so the bind will    */
    /* succeed, then bind to that file.    */
    /***************************************/
    client_sockaddr.sun_family = AF_UNIX;
    strcpy(client_sockaddr.sun_path, CLIENT_SOCK_PATH);
    len_client = sizeof(client_sockaddr);

    /*unlink(CLIENT_SOCK_PATH);
      rc = bind(client_sock, (struct sockaddr *)&client_sockaddr, len_client);
      if (rc < 0){
      printf("BIND ERROR: %d\n", rc);
      close(client_sock);
      exit(1);
      }*/


    /***************************************/
    /* Set up the UNIX sockaddr structure  */
    /* for the client socket and connect   */
    /* to it.                              */
    /***************************************/
    rc = connect(client_sock, (struct sockaddr *) &client_sockaddr, len_client);
    if (rc == -1){
        printf("CONNECT ERROR = %d\n", rc);
        close(client_sock);
        exit(1);
    }

    while (run){
        /************************************/
        /* Read and print the data          */
        /* incoming on the connected socket */
        /************************************/
        /*  bytes_rec = recv(client_sock, buff, sizeof(buff), 0);
            if (bytes_rec == -1){
            printf("RECV ERROR: %d\n", bytes_rec);
            close(server_sock);
            close(client_sock);
            exit(1);
            }
            else {
            run = buff[0];
            }*/

        //TODO: EXECUTE POWER COMPUTE FUNCTIONS

        /************************************/
        /* Create and send fake data for    */
        /* testing purposes, when a         */
        /* negative number is sent, the     */
        /* GUI will know that the test has  */
        /* finished and send the command to */
        /* end the while loop and close the */
        /* sockets                          */
        /************************************/
        if(counter < TEST_RUN_LENGTH){
            printf("Test Run Length is: %d, Counter is at: %d\r\n", TEST_RUN_LENGTH, counter);
            test_data.data = 1 + counter;
            test_data.angle = 2 * counter;
            printf("Test Data Data is: %d, Test Data Angle is: %d\r\n", test_data.data, test_data.angle);
            /******************************************/
            /* Send data back to the connected socket */
            /******************************************/
            printf("Sending data...\n");
            rc = send(client_sock, &test_data, BUFF_SIZE, 0);
            if (rc == -1) {
                printf("SEND ERROR: %d", rc);
                close(client_sock);
                exit(1);
            }
            else {
                printf("Data sent!\n");
            }
        }
        else{
            run = false;
            //            test_data.data = -1;
            //          test_data.angle = num_angles * counter;
        }

        counter++;
    }

    /******************************/
    /* Close the sockets and exit */
    /******************************/
    close(client_sock);
    return 0;
}
