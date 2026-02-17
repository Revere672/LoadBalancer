#include <string>
#include <cstdlib>

class Request {
    public:
        std::string IPin;
        std::string IPout;
        int processTime;
        char jobType;

        Request(int processTime, char jobType) {
            this->IPin = generateIP();
            this->IPout = generateIP();
            this->processTime = processTime;
            this->jobType = jobType;
        }

        std::string generateIP() {
            std::string IP = "";

            for (int i = 0; i < 4; i++) {
                IP += std::to_string(rand() % 256);
                if (i < 3) {
                    IP += ".";
                }
            }

            return IP;
        }

        std::string getIPin() {
            return this->IPin;
        }

        int getProcessTime() {
            return this->processTime;
        }

        char getJobType() {
            return this->jobType;
        }
};
