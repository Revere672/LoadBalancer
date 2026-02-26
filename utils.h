/**
 * @file utils.h
 * @brief Utility functions for the load balancer simulation.
 *
 * @details Declares helper functions used across multiple translation units
 * in the load balancer system. Currently exposes a factory function for
 * creating Request objects.
 *
 * @author Load Balancer Project
 * @date 2025
 */

#ifndef UTILS_H
#define UTILS_H

#include "request.h"

/**
 * @brief Factory function that constructs and returns a new Request.
 *
 * @details Provides a centralized way to create Request objects outside
 * of the Request class itself, supporting cleaner separation of concerns
 * between the Switch/LoadBalancer and the Request data model.
 *
 * @param processTime The number of clock cycles needed to process the request.
 * @param jobType     The job category ('P' for primary, 'S' for secondary).
 * @return A fully initialized Request object.
 */
Request generateRequest(int processTime, char jobType);

#endif
