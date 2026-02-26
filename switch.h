/**
 * @file switch.h
 * @brief Declaration of the Switch class, the top-level simulation coordinator.
 *
 * @details The Switch acts as the entry point for the simulation loop. It owns
 * two LoadBalancer instances — one for primary ('P') requests and one for
 * secondary ('S') requests — and drives the overall clock by calling each
 * balancer's runCycle() every tick.
 *
 * On a random subset of cycles (~10% probability) the Switch generates a burst
 * of between 1 and 80 new requests and distributes them to the appropriate
 * balancer based on job type.
 *
 * @author Load Balancer Project
 * @date 2025
 */

#ifndef SWITCH_H
#define SWITCH_H

#include "loadBalancer.h"
#include "request.h"

/**
 * @class Switch
 * @brief Top-level coordinator that drives two LoadBalancer instances.
 *
 * @details The Switch class aggregates a primary LoadBalancer (@c loadBalancer_P)
 * and a secondary LoadBalancer (@c loadBalancer_S). Its run() method advances
 * the simulation for the requested number of clock cycles, injecting random
 * bursts of requests and forwarding them to the appropriate balancer.
 */
class Switch {
    public:
        /**
         * @brief Constructs the Switch and initializes both load balancers.
         *
         * @param requestQueue_P Initial request queue for the primary load balancer.
         * @param requestQueue_S Initial request queue for the secondary load balancer.
         * @param webServers_P   Initial server pool for the primary load balancer.
         * @param webServers_S   Initial server pool for the secondary load balancer.
         * @param minThreshold   Passed through to both LoadBalancer instances.
         * @param maxThreshold   Passed through to both LoadBalancer instances.
         * @param cooldownTime   Passed through to both LoadBalancer instances.
         * @param maxProcessTime Maximum processing time (in cycles) for generated requests.
         */
        Switch(std::queue<Request> requestQueue_P, 
               std::queue<Request> requestQueue_S,
               std::vector<WebServer> webServers_P,
               std::vector<WebServer> webServers_S,
               int minThreshold, int maxThreshold, int cooldownTime, int maxProcessTime);

        /**
         * @brief Runs the simulation for the specified number of clock cycles.
         *
         * @details Each cycle:
         *  -# With ~10% probability, generates 1–80 new requests of random job type.
         *  -# Calls loadBalancer_P.runCycle() with the new primary requests.
         *  -# Calls loadBalancer_S.runCycle() with the new secondary requests.
         *  -# Increments the internal clock counter.
         *
         * @param clockCycles Total number of cycles to simulate.
         * @param logFile     Open output stream used for event logging across both balancers.
         */
        void run(int clockCycles, std::ofstream& logFile);

    private:
        LoadBalancer loadBalancer_P;  ///< Load balancer handling primary ('P') requests.
        LoadBalancer loadBalancer_S;  ///< Load balancer handling secondary ('S') requests.
        int clockTime;                ///< Current simulation clock (incremented each cycle).
        int maxProcessTime;           ///< Upper bound on randomly generated request durations.

        std::ofstream logFile;        ///< Internal log file stream (unused; logging via run() param).
};

#endif
