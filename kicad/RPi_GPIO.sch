EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:RPi_Hat-cache
LIBS:emeb_library
LIBS:icehat-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 4
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L RPi_GPIO J2
U 1 1 5516AE26
P 4650 2800
F 0 "J2" H 5400 3050 60  0000 C CNN
F 1 "RPi_GPIO" H 5400 2950 60  0000 C CNN
F 2 "emeb:Pin_Header_Straight_2x20" H 4650 2800 60  0001 C CNN
F 3 "" H 4650 2800 60  0000 C CNN
	1    4650 2800
	1    0    0    -1  
$EndComp
Text Notes 4950 5100 0    60   Italic 0
Thru-Hole Connector
Text GLabel 7000 3500 2    60   Input ~ 0
GPIO23_CDN
Text GLabel 7000 3600 2    60   Input ~ 0
GPIO24_CRS
Text GLabel 7000 3800 2    60   Input ~ 0
GPIO25_SS
$Comp
L GND #PWR01
U 1 1 57F6FA86
P 6700 4950
F 0 "#PWR01" H 6700 4700 50  0001 C CNN
F 1 "GND" H 6700 4800 50  0000 C CNN
F 2 "" H 6700 4950 50  0000 C CNN
F 3 "" H 6700 4950 50  0000 C CNN
	1    6700 4950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 57F6FA9C
P 4300 5000
F 0 "#PWR02" H 4300 4750 50  0001 C CNN
F 1 "GND" H 4300 4850 50  0000 C CNN
F 2 "" H 4300 5000 50  0000 C CNN
F 3 "" H 4300 5000 50  0000 C CNN
	1    4300 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	6350 3000 6700 3000
Wire Wire Line
	6700 3000 6700 4950
Wire Wire Line
	6350 3400 6700 3400
Connection ~ 6700 3400
Wire Wire Line
	6350 3700 6700 3700
Connection ~ 6700 3700
Wire Wire Line
	6350 4200 6700 4200
Connection ~ 6700 4200
Wire Wire Line
	6350 4400 6700 4400
Connection ~ 6700 4400
Wire Wire Line
	4450 3200 4300 3200
Wire Wire Line
	4300 3200 4300 5000
Wire Wire Line
	4450 4700 4300 4700
Connection ~ 4300 4700
Wire Wire Line
	4450 4000 4300 4000
Connection ~ 4300 4000
Wire Wire Line
	6350 3500 7000 3500
Wire Wire Line
	6350 3600 7000 3600
Wire Wire Line
	6350 3800 7000 3800
Text GLabel 7000 3900 2    60   Input ~ 0
SPI_CE0
Wire Wire Line
	6350 3900 7000 3900
Text GLabel 3850 3700 0    60   Input ~ 0
SPI_MOSI
Text GLabel 3850 3800 0    60   Input ~ 0
SPI_MISO
Text GLabel 3800 3900 0    60   Input ~ 0
SPI_CLK
Wire Wire Line
	4450 3900 3800 3900
Wire Wire Line
	4450 3800 3850 3800
Wire Wire Line
	4450 3700 3850 3700
Text GLabel 7000 4700 2    60   Input ~ 0
I2S_DO
Wire Wire Line
	7000 4700 6350 4700
Text GLabel 7000 3300 2    60   Input ~ 0
I2S_BCLK
Wire Wire Line
	7000 3300 6350 3300
$Comp
L +5V #PWR03
U 1 1 57F6FD1C
P 6700 2600
F 0 "#PWR03" H 6700 2450 50  0001 C CNN
F 1 "+5V" H 6700 2740 50  0000 C CNN
F 2 "" H 6700 2600 50  0000 C CNN
F 3 "" H 6700 2600 50  0000 C CNN
	1    6700 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 2900 6350 2900
Wire Wire Line
	6700 2600 6700 2900
Wire Wire Line
	6350 2800 6700 2800
Connection ~ 6700 2800
Text GLabel 3850 4500 0    60   Input ~ 0
I2S_LRCK
Wire Wire Line
	3850 4500 4450 4500
Text GLabel 7000 4600 2    60   Input ~ 0
I2S_DI
Wire Wire Line
	6350 4600 7000 4600
$EndSCHEMATC
