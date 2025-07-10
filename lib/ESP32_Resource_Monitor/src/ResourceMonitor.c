/*
 * SPDX-FileCopyrightText: 2025 baciuaurelflorin@gmail.com
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "version.h"

#include <stdbool.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include <time.h>

#include "ResourceMonitor.h"

#if CONFIG_ESP32_RESOURCE_MONITOR_ENABLED

// #if !defined(CONFIG_RESOURCE_MONITOR_USE_KCONFIG)
//   #include "config.h"
// #endif

// -------------------------------------------------------------------------

#define TASK_MAX_COUNT             CONFIG_TASKMON_MAX_TASKS
#define DUMP_INTERVAL_S            (CONFIG_TASKMONITOR_DUMP_INTERVAL)
#define DUMP_INTERVAL_MS           (CONFIG_TASKMONITOR_DUMP_INTERVAL * 1000)
#define DUMP_INTERVAL_US           (CONFIG_TASKMONITOR_DUMP_INTERVAL * 1000000)
#define SECONDS_TO_MICROSECONDS(x) ((x) * 1000000)

/* For future use*/
#undef DUMP_INTERVAL_S   // not used yet
#undef DUMP_INTERVAL_MS  // not used yet
#undef DUMP_INTERVAL_US  // not used yet

// -------------------------------------------------------------------------

const static char *TAG = "MY-RESOURCE-MONITOR";
const char *resource_monitor_task_name = "RESMONITOR(R)";  // max 16 chars cu NULL

// -------------------------------------------------------------------------

const char *task_state[] = {"Running", "Ready", "Blocked", "Suspend", "Deleted", "Invalid"};
const char *task_state_ro[] = {"Ruleaza", "Gata", "Blocat", "Suspendat", "Sters", "Invalid"};
// -------------------------------------------------------------------------

typedef struct {
  uint32_t ulRunTimeCounter;
  uint32_t xTaskNumber;
} task_data_t;

// -------------------------------------------------------------------------

static task_data_t previous_snapshot[TASK_MAX_COUNT];
static int task_top_id = 0;
static uint32_t total_runtime = 0;

// -------------------------------------------------------------------------

/**
  * @brief Function to get the previous task data
  * @param xTaskNumber Task number
  * @return task_data_t* Pointer to the task data
  * @note This function is used to get the previous task data
  * @note If the task is not found, a new entry is allocated
  * @note The function is used to calculate the task load
  */
task_data_t *getPreviousTaskData(uint32_t xTaskNumber) {
  for (int i = 0; i < task_top_id; i++) {
    if (previous_snapshot[i].xTaskNumber == xTaskNumber) {
      return &previous_snapshot[i];
    }
  }  // Try to find the task in the list of tasks
  assert(task_top_id < TASK_MAX_COUNT);  // Allocate a new entry
  task_data_t *result = &previous_snapshot[task_top_id];
  result->xTaskNumber = xTaskNumber;
  task_top_id++;
  return result;
}

// -------------------------------------------------------------------------

/**
  * @brief Function to monitor the tasks
  * @param xTimer Timer handle
  * @note This function is called by the timer
  * @note It calls the task_monitor_cb() function
  */
#if (CONFIG_TIME_STAMP_ON == 1)
const char *getTimestamp() {
  static char timestamp[20];
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);
  return timestamp;
}
#endif /* #if (TIME_STAMP_ON == 1) */

// -------------------------------------------------------------------------

void printMemoryInfo() {
  // Obținerea informațiilor despre memorie
  ESP_LOGI(TAG, "-----------------Memory Info Start---------------");
  printf("\n\r");
  size_t total_psram = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
  size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
  size_t used_psram = total_psram - free_psram;
  size_t total_heap = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
  size_t free_heap = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
  size_t used_heap = total_heap - free_heap;
  size_t total_ram = total_psram + total_heap;  // Suma PSRAM și heap
  size_t free_ram = free_psram + free_heap;     // Suma memoriei libere
  ESP_LOGI("MemoryInfo", "Total PSRAM: %zu bytes \t Free PSRAM: %zu bytes \t Used PSRAM: %zu bytes", total_psram, free_psram, used_psram);
  ESP_LOGI("MemoryInfo", "Total Heap: %zu bytes  \t Free Heap: %zu bytes  \t Used Heap: %zu bytes", total_heap, free_heap, used_heap);
  ESP_LOGI("MemoryInfo", "Total RAM: %zu bytes   \t Free RAM: %zu bytes", total_ram, free_ram);
  printf("\n\r");
  ESP_LOGI(TAG, "-----------------Memory Info End-----------------");
}

