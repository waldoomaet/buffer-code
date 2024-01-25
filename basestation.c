#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "dev/leds.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"

#define STILL_INTERVAL CLOCK_SECOND * 5

PROCESS(timer_process, "Timer process");
PROCESS(basestation_process, "Clicker basestation");
AUTOSTART_PROCESSES(&basestation_process, &timer_process);

// static struct etimer et;

static void recv(const void *data, uint16_t len,
                 const linkaddr_t *src, const linkaddr_t *dest)
{
  leds_off(LEDS_ALL);
  leds_on(0b1111);

  printf("Basestation got something: ");
  int len_count = 0;
  while (len_count < len)
  {
    printf("%d ", *(int16_t *)(data + len_count));
    len_count += 2;
  }
  printf(" \n");
  process_poll(&timer_process);
  // etimer_reset(&et);
}

PROCESS_THREAD(basestation_process, ev, data)
{
  PROCESS_BEGIN();

  nullnet_set_input_callback(recv);

  PROCESS_END();
}

PROCESS_THREAD(timer_process, ev, data)
{
  // etimer_set(&et, STILL_INTERVAL);
  PROCESS_BEGIN();

  printf("Event in timer process!\n");

  // while (1)
  // {
  //   PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  //   leds_off(LEDS_ALL);
  //   printf("All off!\n");
  //   etimer_reset(&et);
  // }

  PROCESS_END();
}