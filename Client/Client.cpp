#define WIN32_LEAN_AND_MEAN

#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#pragma warning(disable: 4996)


using namespace std;

int main() {
    // Инициализация Winsock
    WSADATA ws;
    WSAStartup(MAKEWORD(2, 2), &ws);

    // Создание сокета
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        perror("socket call failed");
        exit(1);
    }

    // Подключение к серверу
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1111);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int connectResult = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (connectResult == SOCKET_ERROR) {
        perror("connect call failed");
        exit(1);
    }

    // Выбор режима работы
    char choice;
    cout << "Read or write file (0/1): ";
    cin >> choice;

    // Отправка выбранного режима на сервер
    send(clientSocket, &choice, sizeof(choice), 0);

    // Режим чтения
    if (choice == '0') {
        // Ввод имени файла
        string fileName;
        cout << "Enter file name: ";
        cin >> fileName;

        // Отправка имени файла на сервер
        send(clientSocket, fileName.c_str(), fileName.size() + 1, 0);

        // Получение содержимого файла
        char buffer[1024];
        int bytesReceived;
        while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
            cout << buffer;
        }
        cout << endl;
        // Режим записи
    }
    else if (choice == '1') {
        // Ввод имени файла
        string fileName;
        cout << "Enter file name: ";
        cin >> fileName;

        // Отправка имени файла на сервер
        send(clientSocket, fileName.c_str(), fileName.size() + 1, 0);

        // Отправка содержимого файла
        ifstream file(fileName, ios::binary);
        if (!file) {
            cout << "Failed to open file." << endl;
            return -1;
        }

        char buffer[1024];
        while (!file.eof()) {
            file.read(buffer, sizeof(buffer));
            send(clientSocket, buffer, file.gcount(), 0);
        }

        file.close();
        cout << "File sent successfully." << endl;
    }

    // Закрытие сокета
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}