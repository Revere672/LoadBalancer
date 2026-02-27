/**
 * @file webServer.cpp
 * @brief Implementation of the WebServer class.
 *
 * @details Provides the simulation logic for a single web server node,
 * including request assignment and per-cycle state updates.
 *
 * @author Load Balancer Project
 * @date 2025
 */

#include "webServer.h"

/**
 * @brief Constructs a WebServer with the given ID in the idle (available) state.
 *
 * @param id Unique integer identifier for this server.
 */
WebServer::WebServer(int id) {
    this->id = id;
    this->isAvailable = true;
    this->timeRemaining = 0;
}

/**
 * @brief Assigns a request to this server and begins processing.
 *
 * @details Stores the incoming request, sets the server to busy, and
 * initializes the countdown timer from the request's processing time.
 *
 * @param request The Request to process.
 */
void WebServer::processRequest(const Request& request) {
    currentRequest = request;
    isAvailable = false;
    timeRemaining = request.getProcessTime();
}

/**
 * @brief Advances the server state by one clock cycle.
 *
 * @details Decrements timeRemaining if the server is busy. When the timer
 * reaches zero, the server transitions back to the available state.
 */
void WebServer::update() {
    if (!isAvailable) {
        timeRemaining--;

        if (timeRemaining <= 0) {
            isAvailable = true;
        }
    }
}

/**
 * @brief Returns whether the server is idle and ready for a new request.
 * @return @c true if isAvailable is set; @c false otherwise.
 */
bool WebServer::isReady() const {
    return isAvailable;
}

/**
 * @brief Returns the server's unique identifier.
 * @return The integer ID set at construction.
 */
int WebServer::getId() const {
    return id;
}
