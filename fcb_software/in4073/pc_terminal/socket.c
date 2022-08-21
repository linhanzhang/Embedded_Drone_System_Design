/* client.c */

#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(1)

// typedef struct payload_t {
//     uint32_t id;
//     uint32_t counter;
//     float temp;
// } payload;
typedef struct payload_t {
    int16_t phi;
    int16_t theta;
	int16_t psi;
    int16_t sp;
    int16_t sq;
	int16_t sr;

} payload;

#pragma pack()

void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
  
    // Storing start time
    clock_t start_time = clock();
  
    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}

void sendMsg(int sock, void* msg, uint32_t msgsize)
{
    if (write(sock, msg, msgsize) < 0)
    {
        printf("Can't send message.\n");
        close(sock);
        exit(1);
    }
    

    printf("Message sent (%d bytes).\n", msgsize);
    return;
}

int main()
{
    const int PORT = 2300;
    const char* SERVERNAME = "localhost";
    int BUFF_SIZE = sizeof(payload);
    char buff[BUFF_SIZE];
    int sock;
    int nread;
    float mintemp = -10.0;
    float maxtemp = 30.0;
    time_t t;

    srand((unsigned) time(&t));
    
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    inet_pton(AF_INET, SERVERNAME, &server_address.sin_addr);
    server_address.sin_port = htons(PORT);

    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        printf("ERROR: Socket creation failed\n");
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        printf("ERROR: Unable to connect to server\n");
        return 1;
    }

    printf("Connected to %s\n", SERVERNAME);

    payload data;
    for(int i = 0; i < 30; i++) {
        data.phi = i;
        data.sq = 2* i;
        data.psi = 4 * i;
        data.theta = i;
        data.sp = 2* i;
        data.sr = 4 * i;
        printf("\nSending id=%d, counter=%d, temp=%d\n", data.sp, data.sq, data.sr);
        delay(1000);
        sendMsg(sock, &data, sizeof(payload));
        bzero(buff, BUFF_SIZE);
    

        nread = read(sock, buff, BUFF_SIZE);
        printf("Received %d bytes\n", nread);
        payload *p = (payload*) buff;
        // printf("Received id=%d, counter=%d, temp=%f\n",
        //         p->id, p->counter, p->temp);
    }
    
    // close the socket
    close(sock);
    return 0;

}