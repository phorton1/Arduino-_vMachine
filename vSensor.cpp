//-------------------------------------
// vSensor.cpp
//-------------------------------------
// I have changed to infrared sensors that can probably
// be used with digitalRead() and it is not clear how
// much I need, if at all, the sample averaging stuff.
// But the new sensors work with this code AS-IS and
// so I am not changing it.  It could be cleaned up.

#include "vSensor.h"
#include "vMachine.h"
#include "my_ws2812b.h"

#include <gStatus.h>			// FluidNC_extensions
#include <FluidDebug.h>         // FluidNC_extensions

#define DEBUG_SENSOR  0

#define  VTASK_DELAY_MS  20

vSensor x_sensor;
vSensor y_sensor;


int getVSensorState()
	// called from homing
{
    bool x = x_sensor.getState();
    bool y = y_sensor.getState();
    return
        (x ? bit(X_AXIS) : 0) |
        (y ? bit(Y_AXIS) : 0);
}



//---------------------------------
// vSensor
//---------------------------------


vSensor::vSensor()
{

}


void vSensor::init(int axis_num, int pin)
{
    m_pin = pin;
    m_axis_num = axis_num;
    pinMode(m_pin,INPUT);
    memset(m_buf,0,VSENSOR_BUFSIZE * sizeof(int));
	g_info("vSensor(%d) started on pin(%d)",axis_num,pin);
}


void vSensor::setThresholds(int v_on, int v_off)
{
    m_v_on = v_on;
    m_v_off = v_off;
}


bool vSensor::pollState()
    // this method should be called 50-100 times a second
    // it returns true if the sensor is high or false if
    // it is low. The client method getState() does NOT
    // poll so-as to preserve timing.
{
    int value = analogRead(m_pin);
    // g_debug("vSensor[%s] %d]\r\n",m_axis_num ? "Y" : "X",value);

    m_value += value;
    m_value -= m_buf[m_ptr];

    m_buf[m_ptr++] = value;
    if (m_ptr == VSENSOR_BUFSIZE)
        m_ptr = 0;

    uint32_t avg = m_value / VSENSOR_BUFSIZE;
    if (m_state)
    {
        if (avg < m_v_off)
            m_state = false;
    }
    else if (avg >= m_v_on)
        m_state = true;

    if (m_prev_state != m_state)
    {
        m_prev_state = m_state;
        #if DEBUG_SENSOR
            g_debug("vSensor[%s] %s",
                m_axis_num ? "Y" : "X",
                m_state ? "HIGH" : "LOW");
        #endif
    }
    return m_state;
}


//------------------------------
// Pixels
//------------------------------

#ifdef WITH_PIXELS
	int getJobStateColor(JobState job_state)
		// duplicated from cnc3018/switches.cpp
	{
		switch (job_state)
		{
			case JOB_NONE:		return 0;
			case JOB_IDLE:		return MY_LED_BLUE;
			case JOB_HOLD:		return MY_LED_CYAN;
			case JOB_BUSY:
			case JOB_HOMING:
			case JOB_PROBING:
			case JOB_MESHING:	return MY_LED_YELLOW;
			case JOB_ALARM:		return MY_LED_RED;
		}
		g_error("UNKNOWN JobState(%d)",(int)job_state);
		return MY_LED_MAGENTA;
	}
#endif


//------------------------------
// vSensorTask
//------------------------------

// #include <Uart.h>


void vSensorTask(void* pvParameters)
    // this method maybe should use a global "v_in_homing" boolean,
    // and if not, do the "panic" mc_reset() and set an alarm if a
    // limit is hit.  If not, it shold just keep trucking ...
{
    x_sensor.init(0,X_VLIMIT_PIN);
    y_sensor.init(1,Y_VLIMIT_PIN);

	#ifdef WITH_PIXELS
		pixels.setBrightness(50);
	#endif

	g_info("vSensorTask running on core %d at priority %d",xPortGetCoreID(),uxTaskPriorityGet(NULL));
	delay(1000);

    while (true)
    {
        vTaskDelay(VTASK_DELAY_MS / portTICK_PERIOD_MS);

		// get and note sensors changing

        bool x = x_sensor.pollState();
        bool y = y_sensor.pollState();

		#if 0
			uint32_t xv = x_sensor.getValue();
			uint32_t yv = y_sensor.getValue();
			Uart0.print(xv);
			Uart0.print(",0,4000,");
			Uart0.println(yv);
		#endif


		#ifdef WITH_PIXELS

			// update gStatus (for jobState)
			// if not WITH_UI

			bool show_leds = false;
			bool sensor_tripped = x || y;
			static bool last_sensor_tripped = 0;
			if (last_sensor_tripped != sensor_tripped)
			{
				last_sensor_tripped = sensor_tripped;

					#if DEBUG_SENSOR
						g_debug("setting pixels x=%d y=%d",x,y);
					#endif

					pixels.setPixelColor(PIXEL_LEFT_SENSOR, x ? MY_LED_RED : 0);
					pixels.setPixelColor(PIXEL_RIGHT_SENSOR, y ? MY_LED_RED : 0);
					show_leds = true;
			}

			#ifndef WITH_UI
				g_status.updateStatus();
			#endif

			static bool led_on = false;
			static uint32_t led_flash = 0;
			static JobState last_job_state = JOB_NONE;
			JobState job_state = g_status.getJobState();

			if (last_job_state != job_state)
			{
				last_job_state = job_state;

				pixels.setPixelColor(PIXEL_SYS_LED,getJobStateColor(job_state));

				if (job_state == JOB_ALARM ||
					job_state == JOB_HOMING ||
					job_state == JOB_PROBING ||
					job_state == JOB_MESHING)
				{
					led_on = true;
					led_flash = millis();
				}
				else
				{
					led_flash = 0;
				}

				show_leds = true;
			}
			else if (led_flash && millis() > led_flash + 300)
			{
				led_flash = millis();
				if (led_on)
				{
					led_on = 0;
					pixels.setPixelColor(PIXEL_SYS_LED, MY_LED_BLACK);
				}
				else
				{
					led_on = 1;
					pixels.setPixelColor(PIXEL_SYS_LED,getJobStateColor(job_state));
				}
				show_leds = true;
			}

			if (show_leds)
				pixels.show();

		#endif	// WITH_PIXELS

	}	// while (true)
}	// vSensorTask()
