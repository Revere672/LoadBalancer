#include "utils.h"

Request generateRequest(int processTime, char jobType) {
    return Request(processTime, jobType);
}