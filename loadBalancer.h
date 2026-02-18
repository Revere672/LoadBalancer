#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include <queue>
#include <vector>
#include <fstream>
#include "request.h"
#include "webServer.h"

class LoadBalancer {
    public:
        LoadBalancer(std::queue<Request> requestQueue,
                    std::vector<WebServer> webServers,
                    int minThreshold,
                    int maxThreshold,
                    int cooldownTime,
                    int maxProcessTime);

        void run(int clockCycles);

    private:
        std::queue<Request> requestQueue;
        std::vector<WebServer> webServers;

        int clockTime;
        int minThreshold;
        int maxThreshold;
        int cooldownTime;
        int maxProcessTime;

        std::ofstream logFile;

        Request generateRequest(int processTime, char jobType);
        bool sendRequest(const Request& request, int serverId);
        WebServer* allocateServer();
        void deallocateServer();
};

#endif