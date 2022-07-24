//    scrutiny_loop_handler.cpp
//        LoopHandler implementation.
//        Loop Handler is to be run in a specific time domain and will make some features available
//        that depends on the execution requency such as embedded datalogging
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include "scrutiny_loop_handler.h"


namespace scrutiny
{

    LoopHandler::LoopHandler(LoopType type, float frequency) :
        m_loop_type(type),
        m_frequency(frequency)
    {

    }

    void LoopHandler::init()
    {

    }

    void LoopHandler::process()
    {

    }

}