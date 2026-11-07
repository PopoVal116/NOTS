#include <stdio.h>
#include <unistd.h>
#include "counters.h"

int main(void)
{
  counter_t *rrc_req = counter_create("rrc_connection_request");
  counter_t *rrc_setup = counter_create("rrc_connection_setup");
  counter_t *pdcp_tx = counter_create("pdcp_sdus_tx");

  if (!rrc_req || !rrc_setup || !pdcp_tx) {
    printf("Не удалось создать счётчики\n");
    return 1;
  }

  if (counters_start_periodic_dump("counters.json", 2) != 0) {
    printf("Не удалось запустить периодическую выгрузку\n");
    return 1;
  }

  printf("Периодическая выгрузка запущена (каждые 2 сек). Ждём 6 секунд...\n");

  counter_inc(rrc_req);
  counter_inc(rrc_req);
  sleep(2);

  counter_inc(rrc_setup);
  counter_add(pdcp_tx, 100);
  sleep(2);

  counter_add(pdcp_tx, 50);
  sleep(2);

  counters_stop_periodic_dump();

  printf("rrc_connection_request = %lu\n", (unsigned long)counter_get(rrc_req));
  printf("rrc_connection_setup   = %lu\n", (unsigned long)counter_get(rrc_setup));
  printf("pdcp_sdus_tx           = %lu\n", (unsigned long)counter_get(pdcp_tx));
  printf("Финальный JSON:\n");
  system("cat counters.json");

  return 0;
}