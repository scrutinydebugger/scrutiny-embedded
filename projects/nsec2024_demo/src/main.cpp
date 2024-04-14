#include <stdio.h>
#include <Arduino.h>
#include "scrutiny.hpp"
#include "scrutiny_integration.hpp"


void toggle_io(){
    static int state=LOW;
    digitalWrite(LED_BUILTIN, state);
    state = (state == LOW) ? HIGH : LOW;
}


void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    nsec2024_demo_config_scrutiny();
}

void task_100hz(){
    static volatile uint32_t var_100hz=0;
    var_100hz++;
    task_100hz_loop_handler.process();
}

void task_1hz(){
    static volatile uint32_t var_1hz=0;
    static int led_state = LOW;
    var_1hz++;

    digitalWrite(LED_BUILTIN, led_state);
    led_state = (led_state == LOW) ? HIGH : LOW;

    task_1hz_loop_handler.process();
}


void loop() {
    static uint32_t last_timestamp_us = micros();
    static uint32_t last_timestamp_task_1hz_us = micros();
    static uint32_t last_timestamp_task_100hz_us = micros();

    uint32_t const timestamp_us = micros();

    if (timestamp_us - last_timestamp_task_100hz_us >= 1e6/100){
        task_100hz();   // Could be in a different thread
        last_timestamp_task_100hz_us = timestamp_us;
    }

    if (timestamp_us - last_timestamp_task_1hz_us >= 1e6/1){
        task_1hz();   // Could be in a different thread
        last_timestamp_task_1hz_us = timestamp_us;
    }

    uint32_t const timediff_100ns { (timestamp_us - last_timestamp_us) * 10};
    task_idle_loop_handler.process(timediff_100ns); // Variable Frequency loop. Need to provide the timestep
    nsec2024_demo_update_scrutiny_main(timediff_100ns);
    last_timestamp_us = timestamp_us;
}