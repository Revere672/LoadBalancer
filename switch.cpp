/**
 * @file switch.cpp
 * @brief Implementation of the Switch class.
 *
 * @details Implements the Switch constructor and the main simulation loop
 * that orchestrates the Firewall and both LoadBalancer instances.
 *
 * @author Load Balancer Project
 * @date 2025
 */

#include "switch.h"
#include <iostream>

/**
 * @brief Constructs the Switch and wires together all simulation components.
 *
 * @details Uses member-initializer syntax to construct @c loadBalancer_P,
 * @c loadBalancer_S, and @c firewall in place.
 *
 * The Firewall is configured with:
 *  - @c dosRateLimit  = 5  requests per IP per window
 *  - @c dosWindowSize = 20 clock cycles
 *
 * Three example blocked ranges are pre-loaded to demonstrate static blocking:
 *  - @c 10.0.0.0/8    — Private class-A range
 *  - @c 172.16.0.0/12 — Private class-B range
 *  - @c 192.168.0.0/16 — Private class-C range
 *
 * In a production system these would be read from a config file.
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
      loadBalancer_S(requestQueue_S, webServers_S, 'S', minThreshold, maxThreshold, cooldownTime),
      firewall(5, 20)
{
    clockTime = 0;
    this->maxProcessTime = maxProcessTime;

    // --- Static blocked ranges (firewall rules) ---
    // These three cover all RFC-1918 private address space, which would
    // not be valid source IPs on a public-facing load balancer.
    firewall.blockRange("10.0.0.0/8");
    firewall.blockRange("172.16.0.0/12");
    firewall.blockRange("192.168.0.0/16");
}

/**
 * @brief Runs the simulation for the specified number of clock cycles.
 *
 * @details Each iteration:
 *  -# Optionally generates a burst of new requests.
 *  -# Passes the full burst through Firewall::filterRequests(), which enforces
 *     both static IP-range blocks and dynamic DoS rate limits.
 *  -# Splits the filtered requests by job type and forwards them to the
 *     appropriate LoadBalancer via runCycle().
 *  -# Increments @c clockTime.
 *
 * After all cycles complete, prints a summary of how many requests the
 * Firewall blocked in total.
 *
 * @param clockCycles Total number of clock cycles to run.
 * @param logFile     Open output stream for logging all events.
 */
void Switch::run(int clockCycles, std::ofstream& logFile) {
    for (int i = 0; i < clockCycles; i++) {
        std::vector<Request> rawRequests;

        if (rand() % 11 == 10) {
            int newRequests = rand() % 80 + 1;
            for (int j = 0; j < newRequests; j++) {
                int  processTime = rand() % maxProcessTime + 1;
                char jobType = rand() % 2 == 0 ? 'P' : 'S';
                rawRequests.push_back(generateRequest(processTime, jobType));
            }
        }

        std::vector<Request> allowed = firewall.filterRequests(rawRequests, clockTime, logFile);

        std::vector<Request> filtered_P;
        std::vector<Request> filtered_S;
        for (const Request& req : allowed) {
            if (req.getJobType() == 'P')
                filtered_P.push_back(req);
            else
                filtered_S.push_back(req);
        }

        loadBalancer_P.runCycle(&filtered_P, logFile);
        loadBalancer_S.runCycle(&filtered_S, logFile);

        clockTime++;
    }

    std::cout << RED << "\n[Firewall] Simulation complete. Total requests blocked: " << firewall.getTotalBlocked() << RESET << std::endl;
    logFile << "\n[Firewall] Simulation complete. Total requests blocked: " << firewall.getTotalBlocked() << std::endl;
    firewall.printBlockedRanges(logFile);
}