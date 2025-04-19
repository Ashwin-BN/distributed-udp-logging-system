/**
 * @file client/Logger.h
 * @brief Header file for the distributed logging system
 * @author Ashwin B N
 */

 #ifndef LOGGER_H
 #define LOGGER_H
 
 #include <pthread.h>
 
 // Maximum length for log messages
 #define BUF_LEN 1024
 
 // Default server settings
 #define SERVER_IP "127.0.0.1"
 #define SERVER_PORT 9999
 
 /**
  * @enum LOG_LEVEL
  * @brief Defines the different severity levels for logging
  */
 typedef enum {
     DEBUG = 0,        /// Debug-level messages
     WARNING = 1,      /// Warning-level messages
     ERROR = 2,        /// Error-level messages
     CRITICAL = 3      /// Critical-level messages
 } LOG_LEVEL;
 
 /**
  * @brief Initializes the logging system
  * @return 0 on success, negative value on failure
  */
 int InitializeLog();
 
 /**
  * @brief Sets the minimum log level for filtering
  * @param level The minimum severity level to log
  */
 void SetLogLevel(LOG_LEVEL level);
 
 /**
  * @brief Logs a message with the specified parameters
  * @param level The severity level of the message
  * @param prog The source file name (__FILE__)
  * @param func The source function name (__func__)
  * @param line The source line number (__LINE__)
  * @param message The actual log message
  */
 void Log(LOG_LEVEL level, const char *prog, const char *func, int line, const char *message);
 
 /**
  * @brief Cleans up logging resources
  */
 void ExitLog();
 
 #endif // LOGGER_H