#ifndef SCRUTINY_INTEGRATION_HPP
#define SCRUTINY_INTEGRATION_HPP

#include <stdint.h>
#include "scrutiny.hpp"

void nsec2024_demo_config_scrutiny();
void nsec2024_demo_update_scrutiny_main(uint32_t const diff_time_us);

extern scrutiny::MainHandler scrutiny_main;
extern scrutiny::VariableFrequencyLoopHandler task_idle_loop_handler;
extern scrutiny::FixedFrequencyLoopHandler task_100hz_loop_handler; 
extern scrutiny::FixedFrequencyLoopHandler task_1hz_loop_handler; 

#endif