#include "SearchServer.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>

SearchServer::SearchServer(InvertedIndex& index, int port, int workerThreads)
    : index_(index), port_(port), pool_(workerThreads) {}

void SearchServer::initWinSock() {
    WSADATA wsaData;
    int r = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (r != 0) throw std::runtime_error("WSAStartup failed");
}

void SearchServer::cleanupWinSock() {
    WSACleanup();
}

int SearchServer::createListenSocket() {
    SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSock == INVALID_SOCKET)
        throw std::runtime_error("socket() failed");

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR,
               (const char*)&opt, sizeof(opt));

    if (bind(listenSock, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        closesocket(listenSock);
        throw std::runtime_error("bind() failed");
    }

    if (listen(listenSock, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(listenSock);
        throw std::runtime_error("listen() failed");
    }

    return (int)listenSock;
}

std::string SearchServer::recvLine(int sock) {
    std::string line;
    char ch;

    while (true) {
        int n = recv(sock, &ch, 1, 0);
        if (n <= 0) break;
        if (ch == '\n') break;
        if (ch != '\r') line.push_back(ch);
    }
    return line;
}

void SearchServer::sendAll(int sock, const std::string& data) {
    size_t total = 0;
    while (total < data.size()) {
        int n = send(sock, data.data() + total,
                     (int)(data.size() - total), 0);
        if (n <= 0) break;
        total += (size_t)n;
    }
}

void SearchServer::handleClient(int clientSock) {
    auto clientStart = std::chrono::high_resolution_clock::now();

    try {
        std::string request = recvLine(clientSock);
        if (request.empty()) {
            std::cout << "[Server] Empty request / client disconnected\n";
            closesocket(clientSock);
            return;
        }

        std::cout << "[Server] Request: " << request << "\n";

        std::istringstream iss(request);
        std::string cmd;
        iss >> cmd;

        std::transform(cmd.begin(), cmd.end(), cmd.begin(),
                       [](unsigned char c){ return (char)std::toupper(c); });

        auto cmdStart = std::chrono::high_resolution_clock::now();

        if (cmd == "HELLO") {
            std::cout << "[Server] Command HELLO\n";
            sendAll(clientSock, "HELLO_OK Server is alive!\n");

            auto cmdEnd = std::chrono::high_resolution_clock::now();
            double ms = std::chrono::duration<double, std::milli>(cmdEnd - cmdStart).count();
            std::cout << "[Server] HELLO time: " << ms << " ms\n";

            closesocket(clientSock);
            return;
        }

        if (cmd == "INFO") {
            std::cout << "[Server] Command INFO\n";
            std::ostringstream out;
            out << "INFO_OK\n";
            out << "documents: " << index_.totalDocuments() << "\n";
            out << "unique_words: " << index_.totalWords() << "\n";
            sendAll(clientSock, out.str());

            auto cmdEnd = std::chrono::high_resolution_clock::now();
            double ms = std::chrono::duration<double, std::milli>(cmdEnd - cmdStart).count();
            std::cout << "[Server] INFO time: " << ms << " ms\n";

            closesocket(clientSock);
            return;
        }

        if (cmd == "SEARCH_PHRASE") {
            std::string phrase;
            std::getline(iss, phrase);
            while (!phrase.empty() && phrase[0] == ' ') phrase.erase(0, 1);

            if (phrase.empty()) {
                sendAll(clientSock, "ERROR phrase is empty\n");

                auto cmdEnd = std::chrono::high_resolution_clock::now();
                double ms = std::chrono::duration<double, std::milli>(cmdEnd - cmdStart).count();
                std::cout << "[Server] SEARCH_PHRASE (bad) time: " << ms << " ms\n";

                closesocket(clientSock);
                return;
            }

            std::cout << "[Server] Command SEARCH_PHRASE: \"" << phrase << "\"\n";

            auto docs = index_.searchPhrase(phrase);
            std::cout << "[Server] Phrase results: " << docs.size() << " docs\n";

            std::ostringstream out;
            out << "DOCS " << docs.size() << "\n";
            for (int id : docs) out << index_.docPath(id) << "\n";

            sendAll(clientSock, out.str());

            auto cmdEnd = std::chrono::high_resolution_clock::now();
            double ms = std::chrono::duration<double, std::milli>(cmdEnd - cmdStart).count();
            std::cout << "[Server] SEARCH_PHRASE time: " << ms << " ms\n";

            closesocket(clientSock);
            return;
        }

        if (cmd == "SEARCH") {
            std::string word;
            iss >> word;

            if (word.empty()) {
                sendAll(clientSock, "ERROR Bad request. Use: SEARCH <word>\n");

                auto cmdEnd = std::chrono::high_resolution_clock::now();
                double ms = std::chrono::duration<double, std::milli>(cmdEnd - cmdStart).count();
                std::cout << "[Server] SEARCH (bad) time: " << ms << " ms\n";

                closesocket(clientSock);
                return;
            }

            std::cout << "[Server] Command SEARCH: " << word << "\n";

            auto docs = index_.search(word);
            std::cout << "[Server] Word results: " << docs.size() << " docs\n";

            std::ostringstream out;
            out << "DOCS " << docs.size() << "\n";
            for (int id : docs) out << index_.docPath(id) << "\n";

            sendAll(clientSock, out.str());

            auto cmdEnd = std::chrono::high_resolution_clock::now();
            double ms = std::chrono::duration<double, std::milli>(cmdEnd - cmdStart).count();
            std::cout << "[Server] SEARCH time: " << ms << " ms\n";

            closesocket(clientSock);
            return;
        }

        std::cout << "[Server] Unknown command: " << cmd << "\n";
        sendAll(clientSock, "ERROR Unknown command\n");

        auto cmdEnd = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(cmdEnd - cmdStart).count();
        std::cout << "[Server] UNKNOWN time: " << ms << " ms\n";
    }
    catch (const std::exception& e) {
        auto errEnd = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double, std::milli>(errEnd - clientStart).count();

        std::cout << "[Server] Exception: " << e.what() << "\n";
        std::cout << "[Server] EXCEPTION time: " << ms << " ms\n";

        sendAll(clientSock, std::string("ERROR ") + e.what() + "\n");
    }

    auto clientEnd = std::chrono::high_resolution_clock::now();
    double totalMs = std::chrono::duration<double, std::milli>(clientEnd - clientStart).count();
    std::cout << "[Server] Client total time: " << totalMs << " ms\n";

    closesocket(clientSock);
}

void SearchServer::run() {
    initWinSock();

    SOCKET listenSock = (SOCKET)createListenSocket();
    running_ = true;

    std::cout << "[Server] Listening on port " << port_ << "...\n";

    while (running_) {
        sockaddr_in clientAddr{};
        int addrLen = sizeof(clientAddr);

        SOCKET clientSock = accept(listenSock, (sockaddr*)&clientAddr, &addrLen);
        if (clientSock == INVALID_SOCKET) {
            if (running_) std::cerr << "[Server] accept() failed\n";
            continue;
        }

        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, ipStr, INET_ADDRSTRLEN);
        int clientPort = ntohs(clientAddr.sin_port);

        std::cout << "[Server] Client connected from "
                  << ipStr << ":" << clientPort << "\n";

        pool_.enqueue([this, clientSock]() {
            handleClient((int)clientSock);
            return 0;
        });
    }

    closesocket(listenSock);
    cleanupWinSock();
}

void SearchServer::stop() {
    running_ = false;
}
