#include "loadBalancer.h"
#include <iostream>
#include <cstdlib>

LoadBalancer::LoadBalancer(std::queue<Request> requestQueue, 
                           std::vector<WebServer> webServers,
                           char name,
                           int minThreshold, 
                           int maxThreshold, 
                           int cooldownTime) {
    this->requestQueue = requestQueue;
    this->webServers = webServers;
    this->name = name;
    this->clockTime = 0;
    this->minThreshold = minThreshold;
    this->maxThreshold = maxThreshold;
    this->cooldownTime = cooldownTime;
}

void LoadBalancer::runCycle(std::vector<Request> *newRequests, std::ofstream& logFile) {
    std::cout << "Load Balancer " << name << " - Running cycle at clock time: " << clockTime << std::endl;
    logFile << "Load Balancer " << name << " - Running cycle at clock time: " << clockTime << std::endl;

    std::cout << "Generated " << newRequests->size() << " new requests." << std::endl;
    logFile << "Generated " << newRequests->size() << " new requests." << std::endl;
    while (!newRequests->empty()) {
        requestQueue.push(newRequests->front());
        newRequests->pop_back();
    }

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

    std::cout << "Queue Size: " << requestQueue.size() << ", Active Servers: " << webServers.size() << std::endl;
    logFile << "Queue Size: " << requestQueue.size() << ", Active Servers: " << webServers.size() << std::endl;

    for (WebServer &server: this->webServers) {
        server.update();
    }

    if (clockTime % cooldownTime == 0) {
        if (requestQueue.size() < minThreshold*webServers.size())
            deallocateServer(logFile);
        else if (requestQueue.size() > maxThreshold*webServers.size())
            allocateServer(logFile);
    }

    clockTime++;

    std::cout << "End of cycle for Load Balancer " << name << std::endl << std::endl;
    logFile << "End of cycle for Load Balancer " << name << std::endl << std::endl;
    logFile.flush();
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

WebServer* LoadBalancer::allocateServer(std::ofstream& logFile) {
    webServers.push_back(WebServer(clockTime));
    std::cout << "Allocated new server with ID: " << webServers.back().getId() << std::endl;
    logFile << "Allocated new server with ID: " << webServers.back().getId() << std::endl;
    return &webServers.back();
}

void LoadBalancer::deallocateServer(std::ofstream& logFile) {
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