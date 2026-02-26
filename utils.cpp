/**
 * @file utils.cpp
 * @brief Implementation of utility functions for the load balancer simulation.
 *
 * @author Load Balancer Project
 * @date 2025
 */

#include "utils.h"

/**
 * @brief Creates and returns a new Request with the given parameters.
 *
 * @details Delegates directly to the Request parameterized constructor.
 * Source and destination IP addresses are randomly generated inside
 * the Request constructor.
 *
 * @param processTime The number of clock cycles the request will take to process.
 * @param jobType     The job category identifier ('P' or 'S').
 * @return A new Request object ready to be queued.
 */
Request generateRequest(int processTime, char jobType) {
    return Request(processTime, jobType);
}
