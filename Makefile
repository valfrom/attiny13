CPU=attiny13
PORT=/dev/cu.usbmodem1411
AVRDUDE=/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude
CONF=/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf
AVRGCC=/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avr-gcc
AVROBJCOPY=/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avr-objcopy
AVRSIZE=/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avr-size
PROGRAMMER=-cstk500v1 -P $(PORT) -b19200

all: main.o
	$(AVRGCC) -mmcu=$(CPU) main.o -o out.bin
	$(AVROBJCOPY) -j .text -O ihex out.bin out.hex
	$(AVRSIZE) out.bin 

burn:	all
	$(AVRDUDE) -C $(CONF) -q -q -p $(CPU) $(PROGRAMMER) -Uflash:w:out.hex:i

fuse:
	$(AVRDUDE) -C $(CONF) -p $(CPU) $(PROGRAMMER) -e -Uhfuse:w:0xff:m -Ulfuse:w:0x29:m

main.o: main.cpp
	$(AVRGCC) -mmcu=$(CPU) -D__DELAY_BACKWARD_COMPATIBLE__ -DF_CPU=600000 -c main.cpp -Os

clean:
	rm -rf *.o out.bin out.hex
