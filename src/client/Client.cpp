#include <iostream>
#include <string>
#include <vector>

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

struct LineReader {
    std::string buf;

    std::string recvLine(SOCKET sock) {
        char tmp[512];

        while (true) {
            auto pos = buf.find('\n');
            if (pos != std::string::npos) {
                std::string line = buf.substr(0, pos);
                buf.erase(0, pos + 1);

                if (!line.empty() && line.back() == '\r')
                    line.pop_back();
                return line;
            }

            int n = recv(sock, tmp, (int)sizeof(tmp), 0);
            if (n <= 0) {
                std::string line = buf;
                buf.clear();
                if (!line.empty() && line.back() == '\r')
                    line.pop_back();
                return line;
            }

            buf.append(tmp, tmp + n);
        }
    }
};

static bool sendAll(SOCKET sock, const std::string& data) {
    size_t total = 0;
    while (total < data.size()) {
        int n = send(sock, data.data() + total,
                     (int)(data.size() - total), 0);
        if (n <= 0) return false;
        total += (size_t)n;
    }
    return true;
}

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    std::string host = "127.0.0.1";
    int port = 8080;

    while (true) {
        std::cout << "Enter command (HELLO / INFO / SEARCH <word> / SEARCH_PHRASE <text> / exit): ";
        std::string cmd;
        std::getline(std::cin, cmd);

        if (cmd == "exit") break;
        if (cmd.empty()) continue;

        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET) {
            std::cerr << "Socket create error\n";
            continue;
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons((u_short)port);
        inet_pton(AF_INET, host.c_str(), &addr.sin_addr);

        if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            std::cerr << "Cannot connect to server\n";
            closesocket(sock);
            continue;
        }

        std::string msg = cmd + "\n";
        if (!sendAll(sock, msg)) {
            std::cerr << "Send failed\n";
            closesocket(sock);
            continue;
        }

        LineReader reader;
        std::vector<std::string> lines;

        while (true) {
            std::string line = reader.recvLine(sock);
            if (line.empty()) break;
            if (line.rfind("WAIT", 0) == 0 || line.rfind("QUEUE", 0) == 0) {
                std::cout << "[Client] " << line << "\n";
                continue;
            }
            lines.push_back(line);
            break;
        }

        while (true) {
            std::string line = reader.recvLine(sock);
            if (line.empty()) break;
            if (line.rfind("WAIT", 0) == 0 || line.rfind("QUEUE", 0) == 0) {
                std::cout << "[Client] " << line << "\n";
                continue;
            }
            lines.push_back(line);
        }

        if (!lines.empty()) {
            std::cout << "Server response:\n";
            for (auto& l : lines) std::cout << l << "\n";
        } else {
            std::cout << "No response (server closed connection)\n";
        }

        closesocket(sock);
    }

    WSACleanup();
    return 0;
}
