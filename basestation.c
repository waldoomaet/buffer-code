#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "dev/leds.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"

#define STILL_INTERVAL CLOCK_SECOND * 5

PROCESS(basestation_process, "Clicker basestation");
AUTOSTART_PROCESSES(&basestation_process);

static struct stimer st;
stimer_set(&st, STILL_INTERVAL);

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

  stimer_restart(&st);
}

/* Our main process. */
PROCESS_THREAD(basestation_process, ev, data)
{
  PROCESS_BEGIN();

  nullnet_set_input_callback(recv);

  if (stimer_expired(&st))
  {
    leds_off(LEDS_ALL);
    stimer_restart(&st);
  }

  PROCESS_END();
}
