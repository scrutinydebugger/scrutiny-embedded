#ifndef ___SCRUTINY_DATALOGGER_H___
#define ___SCRUTINY_DATALOGGER_H___

#include "datalogging/scrutiny_datalogging_types.hpp"

namespace scrutiny
{
    namespace datalogging
    {
        class DataLogger
        {
            void configure();
            
        protected:
            uint8_t *m_buffer;
        };
    }
}
#endif // ___SCRUTINY_DATALOGGER_H___