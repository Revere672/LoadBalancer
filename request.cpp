#include "request.h"
#include <cstdlib>

Request::Request() {
    IPin = "";
    IPout = "";
    processTime = 0;
    jobType = 'P';
}

Request::Request(int processTime, char jobType) {
    this->IPin = generateIP();
    this->IPout = generateIP();
    this->processTime = processTime;
    this->jobType = jobType;
}

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

std::string Request::getIPin() const {
    return IPin;
}

std::string Request::getIPout() const {
    return IPout;
}

int Request::getProcessTime() const {
    return processTime;
}

char Request::getJobType() const {
    return jobType;
}
