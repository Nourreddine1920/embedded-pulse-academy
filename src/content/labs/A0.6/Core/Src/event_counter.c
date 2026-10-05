/**
 * @file    event_counter.c
 * @brief   A0.6: private module state with static, and where each object lands.
 *
 *   object                 linkage    section    costs
 *   g_eventCounterVersion  external   .rodata    4 B Flash
 *   k_names                internal   .rodata    12 B Flash (+ the strings)
 *   s_counts               internal   .bss       12 B RAM, zeroed by startup
 *   s_bootMarker           internal   .data      4 B RAM + 4 B Flash (copied at boot)
 */
#include "event_counter.h"

#include <assert.h>
#include <stddef.h>

#include "console.h"

#define EVENT_BOOT_MARKER   (0xB007C0DEUL)

const uint32_t g_eventCounterVersion = 0x00010006UL;         /* the ONE definition */

static const char *const k_names[] = { "BUTTON", "TICK", "SELFTEST" };
static_assert((sizeof k_names / sizeof k_names[0]) == (size_t)EVENT_KIND_COUNT,
              "k_names needs exactly one entry per EventKind");

static uint32_t s_counts[EVENT_KIND_COUNT];                  /* zero-initialised: .bss */
static uint32_t s_bootMarker = EVENT_BOOT_MARKER;            /* initialised: .data     */

void EventCounter_Record(EventKind kind)
{
    if ((uint32_t)kind < (uint32_t)EVENT_KIND_COUNT)
    {
        s_counts[kind]++;
    }
}

uint32_t EventCounter_Get(EventKind kind)
{
    uint32_t count = 0U;

    if ((uint32_t)kind < (uint32_t)EVENT_KIND_COUNT)
    {
        count = s_counts[kind];
    }
    return count;
}

const char *EventCounter_Name(EventKind kind)
{
    const char *name = "?";

    if ((uint32_t)kind < (uint32_t)EVENT_KIND_COUNT)
    {
        name = k_names[kind];
    }
    return name;
}

void EventCounter_PrintStorage(void)
{
    Console_Printf("  .rodata g_eventCounterVersion @ %p = 0x%08lX\r\n",
                   (const void *)&g_eventCounterVersion, (unsigned long)g_eventCounterVersion);
    Console_Printf("  .rodata k_names               @ %p\r\n", (const void *)k_names);
    Console_Printf("  .data   s_bootMarker          @ %p = 0x%08lX%s\r\n",
                   (void *)&s_bootMarker, (unsigned long)s_bootMarker,
                   (s_bootMarker == EVENT_BOOT_MARKER) ? " (copied from Flash by startup)" : " CORRUPTED");
    Console_Printf("  .bss    s_counts              @ %p = {%lu, %lu, %lu}\r\n",
                   (void *)s_counts, (unsigned long)s_counts[EVENT_BUTTON],
                   (unsigned long)s_counts[EVENT_TICK], (unsigned long)s_counts[EVENT_SELFTEST]);
}
