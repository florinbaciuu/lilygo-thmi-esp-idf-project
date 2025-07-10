# 🧠 ESP32_Resource_Monitor

![PlatformIO](https://img.shields.io/badge/PlatformIO-Ready-orange?logo=platformio)
![ESP-IDF](https://img.shields.io/badge/ESP--IDF-5.x-blue?logo=espressif)

**Advanced resource monitor for ESP32**  
📟 Tracks tasks, memory, PSRAM, heap, timers, and more, with detailed configuration options via Kconfig.  
✍️ Created by [Baciu Aurel Florin](mailto:baciuaurelflorin@gmail.com) (a.k.a. The Lightbringer)

---

## ✨ Features

- ✅ **CPU usage per task** (load, stack usage, core ID, priority)
- ✅ Advanced **heap and PSRAM dump**
- ✅ **Segmented stats** (DMA, RTCRAM, 8bit/32bit, etc.)
- ✅ Supports **ESP timer dump**
- ✅ Optional: **full memory heap dump** (`heap_caps_dump()`)
- ✅ Debug mode with **timestamp and benchmarking**
- ✅ **Full Kconfig** configuration
- ✅ Runs using:
  - Dedicated task (with notify)
  - Directly from `esp_timer` (with or without task)
  - Pure FreeRTOS mode (`vTaskDelayUntil`) 
- ✅ ESP-IDF v5.x+ compatible

---

## 📂 Structure

```text
ESP32S3_Resource_Monitor/
├── CMakeLists.txt
├── Kconfig
├── include/
│   └── ResourceMonitor.h
├── src/
│   └── ResourceMonitor.c
└── README.md
```

---

## ⚙️ Kconfig Options

Component has a dedicated menu:

```menuconfig
Component config  --->
  ESP32S3 Resource Monitor Configuration  --->
    [*] Enable Resource Monitor
    [*] Use Kconfig for configuration
        (4)  Dump interval (seconds)
        (40) Max number of monitored tasks
        [*] Include timestamp in logs
        [*] Enable task monitoring output
        [*] Enable memory usage dump
        [*] Use printf instead of ESP_LOGI for memory info
        [ ] Enable full heap memory dump
        [*] Enable ESP timer diagnostics
        [*] Use ESP timer for triggering resource monitor
        [ ] Run monitor directly inside ESP timer
```

---

## 🛠️ Usage

### 1. **Include the component:**

In your `CMakeLists.txt`:
```cmake
set(EXTRA_COMPONENT_DIRS "<path>/ESP32S3_Resource_Monitor")
```

### 2. **Use in your code:**

```c
#include "ResourceMonitor.h"

void app_main() {
    start_resource_monitor();  // Start the monitor
}
```

---

## 🧪 Sample Output

📌 Real console output:

```
I (652) MY-RESOURCE-MONITOR (BY Florin Baciu): ESPResourceMonitor v1.0.0 initialized
I (5647) MY-RESOURCE-MONITOR (BY Florin Baciu): Timestamp: [1970-01-01 02:38:50]
I (5647) MY-RESOURCE-MONITOR (BY Florin Baciu): -----------------Task Dump Start-----------------

Load    Stack   State   CoreID  PRIO    Name
0.01    1744    Running 1       3       [RESMONITOR(R)   ] 
98.56   2484    Ready   0       0       [IDLE0           ] 
92.65   2492    Ready   1       0       [IDLE1           ] 
0.21    2408    Blocked 1       24      [LVGL Tick Task  ] 
0.05    2196    Suspend 0       19      [ipc0            ] 
0.01    1996    Suspend 1       19      [v_check_0_pin_s ] 
0.08    6704    Suspend 1/2     3       [swdraw          ] 
0.57    6648    Suspend 1/2     3       [swdraw          ] 
0.00    3860    Suspend 0       22      [esp_timer       ] 
7.00    3616    Suspend 1       22      [LVGL Main Task  ] 
0.05    2332    Suspend 1       24      [ipc1            ] 

I (5651) MY-RESOURCE-MONITOR (BY Florin Baciu): -----------------Task Dump End-------------------
I (5651) MY-RESOURCE-MONITOR (BY Florin Baciu): -----------------Memory Info Start---------------

╔═══════════════════════════════ MEMORY STATS ═════════════════════════════╗
║   Segment     │    Total     │    Used     │   Free      │ Utilizare %   ║
╟───────────────┼──────────────┼─────────────┼─────────────┼────────────────╢
║ RAM          │   274.2 KB  │    73.8 KB │   200.4 KB │    26.93 %     ║
║ RAM-DMA      │   266.5 KB  │    73.4 KB │   193.1 KB │    27.56 %     ║
║ RAM 8 bit    │   274.2 KB  │    73.8 KB │   200.4 KB │    26.93 %     ║
║ RAM 32 bit   │   274.2 KB  │    73.8 KB │   200.4 KB │    26.93 %     ║
║ RTC RAM      │     7.7 KB  │     0.4 KB │     7.3 KB │     4.97 %     ║
║ PSRAM        │  7104.0 KB  │  1363.1 KB │  5740.9 KB │    19.19 %     ║
║ PSRAM 8 bit  │  7104.0 KB  │  1363.1 KB │  5740.9 KB │    19.19 %     ║
║ PSRAM 32 bit │  7104.0 KB  │  1363.1 KB │  5740.9 KB │    19.19 %     ║
╚══════════════════════════════════════════════════════════════════════════╝


I (5656) MY-RESOURCE-MONITOR (BY Florin Baciu): -----------------Memory Info End-----------------
I (5656) MY-RESOURCE-MONITOR (BY Florin Baciu): -----------------ESP Timer Dump Start------------

Timer stats:
Name                  Period      Alarm         Times_armed   Times_trigg   Times_skip    Cb_exec_time
res_mon_timer         5000000     10159844      1             1             0             10

I (5657) MY-RESOURCE-MONITOR (BY Florin Baciu): -----------------ESP Timer Dump End--------------
I (5658) RESMON_BENCH: resource_monitor_callback() execution time: 10648 µs (10.65 ms)
I (10647) RESMON_BENCH: Δ timp între apeluri: 5000000 µs (5.00 sec)
I (10647) MY-RESOURCE-MONITOR (BY Florin Baciu): Timestamp: [1970-01-01 02:38:55]
```

---

## 🧩 Extensibility

Callback functions are modular:
- `task_monitor_cb()`
- `memory_monitor_cb()`
- `esp_timers_monitor_cb()`
- `memory_dump_cb()`

You can invoke them independently or integrate into custom diagnostics.

---

## 🔐 License

Apache 2.0 — use freely, contribute confidently.

---

## 🤝 Contribute

Pull requests, feedback, and enhancements are welcome!  
Please follow the existing code style and provide comments.

---

## 📧 Contact

📮 Author: Baciu Aurel Florin  
✉️ Email: [baciuaurelflorin@gmail.com](mailto:baciuaurelflorin@gmail.com)

---

## 🧙‍♂️ The Lightbringer Rulez

This isn't just a library. It's your eyes inside the system.  
The heresy of idle tasks shall burn in the fire of RAM.

---

# 🧠 ESP32_Resource_Monitor (în română)

**Monitor avansat de resurse pentru ESP32**  
📟 Monitorizează taskuri, memorie, PSRAM, heap, timere și mai mult, cu opțiuni detaliate de configurare via Kconfig.  
✍️ Creat de [Baciu Aurel Florin](mailto:baciuaurelflorin@gmail.com) (a.k.a. The Lightbringer)

---

## ✨ Caracteristici

- ✅ Monitorizare **CPU per task** (load, stack usage, core ID, prioritate)
- ✅ Dump avansat de **heap și PSRAM**
- ✅ Statistici **segmentate** (DMA, RTCRAM, 8bit/32bit, etc.)
- ✅ Suport pentru **ESP timer dump**
- ✅ Opțional: **memory heap dump complet** (`heap_caps_dump()`)
- ✅ Mod debug cu **timestamp și benchmark**
- ✅ **Kconfig complet** pentru configurare ușoară
- ✅ Suport pentru rulare în:
  - Task dedicat (cu notificare)
  - Direct în `esp_timer` (cu sau fără task)
  - Mod FreeRTOS pur (`vTaskDelayUntil`)
- ✅ Compatibil ESP-IDF v5.x+

---

## 📂 Structură

```text
ESP32S3_Resource_Monitor/
├── CMakeLists.txt
├── Kconfig
├── include/
│   └── ResourceMonitor.h
├── src/
│   └── ResourceMonitor.c
└── README.md
```

---

## ⚙️ Configurare Kconfig

Librăria vine cu un meniu Kconfig dedicat:

```menuconfig
Component config  --->
  ESP32S3 Resource Monitor Configuration  --->
    [*] Enable Resource Monitor
    [*] Use Kconfig for configuration
        (4)  Dump interval (seconds)
        (40) Max number of monitored tasks
        [*] Include timestamp in logs
        [*] Enable task monitoring output
        [*] Enable memory usage dump
        [*] Use printf instead of ESP_LOGI for memory info
        [ ] Enable full heap memory dump
        [*] Enable ESP timer diagnostics
        [*] Use ESP timer for triggering resource monitor
        [ ] Run monitor directly inside ESP timer
```

---

## 🛠️ Utilizare

### 1. **Include componenta în proiectul tău:**

Adaugă în `CMakeLists.txt`:
```cmake
set(EXTRA_COMPONENT_DIRS "<cale>/ESP32S3_Resource_Monitor")
```

### 2. **Include header-ul în codul tău:**

```c
#include "ResourceMonitor.h"

void app_main() {
    start_resource_monitor();  // Pornește monitorul
}
```

---

## 🧪 Exemplu de output

📌 Output Real:

```
I (652) MY-RESOURCE-MONITOR (BY Florin Baciu): ESPResourceMonitor v1.0.0 initialized
I (5647) MY-RESOURCE-MONITOR (BY Florin Baciu): Timestamp: [1970-01-01 02:38:50]
I (5647) MY-RESOURCE-MONITOR (BY Florin Baciu): -----------------Task Dump Start-----------------

Load    Stack   State   CoreID  PRIO    Name
0.01    1744    Running 1       3       [RESMONITOR(R)   ] 
98.56   2484    Ready   0       0       [IDLE0           ] 
92.65   2492    Ready   1       0       [IDLE1           ] 
0.21    2408    Blocked 1       24      [LVGL Tick Task  ] 
0.05    2196    Suspend 0       19      [ipc0            ] 
0.01    1996    Suspend 1       19      [v_check_0_pin_s ] 
0.08    6704    Suspend 1/2     3       [swdraw          ] 
0.57    6648    Suspend 1/2     3       [swdraw          ] 
0.00    3860    Suspend 0       22      [esp_timer       ] 
7.00    3616    Suspend 1       22      [LVGL Main Task  ] 
0.05    2332    Suspend 1       24      [ipc1            ] 

I (5651) MY-RESOURCE-MONITOR (BY Florin Baciu): -----------------Task Dump End-------------------
I (5651) MY-RESOURCE-MONITOR (BY Florin Baciu): -----------------Memory Info Start---------------

╔═══════════════════════════════ MEMORY STATS ═════════════════════════════╗
║   Segment     │    Total     │    Used     │   Free      │ Utilizare %   ║
╟───────────────┼──────────────┼─────────────┼─────────────┼────────────────╢
║ RAM          │   274.2 KB  │    73.8 KB │   200.4 KB │    26.93 %     ║
║ RAM-DMA      │   266.5 KB  │    73.4 KB │   193.1 KB │    27.56 %     ║
║ RAM 8 bit    │   274.2 KB  │    73.8 KB │   200.4 KB │    26.93 %     ║
║ RAM 32 bit   │   274.2 KB  │    73.8 KB │   200.4 KB │    26.93 %     ║
║ RTC RAM      │     7.7 KB  │     0.4 KB │     7.3 KB │     4.97 %     ║
║ PSRAM        │  7104.0 KB  │  1363.1 KB │  5740.9 KB │    19.19 %     ║
║ PSRAM 8 bit  │  7104.0 KB  │  1363.1 KB │  5740.9 KB │    19.19 %     ║
║ PSRAM 32 bit │  7104.0 KB  │  1363.1 KB │  5740.9 KB │    19.19 %     ║
╚══════════════════════════════════════════════════════════════════════════╝


I (5656) MY-RESOURCE-MONITOR (BY Florin Baciu): -----------------Memory Info End-----------------
I (5656) MY-RESOURCE-MONITOR (BY Florin Baciu): -----------------ESP Timer Dump Start------------

Timer stats:
Name                  Period      Alarm         Times_armed   Times_trigg   Times_skip    Cb_exec_time
res_mon_timer         5000000     10159844      1             1             0             10

I (5657) MY-RESOURCE-MONITOR (BY Florin Baciu): -----------------ESP Timer Dump End--------------
I (5658) RESMON_BENCH: resource_monitor_callback() execution time: 10648 µs (10.65 ms)
I (10647) RESMON_BENCH: Δ timp între apeluri: 5000000 µs (5.00 sec)
I (10647) MY-RESOURCE-MONITOR (BY Florin Baciu): Timestamp: [1970-01-01 02:38:55]
```

---

## 🧩 Extensibilitate

Toate callback-urile de monitorizare sunt separate:
- `task_monitor_cb()`
- `memory_monitor_cb()`
- `esp_timers_monitor_cb()`
- `memory_dump_cb()`

Le poți apela individual sau le poți integra într-un sistem propriu de diagnostic.

---

## 🔐 Licență

Apache 2.0 — folosește liber, contribuie cu încredere.

---

## 🤝 Contribuie

Pull requests, idei și îmbunătățiri sunt binevenite!  
Te rog adaugă comentarii clare și păstrează stilul de cod existent.

---

## 📧 Contact

📮 Autor: Baciu Aurel Florin  
✉️ Email: [baciuaurelflorin@gmail.com](mailto:baciuaurelflorin@gmail.com)

---

## 🧙‍♂️ The Lightbringer Rulez

Aceasta nu este doar o librărie. Este o extensie a ochilor tăi în sistem.  
Eresul taskurilor inutile va fi ars în focul RAM-ului.

---
