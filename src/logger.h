#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

/* Minimal structured logging for the MVP; swap for a real sink in production */
#define LOG_INFO(fmt, ...)  (void)printf("[INFO] " fmt "\n", ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) (void)printf("[ERROR] " fmt "\n", ##__VA_ARGS__)

#endif /* LOGGER_H */
