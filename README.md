# Scrutiny Embedded
Scrutiny Debugger embedded C++ instrumentation library.
This library must be linked against your C++ project and called periodically.

The library has 2 data streams going in and out. It is the responsability of the integrator to bring the data to and from a hardware communication device such as a Serial Port, a CAN bus, IP stack or any other. 

For the integration guide, see : https://scrutinydebugger.com/guide-instrumentation.html

## Example of integration
The following example is taken from the Arduino example and show how to initialize the library and how the glue code between Scrutiny Embedded Lib and a serial port can be written.

```c++
#include <scrutiny.h>
#include <stdint.h>

scrutiny::MainHandler scrutiny_handler; // Main scrutiny handler

uint8_t scrutiny_rx_buffer[32];   // Receive buffer - Keep global
uint8_t scrutiny_tx_buffer[48];   // Transmit buffer - Keep global

void process_scrutiny_loop()
{
    static uint32_t last_call_us = 0;
  
    // Compute time difference
    uint32_t current_us = micros(); // Reads microseconds
    uint32_t timestep_us = current_us - last_call_us;
    
    // Receive data
    int16_t c = Serial.read();
    if (c != -1)
    {
        uint8_t uc = static_cast<uint8_t>(c);
        scrutiny_handler.receive_data(&uc, 1);  // Data from Serial port pushed into scrutiny-embedded lib
    }
    
    scrutiny_handler.process(timestep_us * 10); // Timesteps are counted in multiple of 100ns
    
    // Sends data
    uint8_t buffer[16];
    if (scrutiny_handler.data_to_send() > 0)
    {
        uint16_t nread = scrutiny_handler.pop_data(buffer, sizeof(buffer));  // Reads data from scrutiny lib
        Serial.write(buffer, nread);                         // Sends data to the serial port
    }


    last_call_us = current_us;  
}


void scrutiny_configure()
{
  scrutiny::Config scrutiny_config;   // Scrutiny runtime configuration. Can be local, will be copied

  // Only required configuration is the comm buffers
  config.set_buffers(scrutiny_rx_buffer, sizeof(scrutiny_rx_buffer), scrutiny_tx_buffer, sizeof(scrutiny_tx_buffer));

  config.max_bitrate = 100000;      // Optional bitrate limit
  config.display_name = "MyDevice"; // Optional name for broadcasting
  /* 
    Multiple additional configurations to control:
    - Memory regions access (forbidden and read-only)
    - Configure datalogging
    - Declare some Runtime Published Values (values identified by a unique ID handled by the app without debug symbols)
    - Configure User Command service
    - etc.
  */
  scrutiny_handler.init(&config);
}


void main()
{
  scrutiny_configure();
  // Rest of Application init code
  while (true)
  {
    process_scrutiny_loop();
    // Rest of Application loop code
  }
}

```
