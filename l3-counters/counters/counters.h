#ifndef COUNTERS_H
#define COUNTERS_H

#include <stdint.h>

typedef struct counter_s counter_t;

counter_t *counter_create(const char *name);
void counter_set(counter_t *cnt, uint64_t value);
void counter_inc(counter_t *cnt);
void counter_add(counter_t *cnt, uint64_t value);
uint64_t counter_get(const counter_t *cnt);
void counter_clear(counter_t *cnt);
void counter_destroy(counter_t *cnt);

int counters_dump_json(const char *filename);
int counters_start_periodic_dump(const char *filename, int period_sec);
void counters_stop_periodic_dump(void);

#endif