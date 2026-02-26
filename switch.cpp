#include "switch.h"
#include <iostream>

Switch::Switch(std::queue<Request> requestQueue_P, 
               std::queue<Request> requestQueue_S,
               std::vector<WebServer> webServers_P,
               std::vector<WebServer> webServers_S,
               int minThreshold, int maxThreshold, int cooldownTime, int maxProcessTime)
    : loadBalancer_P(requestQueue_P, webServers_P, 'P', minThreshold, maxThreshold, cooldownTime),
      loadBalancer_S(requestQueue_S, webServers_S, 'S', minThreshold, maxThreshold, cooldownTime) {
        
        clockTime = 0;
        this->maxProcessTime = maxProcessTime;
}

void Switch::run(int clockCycles, std::ofstream& logFile) {
    for (int i = 0; i < clockCycles; i++) {
        std::vector<Request> requestQueue_P;
        std::vector<Request> requestQueue_S;

        if (rand() % 11 == 10) {
            int newRequests = rand() % 80 + 1;
            
            for (int i = 0; i < newRequests; i++) {
                int processTime = rand() % maxProcessTime + 1;
                char jobType = rand() % 2 == 0 ? 'P' : 'S';
                Request newRequest = generateRequest(processTime, jobType);
                if (jobType == 'P') {
                    requestQueue_P.push_back(newRequest);
                } else {
                    requestQueue_S.push_back(newRequest);
                }
            }
        }

        loadBalancer_P.runCycle(&requestQueue_P, logFile);
        loadBalancer_S.runCycle(&requestQueue_S, logFile);

        clockTime++;
    }
}