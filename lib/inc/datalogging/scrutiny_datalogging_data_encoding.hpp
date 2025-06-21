//    scrutiny_datalogging_data_encoding.hpp
//        File to be included to get access to the datalogging encoders/readers. Abstract the
//        handling of different encoding.
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

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
        typedef RawFormatEncoder DataEncoder;
        typedef RawFormatReader DataReader;
#endif
    }
}

#endif
