#include <assert.h>
#include "event.h"
#include "table.h"
#include "dispatcher.h"
#include "rbuff.h"

typedef struct EventDispatcher
{
    Table *table;
    RingBuffer *rbuff;
} EventDispatcher;

EventDispatcher *event_dispatcher = NULL;

void create_event_dispatcher(void)
{
    event_dispatcher = malloc(sizeof(EventDispatcher));
    event_dispatcher->table = create_table();
    event_dispatcher->rbuff = create_ringbuffer(32, sizeof(Event));
}
void destroy_event_dispatcher(void)
{
    assert(event_dispatcher);

    destroy_table(&event_dispatcher->table);
    destroy_ringbuffer(&event_dispatcher->rbuff);

    free(event_dispatcher);
    event_dispatcher = NULL;
}

void register_listener(Event event, EventHandler handler)
{
    table_insert(event_dispatcher->table, event, handler);
}
void deregister_listener(Event event)
{
    table_erase(event_dispatcher->table, event);
}

void submit_event(Event event)
{
    assert(event_dispatcher);

    if (!full(event_dispatcher->rbuff))
        write(event_dispatcher->rbuff, &event);
}
void dispatch_events(void)
{
    while (!empty(event_dispatcher->rbuff))
    {
        Event event = *(Event*)read(event_dispatcher->rbuff);
        EventHandler handler = table_get(event_dispatcher->table, event);

        if (handler)
            handler();
    }

    clear(event_dispatcher->rbuff);
}
