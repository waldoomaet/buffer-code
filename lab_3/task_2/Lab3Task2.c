#include "contiki.h"
#include <stdlib.h>
#include "sys/node-id.h"
#include "sys/log.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/ipv6/uip-sr.h"
#include "net/mac/tsch/tsch.h"
#include "net/routing/routing.h"
#include "dev/leds.h"
#define DEBUG DEBUG_PRINT
#include "net/ipv6/uip-debug.h"

#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_DBG

/*---------------------------------------------------------------------------*/
PROCESS(node_process, "RPL Node");
AUTOSTART_PROCESSES(&node_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(node_process, ev, data)
{
  static int is_coordinator;

  PROCESS_BEGIN();

  is_coordinator = 0;

  is_coordinator = (node_id == 1);

  if (is_coordinator)
  {
    NETSTACK_ROUTING.root_start();
  }

  NETSTACK_MAC.on();

  {
    static struct etimer et;
    /* Print out routing tables every minute */
    etimer_set(&et, CLOCK_SECOND * 4);
    while (1)
    {
      int routes_num = uip_ds6_route_num_routes();
      LOG_INFO("Routing entries: %u\n", routes_num);
      uip_ds6_route_t *route = uip_ds6_route_head();
      uip_ds6_nbr_t *nbr = uip_ds6_nbr_head();
      if (nbr == NULL)
      {
        leds_off(LEDS_RED);
        leds_off(LEDS_YELLOW);
        leds_off(LEDS_GREEN);
      }
      else if (is_coordinator) // well... master
      {
        leds_off(LEDS_RED);
        leds_off(LEDS_YELLOW);
        leds_on(LEDS_GREEN);
      }
      else if (routes_num == 0) // if it has no routes it means that is and endpoint
      {
        leds_off(LEDS_YELLOW);
        leds_off(LEDS_GREEN);
        leds_on(LEDS_RED);
        while (nbr)
        {
          nbr = uip_ds6_nbr_next();
          if (nbr)
          {
            LOG_INFO("Route: ");
            LOG_INFO_6ADDR(nbr);
            LOG_INFO("\n");
          }
        }
      }
      else
      { // otherwise is an intermediate node
        leds_off(LEDS_GREEN);
        leds_off(LEDS_RED);
        leds_on(LEDS_YELLOW);
      }

      while (route)
      {
        LOG_INFO("Route ");
        LOG_INFO_6ADDR(&route->ipaddr);
        LOG_INFO_(" via ");
        LOG_INFO_6ADDR(uip_ds6_route_nexthop(route));
        LOG_INFO_("\n");
        route = uip_ds6_route_next(route);
      }
      PROCESS_YIELD_UNTIL(etimer_expired(&et));
      etimer_reset(&et);
    }
  }

  PROCESS_END();
}
