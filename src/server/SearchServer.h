#pragma once
#include <string>
#include "../index/InvertedIndex.h"
#include "../threading/ThreadPool.h"

class SearchServer {
public:
    SearchServer(InvertedIndex& index, int port, int workerThreads);

    void run();
    void stop();

private:
    InvertedIndex& index_;
    int port_;
    ThreadPool pool_;
    bool running_ = false;

    void handleClient(int clientSock);

    int createListenSocket();
    static std::string recvLine(int sock);
    static void sendAll(int sock, const std::string& data);

    static void initWinSock();
    static void cleanupWinSock();
};
