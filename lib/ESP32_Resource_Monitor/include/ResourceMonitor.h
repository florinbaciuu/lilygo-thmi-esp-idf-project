#pragma once
#ifndef RESOURCE_MONITOR_H
#define RESOURCE_MONITOR_H

#if CONFIG_ESP32_RESOURCE_MONITOR_ENABLED

#ifdef CONFIG_RESOURCE_MONITOR_USE_KCONFIG
#include "sdkconfig.h"
#else
#include "config.h"
#endif

// Fallbacks pentru opțiunile care nu sunt setate niciunde (doar ca backup total, rareori necesar)
#ifndef CONFIG_TASKMON_MAX_TASKS
#define CONFIG_TASKMON_MAX_TASKS 40
#endif
#ifndef CONFIG_TASKMON_MAX_TASK_NAME_LEN
#define CONFIG_TASKMON_MAX_TASK_NAME_LEN 16
#endif
#ifndef CONFIG_TASKMONITOR_DUMP_INTERVAL
#define CONFIG_TASKMONITOR_DUMP_INTERVAL 5
#endif
#ifndef CONFIG_TIME_STAMP_ON
#define CONFIG_TIME_STAMP_ON 0
#endif
#ifndef CONFIG_DEBUG_TASKS
#define CONFIG_DEBUG_TASKS 1
#endif
#ifndef CONFIG_DEBUG_ESP_TIMERS
#define CONFIG_DEBUG_ESP_TIMERS 1
#endif
#ifndef CONFIG_DEBUG_MEMORY
#define CONFIG_DEBUG_MEMORY 1
#endif
#ifndef CONFIG_DEBUG_MEMORY_USE_PRINTF
#define CONFIG_DEBUG_MEMORY_USE_PRINTF 1
#endif
#ifndef CONFIG_MEMORY_DUMP
#define CONFIG_MEMORY_DUMP 0
#endif
#ifndef CONFIG_USE_ESP_TIMER
#define CONFIG_USE_ESP_TIMER 1
#endif
#ifndef CONFIG_RESMON_USE_ESP_TIMER_DISPATCH
#define CONFIG_RESMON_USE_ESP_TIMER_DISPATCH 0
#endif

/**
 * Internal defines for debugging and performance monitoring of this component
 */
#define ENABLE_COMPONENT_DEBUG_BENCHMARK 1  // activează/dezactivează benchmark-ul intern

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

// Function prototypes
void printMemoryInfo();            // Print memory information
void printMemoryInfo2();           // Print memory information with more details
void resource_monitor_callback();  // Callback function to monitor resources

/***
 * @attention Important function
 * @brief Start the resource monitor task
 * @note This function creates the resource monitor task
 * @note It uses FreeRTOS APIs to create the task
 * @note The task is created in the suspended state
 * @note The task is resumed after the timer is started
 */
void start_resource_monitor();  // Start the resource monitor

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // ESP32_RESOURCE_MONITOR_ENABLED

#endif  // RESOURCE_MONITOR_H