// -------------------------------------------------------------------------

#define BYTES_TO_KB(x)    ((float)(x) / 1024.0f)
#define PERC(used, total) ((total) > 0 ? ((float)(used) / (float)(total)) * 100.0f : 0.0f)
#define MEM_TAG           "MEMORY"

const char *MEMORY_TAG = MEM_TAG;

//--------------------------

#if (CONFIG_DEBUG_MEMORY_USE_PRINTF == 1)
// Helper macro pentru printare frumoasă
#define MEM_PRINT_ROW(label, total, free)                                                                                                                 \
  do {                                                                                                                                                    \
    size_t used = (total) - (free);                                                                                                                       \
    printf(                                                                                                                                               \
      "║ %-13s│ %7.1f KB  │ %7.1f KB │ %7.1f KB │   %6.2f %%     ║\n", label, BYTES_TO_KB(total), BYTES_TO_KB(used), BYTES_TO_KB(free), PERC(used, total) \
    );                                                                                                                                                    \
  } while (0)
#else
// Helper macro
#define MEM_PRINT_ROW(label, total, free)                                                                                                         \
  do {                                                                                                                                            \
    size_t used = (total) - (free);                                                                                                               \
    ESP_LOGI(                                                                                                                                     \
      MEMORY_TAG, "║ %-13s│ %7.1f KB  │ %7.1f KB │ %7.1f KB │   %6.2f %%     ║", label, BYTES_TO_KB(total), BYTES_TO_KB(used), BYTES_TO_KB(free), \
      PERC(used, total)                                                                                                                           \
    );                                                                                                                                            \
  } while (0)
#endif

//--------------------------
#if (CONFIG_DEBUG_MEMORY == 1)
/**
  * @brief Function to print memory information
  * @note This function prints the memory information in a formatted way
  * @note It uses ESP_LOGI for logging if USE_ESP_LOGI is defined, otherwise it uses printf
  */
void printMemoryInfo2() {
#if (CONFIG_DEBUG_MEMORY_USE_PRINTF == 1)
  printf("╔═══════════════════════════════ MEMORY STATS ═════════════════════════════╗\n");
  printf("║   Segment     │    Total     │    Used     │   Free      │ Utilizare %%   ║\n");
  printf("╟───────────────┼──────────────┼─────────────┼─────────────┼────────────────╢\n");
  MEM_PRINT_ROW("RAM", heap_caps_get_total_size(MALLOC_CAP_INTERNAL), heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
  MEM_PRINT_ROW("RAM-DMA", heap_caps_get_total_size(MALLOC_CAP_DMA), heap_caps_get_free_size(MALLOC_CAP_DMA));
  MEM_PRINT_ROW("RAM 8 bit", heap_caps_get_total_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT), heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
  MEM_PRINT_ROW(
    "RAM 32 bit", heap_caps_get_total_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT), heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT)
  );
  MEM_PRINT_ROW("RTC RAM", heap_caps_get_total_size(MALLOC_CAP_RTCRAM), heap_caps_get_free_size(MALLOC_CAP_RTCRAM));
  MEM_PRINT_ROW("PSRAM", heap_caps_get_total_size(MALLOC_CAP_SPIRAM), heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
  MEM_PRINT_ROW("PSRAM 8 bit", heap_caps_get_total_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT), heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
  MEM_PRINT_ROW("PSRAM 32 bit", heap_caps_get_total_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_32BIT), heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_32BIT));
  printf("╚══════════════════════════════════════════════════════════════════════════╝\n");
  printf("\n\r");
#else
  ESP_LOGI(MEMORY_TAG, "╔═══════════════════════════════ MEMORY STATS ═════════════════════════════╗");
  ESP_LOGI(MEMORY_TAG, "║   Segment     │    Total     │    Used     │   Free      │ Utilizare %%   ║");
  ESP_LOGI(MEMORY_TAG, "╟───────────────┼──────────────┼─────────────┼─────────────┼────────────────╢");
  MEM_PRINT_ROW("RAM", heap_caps_get_total_size(MALLOC_CAP_INTERNAL), heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
  MEM_PRINT_ROW("RAM-DMA", heap_caps_get_total_size(MALLOC_CAP_DMA), heap_caps_get_free_size(MALLOC_CAP_DMA));
  MEM_PRINT_ROW("RAM 8 bit", heap_caps_get_total_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT), heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT));
  MEM_PRINT_ROW(
    "RAM 32 bit", heap_caps_get_total_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT), heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_32BIT)
  );
  MEM_PRINT_ROW("RTC RAM", heap_caps_get_total_size(MALLOC_CAP_RTCRAM), heap_caps_get_free_size(MALLOC_CAP_RTCRAM));
  MEM_PRINT_ROW("PSRAM", heap_caps_get_total_size(MALLOC_CAP_SPIRAM), heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
  MEM_PRINT_ROW("PSRAM 8 bit", heap_caps_get_total_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT), heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
  MEM_PRINT_ROW("PSRAM 32 bit", heap_caps_get_total_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_32BIT), heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_32BIT));
  ESP_LOGI(MEMORY_TAG, "╚══════════════════════════════════════════════════════════════════════════╝");
