/* A Simple Accelerometer Example
 *
 * Values only in the x-axis are detected in the following example.
 * For your Lab1, use extend the code for Y and Z axes.
 * Finally, interface them with a button so that the sensing starts onlt after the press of a button.
 *
 */
 
#include "UserButton.h"
#include "printfZ1.h"


module T1C @safe()
{
  	uses interface Leds;
  	uses interface Boot;

  	/* We use millisecond timer to check the shaking of client.*/
	uses interface Timer<TMilli> as TimerAccel;

  	/*Accelerometer Interface*/
	uses interface Read<uint16_t> as Xaxis;
	uses interface Read<uint16_t> as Yaxis;
	uses interface Read<uint16_t> as Zaxis;
	uses interface SplitControl as AccelControl;
}


implementation
{
    uint16_t error=100; //Set the error value
    uint16_t x;
	uint16_t y;
	uint16_t z;

    event void Boot.booted() 
    {
		printfz1_init();
		x = 0;
		y = 0;
		z = 0;
		call AccelControl.start();
   		call TimerAccel.startPeriodic(100);
    }

    event void AccelControl.startDone(error_t err)
	{
		printfz1("Accelerometer Started!\n");
	}

	event void AccelControl.stopDone(error_t err) 
	{
		printfz1("Accelerometer Stopped!\n");

	}

	event void TimerAccel.fired()
	{
		call Xaxis.read();
	}

    event void Xaxis.readDone(error_t result, uint16_t uData)
	{
		int16_t data = (int16_t)uData;
		printfz1("X -> (%d)\n", data);
		if (abs(x - data) > error) 
    	{
      		call Leds.led0On();
		}
    	else
    	{
      		call Leds.led0Off();
    	}
		x = data;
		call Yaxis.read();
	}

	event void Yaxis.readDone(error_t result, uint16_t uData)
	{
		int16_t data = (int16_t)uData;
		printfz1("Y -> (%d)\n", data);
		if (abs(y - data) > error) 
    	{
      		call Leds.led1On();
		}
    	else
    	{
      		call Leds.led1Off();
    	}
		y = data;
		call Zaxis.read();
	}

	event void Zaxis.readDone(error_t result, uint16_t uData)
	{
		int16_t data = (int16_t)uData;
		printfz1("Z -> (%d)\n\n\n", data);
		if (abs(z - data) > error) 
    	{
      		call Leds.led2On();
		}
    	else
    	{
      		call Leds.led2Off();
    	}
		z = data;
	}
}

