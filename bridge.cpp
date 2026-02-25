#include <iostream>
#include <stdio.h>
#include <WinSock2.h>
#include <string.h>


#pragma comment(lib,"WS2_32")

int main()
{
    int iResult = 1;
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (res != NO_ERROR) {
        printf("WSAStartup failed with error %d\n", iResult);
        return 1;
    }

    SOCKET Socket = INVALID_SOCKET;
    Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (Socket == INVALID_SOCKET) {
        printf("socket failed %d\n", WSAGetLastError());
    }

    struct sockaddr_in Addr;
    short port = 27015;

    Addr.sin_family = AF_INET;
    Addr.sin_port = htons(port);

    Addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(Socket, (SOCKADDR*)&Addr, sizeof(Addr))) {
        printf("Bind failed with error %d\n", WSAGetLastError());
        return 1;
    }

    char buf[1024];
    struct sockaddr_in senderAddr;
    int senderSize = sizeof(senderAddr);

    printf("Waiting for packets...\n");
    while (true) {
        int bytesReceived = recvfrom(Socket, buf, sizeof(buf), 0, (SOCKADDR*)&senderAddr, &senderSize);
        if (bytesReceived == SOCKET_ERROR) {
            printf("Recvfrom failed %d\n", WSAGetLastError());
            break;
        }
        buf[bytesReceived] = '\0';
        printf("Received: %s\n", buf);
        char* found = strstr(buf, "\"x\":");
        if (found) {
            float x;
            sscanf_s(found, "\"x\":%f", &x);
            printf("x value: %f\n", x);
        }
    }
}


