/**
 * @file request.h
 * @brief Declaration of the Request class representing a network request.
 *
 * @details This header defines the Request class, which models an incoming
 * network request processed by the load balancer system. Each request
 * carries source and destination IP addresses, an estimated processing
 * time, and a job type identifier.
 *
 * @author Load Balancer Project
 * @date 2025
 */

#ifndef REQUEST_H
#define REQUEST_H

#include <string>

/**
 * @class Request
 * @brief Represents a single network request to be handled by a web server.
 *
 * @details A Request object encapsulates all information needed to route and
 * process a network request, including randomly generated source/destination
 * IP addresses, the number of clock cycles required to process it, and the
 * job type ('P' for primary, 'S' for secondary).
 */
class Request {
    public:
        /**
         * @brief Default constructor. Creates an empty request.
         *
         * Initializes IP addresses to empty strings, processTime to 0,
         * and jobType to 'P'.
         */
        Request();

        /**
         * @brief Parameterized constructor. Creates a request with generated IPs.
         *
         * @param processTime Number of clock cycles required to process this request.
         * @param jobType Character identifying the job type ('P' for primary, 'S' for secondary).
         */
        Request(int processTime, char jobType);

        /**
         * @brief Returns the source (input) IP address of the request.
         * @return A string in dotted-decimal IPv4 format (e.g., "192.168.1.1").
         */
        std::string getIPin() const;

        /**
         * @brief Returns the destination (output) IP address of the request.
         * @return A string in dotted-decimal IPv4 format (e.g., "10.0.0.1").
         */
        std::string getIPout() const;

        /**
         * @brief Returns the number of clock cycles needed to process this request.
         * @return An integer representing the processing time in clock cycles.
         */
        int getProcessTime() const;

        /**
         * @brief Returns the job type of this request.
         * @return A character identifying the job type ('P' or 'S').
         */
        char getJobType() const;

    private:
        std::string IPin;     ///< Source IP address (randomly generated).
        std::string IPout;    ///< Destination IP address (randomly generated).
        int processTime;      ///< Processing time in clock cycles.
        char jobType;         ///< Job type identifier ('P' or 'S').

        /**
         * @brief Generates a random IPv4 address string.
         * @return A string in dotted-decimal format with each octet in [0, 255].
         */
        std::string generateIP();
};

#endif
