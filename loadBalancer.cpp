/**
 * @file loadBalancer.cpp
 * @brief Implementation of the LoadBalancer class.
 *
 * @details Implements the per-cycle simulation logic, request dispatching,
 * and dynamic server allocation/deallocation for the LoadBalancer.
 *
 * @author Load Balancer Project
 * @date 2025
 */

#include "loadBalancer.h"
#include <iostream>
#include <cstdlib>

/**
 * @brief Constructs a LoadBalancer with the supplied initial state.
 *
 * @param requestQueue   Pre-populated queue of requests to begin processing.
 * @param webServers     Initial set of web server instances.
 * @param name           Single character identifying this load balancer ('P' or 'S').
 * @param minThreshold   Per-server minimum queue length before deallocation.
 * @param maxThreshold   Per-server maximum queue length before allocation.
 * @param cooldownTime   Cycles between auto-scaling evaluations.
 */
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

/**
 * @brief Runs one full clock cycle of the load balancer simulation.
 *
 * @details The cycle proceeds in the following order:
 *  -# All requests in @p newRequests are pushed onto the internal queue.
 *  -# Each idle server is given the next request from the front of the queue.
 *  -# Every server's update() method is called to decrement processing timers.
 *  -# If the current clock time is a multiple of @c cooldownTime, the queue
 *     depth is compared against the scaled thresholds to decide whether to
 *     allocate or deallocate a server.
 *  -# The clock counter is incremented.
 *
 * @param newRequests Pointer to a vector of requests produced this cycle;
 *                    elements are popped from the back and pushed to the queue.
 * @param logFile     Open output file stream used for persistent event logging.
 */
int LoadBalancer::runCycle(std::vector<Request> *newRequests, std::ofstream& logFile) {
    std::cout << "Load Balancer " << name << " - Running cycle at clock time: " << clockTime << std::endl;
    logFile << "Load Balancer " << name << " - Running cycle at clock time: " << clockTime << std::endl;

    std::cout << BLUE << "Generated " << newRequests->size() << " new requests." << RESET << std::endl;
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

    std::cout << YELLOW << "Queue Size: " << requestQueue.size() << ", Active Servers: " << webServers.size()<< RESET << std::endl;
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

    return requestQueue.size();
}

/**
 * @brief Dispatches a request to the identified server.
 *
 * @details Searches @c webServers for a server whose ID matches @p serverId
 * and whose isReady() returns @c true. If found, processRequest() is called.
 *
 * @param request  The Request to dispatch.
 * @param serverId The integer ID of the target server.
 * @return @c true if the request was dispatched successfully; @c false if no
 *         matching ready server was found.
 */
bool LoadBalancer::sendRequest(const Request& request, int serverId) {
    for (WebServer &server: this->webServers) {
        if (server.getId() == serverId && server.isReady()) {
            server.processRequest(request);
            return true;
        }
    }
    return false;
}

/**
 * @brief Provisions a new WebServer and appends it to the active pool.
 *
 * @details The new server receives an ID equal to the current @c clockTime,
 * which provides a rough timestamp of when it was created.
 *
 * @param logFile Open output file stream; the allocation event is written here.
 * @return Pointer to the newly created WebServer inside the @c webServers vector.
 *
 * @warning The returned pointer may be invalidated if @c webServers is subsequently
 *          modified (e.g., by another allocation that triggers a reallocation).
 */
WebServer* LoadBalancer::allocateServer(std::ofstream& logFile) {
    webServers.push_back(WebServer(clockTime));
    std::cout << GREEN << "Allocated new server with ID: " << webServers.back().getId() << RESET << std::endl;
    logFile << "Allocated new server with ID: " << webServers.back().getId() << std::endl;
    return &webServers.back();
}

/**
 * @brief Removes the first idle server found in the active pool.
 *
 * @details Iterates through @c webServers and erases the first entry whose
 * isReady() returns @c true. If no idle server exists, logs a notice and
 * returns without modifying the pool.
 *
 * @param logFile Open output file stream; the deallocation event is written here.
 */
void LoadBalancer::deallocateServer(std::ofstream& logFile) {
    if (!webServers.empty()) {
        for (unsigned int i = 0; i < webServers.size(); i++) {
            if (webServers[i].isReady()) {
                std::cout << CYAN << "Deallocated server with ID: " << webServers[i].getId() << RESET << std::endl;
                logFile << "Deallocated server with ID: " << webServers[i].getId() << std::endl;
                webServers.erase(webServers.begin() + i);
                return;
            }
        }

        std::cout << CYAN << "No servers available for deallocation." << RESET << std::endl;
        logFile << "No servers available for deallocation." << std::endl;
    }
}
