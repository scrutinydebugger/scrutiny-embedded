//    scrutiny_datalogger.cpp
//        The implementation of the datalogger object that should exist in each loop
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#include "datalogging/scrutiny_datalogger.hpp"
#include "datalogging/scrutiny_datalogging.hpp"
#include "scrutiny_ipc.hpp"

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

namespace scrutiny
{
    namespace datalogging
    {

        bool DataLogger::check_trigger()
        {
            static_assert(MAX_OPERANDS >= 2, "Expect at least 2 operands for relational comparison");

            AnyType opvals[MAX_OPERANDS];
            VariableType optypes[MAX_OPERANDS];
            if (!m_configured)
            {
                return false;
            }

            const unsigned int nb_operand = m_active_condition->get_operand_count();

            if (nb_operand > MAX_OPERANDS)
            {
                return false;
            }

            for (unsigned int i = 0; i < nb_operand; i++)
            {
                if (fetch_operand(m_main_handler, &m_operands[i], &opvals[i], &optypes[i]) == false)
                {
                    return false;
                }
                convert_to_compare_type(&optypes[i], &opvals[i]);
            }

            return m_active_condition->evaluate(reinterpret_cast<VariableTypeCompare *>(optypes), reinterpret_cast<AnyTypeCompare *>(opvals));
        }
    }
}