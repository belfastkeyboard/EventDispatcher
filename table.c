#include <assert.h>
#include <malloc.h>
#include <memory.h>
#include "table.h"

typedef struct Bucket
{
    hash_t hash;
    Event event;
    EventHandler handler;
    bool tombstone;
} Bucket;
typedef struct Table
{
    Bucket *array;
    size_t capacity;
    size_t nmemb;
} Table;

hash_t table_djb2(Event *event)
{
    unsigned char string[sizeof(*event)];
    memcpy(string, event, sizeof(*event));

    unsigned char *str = string;

    hash_t hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}
size_t table_get_index(hash_t hash, size_t capacity)
{
    return hash % capacity;
}
bool table_compare(Event a, Event b)
{
    return a == b;
}

size_t table_probe(Table *table, Event key, size_t index, bool skip_tombstones)
{
    assert(table && table->array);

    size_t found = -1;
    size_t count = 1;
    size_t tombstone = -1;

    while (found == -1)
    {
        Bucket bucket = table->array[index];

        if (skip_tombstones && tombstone == -1 && bucket.hash != -1 && bucket.tombstone)
            tombstone = index;
        else if ((!bucket.tombstone && table_compare(bucket.event, key)) ||
                 (!skip_tombstones && (bucket.hash == -1 || bucket.tombstone)))
            found = index;
        else if (skip_tombstones && bucket.hash == -1)
            break;
        else
            index = (index + 1) % table->capacity;

        if (++count > table->capacity)
            break;
    }

    if (tombstone != -1 && found != -1)
    {
        table->array[tombstone].handler = table->array[found].handler;
        table->array[tombstone].tombstone = false;
        table->array[found].tombstone = true;

        found = tombstone;
    }

    return found;
}
void table_resize(Table *table, float factor)
{
    assert(table->array);

    size_t size = sizeof(Event) * table->capacity;
    size_t capacity = table->capacity;

    Bucket *tmp = malloc(size);
    memcpy(tmp, table->array, size);
    free(table->array);

    table->capacity = (size_t)((float)table->capacity * factor);
    table->array = malloc(sizeof(Event) * table->capacity);
    assert(table->array);
    memset(table->array, -1, (size_t)((float)size * factor));

    Bucket bucket;
    for (int i = 0; i < capacity; i++)
    {
        bucket = tmp[i];

        if (bucket.hash == -1 || bucket.tombstone)
            continue;

        size_t re_index = table_get_index(bucket.hash, table->capacity);
        re_index = table_probe(table,bucket.event, re_index, false);
        table->array[re_index] = bucket;
    }

    free(tmp);
}

void table_insert(Table *table, Event key, EventHandler handler)
{
    if (table->nmemb == 0)
    {
        table->capacity = 8;
        if (table->array)
            free(table->array);
        table->array = malloc(sizeof(Bucket) * table->capacity);
        memset(table->array, -1, sizeof(Bucket) * table->capacity);
    }
    else
    {
        float load_factor = ((float)table->nmemb / (float)table->capacity);
        if (load_factor >= 0.75f)
            table_resize(table, 2.f);
    }

    hash_t hash = table_djb2(&key);
    size_t index = table_get_index(hash, table->capacity);

    assert(table->array);
    index = table_probe(table, key, index, false);

    if (!table->array[index].tombstone && !table_compare(table->array[index].event, key))
        table->nmemb++;

    Bucket bucket = { hash, key, handler, false };
    table->array[index] = bucket;
}
void table_erase(Table *table, Event key)
{
    assert(table->nmemb > 0);

    hash_t hash = table_djb2(&key);
    unsigned int index = table_get_index(hash, table->capacity);

    index = table_probe(table, key, index, true);
    if (index != -1)
    {
        table->array[index].tombstone = true;
        table->nmemb--;
    }

    float load_factor = ((float)table->nmemb / (float)table->capacity);
    if (load_factor <= 0.1f && table->capacity > 8)
        table_resize(table, 0.5f);
}
void table_clear(Table *table)
{
    free(table->array);
    table->capacity = 0;
    table->nmemb = 0;
    table->array = malloc(sizeof(Event) * 0);
}
EventHandler table_get(Table *table, Event event)
{
    EventHandler handler = NULL;

    if (table->nmemb)
    {
        hash_t hash = table_djb2(&event);
        size_t index = table_get_index(hash, table->capacity);

        index = table_probe(table, event, index, true);
        if (index != -1)
            handler = table->array[index].handler;
    }

    return handler;
}

/*bool table_empty(Table *table)
{
    return (table->nmemb == 0);
}*/
size_t table_size(Table *table)
{
    return table->nmemb;
}
size_t table_capacity(Table *table)
{
    return table->capacity;
}

Table *create_table(void)
{
    Table *table = malloc(sizeof(Table));

    table->array = NULL;
    table->capacity = 0;
    table->nmemb = 0;

    return table;
}
void destroy_table(Table **table)
{
    if ((*table)->array)
        free((*table)->array);

    free(*table);
    *table = NULL;
}
