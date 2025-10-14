all: main.cpp firmware_0.bin firmware_1.bin firmware_2.bin firmware_3.bin firmware_4.bin 
	g++ main.cpp -o main -Wall
	./main > salida.txt
	sudo cp ./firmware_0.bin /usr/local/bin/emasic_firmware
	sudo cp ./firmware_1.bin /usr/local/bin/emasic_firmware
	sudo cp ./firmware_2.bin /usr/local/bin/emasic_firmware
	sudo cp ./firmware_3.bin /usr/local/bin/emasic_firmware
	sudo cp ./firmware_4.bin /usr/local/bin/emasic_firmware