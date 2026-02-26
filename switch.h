#ifndef SWITHCH_H
#define SWITCH_H

#include "loadBalancer.h"
#include "request.h"

class Switch {
    public:
        Switch(std::queue<Request> requestQueue_P, 
               std::queue<Request> requestQueue_S,
               std::vector<WebServer> webServers_P,
               std::vector<WebServer> webServers_S,
               int minThreshold, int maxThreshold, int cooldownTime, int maxProcessTime);

        void run(int clockCycles, std::ofstream& logFile);

    private:
        LoadBalancer loadBalancer_P;
        LoadBalancer loadBalancer_S;
        int clockTime;
        int maxProcessTime;

        std::ofstream logFile;
};

#endif