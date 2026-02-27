/**
 * @file webServer.h
 * @brief Declaration of the WebServer class representing a single server node.
 *
 * @details Defines WebServer, which simulates a single server in the load
 * balancing pool. A WebServer can be in one of two states: available (ready
 * to accept a new request) or busy (processing a current request). Each clock
 * cycle, a busy server decrements its remaining processing time until it
 * becomes available again.
 *
 * @author Load Balancer Project
 * @date 2025
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "request.h"

/**
 * @class WebServer
 * @brief Simulates a single web server node that processes network requests.
 *
 * @details Each WebServer has a unique integer ID assigned at construction.
 * It maintains a current request, a countdown timer, and an availability flag.
 * The LoadBalancer dispatches requests to available servers and calls update()
 * each cycle to advance the simulation.
 */
class WebServer {
    public:
        /**
         * @brief Constructs a WebServer with the given unique ID.
         *
         * @param id A unique integer identifier for this server instance.
         *           The server starts in the available (ready) state.
         */
        WebServer(int id);

        /**
         * @brief Assigns a request to this server and marks it as busy.
         *
         * @details Stores the request, sets isAvailable to false, and initializes
         * timeRemaining to the request's processing time.
         *
         * @param request The Request object to be processed.
         */
        void processRequest(const Request& request);

        /**
         * @brief Advances the server by one clock cycle.
         *
         * @details If the server is busy, decrements timeRemaining by one.
         * When timeRemaining reaches zero or below, the server becomes available.
         */
        void update();

        /**
         * @brief Checks whether the server is available to accept a new request.
         * @return @c true if the server is idle; @c false if it is still processing.
         */
        bool isReady() const;

        /**
         * @brief Returns the unique identifier of this server.
         * @return The integer ID assigned at construction.
         */
        int getId() const;

    private:
        int id;                  ///< Unique server identifier.
        bool isAvailable;        ///< True when the server is idle and ready for work.
        Request currentRequest;  ///< The request currently being processed.
        int timeRemaining;       ///< Clock cycles remaining to finish current request.
};

#endif
