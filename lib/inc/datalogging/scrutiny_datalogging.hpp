//    scrutiny_datalogging.hpp
//        The datalogging main include
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2023 Scrutiny Debugger

#ifndef ___SCRUTINY_DATALOGGING_H___
#define ___SCRUTINY_DATALOGGING_H___

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

#include "datalogging/scrutiny_datalogging_types.hpp"
#include "datalogging/scrutiny_datalogger.hpp"
#include "datalogging/scrutiny_datalogging_trigger.hpp"

namespace scrutiny
{
    class MainHandler;
    namespace datalogging
    {
        /// @brief Converts a value stored as AnyType into AnyTypeCompare which is the same,
        /// but the value in the biggest integer format will be valid
        /// @param vtype The variable type. Will be modified by this function (may convert uint8 to uint32)
        /// @param val The data to be modified. Will mostly writes 0 into uninitialized section so that when passing from uint8 to uint32, both values are equals.
        void convert_to_compare_type(VariableType *vtype, AnyType *val);

        /// @brief Reads an operand used for log trigger
        /// @param main_handler A pointer to the main handler used to fetch the memory and respect forbidden regions
        /// @param operand The operand to read
        /// @param val Output value
        /// @param variable_type Output variable type
        /// @return true on success. false on failure
        bool fetch_operand(const MainHandler *const main_handler, const Operand *const operand, AnyType *const val, VariableType *const variable_type);
    }
}

#endif // ___SCRUTINY_DATALOGGING_H___