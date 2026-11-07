#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>

#define max_counters 256
#define max_len_name 64

struct counter_s {
  char name[max_len_name];
  uint64_t value;
  int used;
};
typedef struct counter_s counter_t;

static counter_t counters[max_counters];

counter_t *counter_create(const char *name)
{
  if (name == NULL || name[0] == '\0' || strlen(name) >= max_len_name)
    return NULL;
  for (int i = 0; i < max_counters; i++) {
    if (!counters[i].used) {
      strncpy(counters[i].name, name, max_len_name - 1);
      counters[i].name[max_len_name - 1] = '\0';
      counters[i].value = 0;
      counters[i].used = 1;
      return &counters[i];
    }
  }
  fprintf(stderr, "нет свободных слотов\n");
  return NULL;
}

void counter_set(counter_t *cnt, uint64_t value)
{
  if (cnt == NULL || !cnt->used)
    return;
  cnt->value = value;
}

void counter_inc(counter_t *cnt)
{
  if (cnt == NULL || !cnt->used)
    return;
  cnt->value++;
}

void counter_add(counter_t *cnt, uint64_t value)
{
  if (cnt == NULL || !cnt->used)
    return;
  cnt->value += value;
}

uint64_t counter_get(const counter_t *cnt)
{
  if (cnt == NULL || !cnt->used)
    return 0;
  return cnt->value;
}

void counter_clear(counter_t *cnt)
{
  if (cnt == NULL || !cnt->used)
    return;
  cnt->value = 0;
}

void counter_destroy(counter_t *cnt)
{
  if (cnt == NULL || !cnt->used)
    return;
  cnt->used = 0;
  cnt->value = 0;
  cnt->name[0] = '\0';
}

int counters_dump_json(const char *filname)
{
  if (filname == NULL)
    return -1;
  FILE *f = fopen(filname, "a");
  if (f == NULL) {
    perror("не удалось открыть файл");
    return -1;
  }

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  char timestamp[64];
  strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S", t);
  fprintf(f, "{\n");
  fprintf(f, "  \"timestamp\": \"%s\",\n", timestamp);
  fprintf(f, "  \"counters\": {\n");
  int first = 1;
  for (int i = 0; i < max_counters; i++) {
    if (counters[i].used) {
      if (!first)
        fprintf(f, ",\n");
      fprintf(f, "    \"%s\": %lu", counters[i].name, (unsigned long)counters[i].value);
      first = 0;
    }
  }

  fprintf(f, "\n  }\n");
  fprintf(f, "}\n");

  fclose(f);
  return 0;
}

static pthread_t dump_thread;
static int dump_running = 0;
static int dump_period_sec = 5;
static char dump_filename[256] = "counters.json";

static void *dump_thread_func(void *arg)
{
  (void)arg;
  while (dump_running) {
    counters_dump_json(dump_filename);
    sleep(dump_period_sec);
  }
  return NULL;
}

int counters_start_periodic_dump(const char *filename, int period_sec)
{
  if (filename == NULL || period_sec <= 0 || dump_running)
    return -1;

  strncpy(dump_filename, filename, sizeof(dump_filename) - 1);
  dump_filename[sizeof(dump_filename) - 1] = '\0';
  dump_period_sec = period_sec;
  dump_running = 1;

  if (pthread_create(&dump_thread, NULL, dump_thread_func, NULL) != 0) {
    dump_running = 0;
    return -1;
  }
  return 0;
}

void counters_stop_periodic_dump(void)
{
  if (!dump_running)
    return;
  dump_running = 0;
  pthread_join(dump_thread, NULL);
}