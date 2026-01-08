#ifndef ASYNC_CONSOLE_H
#define ASYNC_CONSOLE_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>

/**
 * DEBUG_MODE:
 * - หากไม่ได้ define ไว้ โค้ดทั้งหมดจะถูกตัดออกในขั้นตอน compile
 * - คุณสามารถเพิ่ม #define DEBUG_MODE ลงใน main.cpp หรือ platformio.ini ได้
 */

// --- Configure Colors Here ---
#define CONSOLE_COLOR_DEBUG "\033[0;36m"    // Cyan
#define CONSOLE_COLOR_INFO "\033[0;32m"     // Green
#define CONSOLE_COLOR_WARN "\033[0;33m"     // Yellow
#define CONSOLE_COLOR_ERROR "\033[0;31m"    // Red
#define CONSOLE_COLOR_PROGRESS "\033[0;35m" // Magenta
// -----------------------------

typedef enum {
  LOG_LEVEL_DEBUG,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_RAW
} LogLevel_t;

// โครงสร้างข้อมูลสำหรับส่งเข้า Queue
typedef struct {
  LogLevel_t level;
  uint32_t timestamp;
  char taskName[16];
  char *message; // Dynamic allocation สำหรับข้อความ
} LogMessage_t;

class AsyncConsole {
public:
  static bool begin(uint32_t baudRate = 115200, bool useColor = true,
                    size_t queueSize = 50, Stream *serial = &Serial);
  static void log(LogLevel_t level, const char *format, ...);
  static void progress(const char *label, int percent, int width = 20);
  static void flush();

private:
  static QueueHandle_t _logQueue;
  static bool _useColor;
  static Stream *_serial;
  static void _loggerTask(void *pvParameters);
  static const char *_levelToString(LogLevel_t level);
  static const char *_levelToColor(LogLevel_t level);
};

#ifdef DEBUG_MODE

// Macros สำหรับการใช้งานที่ง่ายและ plug & play
#define CONSOLE_INIT(...) AsyncConsole::begin(__VA_ARGS__)
#define CONSOLE_DEBUG(format, ...)                                             \
  AsyncConsole::log(LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)
#define CONSOLE_INFO(format, ...)                                              \
  AsyncConsole::log(LOG_LEVEL_INFO, format, ##__VA_ARGS__)
#define CONSOLE_LOG(format, ...)                                               \
  AsyncConsole::log(LOG_LEVEL_RAW, format "\n", ##__VA_ARGS__)
#define CONSOLE_WARN(format, ...)                                              \
  AsyncConsole::log(LOG_LEVEL_WARN, format, ##__VA_ARGS__)
#define CONSOLE_ERROR(format, ...)                                             \
  AsyncConsole::log(LOG_LEVEL_ERROR, format, ##__VA_ARGS__)
#define CONSOLE_PRINT(format, ...)                                             \
  AsyncConsole::log(LOG_LEVEL_RAW, format, ##__VA_ARGS__)
#define CONSOLE_PROGRESS(label, percent, ...)                                  \
  AsyncConsole::progress(label, percent, ##__VA_ARGS__)
#define CONSOLE_FLUSH() AsyncConsole::flush()

// สำหรับผู้ที่ยังติดการใช้ LOG_ (Aliases)
#define LOG_INIT CONSOLE_INIT
#define LOG_D CONSOLE_DEBUG
#define LOG_I CONSOLE_INFO
#define LOG_W CONSOLE_WARN
#define LOG_E CONSOLE_ERROR
#define LOG_PRINT CONSOLE_PRINT
#define LOG_PROGRESS CONSOLE_PROGRESS

#else // หากไม่ได้เปิด DEBUG_MODE ให้ Macros เป็นโค้ดว่าง

#define CONSOLE_INIT(...) ((void)0)
#define CONSOLE_DEBUG(format, ...) ((void)0)
#define CONSOLE_INFO(format, ...) ((void)0)
#define CONSOLE_LOG(format, ...) ((void)0)
#define CONSOLE_WARN(format, ...) ((void)0)
#define CONSOLE_ERROR(format, ...) ((void)0)
#define CONSOLE_PRINT(format, ...) ((void)0)
#define CONSOLE_PROGRESS(label, percent, ...) ((void)0)
#define CONSOLE_FLUSH() ((void)0)

#define LOG_INIT(...) ((void)0)
#define LOG_D(format, ...) ((void)0)
#define LOG_I(format, ...) ((void)0)
#define LOG_W(format, ...) ((void)0)
#define LOG_E(format, ...) ((void)0)
#define LOG_PRINT(format, ...) ((void)0)
#define LOG_PROGRESS(label, percent, ...) ((void)0)

#endif // DEBUG_MODE

#endif // ASYNC_CONSOLE_H
