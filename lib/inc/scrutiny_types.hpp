//    scrutiny_types.h
//        Aliases on the types used by the Scrutiny project
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_TYPES_H___
#define ___SCRUTINY_TYPES_H___

namespace scrutiny 
{

	typedef unsigned int loop_id_t;

    typedef void (*user_command_callback_t)(const uint8_t subfunction, const uint8_t* request_data, const uint16_t request_data_length, uint8_t* response_data, uint16_t* response_data_length, const uint16_t response_max_data_length);
    typedef void (*rpv_read_callback)(const uint16_t rpv_id, int* length);
    typedef void (*rpv_write_callback)(const uint16_t rpv_id, int length, uint8_t *data);
    
    // This enum must match the definition in the python core module.
    enum class VariableType 
    {
        sint8 = 0,
        sint16 = 1,
        sint32 = 2,
        sint64 = 3,
        sint128 = 4,
        sint256 = 5,
        uint8 = 10,
        uint16 = 11,
        uint32 = 12,
        uint64 = 13,
        uint128 = 14,
        uint256 = 15,
        float8 = 20,
        float16 = 21,
        float32 = 22,
        float64 = 23,
        float128 = 24,
        float256 = 25,
        cfloat8 = 30,
        cfloat16 = 31,
        cfloat32 = 32,
        cfloat64 = 33,
        cfloat128 = 34,
        cfloat256 = 35,
        boolean = 40
    };

    struct RuntimePublishedValue
    {
        public:
            uint16_t id;
            VariableType type;
            void* addr;
    };

};

#endif   //  ___SCRUTINY_TYPES_H___