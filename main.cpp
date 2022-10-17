/* **SERVER** */

#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main() {

    WSADATA wsaData;
    ADDRINFO hints;
    ADDRINFO *addrResult = NULL;
    SOCKET clientSocket = INVALID_SOCKET;
    SOCKET listenerSocket = INVALID_SOCKET;

    const char* sendBuffer = "Hello from server!";
    char recvBuffer[512];

    int result = WSAStartup(MAKEWORD(2,2),&wsaData);
    if(result!=0){
        std::cout << "WSAStart failed, result = " << result << std::endl;
        return 1;
    }

    ZeroMemory(&hints , sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(NULL, "666", &hints , &addrResult);
    if(result!=0){
        std::cout << "getaddrinfo failed, result = " << result << std::endl;
        WSACleanup();
        return 1;
    }

    listenerSocket = socket(addrResult->ai_family , addrResult->ai_socktype, addrResult->ai_protocol);
    if(listenerSocket == INVALID_SOCKET){
        std::cout << "clientSocket creation failed" <<std::endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = bind(listenerSocket , addrResult->ai_addr , (int)addrResult->ai_addrlen);
    if(result == SOCKET_ERROR){
        std::cout << "Binding socket failed" << std::endl;
        closesocket(listenerSocket);
        listenerSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = listen(listenerSocket,SOMAXCONN);
    if(result == SOCKET_ERROR){
        std::cout << "Listening socket failed" << std::endl;
        closesocket(listenerSocket);
        listenerSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    clientSocket = accept(listenerSocket,NULL,NULL);
    if(clientSocket == INVALID_SOCKET){
        std::cout << "Accepting socket failed" << std::endl;
        closesocket(listenerSocket);
        listenerSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    closesocket(listenerSocket);

    do {
        ZeroMemory(recvBuffer, 512);
        result = recv(clientSocket, recvBuffer, 512 , 0);
        if(result > 0){
            std::cout << "Received: " << result << std::endl;
            std::cout << "Received data: " << recvBuffer << std::endl;
            result = send(clientSocket ,sendBuffer ,(int)strlen(sendBuffer), 0);
            if(result == SOCKET_ERROR){
                std::cout << " Failed to send data back" << std::endl;
                closesocket(clientSocket);
                clientSocket = INVALID_SOCKET;
                freeaddrinfo(addrResult);
                WSACleanup();
            }
        }
        else if(result == 0 ){
            std::cout << "Connection closing..." << std::endl;
        }
        else{
            std::cout << "recv failed with error " <<std::endl;
            closesocket(clientSocket);
            clientSocket = INVALID_SOCKET;
            freeaddrinfo(addrResult);
            WSACleanup();
            return 1;
        }
    } while (result > 0);

    result = shutdown(clientSocket, SD_SEND);
    if(result == SOCKET_ERROR){
        std::cout << "shutdown client socket failed" <<std::endl;
    }

    closesocket(clientSocket);
    clientSocket = INVALID_SOCKET;
    freeaddrinfo(addrResult);
    WSACleanup();
    return 0;
}
