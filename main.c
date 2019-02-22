#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/*******************************************/
/* Define global variables                 */
/*******************************************/
#define SERVER_SOCK_PATH "/var/run/power_data.sock"
#define CLIENT_SOCK_PATH "/var/run/gui_control.sock"
#define BUFF_SIZE 4
#define TEST_RUN_LENGTH 10

/*******************************************/
/* Create data structure for system return */
/* values                                  */
/*******************************************/
struct rf_data{
    uint16_t data;
    uint16_t angle;
}

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
    int rc;
    struct sockaddr_un server_sockaddr;
    struct sockaddr_un client_sockaddr;
    char buff[BUFF_SIZE];
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
        printf("SOCKET ERROR: %d\n", sock_errno());
        exit(1);
    }

    /**************************************/
    /* Create the client UNIX domain      */
    /* stream socket                      */
    /**************************************/
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0){
        printf("SOCKET ERROR: %d\n", sock_errno());
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

    unlink(CLIENT_SOCK_PATH);
    rc = bind(client_sock, (struct sockaddr *)&client_sockaddr, len_client);
    if (rc < 0){
        printf("BIND ERROR: %d\n", sock_errno());
        close(client_sock);
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
    server_sockaddr.sun_family = AF_UNIX;   
    strcpy(server_sockaddr.sun_path, SERVER_SOCK_PATH); 
    len_server = sizeof(server_sockaddr);
    
    unlink(SOCK_PATH);
    rc = bind(server_sock, (struct sockaddr *) &server_sockaddr, len_server);
    if (rc == -1){
        printf("BIND ERROR: %d\n", sock_errno());
        close(server_sock);
        exit(1);
    }

    /***************************************/
    /* Set up the UNIX sockaddr structure  */
    /* for the client socket and connect   */
    /* to it.                              */
    /***************************************/
    rc = connect(client_sock, (struct sockaddr *) &server_sockaddr, len_client);
    if (rc == -1){
        printf("CONNECT ERROR = %d\n", sock_errno());
        close(client_sock);
        exit(1);
    }

    /*********************************/
    /* Listen for any client sockets */
    /*********************************/
    rc = listen(server_sock, backlog);
    if (rc == -1){ 
        printf("LISTEN ERROR: %d\n", sock_errno());
        close(server_sock);
        exit(1);
    }
    printf("Socket listening...\n");

    /*********************************/
    /* Accept an incoming connection */
    /*********************************/
    client_sock = accept(server_sock, (struct sockaddr *) &client_sockaddr, &len);
    if (client_sock == -1){
        printf("ACCEPT ERROR: %d\n", sock_errno());
        close(server_sock);
        close(client_sock);
        exit(1);
    }

    /****************************************/
    /* Get the name of the connected socket */
    /****************************************/
    len = sizeof(client_sockaddr);
    rc = getpeername(client_sock, (struct sockaddr *) &client_sockaddr, &len);
    if (rc == -1){
        printf("GETPEERNAME ERROR: %d\n", sock_errno());
        close(server_sock);
        close(client_sock);
        exit(1);
    }
    else {
        printf("Client socket filepath: %s\n", client_sockaddr.sun_path);
    }

    while (run){
        /************************************/
        /* Read and print the data          */
        /* incoming on the connected socket */
        /************************************/
        bytes_rec = recv(client_sock, buf, sizeof(buf), 0);
        if (bytes_rec == -1){
            printf("RECV ERROR: %d\n", sock_errno());
            close(server_sock);
            close(client_sock);
            exit(1);
        }
        else {
            run = buff[0];
        }

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
            test_data.data = counter % 2;
            test_data.angle = num_angles * counter;
        }
        else{
            test_data.data = -1;
            test_data.angle = num_angles * counter;
        }

        /******************************************/
        /* Send data back to the connected socket */
        /******************************************/
        memset(buff, 0, BUFF_SIZE);
        strcpy(buff, rf_data);      
        printf("Sending data...\n");
        rc = send(client_sock, buff, BUFF_SIZE, 0);
        if (rc == -1) {
            printf("SEND ERROR: %d", sock_errno());
            close(server_sock);
            close(client_sock);
            exit(1);
        }   
        else {
            printf("Data sent!\n");
        }
        counter++;
    }

    /******************************/
    /* Close the sockets and exit */
    /******************************/
    close(server_sock);
    close(client_sock);
    return 0;
}
