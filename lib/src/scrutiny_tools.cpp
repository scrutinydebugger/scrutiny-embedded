#include <stdint.h>
#include "scrutiny_types.hpp"
#include "scrutiny_tools.hpp"

namespace scrutiny
{
    namespace tools
    {
        VariableTypeSize get_required_type_size(uint_fast8_t newsize)
        {
            if (newsize <= 1)
            {
                return VariableTypeSize::_8;
            }
            else if (newsize <= 2)
            {
                return VariableTypeSize::_16;
            }
            else if (newsize <= 4)
            {
                return VariableTypeSize::_32;
            }
            else if (newsize <= 8)
            {
                return VariableTypeSize::_64;
            }
            else if (newsize <= 16)
            {
                return VariableTypeSize::_128;
            }
            else if (newsize <= 32)
            {
                return VariableTypeSize::_256;
            }
            else
            {
                return VariableTypeSize::_undef;
            }
        }
    }
}