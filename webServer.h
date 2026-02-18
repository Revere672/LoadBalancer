#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "request.h"

class WebServer {
    public:
        WebServer(int id);

        void processRequest(const Request& request);
        void update();
        bool isReady() const;

        int getId() const;

    private:
        int id;
        bool isAvailable;
        Request currentRequest;
        int timeRemaining;
};

#endif