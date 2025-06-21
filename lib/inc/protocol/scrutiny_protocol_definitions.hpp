//    scrutiny_protocol_definitions.hpp
//        Defines Request/Rsponse objects and commands/subfunctions IDs.
//        Not version specific
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021 Scrutiny Debugger

#ifndef ___SCRUTINY_PROTOCOL_DEFINITION_H___
#define ___SCRUTINY_PROTOCOL_DEFINITION_H___

#include "scrutiny_setup.hpp"

namespace scrutiny
{
    namespace protocol
    {
        struct Request
        {
            void reset()
            {
                command_id = 0;
                subfunction_id = 0;
                data_length = 0;
            }

            uint8_t command_id;
            uint8_t subfunction_id;
            uint16_t data_length;
            uint16_t data_max_length;
            uint8_t *data;
            uint32_t crc;
        };

        struct Response
        {
            void reset()
            {
                command_id = 0;
                subfunction_id = 0;
                response_code = 0;
                data_length = 0;
            }

            uint8_t command_id;
            uint8_t subfunction_id;
            uint8_t response_code;
            uint16_t data_length;
            uint16_t data_max_length;
            uint8_t *data;
            uint32_t crc;
        };

        class CommandId
        {
            public:
            SCRUTINY_ENUM(uint_least8_t)
            {
                GetInfo = 0x01,
                CommControl = 0x02,
                MemoryControl = 0x03,
                UserCommand = 0x04,
                DataLogControl = 0x05
            };
        };

        class ResponseCode
        {
            public:
            SCRUTINY_ENUM(uint_least8_t)
            {
                OK = 0,
                InvalidRequest = 1,
                UnsupportedFeature = 2,
                Overflow = 3,
                Busy = 4,
                FailureToProceed = 5,
                Forbidden = 6,
                NoResponseToSend = 0xFE,
                ProcessAgain = 0xFF
            };
        };

        class RxError
        {
            public:
            SCRUTINY_ENUM(uint_least8_t)
            {
                None,
                Overflow,
                Disabled,
                InvalidCommand
            };
        };

        class TxError
        {
            public:
            SCRUTINY_ENUM(uint_least8_t)
            {
                None,
                Overflow,
                Busy,
                Disabled
            };
        };

        struct Version
        {
            uint8_t major;
            uint8_t minor;
        };

        namespace GetInfo
        {
            class Subfunction
            {
                public:
                SCRUTINY_ENUM(uint_least8_t)
                {
                    GetprotocolVersion = 1,
                    GetSoftwareId = 2,
                    GetSupportedFeatures = 3,
                    GetSpecialMemoryRegionCount = 4,
                    GetSpecialMemoryLocation = 5,
                    GetRuntimePublishedValuesCount = 6,
                    GetRuntimePublishedValuesDefinition = 7,
                    GetLoopCount = 8,
                    GetLoopDefinition = 9
                };
            };

            class MemoryRegionType
            {
                public:
                SCRUTINY_ENUM(uint_least8_t)
                {
                    ReadOnly = 0,
                    Forbidden = 1
                };
            };
        }

        namespace CommControl
        {
            extern uint8_t const DISCOVER_MAGIC[4];
            extern uint8_t const CONNECT_MAGIC[4];

            class Subfunction
            {
                public:
                SCRUTINY_ENUM(uint_least8_t)
                {
                    Discover = 1,
                    Heartbeat = 2,
                    GetParams = 3,
                    Connect = 4,
                    Disconnect = 5
                };
            };
        }

        namespace MemoryControl
        {
            class Subfunction
            {
                public:
                SCRUTINY_ENUM(uint_least8_t)
                {
                    Read = 1,
                    Write = 2,
                    WriteMasked = 3,
                    ReadRPV = 4,
                    WriteRPV = 5
                };
            };
        }

        namespace DataLogControl
        {
            class Subfunction
            {
                public:
                SCRUTINY_ENUM(uint_least8_t)
                {
                    GetSetup = 1,
                    ConfigureDatalog = 2,
                    ArmTrigger = 3,
                    DisarmTrigger = 4,
                    GetStatus = 5,
                    GetAcquisitionMetadata = 6,
                    ReadAcquisition = 7,
                    ResetDatalogger = 8
                };
            };
        }

    }
}

#endif // ___SCRUTINY_PROTOCOL_DEFINITION_H___
