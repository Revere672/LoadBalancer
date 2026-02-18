#include "loadBalancer.h"
#include <iostream>
#include <cstdlib>

LoadBalancer::LoadBalancer(std::queue<Request> requestQueue, 
                           std::vector<WebServer> webServers, 
                           int minThreshold, 
                           int maxThreshold, 
                           int cooldownTime, 
                           int maxProcessTime) {
    this->requestQueue = requestQueue;
    this->webServers = webServers;
    this->clockTime = webServers.size();
    this->minThreshold = minThreshold;
    this->maxThreshold = maxThreshold;
    this->cooldownTime = cooldownTime;
    this->maxProcessTime = maxProcessTime;
    this->logFile.open("loadBalancer.log", std::ios::out | std::ios::trunc);
}

void LoadBalancer::run(int clockCycles) {
    while (clockTime < clockCycles) {
        for (WebServer &server: this->webServers) {
            if (requestQueue.empty()) {
                break;
            }

            if (server.isReady()) {
                Request request = requestQueue.front();
                requestQueue.pop();
                sendRequest(request, server.getId());
            }
        }

        std::cout << "Clock Time: " << clockTime << ", Queue Size: " << requestQueue.size() << ", Active Servers: " << webServers.size() << std::endl;
        logFile << "Clock Time: " << clockTime << ", Queue Size: " << requestQueue.size() << ", Active Servers: " << webServers.size() << std::endl;

        for (WebServer &server: this->webServers) {
            server.update();
        }

        if (rand() % 11 == 10) {
            int newRequests = rand() % 100 + 1;
            for (int i = 0; i < newRequests; i++) {
                int processTime = rand() % maxProcessTime + 1;
                char jobType = rand() % 2 == 0 ? 'P' : 'S';
                Request newRequest = generateRequest(processTime, jobType);
                requestQueue.push(newRequest);
            }
            std::cout << "Generated " << newRequests << " new requests." << std::endl;
            logFile << "Generated " << newRequests << " new requests." << std::endl;
        }

        if (clockTime % cooldownTime == 0) {
            if (requestQueue.size() < minThreshold*webServers.size())
                deallocateServer();
            else if (requestQueue.size() > maxThreshold*webServers.size())
                allocateServer();
        }

        clockTime++;
    }
}

Request LoadBalancer::generateRequest(int processTime, char jobType) {
    return Request(processTime, jobType);
}

bool LoadBalancer::sendRequest(const Request& request, int serverId) {
    for (WebServer &server: this->webServers) {
        if (server.getId() == serverId && server.isReady()) {
            server.processRequest(request);
            return true;
        }
    }
    return false;
}

WebServer* LoadBalancer::allocateServer() {
    webServers.push_back(WebServer(clockTime));
    std::cout << "Allocated new server with ID: " << webServers.back().getId() << std::endl;
    logFile << "Allocated new server with ID: " << webServers.back().getId() << std::endl;
    return &webServers.back();
}

void LoadBalancer::deallocateServer() {
    if (!webServers.empty()) {
        for (unsigned int i = 0; i < webServers.size(); i++) {
            if (webServers[i].isReady()) {
                std::cout << "Deallocated server with ID: " << webServers[i].getId() << std::endl;
                logFile << "Deallocated server with ID: " << webServers[i].getId() << std::endl;
                webServers.erase(webServers.begin() + i);
                return;
            }
        }

        std::cout << "No servers available for deallocation." << std::endl;
        logFile << "No servers available for deallocation." << std::endl;
    }
}