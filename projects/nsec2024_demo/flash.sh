#/bin/bash
SCRIPT_DIR=$(realpath "$(dirname ${BASH_SOURCE[0]})")
cd "$SCRIPT_DIR" 

source common.sh
BINNAME=scrutiny-nsec2024.hex
BINPATH=./build/$BINNAME
DEFAULT_PORT=/dev/ttyACM0

[ ! -f "$BINPATH" ] && fatal "Missing $BINNAME from build folder"
if [ -z ${ARDUINO_PORT:+x} ]; then
    warn "ARDUINO_PORT not set. Using $DEFAULT_PORT"
    ARDUINO_PORT=$DEFAULT_PORT
fi

avrdude -C "/etc/avrdude.conf" -p atmega2560 -cwiring -P $ARDUINO_PORT -b 115200 -F -D -V -U flash:w:$BINPATH -v 