#endif
}
#endif  // CONFIG_DEBUG_MEMORY

#undef BYTES_TO_KB
#undef BYTES_TO_MB
#undef PERC

#if (CONFIG_DEBUG_TASKS == 1)
void printTaskInfo() {
  // This function is a placeholder for task information printing.
  // It can be used to call the task_monitor_cb() function if needed.
  // Currently, it does nothing.
  uint32_t totalRunTime = 0;
  TaskStatus_t taskStats[TASK_MAX_COUNT] = {0};
  uint32_t taskCount = uxTaskGetSystemState(taskStats, TASK_MAX_COUNT, &totalRunTime);
  assert(task_top_id < TASK_MAX_COUNT);
  uint32_t totalDelta = totalRunTime - total_runtime;
  float f = 100.0 / totalDelta;
  printf("%.4s\t%.6s\t%.8s\t%.8s\t%.4s\t%-20s\n", "Load", "Stack", "State", "CoreID", "PRIO", "Name");  // Format headers in a more visually appealing way
  for (uint32_t i = 0; i < taskCount; i++) {
    TaskStatus_t *stats = &taskStats[i];
    task_data_t *previousTaskData = getPreviousTaskData(stats->xTaskNumber);
    uint32_t taskRunTime = stats->ulRunTimeCounter;
    float load = f * (taskRunTime - previousTaskData->ulRunTimeCounter);

    char formattedTaskName[19];  // 16 caractere + 1 caracter pt terminatorul '\0' + 2 caractere pt paranteze"[]"
    snprintf(formattedTaskName, sizeof(formattedTaskName), "[%-16s]", stats->pcTaskName);  // Format for the task's name with improved visibility
    char core_id_str[16];
    if (stats->xCoreID == -1 || stats->xCoreID == 2147483647) {
      snprintf(core_id_str, sizeof(core_id_str), "1/2");
    } else {
      snprintf(core_id_str, sizeof(core_id_str), "%d", stats->xCoreID);
    }  // Customize how core ID is displayed for better clarity
    printf(
      "%.2f\t%" PRIu32 "\t%-4s\t%-4s\t%-4u\t%-19s\n", load, stats->usStackHighWaterMark, task_state[stats->eCurrentState], core_id_str, stats->uxBasePriority,
      formattedTaskName
    );  // Print formatted output
    previousTaskData->ulRunTimeCounter = taskRunTime;
  }
  total_runtime = totalRunTime;
}
#endif  // CONFIG_DEBUG_TASKS


// -------------------------------------------------------------------------

/***
  * task_monitor_cb()
  * @note Function to print the task information
  * @note This function is called by the task_monitor_cb() function. It prints the task information, calculates the task load,
  * prints the total free heap, prints the total run time, prints the total run time delta, prints the total run time delta in percentage
  * and prints the task information.
  */
