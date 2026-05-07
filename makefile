all: main.cpp firmware_0.bin firmware_1.bin firmware_2.bin firmware_3.bin firmware_4.bin 
	g++ -std=c++20 main.cpp -o main -Wall
	./main > salida.txt