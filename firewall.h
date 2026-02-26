/**
 * @file firewall.h
 * @brief Declaration of the Firewall class for IP range blocking and DoS prevention.
 *
 * @details The Firewall class acts as a network perimeter guard, sitting between
 * the Switch and the LoadBalancer instances. It provides two layers of protection:
 *
 *  1. **Static IP range blocking** — Administrators can manually add CIDR-style
 *     IP ranges (e.g., "192.168.1.0/24") that are permanently blocked.
 *
 *  2. **Dynamic DoS detection** — Tracks how many requests each source IP has
 *     submitted within a rolling time window. If a single IP exceeds the
 *     configured rate limit, it is automatically added to the block list.
 *
 * Every request passes through isBlocked() before being forwarded to a
 * LoadBalancer. Blocked requests are dropped and logged.
 *
 * @author Load Balancer Project
 * @date 2025
 */

#ifndef FIREWALL_H
#define FIREWALL_H

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "request.h"
#include "utils.h"

/**
 * @struct IpRange
 * @brief Represents a blocked IPv4 subnet in network/mask form.
 *
 * @details Stores a pre-parsed network address and its bitmask so that
 * membership checks are a single bitwise AND comparison.
 */
struct IpRange {
    unsigned int network; ///< Network address as a 32-bit integer.
    unsigned int mask;    ///< Subnet mask as a 32-bit integer.
    std::string label;    ///< Human-readable CIDR string (e.g. "10.0.0.0/8") for logging.
};

/**
 * @class Firewall
 * @brief Filters incoming requests by IP range and per-IP request rate.
 *
 * @details Instantiated once inside the Switch. On each clock cycle the Switch
 * calls filterRequests() with the raw burst of new requests; the Firewall
 * returns only those that are allowed to proceed to the LoadBalancers.
 *
 * ### Blocked-range check
 * Each source IP is converted to a 32-bit integer and tested against every
 * registered IpRange via `(ip & mask) == network`. If any range matches,
 * the request is dropped.
 *
 * ### DoS rate-limit check
 * A per-IP counter tracks how many requests that IP has sent during the current
 * window of @c dosWindowSize clock cycles. Once the counter exceeds
 * @c dosRateLimit the IP is auto-blocked for the remainder of the simulation
 * and logged as a DoS source.
 */
class Firewall {
public:
    /**
     * @brief Constructs a Firewall with the given DoS detection parameters.
     *
     * @param dosRateLimit  Maximum requests a single IP may send within
     *                      @p dosWindowSize cycles before being auto-blocked.
     * @param dosWindowSize Number of clock cycles that form one rate-limit window.
     *                      Counters reset at the start of each new window.
     */
    Firewall(int dosRateLimit, int dosWindowSize);

    /**
     * @brief Registers a static blocked IP range.
     *
     * @details Parses the CIDR string into an IpRange and stores it internally.
     * All future requests whose source IP falls within this range will be dropped.
     *
     * @param cidr A CIDR notation string such as @c "192.168.1.0/24".
     *
     * @note Passing an invalid CIDR string prints a warning and skips the entry.
     */
    void blockRange(const std::string& cidr);

    /**
     * @brief Filters a vector of requests, dropping any that are blocked.
     *
     * @details For each request in @p requests:
     *  -# The source IP is checked against all static blocked ranges.
     *  -# The per-IP request counter for this window is incremented; if it
     *     exceeds @c dosRateLimit the IP is auto-blocked and the request dropped.
     *  -# Passing requests are appended to the returned vector.
     *
     * At the start of each new DoS window (determined by @p clockTime), all
     * per-IP counters are reset.
     *
     * @param requests  The raw incoming requests to filter.
     * @param clockTime The current simulation clock tick (used for window resets).
     * @param logFile   Open output stream; blocked events are written here.
     * @return          A vector containing only the requests that passed filtering.
     */
    std::vector<Request> filterRequests(const std::vector<Request>& requests,
                                        int clockTime,
                                        std::ofstream& logFile);

    /**
     * @brief Returns the total number of requests blocked since construction.
     * @return Cumulative count of dropped requests.
     */
    int getTotalBlocked() const;

    /**
     * @brief Prints all currently blocked IP ranges to stdout and the log file.
     * @param logFile Open output stream for logging.
     */
    void printBlockedRanges(std::ofstream& logFile) const;

private:
    std::vector<IpRange> blockedRanges; ///< Statically configured blocked subnets.

    /**
     * @brief Maps a source IP string to its request count in the current window.
     *
     * Entries are reset every @c dosWindowSize clock cycles.
     */
    std::unordered_map<std::string, int> ipRequestCount;

    /**
     * @brief Set of IPs that have been auto-blocked due to DoS detection.
     *
     * Once an IP is in this set it is blocked for the life of the simulation,
     * even after window resets.
     */
    std::vector<std::string> autoBlockedIps;

    int dosRateLimit;   ///< Max requests per IP per window before auto-blocking.
    int dosWindowSize;  ///< Clock cycles per rate-limit window.
    int totalBlocked;   ///< Running total of all dropped requests.

    /**
     * @brief Converts a dotted-decimal IPv4 string to a 32-bit unsigned integer.
     *
     * @param ip A string such as @c "192.168.1.100".
     * @return   The IP address packed into a @c uint32_t, or 0 on parse failure.
     */
    unsigned int ipToUint(const std::string& ip) const;

    /**
     * @brief Tests whether @p ip falls inside any registered blocked range.
     *
     * @param ip Dotted-decimal IPv4 string to test.
     * @return   @c true if the IP is covered by at least one blocked range.
     */
    bool isInBlockedRange(const std::string& ip) const;

    /**
     * @brief Tests whether @p ip has been auto-blocked by DoS detection.
     *
     * @param ip Dotted-decimal IPv4 string to test.
     * @return   @c true if the IP appears in @c autoBlockedIps.
     */
    bool isAutoBlocked(const std::string& ip) const;
};

#endif