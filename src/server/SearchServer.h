#pragma once
#include <string>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "../index/InvertedIndex.h"
#include "../threading/ThreadPool.h"

class SearchServer {
public:
    SearchServer(InvertedIndex& index, int port, int workerThreads, int maxClients);

    void run();
    void stop();

private:
    InvertedIndex& index_;
    int port_;
    ThreadPool pool_;

    int maxClients_;
    std::atomic<int> activeClients_{0};

    std::deque<SOCKET> waitQueue_;
    std::mutex queueMtx_;
    std::condition_variable queueCv_;
    std::thread queueThread_;

    bool running_ = false;

    void handleClient(SOCKET clientSock);
    void queueLoop();

    SOCKET createListenSocket();
    static std::string recvLine(SOCKET sock);
    static void sendAll(SOCKET sock, const std::string& data);

    static void initWinSock();
    static void cleanupWinSock();
};
