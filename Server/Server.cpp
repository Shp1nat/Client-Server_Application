#define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#pragma warning(disable: 4996)
using namespace std;

int main() {
    // Инициализация Winsock
    WSADATA ws;
    WSAStartup(MAKEWORD(2, 2), &ws);

    // Создание сокета
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        perror("socket call failed");
        exit(1);
    }

    // Привязка сокета к порту
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1111);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    int bindResult = bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (bindResult == SOCKET_ERROR) {
        perror("bind call failed");
        exit(1);
    }

    // Прослушивание порта
    listen(serverSocket, 5);

    // Прием подключения от клиента
    SOCKET clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        perror("accept call failed");
        exit(1);
    }
    else {
        cout << "connection is successful";
    }

    // Получение выбранного режима от клиента
    char choice;
    recv(clientSocket, &choice, sizeof(choice), 0);

    // Режим чтения
    if (choice == '0') {
        // Получение имени файла от клиента
        char fileName[256];
        recv(clientSocket, fileName, sizeof(fileName), 0);

        // Открытие файла
        ifstream file(fileName, ios::binary);
        if (!file) {
            cout << "File not found." << endl;
            return -1;
        }

        // Отправка содержимого файла клиенту
        string buffer, content;
        while (getline(file, buffer)) {
            content += buffer + "\n";
        }
        send(clientSocket, content.c_str(), content.size() + 1, 0);

        file.close();

        // Режим записи
    }
    else if (choice == '1') {
        // Получение имени файла от клиента
        char fileName[256];
        recv(clientSocket, fileName, sizeof(fileName), 0);

        // Создание файла
        ofstream file(fileName);

        if (!file) {
            cout << "Failed to create file." << endl;
            return -1;
        }

        // Получение содержимого файла от клиента
        char buffer[1024];
        int bytesReceived;
        while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
            file.write(buffer, bytesReceived);
        }

        file.close();
        cout << "File received successfully." << endl;
    }

    // Закрытие сокетов
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}