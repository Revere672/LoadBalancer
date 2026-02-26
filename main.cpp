/**
 * @file main.cpp
 * @brief Entry point for the load balancer simulation.
 *
 * @details Reads simulation parameters from a configuration file
 * (@c config.txt), initializes the request queues and web server pools,
 * constructs the Switch, and runs the simulation for the configured number
 * of clock cycles.
 *
 * <b>Configuration file format (config.txt):</b>
 * @code
 * initialServers: <int>
 * clockCycles:    <int>
 * minThreshold:   <int>
 * maxThreshold:   <int>
 * cooldownTime:   <int>
 * maxProcessingTime: <int>
 * @endcode
 *
 * Each load balancer starts with @c initialServers servers and a pre-filled
 * queue of @c initialServers * 100 requests. New requests may arrive randomly
 * during the run managed by the Switch.
 *
 * @author Load Balancer Project
 * @date 2025
 *
 * @return 0 on successful completion.
 */

#include <queue>
#include <vector>
#include <iostream>
#include "switch.h"

/**
 * @brief Program entry point.
 *
 * @details Performs the following initialization steps:
 *  -# Opens the log file and configuration file.
 *  -# Parses each configuration parameter by extracting the value after ':'.
 *  -# Creates @c initialServers WebServer objects for each load balancer.
 *  -# Populates each balancer's initial queue with @c initialServers*100 requests
 *     whose processing times are uniformly random in [1, maxProcessingTime].
 *  -# Constructs and runs the Switch for @c clockCycles ticks.
 *
 * @return 0 on success.
 */
int main() {
    std::queue<Request> requestQueue_P;
    std::queue<Request> requestQueue_S;
    std::vector<WebServer> webServers_P;
    std::vector<WebServer> webServers_S;

    std::ofstream logFile("loadBalancer.log", std::ios::out | std::ios::trunc);
    std::ifstream configFile("config.txt");
    
    std::string intialServersLine, minThresholdLine, maxThresholdLine, cooldownTimeLine, maxProcessingTimeLine, clockCyclesLine;
    
    std::getline(configFile, intialServersLine);
    int initialServers = std::stoi(intialServersLine.substr(intialServersLine.find(":") + 1));
    logFile << "Initial Servers: " << initialServers << std::endl;

    std::getline(configFile, clockCyclesLine);
    int clockCycles = std::stoi(clockCyclesLine.substr(clockCyclesLine.find(":") + 1));
    logFile << "Clock Cycles: " << clockCycles << std::endl;

    std::getline(configFile, minThresholdLine);
    int minThreshold = std::stoi(minThresholdLine.substr(minThresholdLine.find(":") + 1));
    logFile << "Min Threshold: " << minThreshold << std::endl;

    std::getline(configFile, maxThresholdLine);
    int maxThreshold = std::stoi(maxThresholdLine.substr(maxThresholdLine.find(":") + 1));
    logFile << "Max Threshold: " << maxThreshold << std::endl;

    std::getline(configFile, cooldownTimeLine);
    int cooldownTime = std::stoi(cooldownTimeLine.substr(cooldownTimeLine.find(":") + 1));
    logFile << "Cooldown Time: " << cooldownTime << std::endl;

    std::getline(configFile, maxProcessingTimeLine);
    int maxProcessingTime = std::stoi(maxProcessingTimeLine.substr(maxProcessingTimeLine.find(":") + 1));
    logFile << "Max Processing Time: " << maxProcessingTime << std::endl;

    logFile << std::endl;

    for (int i = 0; i < initialServers; i++) {
        webServers_P.push_back(WebServer(i));
        webServers_S.push_back(WebServer(i + initialServers));
    }

    for (int i = 0; i < initialServers*100; i++) {
        int processTime = rand() % maxProcessingTime + 1;
        char jobType = 'P';
        Request newRequest = Request(processTime, jobType);
        requestQueue_P.push(newRequest);
    }

    for (int i = 0; i < initialServers*100; i++) {
        int processTime = rand() % maxProcessingTime + 1;
        char jobType = 'S';
        Request newRequest = Request(processTime, jobType);
        requestQueue_S.push(newRequest);
    }

    Switch switch_(requestQueue_P, requestQueue_S, webServers_P, webServers_S, minThreshold, maxThreshold, cooldownTime, maxProcessingTime);
    switch_.run(clockCycles, logFile);

    return 0;
}
