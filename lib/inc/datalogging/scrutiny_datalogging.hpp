//    scrutiny_datalogging.hpp
//        The datalogging main include
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_DATALOGGING_H___
#define ___SCRUTINY_DATALOGGING_H___

#include "scrutiny_main_handler.hpp"

#include "datalogging/scrutiny_datalogging_types.hpp"
#include "datalogging/scrutiny_datalogger.hpp"
#include "datalogging/scrutiny_datalogging_conditions.hpp"

namespace scrutiny
{
    namespace datalogging
    {
        bool fetch_operand(const MainHandler *const main_handler, const Operand *const operand, AnyType *const val, VariableType *const variable_type);
    }
}

#endif // ___SCRUTINY_DATALOGGING_H___