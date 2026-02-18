#include "webServer.h"

WebServer::WebServer(int id) {
    this->id = id;
    this->isAvailable = true;
    this->timeRemaining = 0;
}

void WebServer::processRequest(const Request& request) {
    currentRequest = request;
    isAvailable = false;
    timeRemaining = request.getProcessTime();
}

void WebServer::update() {
    if (!isAvailable) {
        timeRemaining--;

        if (timeRemaining <= 0) {
            isAvailable = true;
        }
    }
}

bool WebServer::isReady() const {
    return isAvailable;
}

int WebServer::getId() const {
    return id;
}