#ifndef REQUEST_H
#define REQUEST_H

#include <string>

class Request {
    public:
        Request();
        Request(int processTime, char jobType);

        std::string getIPin() const;
        std::string getIPout() const;
        int getProcessTime() const;
        char getJobType() const;

    private:
        std::string IPin;
        std::string IPout;
        int processTime;
        char jobType;

        std::string generateIP();
};

#endif