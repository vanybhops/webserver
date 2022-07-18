#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501
#include <windows.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
#define DEFAULT_BUFLEN 1024
int main(int argc, char *argv[]) 
{
    FILE* ptr;
    char ch;
    char str[50];
    char file[32]="";
    char sajt[1024000];
    char port[6]="1337";
    if(argc<3){
        printf("usage is server.exe --html filename.html [optional --port 1234]");
        return 0;
        }
    for (int i = 0; i <argc; i++)
    {
        if(strncmp(argv[i],"--html",6)==0){
            strcpy(file,argv[i+1]);
        }
        if(strncmp(argv[i],"--port",6)==0){
            if(strlen(argv[i+1])>6||(strlen(argv[i+1])==1&&strncmp(argv[i+1],"0",1)==0)){
                return 0;}
            strcpy(port,argv[i+1]);
        }
    }
    ptr = fopen(file, "a+");
    if (NULL == ptr)
        return 0;
    strcpy(sajt,
                "HTTP/1.1 200 OK\r\n"
                "Content-Type:text/html;charset:UTF-8\r\n\n"
                "<!DOCTYPE html>\r\n"
            );
    while (fgets(str, 50, ptr) != NULL) {
        strcat(sajt,str);
    }
    fclose(ptr);
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0)
        return 1;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    iResult = getaddrinfo(NULL, port, &hints, &result);
    if ( iResult != 0 ) {
        WSACleanup();
        return 1;
    }

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    while (1)
    {
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        do {

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            iSendResult = send( ClientSocket, sajt, strlen(sajt), 0 );
            closesocket(ClientSocket);
            if (iSendResult == SOCKET_ERROR) {
                WSACleanup();
                return 1;
            }
        }
        else if (iResult == 0){
            closesocket(ClientSocket);
            }
        else  {
        }
        }while (iResult>0);

        closesocket(ClientSocket);
    }
    free(sajt);
    closesocket(ClientSocket);

    closesocket(ListenSocket);


    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}
