/**
 * @file firewall.cpp
 * @brief Implementation of the Firewall class.
 *
 * @details Provides IP range parsing, per-IP rate tracking, and request
 * filtering logic used by the Switch to drop unwanted traffic before it
 * reaches either LoadBalancer.
 *
 * @author Load Balancer Project
 * @date 2025
 */

#include "firewall.h"
#include <iostream>
#include <sstream>
#include <algorithm>

/**
 * @brief Constructs a Firewall with the given DoS thresholds.
 *
 * @param dosRateLimit  Max requests a single source IP may send per window.
 * @param dosWindowSize Number of clock cycles in each rate-limit window.
 */
Firewall::Firewall(int dosRateLimit, int dosWindowSize) {
    this->dosRateLimit  = dosRateLimit;
    this->dosWindowSize = dosWindowSize;
    this->totalBlocked  = 0;
}

/**
 * @brief Converts a dotted-decimal IPv4 string to a 32-bit unsigned integer.
 *
 * @details Splits on '.' and packs each octet into the result with bit shifts.
 * Returns 0 if the string cannot be parsed.
 *
 * @param ip Dotted-decimal string (e.g. "10.0.1.55").
 * @return   32-bit packed representation, MSB = first octet.
 */
unsigned int Firewall::ipToUint(const std::string& ip) const {
    unsigned int result = 0;
    std::stringstream ss(ip);
    std::string octet;
    int shift = 24;

    while (std::getline(ss, octet, '.') && shift >= 0) {
        try {
            unsigned int val = std::stoul(octet);
            if (val > 255) return 0;          // invalid octet
            result |= (val << shift);
        } catch (...) {
            return 0;
        }
        shift -= 8;
    }
    return result;
}

/**
 * @brief Parses a CIDR string and adds it to the blocked range list.
 *
 * @details Expected format: @c "A.B.C.D/prefix". The prefix length determines
 * the subnet mask: prefix bits set from the MSB, remaining bits cleared.
 * A prefix of 32 blocks exactly one host; a prefix of 0 blocks everything.
 *
 * @param cidr CIDR notation string such as @c "192.168.0.0/16".
 */
void Firewall::blockRange(const std::string& cidr) {
    // Split on '/'
    size_t slash = cidr.find('/');
    if (slash == std::string::npos) {
        std::cerr << "[Firewall] WARNING: invalid CIDR '" << cidr << "' — skipping." << std::endl;
        return;
    }

    std::string ipPart     = cidr.substr(0, slash);
    std::string prefixPart = cidr.substr(slash + 1);

    int prefix = 0;
    try {
        prefix = std::stoi(prefixPart);
    } catch (...) {
        std::cerr << "[Firewall] WARNING: invalid prefix in '" << cidr << "' — skipping." << std::endl;
        return;
    }

    if (prefix < 0 || prefix > 32) {
        std::cerr << "[Firewall] WARNING: prefix out of range in '" << cidr << "' — skipping." << std::endl;
        return;
    }

    unsigned int network = ipToUint(ipPart);
    // Build mask: prefix 1-bits from the MSB
    unsigned int mask = (prefix == 0) ? 0u : (~0u << (32 - prefix));

    IpRange range;
    range.network = network & mask;   // ensure host bits are zeroed
    range.mask    = mask;
    range.label   = cidr;

    blockedRanges.push_back(range);
    std::cout << "[Firewall] Blocked range added: " << cidr << std::endl;
}

/**
 * @brief Tests whether a dotted-decimal IP falls within any blocked range.
 *
 * @param ip Source IP string to test.
 * @return   @c true if at least one range matches.
 */
