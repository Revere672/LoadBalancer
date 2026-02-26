/**
 * @file switch.h
 * @brief Declaration of the Switch class, the top-level simulation coordinator.
 *
 * @details The Switch acts as the entry point for all simulated network traffic.
 * It owns two LoadBalancer instances (primary 'P' and secondary 'S') and a
 * Firewall that filters every incoming request before it reaches either balancer.
 *
 * On a random subset of cycles (~10% probability) the Switch generates a burst
 * of between 1 and 80 new requests. Each request first passes through the
 * Firewall's IP-range and DoS-rate checks; only allowed requests are forwarded
 * to the appropriate LoadBalancer.
 *
 * @author Load Balancer Project
 * @date 2025
 */

#ifndef SWITCH_H
#define SWITCH_H

#include "loadBalancer.h"
#include "firewall.h"
#include "request.h"
#include "utils.h"

/**
 * @class Switch
 * @brief Top-level coordinator that drives two LoadBalancer instances behind a Firewall.
 *
 * @details Instantiates and wires together all simulation components. The Firewall
 * is the first stop for every request; only requests that pass IP-range and
 * rate-limit checks are forwarded to @c loadBalancer_P or @c loadBalancer_S.
 */
class Switch {
    public:
        /**
         * @brief Constructs the Switch, initializes both LoadBalancers and the Firewall.
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
         *  -# With ~1/11 probability, generates 1–80 new requests of random job type.
         *  -# Passes all new requests through the Firewall for filtering.
         *  -# Calls loadBalancer_P.runCycle() with the filtered primary requests.
         *  -# Calls loadBalancer_S.runCycle() with the filtered secondary requests.
         *  -# Increments the internal clock counter.
         *
         * At the end of the run, prints a firewall summary (total blocked count).
         *
         * @param clockCycles Total number of cycles to simulate.
         * @param logFile     Open output stream used for event logging.
         */
        void run(int clockCycles, std::ofstream& logFile);

    private:
        LoadBalancer loadBalancer_P;  ///< Load balancer handling primary ('P') requests.
        LoadBalancer loadBalancer_S;  ///< Load balancer handling secondary ('S') requests.
        Firewall firewall;            ///< Perimeter firewall; filters all incoming requests.
        int clockTime;                ///< Current simulation clock (incremented each cycle).
        int maxProcessTime;           ///< Upper bound on randomly generated request durations.
};

#endif