
#include "vSensor.h"

#if WITH_VMACHINE

#include "v2812b.h"
#include <Report.h>


#define DEBUG_SENSOR  0

// prh Todo - paint outside stops (already used in homing)
// prh Someday - use sensors for hard stops

// GRBL assumes hardware switches for limits with clear ON/OFF states
// I have implemented optical sensors which detect a white strip on a belt
// using analogRead(), along with a moving average and a pair of
// thresholds.  It also needs to be called in a fairly tight timing loop.
//
// limits_get_state() is the issue.
//
// It is generally called as a real-time method, and/or when an ISR is triggered
//
// SOFT_LIMITS appear to check the current GRBL position against the defined
//      numerical limits, with either calls to limits_soft_check() or confusingly
//      also via limitsCheckTravel() which appears to get callled generally
//      and is bound weakly, and which I could easily override to just return
//      false.
//
// If HARD_LIMITS, GRBL will install an interrupt handler for defined limit pins
//      and if CHECK_LIMITS_AT_INIT will call limits_getstate() at startup
// 		and go into an alarm mode if any bit is set
//
// If HARD_LIMIT_FORCE_STATE_CHECK limits_get_state() will be called from the ISR
//      otherwise it just assumes a limit has been hit and in either case
//      calls mc_reset() to stop everything and puts the system in an alarm state with
//      sys_rt_exec_alarm = ExecAlarm::HardLimit
//
// if !HARD_LIMITS then limits_init() merely does a pinMode() on the pin
//
// Funny, the GRBL code still creates the limits_sw_queue and invariantly starts the
// limitCheckTask() that would cycle every 32ms (DEBOUNCE_PERIOD), except that it blocks
// on the limit_sw_queue which will never get anything from the ISR because
// ENABLE_SOFTWARE_DEBOUNCE is not defined.  This is a waste of resources.
//
//      xQueueReceive(limit_sw_queue, &evt, portMAX_DELAY);  // block until receive queue
//
// I have changed Limits.cpp so that  limits_get_state() is weakly bound
// so I can override it.
//
// Even then, the complicated limits_go_home() method, which actually does the seek,
// bounce off, and so on, (a) appears to do X and Y simultaneously and (b) would need
// to be modified to feed out Y when doing X, and vice-versa.  So I had to write my
// own entire homing method in vMachine.cpp.

vSensor x_sensor;
vSensor y_sensor;


int getVSensorState()
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
	v_info("vSensor(%d) started on pin(%d)",axis_num,pin);
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
    // v_debug("vSensor[%s] %d]\r\n",m_axis_num ? "Y" : "X",value);

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
            grbl_sendf(CLIENT_SERIAL, "[MSG: vSensor[%s] %s]\r\n",
                m_axis_num ? "Y" : "X",
                m_state ? "HIGH" : "LOW");
        #endif
    }
    return m_state;
}


//-------------------------------------------------
// vSensorTask
//-------------------------------------------------

#define  VTASK_DELAY_MS  10


void vSensorTask(void* pvParameters)
    // this method maybe should use a global "v_in_homing" boolean,
    // and if not, do the "panic" mc_reset() and set an alarm if a
    // limit is hit.  If not, it shold just keep trucking ...
{
    x_sensor.init(0,X_VLIMIT_PIN);
    y_sensor.init(1,Y_VLIMIT_PIN);

	#ifndef WITH_V2812B
		pinMode(V_LIMIT_LED_PIN,OUTPUT);
		digitalWrite(V_LIMIT_LED_PIN,0);
	#endif

	// for (int i=0; i<7; i++)
	// {
	// 	delay(2000);
	// 	bool on = i & 1;
	// 	useDigitalWrite(V_LIMIT_LED_PIN,on);
	// 	v_info("led %d",on);
	// }

	v_info("vSensorTask running on core %d at priority %d",xPortGetCoreID(),uxTaskPriorityGet(NULL));

    while (true)
    {
        vTaskDelay(VTASK_DELAY_MS / portTICK_PERIOD_MS);
        bool x = x_sensor.pollState();
        bool y = y_sensor.pollState();
        // grbl_sendf(CLIENT_SERIAL, "[MSG: vSensor x(%d) y(%d)]\r\n",x,y);

        bool led_on = x || y;
        static bool led_state = 0;
        if (led_state != led_on)
        {
            led_state = led_on;
			#ifdef WITH_V2812B
				#if DEBUG_SENSOR
					v_debug("setting pixels x=%d y=%d",x,y);
				#endif
				pixels.setPixelColor(PIXEL_LEFT_SENSOR, x ? 0xFF0000 : 0);
				pixels.setPixelColor(PIXEL_RIGHT_SENSOR, y ? 0xFF0000 : 0);
				pixels.show();
			#else
				digitalWrite(V_LIMIT_LED_PIN,led_on);
			#endif
        }

        static UBaseType_t uxHighWaterMark = 0;
#ifdef DEBUG_TASK_STACK
        reportTaskStackSize(uxHighWaterMark);
#endif
    }
}


#endif	// WITH_VMACHINE
