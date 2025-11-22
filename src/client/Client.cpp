#include <iostream>
#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

static std::string recvAll(SOCKET sock) {
    std::string res;
    char buf[1024];

    while (true) {
        int n = recv(sock, buf, 1024, 0);
        if (n <= 0) break;
        res.append(buf, n);
        if (n < 1024) break;
    }
    return res;
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    std::string host = "127.0.0.1";
    int port = 8080;

    while (true) {
        std::cout << "Enter command (HELLO / INFO / SEARCH <word> / SEARCH_PHRASE <text> / exit): ";
        std::string cmd;
        std::getline(std::cin, cmd);

        if (cmd == "exit") break;

        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET) {
            std::cerr << "Socket create error\n";
            continue;
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

        if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            std::cerr << "Cannot connect to server\n";
            closesocket(sock);
            continue;
        }

        std::string msg = cmd + "\n";
        send(sock, msg.c_str(), (int)msg.size(), 0);

        std::string response = recvAll(sock);
        std::cout << "Server response:\n" << response << "\n";

        closesocket(sock);
    }

    WSACleanup();
    return 0;
}
