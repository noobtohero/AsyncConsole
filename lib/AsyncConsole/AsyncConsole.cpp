#include "AsyncConsole.h"
#include <stdarg.h>

QueueHandle_t AsyncConsole::_logQueue = NULL;
bool AsyncConsole::_useColor = true;
Stream *AsyncConsole::_serial = &Serial;

bool AsyncConsole::begin(uint32_t baudRate, bool useColor, size_t queueSize,
                         Stream *serial) {
  if (_logQueue != NULL)
    return true; // Already initialized

  _useColor = useColor;
  _serial = serial;

  // ตรวจสอบว่าเป็น HardwareSerial หรือไม่ เพื่อเรียก begin
  if (_serial == &Serial) {
    if (!Serial)
      Serial.begin(baudRate);
  }

  _logQueue = xQueueCreate(queueSize, sizeof(LogMessage_t));
  if (_logQueue == NULL)
    return false;

  // สร้าง Task สำหรับดึง Log ออกมาพิมพ์
  // Priority ต่ำกว่า Main Task (1) หรือเท่ากันเพื่อไม่ให้ขวางงานสำคัญ
  xTaskCreate(_loggerTask, "logger_task", 4096, NULL, 1, NULL);

  return true;
}

void AsyncConsole::log(LogLevel_t level, const char *format, ...) {
  if (_logQueue == NULL)
    return;

  // เตรียม Metadata
  LogMessage_t msg;
  msg.level = level;
  msg.timestamp = millis();

  // ดึงชื่อ Task ปัจจุบัน
  const char *tName = pcTaskGetName(NULL);
  if (tName) {
    strncpy(msg.taskName, tName, sizeof(msg.taskName) - 1);
    msg.taskName[sizeof(msg.taskName) - 1] = '\0';
  } else {
    strcpy(msg.taskName, "unknown");
  }

  // จัดการข้อความ (Dynamic Allocation)
  va_list args;
  va_start(args, format);

  // หาความยาวที่ต้องใช้
  int len = vsnprintf(NULL, 0, format, args);
  va_end(args);

  if (len > 0) {
    msg.message = (char *)malloc(len + 1);
    if (msg.message) {
      va_start(args, format);
      vsnprintf(msg.message, len + 1, format, args);
      va_end(args);

      // ส่งเข้า Queue แบบ Non-blocking (ticksToWait = 0)
      if (xQueueSend(_logQueue, &msg, 0) != pdPASS) {
        // ถ้า Queue เต็ม ให้คืน Memory ทันที (Drop log)
        free(msg.message);
      }
    }
  }
}

void AsyncConsole::_loggerTask(void *pvParameters) {
  LogMessage_t msg;
  while (1) {
    if (xQueueReceive(_logQueue, &msg, portMAX_DELAY) == pdPASS) {
      if (msg.level == LOG_LEVEL_RAW) {
        // พิมพ์ข้อความตรงๆ ไม่ใส่ Header และไม่ขึ้นบรรทัดใหม่
        if (_serial)
          _serial->print(msg.message);
      } else {
        // พิมพ์ Log ในรูปแบบ: [TIME] [LEVEL] [TASK] message
        if (_useColor && _serial) {
          _serial->printf("%s[%10u] [%s] [%s] %s\033[0m\n",
                          _levelToColor(msg.level), msg.timestamp,
                          _levelToString(msg.level), msg.taskName, msg.message);
        } else if (_serial) {
          _serial->printf("[%10u] [%s] [%s] %s\n", msg.timestamp,
                          _levelToString(msg.level), msg.taskName, msg.message);
        }
      }

      // คืน Memory หลังจากพิมพ์เสร็จ
      free(msg.message);
    }
  }
}

void AsyncConsole::progress(const char *label, int percent, int width) {
  if (percent < 0)
    percent = 0;
  if (percent > 100)
    percent = 100;

  int pos = (width * percent) / 100;
  char bar[width + 1];
  for (int i = 0; i < width; i++) {
    bar[i] = (i < pos) ? '=' : ' ';
  }
  bar[width] = '\0';

  if (percent == 100) {
    log(LOG_LEVEL_RAW, "\r%s%s: [%s] %d%%\033[0m [COMPLETED]\n",
        CONSOLE_COLOR_PROGRESS, label, bar, percent);
  } else {
    log(LOG_LEVEL_RAW, "\r%s%s: [%s] %d%%\033[0m", CONSOLE_COLOR_PROGRESS,
        label, bar, percent);
  }
}

void AsyncConsole::flush() {
  if (_logQueue == NULL)
    return;
  // รอจนกว่า Queue จะว่าง
  while (uxQueueMessagesWaiting(_logQueue) > 0) {
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  // ให้เวลางาน Serial พิมพ์ออกไปจริงๆ
  if (_serial)
    _serial->flush();
}

const char *AsyncConsole::_levelToString(LogLevel_t level) {
  switch (level) {
  case LOG_LEVEL_DEBUG:
    return "DEBUG";
  case LOG_LEVEL_INFO:
    return "INFO ";
  case LOG_LEVEL_WARN:
    return "WARN ";
  case LOG_LEVEL_ERROR:
    return "ERROR";
  default:
    return "UNKN ";
  }
}

const char *AsyncConsole::_levelToColor(LogLevel_t level) {
  switch (level) {
  case LOG_LEVEL_DEBUG:
    return CONSOLE_COLOR_DEBUG;
  case LOG_LEVEL_INFO:
    return CONSOLE_COLOR_INFO;
  case LOG_LEVEL_WARN:
    return CONSOLE_COLOR_WARN;
  case LOG_LEVEL_ERROR:
    return CONSOLE_COLOR_ERROR;
  default:
    return "";
  }
}
