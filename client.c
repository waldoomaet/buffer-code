#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "dev/adxl345.h"
#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"

#define ACCM_READ_INTERVAL CLOCK_SECOND / 100

enum Level
{
	X,
	Y,
	Z
};

/* Declare our "main" process, the client process*/
PROCESS(main, "main");

/* The client process should be started automatically when
 * the node has booted. */
AUTOSTART_PROCESSES(&main);

static void recv(const void *data, uint16_t len,
				 const linkaddr_t *src, const linkaddr_t *dest)
{
	printf("Aparently we got something: ");
	int count = 0;
	while (count < len)
	{
		printf("%d", *data);
		count++;
	}
	printf('\n');
}

static struct etimer et;

PROCESS_THREAD(main, ev, data)
{
	static int16_t axes[3];

	nullnet_buf = (uint8_t *)&axes;
	nullnet_len = sizeof(axes);
	nullnet_set_input_callback(recv);

	PROCESS_BEGIN();
	{
		while (1)
		{
			axes[X] = accm_read_axis(X_AXIS);
			axes[Y] = accm_read_axis(Y_AXIS);
			axes[Z] = accm_read_axis(Z_AXIS);
			printf("x: %d y: %d z: %d\n", axes[X], axes[Y], axes[Z]);

			memcpy(nullnet_buf, &axes, sizeof(axes));
			nullnet_len = sizeof(axes);

			NETSTACK_NETWORK.output(NULL);

			etimer_set(&et, ACCM_READ_INTERVAL);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
		}
	}
	PROCESS_END();
}

/* Our main process. */
// PROCESS_THREAD(client_process, ev, data)
// {
// 	static char payload[] = "hej";

// 	PROCESS_BEGIN();

// 	/* Activate the button sensor. */
// 	SENSORS_ACTIVATE(button_sensor);

// 	/* Initialize NullNet */
// 	nullnet_buf = (uint8_t *)&payload;
// 	nullnet_len = sizeof(payload);
// 	nullnet_set_input_callback(recv);

// 	/* Loop forever. */
// 	while (1)
// 	{
// 		/* Wait until an event occurs. If the event has
// 		 * occured, ev will hold the type of event, and
// 		 * data will have additional information for the
// 		 * event. In the case of a sensors_event, data will
// 		 * point to the sensor that caused the event.
// 		 * Here we wait until the button was pressed. */
// 		PROCESS_WAIT_EVENT_UNTIL(ev == sensors_event &&
// 								 data == &button_sensor);

// 		leds_toggle(LEDS_RED);
// 		/* Copy the string "hej" into the packet buffer. */
// 		memcpy(nullnet_buf, &payload, sizeof(payload));
// 		nullnet_len = sizeof(payload);

// 		/* Send the content of the packet buffer using the
// 		 * broadcast handle. */
// 		NETSTACK_NETWORK.output(NULL);
// 	}

// 	PROCESS_END();
// }
