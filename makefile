all: main.cpp
	g++ main.cpp -o main -Wall

update: firmware_0.bin firmware_1.bin firmware_2.bin firmware_3.bin firmware_4.bin 
	sudo cp ./firmware_0.bin /usr/local/bin/emasic_firmware
	sudo cp ./firmware_1.bin /usr/local/bin/emasic_firmware
	sudo cp ./firmware_2.bin /usr/local/bin/emasic_firmware
	sudo cp ./firmware_3.bin /usr/local/bin/emasic_firmware
	sudo cp ./firmware_4.bin /usr/local/bin/emasic_firmware