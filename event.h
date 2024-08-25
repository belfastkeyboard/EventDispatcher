#pragma once

typedef enum
{
    EVENT_STOP, EVENT_START, // define more events here
    NUM_EVENTSls
} Event;

typedef void (*EventHandler)(void);

// define event handlers below
void event_stop(void);
void event_start(void);
