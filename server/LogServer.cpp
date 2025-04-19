/**
 * @file server/LogServer.cpp
 * @brief Implementation of the log server
 * @author Ashwin B N
*/
  
#include <iostream>
#include <fstream>
#include <cstring>
#include <thread>
#include <mutex>
#include <atomic>
#include <csignal>
#include <unistd.h>
#include <arpa/inet.h>

// Constants
#define BUF_LEN 1024            /// Maximum buffer length for messages
#define SERVER_PORT 9999        /// Server listening port
#define LOG_FILE "server.log"   /// Log file path
#define SERVER_IP "127.0.0.1"   /// Localhost IP address

// Global resources shared between threads
std::mutex log_mutex;           ///< Mutex protecting concurrent log operations
std::atomic<bool> is_running(true);  ///< Flag controlling server operation
int sock_fd;                    ///< Socket file descriptor for UDP communication
struct sockaddr_in client_addr;  ///< Client address structure
socklen_t addrlen = sizeof(client_addr);

/**
 * Signal handler for graceful server shutdown
 * @param sig Signal number received (typically SIGINT from Ctrl+C)
 */
void shutdownHandler(int sig) {
    if (is_running) {
        printf("\nShutting down...\n");
        is_running = false;
    }
}

/**
 * Thread function handling incoming log messages
 * Runs continuously until server shutdown signal received
 */
void receiveThreadFunc() {
    char buffer[BUF_LEN];
    
    // Initialize log file for appending
    std::ofstream logFile(LOG_FILE, std::ios::app);
    if (!logFile) {
        std::cerr << "Cannot open log file.\n";
        return;
    }

    // Continuous loop until shutdown signal received
    while (is_running) {
        memset(buffer, 0, BUF_LEN);

        // Receive incoming log messages
        int bytes = recvfrom(sock_fd, buffer, BUF_LEN, 0, (struct sockaddr *)&client_addr, &addrlen) - 1;

        if (bytes > 0) {
            std::lock_guard<std::mutex> lock(log_mutex);
            logFile << buffer << std::endl;
        } else {
            sleep(1);
        }
    }
    logFile.close();
}

/**
 * Dumps contents of the log file to console
 */
void dump_log() {
    std::ifstream infile(LOG_FILE);
    if (!infile) {
        perror("Can't open log file");
        return;
    }
    printf("\n--- Log File Content ---\n");
    std::string line;
    while (getline(infile, line))
        std::cout << line << std::endl;

    printf("\nPress ENTER to continue...");
    std::cin.get();
}

int main() {
    // Set up signal handler for SIGINT
    signal(SIGINT, shutdownHandler);

    // Create UDP socket
    sock_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if (sock_fd < 0) {
        perror("socket creation failed");
        return -1;
    }

    // Configure server address
    struct sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        close(sock_fd);
        return -1;
    }

    // Bind socket to address
    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    // Create and start receive thread
    std::thread recvThread(receiveThreadFunc);

    // Main server loop
    while (is_running) {
        system("clear");  // Clear terminal screen
        printf("╔════════════════════════════════╗\n");
        printf("║         Log Server Menu        ║\n");
        printf("╠════════════════════════════════╣\n");
        printf("║ 1. Set the log level           ║\n");
        printf("║ 2. Dump the log file here      ║\n");
        printf("║ 0. Shut down                   ║\n");
        printf("╚════════════════════════════════╝\n");
        printf("Enter selection: ");
        int option, level;
        scanf("%d", &option);
        getchar();

        switch (option) {
            case 1:
                // Set log level
                printf("Enter log level (0-DEBUG, 1-WARNING, 2-ERROR, 3-CRITICAL): ");
                scanf("%d", &level);
                getchar();
                
                if (level < 0 || level > 3) {
                    printf("Incorrect log level !\n");
                    sleep(1);
                }
                else {
                    char buffer[BUF_LEN];
                    snprintf(buffer, BUF_LEN, "Set Log Level=%d", level);
                    sendto(sock_fd, buffer, strlen(buffer) + 1, 0, (struct sockaddr*)&client_addr, addrlen);
                    sleep(1);
                }
                break;
            case 2: {
                // Dump log file contents
                dump_log();
                break;
            }
            case 0:
                // Initiate shutdown
                is_running = false;
                break;
            default:
                printf("Invalid selection. Try again.\n");
        }
    }

    // Clean up resources
    recvThread.join();
    close(sock_fd);
    printf("\nLog Server exited successfully :)\n");
    return 0;
}