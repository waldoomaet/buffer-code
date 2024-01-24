#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "dev/leds.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"

#define STILL_INTERVAL CLOCK_SECOND

PROCESS(basestation_process, "Clicker basestation");
AUTOSTART_PROCESSES(&basestation_process);

static struct timer t;
timer_set(&t, STILL_INTERVAL);

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

  timer_restart(&t);
}

/* Our main process. */
PROCESS_THREAD(basestation_process, ev, data)
{
  PROCESS_BEGIN();

  nullnet_set_input_callback(recv);

  if (timer_expired(&t))
  {
    leds_off(LEDS_ALL);
    timer_restart(&t);
  }

  PROCESS_END();
}
