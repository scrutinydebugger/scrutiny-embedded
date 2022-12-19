//    scrutiny_datalogging.hpp
//        The datalogging main include
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_DATALOGGING_H___
#define ___SCRUTINY_DATALOGGING_H___

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

#include "scrutiny_main_handler.hpp"

#include "datalogging/scrutiny_datalogging_types.hpp"
#include "datalogging/scrutiny_datalogger.hpp"
#include "datalogging/scrutiny_datalogging_trigger.hpp"

namespace scrutiny
{
    namespace datalogging
    {
        void convert_to_compare_type(VariableType *vtype, AnyType *val);
        bool fetch_operand(const MainHandler *const main_handler, const Operand *const operand, AnyType *const val, VariableType *const variable_type);
    }
}

#endif // ___SCRUTINY_DATALOGGING_H___