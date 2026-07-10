//    scrutiny_datalogging.hpp
//        The datalogging main include
//
//   - License : MIT - See LICENSE file
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//    Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_DATALOGGING_H___
#define ___SCRUTINY_DATALOGGING_H___

#include "datalogging/scrutiny_datalogger.hpp"
#include "datalogging/scrutiny_datalogging_trigger.hpp"
#include "datalogging/scrutiny_datalogging_types.hpp"
#include "scrutiny_setup.hpp"

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

namespace scrutiny
{
    class MainHandler;
    namespace datalogging
    {
        /// @brief Converts a value stored as AnyType into AnyTypeCompare which is the same,
        /// but the value in the biggest integer format will be valid
        /// @param val_type_pair A data structure containing the variable type and data.
        ///     Type will be modified by this function (may convert uint8 to uint32)
        ///     data will mostly write 0 into uninitialized sections so that when passing from uint8 to uint32, both values
        void convert_to_compare_type(AnyValAndTypePair *const val_type_pair);

        /// @brief Reads an operand used for log trigger
        /// @param main_handler A pointer to the main handler used to fetch the memory and respect forbidden regions
        /// @param operand The operand to read
        /// @param val_type_pair Output value and type
        /// @param caller the calling LoopHandler. Null if done by the MainHandler
        /// @return true on success. false on failure
        bool fetch_operand(
            MainHandler const *const main_handler,
            Operand const *const operand,
            AnyValAndTypePair *const val_type_pair,
            LoopHandler *const caller);
    } // namespace datalogging
} // namespace scrutiny

#endif // ___SCRUTINY_DATALOGGING_H___