void task_monitor_cb() {
#if (CONFIG_DEBUG_TASKS == 1)
  ESP_LOGI(TAG, "-----------------Task Dump Start-----------------");
  printf("\n\r");
  printTaskInfo();
  printf("\n\r");
  ESP_LOGI(TAG, "-----------------Task Dump End-------------------");
#endif
}
// -------------------------------------------------------------------------
void memory_monitor_cb() {
// This function is a placeholder for memory monitoring.
// It can be used to call the printMemoryInfo() function if needed.
// Currently, it does nothing.
#if (CONFIG_DEBUG_MEMORY == 1)
  ESP_LOGI(TAG, "-----------------Memory Info Start---------------");
  printf("\n\r");
  printMemoryInfo2();
  printf("\n\r");
  ESP_LOGI(TAG, "-----------------Memory Info End-----------------");
#endif
}
// -------------------------------------------------------------------------
void esp_timers_monitor_cb() {
  // This function is a placeholder for ESP timer monitoring.
  // It can be used to call the esp_timer_dump() function if needed.
  // Currently, it does nothing.
#if (CONFIG_DEBUG_ESP_TIMERS == 1)
  ESP_LOGI(TAG, "-----------------ESP Timer Dump Start------------");
  printf("\n\r");
  esp_timer_dump(stdout);
  printf("\n\r");
  ESP_LOGI(TAG, "-----------------ESP Timer Dump End--------------");
#endif
}
// -------------------------------------------------------------------------
void memory_dump_cb() {
  // This function is a placeholder for memory dumping.
  // It can be used to call the heap_caps_dump() function if needed.
  // Currently, it does nothing.
#if (CONFIG_MEMORY_DUMP == 1)
  ESP_LOGI(TAG, "-----------------Memory Dump Start---------------");
  printf("\n\r");
  heap_caps_dump(MALLOC_CAP_8BIT);             // Dump the heap information
  heap_caps_print_heap_info(MALLOC_CAP_8BIT);  // Print the heap information
  //multi_heap_info_t info;
  //heap_caps_get_info(&info, MALLOC_CAP_DEFAULT);                                                 // Get the heap information
  //printf("Free: %u, Largest free block: %u\n", info.total_free_bytes, info.largest_free_block);  // Print the free memory and largest free block
  printf("\n\r");
  ESP_LOGI(TAG, "-----------------Memory Dump End-----------------");
#endif
}
// -------------------------------------------------------------------------

/**
 * @brief Function to monitor the tasks
 * @note This function is called by the APP
 */
void resource_monitor_callback() {
#if ENABLE_COMPONENT_DEBUG_BENCHMARK
  int64_t start = esp_timer_get_time();  // 🕒 start timing
#endif                                   // ENABLE_COMPONENT_DEBUG_BENCHMARK
#if (CONFIG_TIME_STAMP_ON == 1)
  const char *timestamp = getTimestamp();
  ESP_LOGI(TAG, "Timestamp: [%s]", timestamp);
#endif  // CONFIG_TIME_STAMP_ON
#if (CONFIG_DEBUG_TASKS == 1)
  task_monitor_cb();
#endif  // CONFIG_DEBUG_TASKS
#if (CONFIG_DEBUG_MEMORY == 1)
  memory_monitor_cb();
#endif  // CONFIG_DEBUG_MEMORY
#if (CONFIG_DEBUG_ESP_TIMERS == 1)
  esp_timers_monitor_cb();
#endif  // CONFIG_DEBUG_ESP_TIMERS
#if (CONFIG_MEMORY_DUMP == 1)
  memory_dump_cb();
#endif  // CONFIG_MEMORY_DUMP
#if ENABLE_COMPONENT_DEBUG_BENCHMARK
  int64_t end = esp_timer_get_time();  // 🕒 end timing
  ESP_LOGI("RESMON_BENCH", "resource_monitor_callback() execution time: %lld µs (%.2f ms)", end - start, (end - start) / 1000.0);
#endif  // ENABLE_COMPONENT_DEBUG_BENCHMARK
}

// -------------------------------------------------------------------------

#if (CONFIG_USE_ESP_TIMER == 1)
#if (CONFIG_RESMON_USE_ESP_TIMER_DISPATCH == 1)
/**
 * @brief Function to adapt the resource monitor callback for use with ESP timers
 * @note This function is called by the ESP timer and simply calls the resource_monitor_callback()
 */
void resource_monitor_callback_adapter(void *arg) {
  (void)arg;  // dacă nu-l folosești, suprimă warningul
#if ENABLE_COMPONENT_DEBUG_BENCHMARK
  static int64_t last_start = 0;
  int64_t now = esp_timer_get_time();

  if (last_start != 0) {
    int64_t delta = now - last_start;
    ESP_LOGI("RESMON_BENCH", "Δ timp între apeluri: %lld µs (%.2f sec)", delta, delta / 1e6);
  }

  last_start = now;
#endif  // ENABLE_COMPONENT_DEBUG_BENCHMARK
  resource_monitor_callback();
}
//---------------------

// Definiția structurii de timer
esp_timer_handle_t monitor_timer;

void start_resource_monitor() {
  const esp_timer_create_args_t periodic_timer_args = {
    .callback = &resource_monitor_callback_adapter,
    .arg = NULL,
    .dispatch_method = ESP_TIMER_TASK,
    .name = "res_mon_timer",
    .skip_unhandled_events = false,
  };
  ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &monitor_timer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(monitor_timer, SECONDS_TO_MICROSECONDS(CONFIG_TASKMONITOR_DUMP_INTERVAL)));
}

#else  //

esp_timer_handle_t monitor_timer;
static TaskHandle_t xResourceMonitorTaskHandle = NULL;

