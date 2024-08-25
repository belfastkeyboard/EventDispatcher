#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "event.h"

typedef unsigned long hash_t;

typedef struct Bucket Bucket;
typedef struct Table Table;

hash_t table_djb2(Event *event);
size_t table_get_index(hash_t hash, size_t capacity);
bool table_compare(Event a, Event b);

size_t table_probe(Table *table, Event key, size_t index, bool skip_tombstones);
void table_resize(Table *table, float factor);

void table_insert(Table *table, Event key, EventHandler handler);
void table_erase(Table *table, Event key);
void table_clear(Table *table);
EventHandler table_get(Table *table, Event event);

bool table_empty(Table *table);
size_t table_size(Table *table);
size_t table_capacity(Table *table);

Table *create_table(void);
void destroy_table(Table **table);
