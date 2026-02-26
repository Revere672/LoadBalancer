/**
 * @file request.cpp
 * @brief Implementation of the Request class.
 *
 * @details Implements all member functions of the Request class, including
 * constructors, accessors, and the private IP address generator.
 *
 * @author Load Balancer Project
 * @date 2025
 */

#include "request.h"
#include <cstdlib>

/**
 * @brief Default constructor. Initializes an empty, placeholder request.
 *
 * Sets IP addresses to empty strings, processTime to 0, and jobType to 'P'.
 */
Request::Request() {
    IPin = "";
    IPout = "";
    processTime = 0;
    jobType = 'P';
}

/**
 * @brief Parameterized constructor. Builds a request with random IPs.
 *
 * @param processTime The number of clock cycles this request will take to process.
 * @param jobType The category of the job ('P' for primary, 'S' for secondary).
 */
Request::Request(int processTime, char jobType) {
    this->IPin = generateIP();
    this->IPout = generateIP();
    this->processTime = processTime;
    this->jobType = jobType;
}

/**
 * @brief Generates a random IPv4 address string.
 *
 * @details Produces a dotted-decimal string by generating four random integers
 * each in the range [0, 255] and joining them with '.' separators.
 *
 * @return A randomly generated IPv4 address as a std::string.
 */
std::string Request::generateIP() {
    std::string IP = "";

    for (int i = 0; i < 4; i++) {
        IP += std::to_string(rand() % 256);
        if (i < 3) {
            IP += ".";
        }
    }

    return IP;
}

/**
 * @brief Returns the source IP address.
 * @return The IPin field as a const std::string reference.
 */
std::string Request::getIPin() const {
    return IPin;
}

/**
 * @brief Returns the destination IP address.
 * @return The IPout field as a const std::string reference.
 */
std::string Request::getIPout() const {
    return IPout;
}

/**
 * @brief Returns the processing time required for this request.
 * @return The processTime field as an integer.
 */
int Request::getProcessTime() const {
    return processTime;
}

/**
 * @brief Returns the job type of this request.
 * @return The jobType field as a char.
 */
char Request::getJobType() const {
    return jobType;
}
