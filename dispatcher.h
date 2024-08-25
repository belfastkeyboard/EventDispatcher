#pragma once

#include "event.h"

void create_event_dispatcher(void);
void destroy_event_dispatcher(void);

void register_listener(Event event, EventHandler handler);
void deregister_listener(Event event);

void submit_event(Event event);
void dispatch_events(void);