/**
 * @brief Function to adapt the resource monitor callback for use with ESP timers
 * @note This function is called by the ESP timer and simply calls the resource_monitor_callback()
 */

void resource_monitor_callback_adapter(void *arg) {
  (void)arg;  // dacă nu-l folosești, suprimă warningul

#if ENABLE_COMPONENT_DEBUG_BENCHMARK
  static int64_t last_start = 0;
  int64_t now = esp_timer_get_time();

  if (last_start != 0) {
    int64_t delta = now - last_start;
    ESP_LOGI("RESMON_BENCH", "Δ timp între apeluri: %lld µs (%.2f sec)", delta, delta / 1e6);
  }

  last_start = now;
#endif  // ENABLE_COMPONENT_DEBUG_BENCHMARK

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  if (xResourceMonitorTaskHandle != NULL) {
    vTaskNotifyGiveFromISR(xResourceMonitorTaskHandle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

//---------------
/********************************************** */
/*                   TASK                       */
/********************************************** */
static void vResourceMonitorTask(void *pvParameters) {
  (void)pvParameters;
  ESP_LOGI(TAG, "ESPResourceMonitor v" ESP_RESOURCE_MONITOR_VERSION " initialized");
  xResourceMonitorTaskHandle = xTaskGetCurrentTaskHandle();  // Încoronarea oficială a task-ului
  while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);  // Așteaptă semnalul
    resource_monitor_callback();              // Rulează când e notificat
  }
}

//---------------

void start_resource_monitor() {
  // Creează taskul
  xTaskCreatePinnedToCore(
    vResourceMonitorTask,         // funcția task-ului
    resource_monitor_task_name,   // numele task-ului
    4096,                         // stack size
    NULL,                         // parametru
    3,                            // prioritate
    &xResourceMonitorTaskHandle,  // handle-ul global
    1                             // core-ul (poți schimba)
  );

  // Creează și pornește timerul
  const esp_timer_create_args_t periodic_timer_args = {
    .callback = &resource_monitor_callback_adapter,
    .arg = NULL,
    .dispatch_method = ESP_TIMER_TASK,  // sau ISR dacă e și mai light
    .name = "res_mon_timer",
    .skip_unhandled_events = false,
  };
  ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &monitor_timer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(monitor_timer, SECONDS_TO_MICROSECONDS(CONFIG_TASKMONITOR_DUMP_INTERVAL)));
}
#endif


#else  // !(CONFIG_USE_ESP_TIMER == 1)

static TaskHandle_t xResourceMonitorTaskHandle = NULL;

/**
 * @brief Function to adapt the resource monitor callback for use with ESP timers
 * @note This function is called by the ESP timer and simply calls the resource_monitor_callback()
 */

void resource_monitor_callback_adapter() {
#if ENABLE_COMPONENT_DEBUG_BENCHMARK
  static int64_t last_start = 0;
  int64_t now = esp_timer_get_time();
  if (last_start != 0) {
    int64_t delta = now - last_start;
    ESP_LOGI("RESMON_BENCH", "Δ timp între apeluri: %lld µs (%.2f sec)", delta, delta / 1e6);
  }
  last_start = now;
#endif  // ENABLE_COMPONENT_DEBUG_BENCHMARK
  resource_monitor_callback();
}


/********************************************** */
/*                   TASK                       */
/********************************************** */
static void vResourceMonitorTask(void *pvParameters) {
  (void)pvParameters;
  ESP_LOGI(TAG, "ESPResourceMonitor v" ESP_RESOURCE_MONITOR_VERSION " initialized");
  TickType_t xLastWakeTime = xTaskGetTickCount();            // 🔥 tick-ul inițial
  xResourceMonitorTaskHandle = xTaskGetCurrentTaskHandle();  // Încoronarea oficială
  while (1) {
    resource_monitor_callback_adapter();  // Ce avem de făcut
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CONFIG_TASKMONITOR_DUMP_INTERVAL * 1000));
  }
}

// ----------------------

void start_resource_monitor() {
  xTaskCreatePinnedToCore(
    vResourceMonitorTask,         // funcția task-ului
    resource_monitor_task_name,   // numele task-ului
    4096,                         // stack size
    NULL,                         // parametru
    3,                            // prioritate
    &xResourceMonitorTaskHandle,  // handle-ul global
    1                             // core-ul (poți schimba)
  );
}

//
#endif  //

#undef TASK_MAX_COUNT  // Nu mai e necesara anymore
#undef SECONDS_TO_MICROSECONDS  // Nu mai e necesara anymore

#endif  // CONFIG_ESP_RESOURCE_MONITOR_ENABLED
