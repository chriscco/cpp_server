#include "../inc/error_handler.h"

void errif(bool condition, const char* message) {
    if (condition) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}