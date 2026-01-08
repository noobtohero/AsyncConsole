# AsyncConsole: FreeRTOS Non-blocking Logger for ESP32

Library สำหรับการทำ Logging บน ESP32 ที่ทำงานภายใต้ FreeRTOS โดยไม่ขัดจังหวะการทำงานของ Task หลัก (Non-blocking) และสามารถเลือกที่จะไม่นำโค้ดเข้ากระบวนการ Compile ได้หากไม่ได้อยู่ในโหมด Debug

## คุณสมบัติเด่น

- **Non-blocking**: ใช้ Queue ในการรับ Log และใช้ Task แยกในการส่งออก Serial (ไม่หน่วง Loop หลัก)
- **Plug & Play**: แค่ `#include` และ `#define DEBUG_MODE` ก็ใช้งานได้ทันที
- **Zero Overhead**: หากไม่นิยาม `DEBUG_MODE` โค้ด Logger ทั้งหมดจะถูกตัดออกทันที (ประหยัด Flash/RAM 100%)
- **Log Levels**: รองรับ DEBUG, INFO, WARN, ERROR พร้อมสีสันสวยงาม (ANSI Colors)
- **Console Style**: ใช้คำสั่งแบบ `CONSOLE_` เพื่อความทันสมัยและสื่อความหมายชัดเจน
- **Cool Progress Bar**: แสดงแถบความคืบหน้าแบบ Dynamic ด้วยคำสั่งเดียว
- **Metadata**: แสดง Timestamp และชื่อ Task ที่เรียกใช้งานให้อัตโนมัติ
- **Safe**: มีระบบป้องกัน Queue เต็ม โดยจะข้าม Log ใหม่ทันทีเพื่อไม่ให้ระบบหลักค้าง

## วิธีการใช้งาน

### 1. การติดตั้ง

ก๊อปปี้โฟลเดอร์ `AsyncConsole` ไปไว้ในโฟลเดอร์ `lib` ของโปรเจกต์ PlatformIO

### 2. การเรียกใช้งานในโค้ด

```cpp
#define DEBUG_MODE // ต้องใส่ไว้ก่อน include หรือกำหนดใน platformio.ini
#include "AsyncConsole.h"

void setup() {
    // 1. เริ่มต้นระบบ (Baudrate, UseColor, QueueSize)
    CONSOLE_INIT(115200); 
    
    // 2. แสดง Progress Bar แบบเท่ๆ
    for (int i = 0; i <= 100; i += 10) {
        CONSOLE_PROGRESS("System Boot", i);
        delay(100);
    }

    CONSOLE_INFO("System started successfully!");
}

void loop() {
    CONSOLE_DEBUG("Checking sensor... value: %d", analogRead(34));
    
    if (analogRead(34) > 3000) {
        CONSOLE_WARN("High voltage detected!");
    }
    
    delay(2000);
}
```

### 3. คำสั่งที่รองรับ

- `CONSOLE_INIT(baud, useColor=true, queueSize=50)` : เริ่มการทำงาน
- `CONSOLE_DEBUG(format, ...)` : Log ระดับ Debug (สีฟ้า)
- `CONSOLE_INFO(format, ...)` : Log ระดับ Info (มีหัวข้อ [INFO] และสีเขียว)
- `CONSOLE_LOG(format, ...)` : พิมพ์ข้อความแบบ Clean (ไม่มีหัวข้อ ไม่มีสี และขึ้นบรรทัดใหม่)
- `CONSOLE_WARN(format, ...)` : Log ระดับ Warning (สีเหลือง)
- `CONSOLE_ERROR(format, ...)` : Log ระดับ Error (สีแดง)
- `CONSOLE_PRINT(format, ...)` : พิมพ์ข้อความตรงๆ (Raw) ไม่มี Header และไม่ขึ้นบรรทัดใหม่
- `CONSOLE_PROGRESS(label, percent)` : แสดงแถบความคืบหน้า (Progress Bar) แบบวิ่งอยู่ที่เดิม
- `CONSOLE_FLUSH()` : รอจนกว่า Log ในคิวจะถูกพิมพ์ออกมาจนหมด (มีประโยชน์ก่อนสั่ง Reboot หรือ Deep Sleep)

## การตั้งค่าสีส่วนตัว (Customization)

คุณสามารถเปลี่ยนสีของแต่ละระดับได้โดยตรงในไฟล์ `AsyncConsole.h`:

```cpp
// d:\esp32\esp32-logger\lib\AsyncConsole\AsyncConsole.h
#define CONSOLE_COLOR_DEBUG    "\033[0;36m" 
#define CONSOLE_COLOR_INFO     "\033[0;32m" 
...
```

## การตั้งค่าสี

หาก Terminal ของคุณไม่รองรับสีและแสดงเป็นตัวอักษรแปลกๆ ให้ปิดสีได้โดย:

```cpp
CONSOLE_INIT(115200, false);
```

### การเลือกใช้ Serial อื่น (เช่น Serial2)

คุณสามารถระบุ Serial Port ที่ต้องการได้ในการ Init:

```cpp
Serial2.begin(115200);
CONSOLE_INIT(115200, true, 50, &Serial2);
```

## การตัดโค้ดออกเมื่อ Production (ประหยัดพื้นที่)

หากต้องการให้โค้ด Logger หายไปจากโปรแกรมทันทีเมื่อเสร็จสิ้นการพัฒนา ให้ทำการคอมเมนต์บรรทัด:
`#define DEBUG_MODE`
หรือหากกำหนดไว้ใน `platformio.ini` ให้ลบออกจาก `build_flags` ครับ
