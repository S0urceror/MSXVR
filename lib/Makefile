all: build

build: msxvr.a

MSXVR.o i2c.o gpio.o: MSXVR.cc MSXVR.hh i2c.cc i2c.hh gpio.cc gpio.hh
	g++ -c -o MSXVR.o MSXVR.cc
	g++ -c -o i2c.o i2c.cc
	g++ -c -o gpio.o gpio.cc

msxvr.a: MSXVR.o i2c.o gpio.o
	ar -r msxvr.a MSXVR.o i2c.o gpio.o
