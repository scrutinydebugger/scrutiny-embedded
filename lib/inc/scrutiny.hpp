//    scrutiny.hpp
//        Main .h file to be included in a project
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_H___
#define ___SCRUTINY_H___

#include "scrutiny_types.hpp"
#include "scrutiny_setup.hpp"
#include "scrutiny_software_id.hpp"
#include "scrutiny_common_codecs.hpp"
#include "scrutiny_timebase.hpp"
#include "scrutiny_crc.hpp"
#include "scrutiny_main_handler.hpp"
#include "scrutiny_tools.hpp"

#if SCRUTINY_ENABLE_DATALOGGING
#include "datalogging/scrutiny_datalogging.hpp"
#endif

#include "protocol/scrutiny_protocol.hpp"

#endif
