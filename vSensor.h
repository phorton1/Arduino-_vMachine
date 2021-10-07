#pragma once

// Class and methods for my Infrared "Stop Switches" that
// work with painted white lines on a black 5mm GT2 belt.

#include <cstdint>


#define VSENSOR_BUFSIZE        6
#define VSENSOR_DEFAULT_V_ON   2048     // 2048     // 1.65V
#define VSENSOR_DEFAULT_V_OFF  1900     // 1800     // 1.45V
    // The thresholds are in terms of a 12 bit sample (0..4095)
    // on a 3.3V input pin, so 2048, is for example 1.65 volts.
    // If the value rises above v_on, the state will go high.
    // It must then fall below v_off to go low again (hysterisis).


class vSensor
{
    public:

        vSensor();
        void init(int axis_num, int pin);
        void setThresholds(int v_on, int v_off);
        bool getState() { return m_state; };


    private:

        friend void vSensorTask(void* pvParameters);

        bool pollState();

    private:

        int m_pin;
        int m_axis_num;

        int m_v_on = VSENSOR_DEFAULT_V_ON;
        int m_v_off = VSENSOR_DEFAULT_V_OFF;

        uint32_t m_value = 0;
        bool m_state = false;
        bool m_prev_state = false;

        int m_ptr = 0;
        int m_buf[VSENSOR_BUFSIZE];

};


extern int getVSensorState();

void vSensorTask(void* pvParameters);
