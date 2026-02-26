/**
 * @file loadBalancer.h
 * @brief Declaration of the LoadBalancer class.
 *
 * @details Defines LoadBalancer, which manages a pool of WebServer instances
 * and a queue of incoming Requests. Each call to runCycle() advances the
 * simulation by one clock tick: new requests are enqueued, available servers
 * receive requests from the queue, all servers are updated, and the server
 * pool is dynamically scaled based on configurable threshold parameters.
 *
 * @author Load Balancer Project
 * @date 2025
 */

#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include <queue>
#include <vector>
#include <fstream>
#include "request.h"
#include "webServer.h"
#include "utils.h"

/**
 * @class LoadBalancer
 * @brief Distributes incoming network requests across a dynamic pool of web servers.
 *
 * @details A LoadBalancer owns a request queue and a vector of WebServer objects.
 * It is identified by a single character name (e.g., 'P' or 'S'). On each clock
 * cycle it:
 *  -# Accepts newly generated requests and pushes them onto its queue.
 *  -# Dispatches queued requests to idle servers (first-come, first-served).
 *  -# Calls update() on every server to advance their processing timers.
 *  -# Periodically evaluates the queue length relative to the server count and
 *     allocates or deallocates servers to maintain balance.
 */
class LoadBalancer {
    public:
        /**
         * @brief Constructs a LoadBalancer with pre-populated queues and servers.
         *
         * @param requestQueue   Initial queue of requests to process.
         * @param webServers     Initial pool of web server instances.
         * @param name           Single-character label identifying this load balancer.
         * @param minThreshold   If queue size < minThreshold * serverCount, a server is freed.
         * @param maxThreshold   If queue size > maxThreshold * serverCount, a server is added.
         * @param cooldownTime   Number of clock cycles between auto-scaling evaluations.
         */
        LoadBalancer(std::queue<Request> requestQueue,
                    std::vector<WebServer> webServers,
                    char name,
                    int minThreshold,
                    int maxThreshold,
                    int cooldownTime);

        /**
         * @brief Executes a single clock cycle of the load balancer.
         *
         * @details Enqueues all requests in @p newRequests, dispatches work to
         * available servers, updates server states, and (every @c cooldownTime
         * cycles) auto-scales the server pool. Progress is logged to both
         * stdout and @p logFile.
         *
         * @param newRequests Pointer to a vector of requests generated this cycle
         *                    (vector is consumed during the call).
         * @param logFile     Reference to an open output stream for persistent logging.
         */
        void runCycle(std::vector<Request> *newRequests, std::ofstream& logFile);

    private:
        std::queue<Request> requestQueue;   ///< Queue of pending requests awaiting dispatch.
        std::vector<WebServer> webServers;  ///< Active pool of web server instances.

        char name;          ///< Single-character identifier for this load balancer.
        int clockTime;      ///< Current simulation clock value (incremented each cycle).
        int minThreshold;   ///< Lower bound multiplier for server deallocation.
        int maxThreshold;   ///< Upper bound multiplier for server allocation.
        int cooldownTime;   ///< Cycles between auto-scaling checks.

        /**
         * @brief Sends a request directly to the server with the given ID.
         *
         * @details Searches the server pool for a server matching @p serverId that
         * is currently ready, and calls processRequest() on it.
         *
         * @param request  The request to dispatch.
         * @param serverId The target server's unique ID.
         * @return @c true if the request was successfully dispatched; @c false otherwise.
         */
        bool sendRequest(const Request& request, int serverId);

        /**
         * @brief Adds a new WebServer to the active pool.
         *
         * @details Constructs a new WebServer whose ID is the current clock time
         * and appends it to @c webServers.
         *
         * @param logFile Reference to the log file stream for recording the event.
         * @return Pointer to the newly allocated WebServer.
         */
        WebServer* allocateServer(std::ofstream& logFile);

        /**
         * @brief Removes an idle WebServer from the active pool.
         *
         * @details Finds the first ready server in the pool and removes it. If no
         * server is currently idle, logs a message and returns without modifying
         * the pool.
         *
         * @param logFile Reference to the log file stream for recording the event.
         */
        void deallocateServer(std::ofstream& logFile);
};

#endif
