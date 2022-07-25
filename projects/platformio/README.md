# platformio example app

## Harware Hookup

Hookup an SSD_1306 SPI old screen to an atmega328p arduino board like so:

```
gpio D0 RX -> usb uart TX
gpio D1 TX -> usb uart RX
gpio D8 -> ssd1306 RESET
gpio D9 -> ssd1306 DC
gpio D10 -> ssd1306 CS
gpio D11 -> ssd1306 MOSI
gpio D12 -> ssd1306 MISO
gpio D13 -> ssd1306 SCK
```

## Compile

```
./scripts/with-venv.sh pio run
```

## Flash

```
./scripts/with-venv.sh pio run --upload-port /dev/cu.usbserial-A5027XFR --target upload
```

## Result

You should see
```
Console ready
Scrutiny ready
0x181
```
On the oled display

Launching the scrutiny-python project server using this command
```
./scripts/with-venv.sh scrutiny launch-server --config config/serial.json
```
should result in a successful connection.
