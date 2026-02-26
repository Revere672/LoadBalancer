/**
 * @file switch.cpp
 * @brief Implementation of the Switch class.
 *
 * @details Implements the Switch constructor and the main simulation loop
 * that orchestrates both primary and secondary LoadBalancer instances.
 *
 * @author Load Balancer Project
 * @date 2025
 */

#include "switch.h"
#include <iostream>

/**
 * @brief Constructs the Switch and forwards configuration to both load balancers.
 *
 * @details Uses member-initializer syntax to construct @c loadBalancer_P and
 * @c loadBalancer_S in place, avoiding unnecessary copies.
 *
 * @param requestQueue_P Pre-populated queue of primary requests.
 * @param requestQueue_S Pre-populated queue of secondary requests.
 * @param webServers_P   Initial server pool for the primary balancer.
 * @param webServers_S   Initial server pool for the secondary balancer.
 * @param minThreshold   Per-server queue lower bound for deallocation.
 * @param maxThreshold   Per-server queue upper bound for allocation.
 * @param cooldownTime   Cycles between auto-scaling checks.
 * @param maxProcessTime Maximum processing time for dynamically generated requests.
 */
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

/**
 * @brief Advances the simulation for the specified number of clock cycles.
 *
 * @details On each cycle a random number is drawn. If the value equals 10
 * (approximately a 1-in-11 chance), a burst of 1–80 new requests is created.
 * Each request is randomly assigned job type 'P' or 'S' and placed in the
 * corresponding temporary vector. Both load balancers then process their
 * respective new-request vectors via runCycle().
 *
 * @param clockCycles Total number of cycles to simulate.
 * @param logFile     Open output stream shared by both load balancers for logging.
 */
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
