//    scrutiny_config.h
//        Definition of a run-time configuration of the scrutiny module.
//        To be filled in startup phase
//
//   - License : MIT - See LICENSE file.
//   - Project : Scrutiny Debugger (github.com/scrutinydebugger/scrutiny-embedded)
//
//   Copyright (c) 2021-2022 Scrutiny Debugger

#ifndef ___SCRUTINY_CONFIG_H___
#define ___SCRUTINY_CONFIG_H___

#include "scrutiny_setup.hpp"
#include "scrutiny_types.hpp"

namespace scrutiny
{
    class MainHandler;

    class Config
    {
    public:
        friend class MainHandler;

        Config();
        void clear();
        
        void set_buffers(uint8_t* rx_buffer, const uint16_t rx_buffer_size, uint8_t* tx_buffer, const uint16_t tx_buffer_size);
        void set_forbidden_address_range(const AddressRange* range, const uint8_t count);
        void set_readonly_address_range(const AddressRange* range, const uint8_t count);
        void set_published_values(RuntimePublishedValue* array, uint16_t nbr, RpvReadCallback rd_cb=nullptr, RpvWriteCallback wr_cb=nullptr);
    
        inline bool is_user_command_callback_set() { return user_command_callback != nullptr; }
        inline bool is_buffer_set() {return (m_rx_buffer != nullptr) && (m_tx_buffer != nullptr);}
        inline bool is_forbidden_address_range_set() {return m_forbidden_address_ranges != nullptr;}
        inline bool is_readonly_address_range_set() {return m_readonly_address_ranges != nullptr;}
        inline bool is_read_published_values_configured() { return (m_rpv_read_callback != nullptr &&  m_rpvs != nullptr && m_rpv_count > 0);};
        inline bool is_write_published_values_configured() { return (m_rpv_write_callback != nullptr &&  m_rpvs != nullptr && m_rpv_count > 0);};

        inline const AddressRange* forbidden_ranges() { return m_forbidden_address_ranges; }
        inline uint8_t forbidden_ranges_count() const { return m_forbidden_range_count ; }
        inline const AddressRange* readonly_ranges() { return m_readonly_address_ranges; }
        inline uint8_t readonly_ranges_count() const { return m_readonly_range_count ; }
        inline const RuntimePublishedValue* get_rpvs_array() const {return m_rpvs;}
        inline uint16_t get_rpv_count() const {return m_rpv_count;}
        inline RpvReadCallback get_rpv_read_callback() { return m_rpv_read_callback; }
        inline RpvWriteCallback get_rpv_write_callback() { return m_rpv_write_callback; }

        uint32_t max_bitrate;
        user_command_callback_t user_command_callback;
        uint32_t prng_seed;
        const char* display_name;
        bool memory_write_enable;

    private:
        uint8_t* m_rx_buffer;
        uint16_t m_rx_buffer_size;
        uint8_t* m_tx_buffer;
        uint16_t m_tx_buffer_size;
        const AddressRange* m_forbidden_address_ranges;
        uint8_t m_forbidden_range_count;
        const AddressRange* m_readonly_address_ranges;
        uint8_t m_readonly_range_count;
        const RuntimePublishedValue *m_rpvs;
        uint16_t m_rpv_count;
        RpvReadCallback m_rpv_read_callback;
        RpvWriteCallback m_rpv_write_callback;
    };
}

#endif // ___SCRUTINY_CONFIG_H___