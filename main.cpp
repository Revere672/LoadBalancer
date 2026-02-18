#include <queue>
#include <vector>
#include <iostream>
// #include "request.h"
// #include "webServer.h"
#include "loadBalancer.h"

int main() {
    std::queue<Request> requestQueue;
    std::vector<WebServer> webServers;

    std::cout << "Enter the number of initial web servers: ";
    int initialServers;
    std::cin >> initialServers;

    std::cout << "Enter the number of clock cycles to run the simulation: ";
    int clockCycles;
    std::cin >> clockCycles;

    std::cout << "Enter the minimum threshold for deallocating servers: ";
    int minThreshold;
    std::cin >> minThreshold;

    std::cout << "Enter the maximum threshold for allocating servers: ";
    int maxThreshold;
    std::cin >> maxThreshold;

    std::cout << "Enter the cooldown time for allocating/deallocating servers: ";
    int cooldownTime;
    std::cin >> cooldownTime;

    std::cout << "Enter the maximum processing time for requests: ";
    int maxProcessingTime;
    std::cin >> maxProcessingTime;

    for (int i = 0; i < initialServers; i++) {
        webServers.push_back(WebServer(i));
    }

    for (int i = 0; i < initialServers*100; i++) {
        int processTime = rand() % maxProcessingTime + 1;
        char jobType = rand() % 2 == 0 ? 'P' : 'S';
        Request newRequest = Request(processTime, jobType);
        requestQueue.push(newRequest);
    }

    LoadBalancer loadBalancer(requestQueue, webServers, minThreshold, maxThreshold, cooldownTime, maxProcessingTime);
    loadBalancer.run(clockCycles);

    return 0;
}