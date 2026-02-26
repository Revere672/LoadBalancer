#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include <queue>
#include <vector>
#include <fstream>
#include "request.h"
#include "webServer.h"
#include "utils.h"

class LoadBalancer {
    public:
        LoadBalancer(std::queue<Request> requestQueue,
                    std::vector<WebServer> webServers,
                    char name,
                    int minThreshold,
                    int maxThreshold,
                    int cooldownTime);

        void runCycle(std::vector<Request> *newRequests, std::ofstream& logFile);

    private:
        std::queue<Request> requestQueue;
        std::vector<WebServer> webServers;

        char name;
        int clockTime;
        int minThreshold;
        int maxThreshold;
        int cooldownTime;

        bool sendRequest(const Request& request, int serverId);
        WebServer* allocateServer(std::ofstream& logFile);
        void deallocateServer(std::ofstream& logFile);
};

#endif