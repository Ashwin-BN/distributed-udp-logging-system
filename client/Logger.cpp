/**
 * @file client/Logger.cpp
 * @brief Implementation of the distributed logging system
 * @author Ashwin B N 
*/

#include "Logger.h"
#include <cstdio>
#include <cstring>
#include <ctime>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <mutex>
#include <thread>
#include <atomic>
#include <cstdlib>

// Global static variables for maintaining logging system state
static int sock_fd;                    ///< Socket file descriptor for UDP communication
static struct sockaddr_in server_addr;  ///< Server address configuration
static LOG_LEVEL current_log_level = DEBUG;  ///< Current minimum log level threshold
static std::mutex log_mutex;           ///< Mutex for protecting shared resources
static std::atomic<bool> is_running(true);  ///< Flag indicating system operation status
static std::thread recv_thread;        ///< Thread handling incoming server commands

/**
 * Thread function receiving server commands
 * Continuously monitors for server updates to log level settings
 * @param fd Socket file descriptor for communication
 */
void receiveFunction(int fd) {
    char buf[BUF_LEN];
    
    while (is_running) {
        memset(buf, 0, BUF_LEN);
        // Receive potential server commands
        int bytes = recvfrom(fd, buf, BUF_LEN, 0, nullptr, nullptr);

        if (bytes > 0) {
            // Process log level update command
            if (strncmp(buf, "Set Log Level=", 14) == 0) {
                int level = atoi(buf + 14);
        
                if (level >= DEBUG && level <= CRITICAL) {
                    std::lock_guard<std::mutex> lock(log_mutex);
                    current_log_level = (LOG_LEVEL)level;
                }
            }
        } else {
            sleep(1);
        }
    }
}

/**
 * Initializes the logging system
 * Creates UDP socket and sets up server connection
 * @return 0 on success, negative value on failure
 */
int InitializeLog() {
    // Create non-blocking UDP socket
    sock_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if (sock_fd < 0) {
        perror("socket creation failed");
        return -1;
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    is_running = true;

    // Create receive thread for server commands
    recv_thread = std::thread(receiveFunction, sock_fd);
    if (!recv_thread.joinable()) {
        perror("Failed to create receive thread");
        return -1;
    }
    return 0;
}

/**
 * Sets the current log level threshold
 * @param level New log level to apply
 */
void SetLogLevel(LOG_LEVEL level) {
    std::lock_guard<std::mutex> lock(log_mutex);
    current_log_level = level;
}

/**
 * Logs a message with specified severity
 * Includes timestamp, program information, and message details
 * @param level Log severity level
 * @param prog Program filename
 * @param func Function name where log was generated
 * @param line Line number where log was generated
 * @param message Message to be logged
 */
void Log(LOG_LEVEL level, const char *prog, const char *func, int line, const char *message) {
    if (level >= current_log_level) {

    // Get current timestamp
    time_t now = time(0);
    char *dt = ctime(&now);
    
    // Prepare log message buffer
    char buf[BUF_LEN];
    const char *levelStr[] = {"DEBUG", "WARNING", "ERROR", "CRITICAL"};

    // Format log message 
    int len = snprintf(buf, BUF_LEN, "%s %s %s:%s:%d %s", dt, levelStr[level], prog, func, line, message) + 1;
    buf[len - 1] = '\0';

    // Send message to server with thread safety
    std::lock_guard<std::mutex> lock(log_mutex);
    sendto(sock_fd, buf, len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr)); // changes may needed
    }
}

/**
 * Cleans up logging resources during shutdown
 */
void ExitLog() {
    is_running = false;
    
    if (recv_thread.joinable())
        recv_thread.join();

    close(sock_fd);
}