#ifndef ___SCRUTINY_DATALOGGING_DATA_ENCODING_H___
#define ___SCRUTINY_DATALOGGING_DATA_ENCODING_H___

#include "scrutiny_setup.hpp"

#if SCRUTINY_ENABLE_DATALOGGING == 0
#error "Not enabled"
#endif

#if SCRUTINY_DATALOGGING_ENCODING == SCRUTINY_DATALOGGING_ENCODING_RAW
#include "datalogging/scrutiny_datalogger_raw_encoder.hpp"
#endif

namespace scrutiny
{
    namespace datalogging
    {
#if SCRUTINY_DATALOGGING_ENCODING == SCRUTINY_DATALOGGING_ENCODING_RAW
        using DataEncoder = RawFormatEncoder;
        using DataReader = RawFormatReader;
#endif
    }
}

#endif