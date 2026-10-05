/**
 * @file    event_counter.h
 * @brief   A0.6: a module whose state is private (file-scope static).
 *
 * The header is the module's contract: types, function prototypes and
 * extern DECLARATIONS. The storage itself is defined once, in event_counter.c.
 */
#ifndef EVENT_COUNTER_H
#define EVENT_COUNTER_H

#include <stdint.h>

/** Kinds of event. An enum, not a list of #defines: the debugger shows the
 *  names, -Wswitch reports a missing case, and EVENT_KIND_COUNT sizes arrays. */
typedef enum
{
    EVENT_BUTTON = 0,
    EVENT_TICK,
    EVENT_SELFTEST,
    EVENT_KIND_COUNT      /**< keep last: number of kinds */
} EventKind;

/** Module version: declared here, defined exactly once in event_counter.c. */
extern const uint32_t g_eventCounterVersion;

/** @brief Counts one event. Not ISR-safe: the increment is a read-modify-write (A0.2). */
void EventCounter_Record(EventKind kind);

/** @brief Number of events of @p kind since reset (0 for an invalid kind). */
uint32_t EventCounter_Get(EventKind kind);

/** @brief Printable name of @p kind ("?" for an invalid kind). */
const char *EventCounter_Name(EventKind kind);

/** @brief Prints where the module's objects live (.rodata / .data / .bss). Target only. */
void EventCounter_PrintStorage(void);

#endif /* EVENT_COUNTER_H */