bool Firewall::isInBlockedRange(const std::string& ip) const {
    unsigned int ipInt = ipToUint(ip);
    for (const IpRange& range : blockedRanges) {
        if ((ipInt & range.mask) == range.network) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Tests whether an IP has been auto-blocked by the DoS detector.
 *
 * @param ip Source IP string to test.
 * @return   @c true if found in @c autoBlockedIps.
 */
bool Firewall::isAutoBlocked(const std::string& ip) const {
    return std::find(autoBlockedIps.begin(), autoBlockedIps.end(), ip)
           != autoBlockedIps.end();
}

/**
 * @brief Filters incoming requests, dropping blocked or rate-exceeded sources.
 *
 * @details Processing order per request:
 *  -# If @c clockTime is the start of a new DoS window, reset all per-IP counters
 *     (auto-blocked IPs remain blocked permanently).
 *  -# Drop if the source IP is in a static blocked range.
 *  -# Drop if the source IP was previously auto-blocked.
 *  -# Increment the per-IP request counter; if it now exceeds @c dosRateLimit,
 *     auto-block the IP and drop this request.
 *  -# Otherwise allow the request through.
 *
 * @param requests  Raw burst of incoming requests for this cycle.
 * @param clockTime Current simulation clock tick.
 * @param logFile   Open log stream for recording block events.
 * @return          Vector of requests that passed all firewall checks.
 */
std::vector<Request> Firewall::filterRequests(const std::vector<Request>& requests,
                                               int clockTime,
                                               std::ofstream& logFile) {
    // Reset per-IP counters at the start of each new window
    if (dosWindowSize > 0 && clockTime % dosWindowSize == 0 && clockTime != 0) {
        std::cout << RED << "[Firewall] DoS window reset at clock " << clockTime << RESET << std::endl;
        logFile   << "[Firewall] DoS window reset at clock " << clockTime << std::endl;
        ipRequestCount.clear();
    }

    std::vector<Request> allowed;

    for (const Request& req : requests) {
        const std::string& srcIp = req.getIPin();

        // --- Check 1: static blocked range ---
        if (isInBlockedRange(srcIp)) {
            std::cout << RED << "[Firewall] BLOCKED (range)  src=" << srcIp
                      << "  dst=" << req.getIPout() << RESET << std::endl;
            logFile   << "[Firewall] BLOCKED (range)  src=" << srcIp
                      << "  dst=" << req.getIPout() << std::endl;
            totalBlocked++;
            continue;
        }

        // --- Check 2: previously auto-blocked IP ---
        if (isAutoBlocked(srcIp)) {
            std::cout << RED << "[Firewall] BLOCKED (DoS ban) src=" << srcIp
                      << "  dst=" << req.getIPout() << RESET << std::endl;
            logFile   << "[Firewall] BLOCKED (DoS ban) src=" << srcIp
                      << "  dst=" << req.getIPout() << std::endl;
            totalBlocked++;
            continue;
        }

        // --- Check 3: rate limiting ---
        ipRequestCount[srcIp]++;
        if (ipRequestCount[srcIp] > dosRateLimit) {
            // First time this IP trips the limit — auto-block and log
            if (ipRequestCount[srcIp] == dosRateLimit + 1) {
                autoBlockedIps.push_back(srcIp);
                std::cout << RED << "[Firewall] DoS DETECTED — auto-blocked src=" << srcIp
                          << "  (exceeded " << dosRateLimit
                          << " requests/window)" << RESET << std::endl;
                logFile   << "[Firewall] DoS DETECTED — auto-blocked src=" << srcIp
                          << "  (exceeded " << dosRateLimit
                          << " requests/window)" << std::endl;
            }
            totalBlocked++;
            continue;
        }

        // Request passed all checks
        allowed.push_back(req);
    }

    return allowed;
}

/**
 * @brief Returns the cumulative count of all dropped requests.
 * @return Total blocked request count.
 */
int Firewall::getTotalBlocked() const {
    return totalBlocked;
}

/**
 * @brief Logs all currently registered blocked IP ranges.
 *
 * @param logFile Open output stream for logging.
 */
void Firewall::printBlockedRanges(std::ofstream& logFile) const {
    std::cout << RED << "[Firewall] Blocked IP ranges (" << blockedRanges.size() << "):" << RESET << std::endl;
    logFile   << "[Firewall] Blocked IP ranges (" << blockedRanges.size() << "):" << std::endl;
    for (const IpRange& r : blockedRanges) {
        std::cout << "  " << r.label << std::endl;
        logFile   << "  " << r.label << std::endl;
    }
}