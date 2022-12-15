#ifndef ___SCRUTINY_DATALOGGING_CONDITIONS_H___
#define ___SCRUTINY_DATALOGGING_CONDITIONS_H___

#include <stdarg.h>
#include "datalogging/scrutiny_datalogging_types.hpp"

namespace scrutiny
{
    namespace datalogging
    {
        namespace conditions
        {
            bool equal(Operand *operand1, Operand *operand2);

        }
    }
}

#endif // ___SCRUTINY_DATALOGGING_CONDITIONS_H___