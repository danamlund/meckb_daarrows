
all: qmk_firmware build program

qmk_firmware:
	git clone https://github.com/qmk/qmk_firmware.git

build:
	rm -Rf qmk_firmware/keyboards/daarrows
	cp -Rf daarrows qmk_firmware/keyboards/
	cd qmk_firmware; make daarrows

program:
# pro micro
# note: you have to manually connect RESET and GND a few times before running this
	avrdude -v -patmega32u4 -cavr109 -P/dev/ttyACM1 -b57600 -D \
	        -Uflash:w:qmk_firmware/.build/daarrows_default.hex:i

clean:
	rm -Rf qmk_firmware/.build
