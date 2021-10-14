EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A 8500 11000 portrait
encoding utf-8
Sheet 1 1
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
L cnc3018_Library:BUCK01 M1
U 1 1 616BF0B9
P 2500 3000
F 0 "M1" V 2429 3238 50  0001 L CNN
F 1 "BUCK01" V 2475 3238 50  0000 L CNN
F 2 "cnc3018-PCB:BUCK01" H 2450 3300 50  0001 C CNN
F 3 "" H 2450 3300 50  0001 C CNN
	1    2500 3000
	0    1    1    0   
$EndComp
Wire Wire Line
	2650 2600 2650 3350
Connection ~ 5150 3350
Wire Wire Line
	5150 3500 5150 3350
Wire Wire Line
	3800 3350 3800 3500
Wire Wire Line
	5550 2650 5550 2050
Wire Wire Line
	5450 2650 5450 2050
Wire Wire Line
	5350 2650 5350 2050
Wire Wire Line
	5250 2650 5250 2050
Wire Notes Line
	3550 2600 3550 2700
NoConn ~ 3900 3350
$Comp
L cnc3018_Library:A498_Module M3
U 1 1 618A7124
P 5400 3000
F 0 "M3" H 4900 3000 50  0000 R CNN
F 1 "A4988" H 5500 3000 50  0000 R CNN
F 2 "cnc3018-PCB:a498" H 5400 3600 50  0001 C CNN
F 3 "" H 5400 3600 50  0001 C CNN
	1    5400 3000
	-1   0    0    1   
$EndComp
Wire Wire Line
	5150 3350 5250 3350
NoConn ~ 5350 3350
Wire Wire Line
	5450 3350 5550 3350
$Comp
L Device:CP C2
U 1 1 618A712F
P 3450 2600
F 0 "C2" H 3550 2500 50  0000 R CNN
F 1 "100uf" V 3300 2700 50  0000 R CNN
F 2 "cnc3018-PCB:my2pin" H 3488 2450 50  0001 C CNN
F 3 "~" H 3450 2600 50  0001 C CNN
	1    3450 2600
	0    1    1    0   
$EndComp
$Comp
L Device:CP C3
U 1 1 618A7135
P 4900 2650
F 0 "C3" H 5000 2550 50  0000 R CNN
F 1 "100uf" V 4750 2750 50  0000 R CNN
F 2 "cnc3018-PCB:my2pin" H 4938 2500 50  0001 C CNN
F 3 "~" H 4900 2650 50  0001 C CNN
	1    4900 2650
	0    1    1    0   
$EndComp
Wire Wire Line
	4100 2650 4100 2050
Wire Wire Line
	4000 2650 4000 2050
Wire Wire Line
	3900 2650 3900 2050
Wire Wire Line
	3800 2650 3800 2050
Wire Wire Line
	3700 2300 3700 2650
Connection ~ 3700 2300
Wire Wire Line
	5150 2300 5150 2650
Connection ~ 5150 2300
Connection ~ 3600 2150
Wire Wire Line
	4300 2300 4300 2650
Connection ~ 4300 2300
Wire Wire Line
	5750 2300 5750 2650
Wire Wire Line
	5650 2450 5650 2650
Wire Wire Line
	5650 2450 4550 2450
Wire Wire Line
	4200 2450 4200 2650
Wire Wire Line
	5050 3350 5050 3600
Wire Wire Line
	4750 2650 4750 2300
Connection ~ 4750 2300
Wire Wire Line
	4750 2300 5150 2300
Wire Wire Line
	5050 2650 5050 2150
Wire Wire Line
	3600 2150 3600 2600
Connection ~ 3600 2600
Wire Wire Line
	3600 2600 3600 2650
Connection ~ 5050 2650
Wire Wire Line
	4300 2300 4750 2300
Wire Wire Line
	3600 2150 4750 2150
Wire Wire Line
	4550 2450 4550 3100
Wire Wire Line
	3800 3500 4550 3500
Connection ~ 4550 2450
Wire Wire Line
	4550 2450 4200 2450
Connection ~ 4550 3500
$Comp
L Device:CP C1
U 1 1 618A719B
P 3100 3700
F 0 "C1" V 3250 3650 50  0000 L CNN
F 1 "33uf" V 2950 3600 50  0000 L CNN
F 2 "cnc3018-PCB:my2pin" H 3138 3550 50  0001 C CNN
F 3 "~" H 3100 3700 50  0001 C CNN
	1    3100 3700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3300 2600 3300 2300
Connection ~ 3300 2600
Wire Wire Line
	3300 2300 3700 2300
$Comp
L Connector_Generic:Conn_01x01 V2
U 1 1 618A71BD
P 3600 1950
F 0 "V2" V 3564 1862 50  0001 R CNN
F 1 "v00" H 3800 1950 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 3600 1950 50  0001 C CNN
F 3 "~" H 3600 1950 50  0001 C CNN
	1    3600 1950
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 V4
U 1 1 618A71C3
P 4750 1950
F 0 "V4" V 4714 1862 50  0001 R CNN
F 1 "v01" H 4950 1950 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 4750 1950 50  0001 C CNN
F 3 "~" H 4750 1950 50  0001 C CNN
	1    4750 1950
	0    -1   -1   0   
$EndComp
Connection ~ 4750 2150
Wire Wire Line
	3650 3500 3800 3500
Connection ~ 3800 3500
$Comp
L Connector_Generic:Conn_01x01 V5
U 1 1 618A71D5
P 4750 3100
F 0 "V5" V 4714 3012 50  0001 R CNN
F 1 "v11" H 4800 3200 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 4750 3100 50  0001 C CNN
F 3 "~" H 4750 3100 50  0001 C CNN
	1    4750 3100
	1    0    0    -1  
$EndComp
Connection ~ 4550 3100
$Comp
L Connector_Generic:Conn_01x01 V3
U 1 1 618A71EA
P 3800 3600
F 0 "V3" V 3764 3512 50  0001 R CNN
F 1 "v20" H 4000 3600 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 3800 3600 50  0001 C CNN
F 3 "~" H 3800 3600 50  0001 C CNN
	1    3800 3600
	1    0    0    -1  
$EndComp
Connection ~ 3600 3600
Wire Wire Line
	3600 3600 3600 3700
$Comp
L Connector_Generic:Conn_01x01 V8
U 1 1 618A71F8
P 4850 3600
F 0 "V8" V 4814 3512 50  0001 R CNN
F 1 "v21" H 5050 3600 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 4850 3600 50  0001 C CNN
F 3 "~" H 4850 3600 50  0001 C CNN
	1    4850 3600
	-1   0    0    1   
$EndComp
Wire Wire Line
	4550 3500 5150 3500
Connection ~ 5050 3600
Wire Wire Line
	5050 3600 5050 3700
Connection ~ 3800 3350
Wire Wire Line
	4000 3350 4100 3350
Wire Wire Line
	3700 3350 3800 3350
Wire Wire Line
	3600 3350 3600 3600
$Comp
L Connector_Generic:Conn_01x01 V1
U 1 1 618A7207
P 3450 3500
F 0 "V1" V 3414 3412 50  0001 R CNN
F 1 "v10" H 3500 3600 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 3450 3500 50  0001 C CNN
F 3 "~" H 3450 3500 50  0001 C CNN
	1    3450 3500
	-1   0    0    1   
$EndComp
$Comp
L cnc3018_Library:A498_Module M2
U 1 1 618A720D
P 3950 3000
F 0 "M2" H 3450 3000 50  0000 R CNN
F 1 "A4988" H 4050 3000 50  0000 R CNN
F 2 "cnc3018-PCB:a498" H 3950 3600 50  0001 C CNN
F 3 "" H 3950 3600 50  0001 C CNN
	1    3950 3000
	-1   0    0    1   
$EndComp
Wire Wire Line
	3700 2300 4300 2300
Wire Wire Line
	5150 2300 5750 2300
$Comp
L Connector_Generic:Conn_01x04 J10
U 1 1 619AD3DE
P 3900 1850
F 0 "J10" V 4050 2000 50  0000 R CNN
F 1 "LEFT" V 4050 1800 50  0000 R CNN
F 2 "cnc3018-PCB:my4pin" H 3900 1850 50  0001 C CNN
F 3 "~" H 3900 1850 50  0001 C CNN
	1    3900 1850
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J11
U 1 1 619AFE0D
P 5350 1850
F 0 "J11" V 5500 2000 50  0000 R CNN
F 1 "RIGHT" V 5500 1800 50  0000 R CNN
F 2 "cnc3018-PCB:my4pin" H 5350 1850 50  0001 C CNN
F 3 "~" H 5350 1850 50  0001 C CNN
	1    5350 1850
	0    -1   -1   0   
$EndComp
$Comp
L Switch:SW_SPDT SW1
U 1 1 61D824E9
P 2150 1800
F 0 "SW1" H 2150 2085 50  0000 C CNN
F 1 "ON_OFF" H 2150 2200 50  0000 C CNN
F 2 "cnc3018-PCB:my3pin" H 2150 1800 50  0001 C CNN
F 3 "~" H 2150 1800 50  0001 C CNN
	1    2150 1800
	1    0    0    -1  
$EndComp
$Comp
L Connector:Jack-DC J1
U 1 1 616BF0F3
P 1650 2200
F 0 "J1" H 1707 2525 50  0000 C CNN
F 1 "19.5V" H 1707 2434 50  0000 C CNN
F 2 "cnc3018-PCB:myBarrelJack" H 1700 2160 50  0001 C CNN
F 3 "~" H 1700 2160 50  0001 C CNN
	1    1650 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 1600 2350 1600
Wire Wire Line
	2350 1600 2350 1700
Wire Wire Line
	1900 2100 1950 2100
Wire Wire Line
	1950 2100 1950 1800
Connection ~ 1950 1800
Wire Wire Line
	1900 2300 1950 2300
Connection ~ 1950 2100
Connection ~ 1950 2300
Wire Wire Line
	1950 2300 2650 2300
Connection ~ 3300 2300
Wire Wire Line
	2650 2600 2650 2300
Connection ~ 2650 2600
Wire Wire Line
	2350 1900 2350 2150
Wire Wire Line
	4200 3350 4200 3800
Wire Wire Line
	4200 3800 4000 3800
Wire Wire Line
	2350 2150 3600 2150
Connection ~ 2350 2150
Wire Wire Line
	2350 2150 2350 2600
Wire Wire Line
	2650 2300 3300 2300
Connection ~ 2650 2300
Wire Wire Line
	3600 4950 3400 4950
Wire Wire Line
	6050 4650 5900 4650
$Comp
L Connector_Generic:Conn_01x03 J14
U 1 1 63ACE9B6
P 6250 4750
F 0 "J14" H 6330 4792 50  0000 L CNN
F 1 "Sensor Right" H 6330 4701 50  0000 L CNN
F 2 "cnc3018-PCB:my3pin" H 6250 4750 50  0001 C CNN
F 3 "~" H 6250 4750 50  0001 C CNN
	1    6250 4750
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J13
U 1 1 63A85F26
P 6250 4450
F 0 "J13" H 6330 4492 50  0000 L CNN
F 1 "Sensor Left" H 6330 4401 50  0000 L CNN
F 2 "cnc3018-PCB:my3pin" H 6250 4450 50  0001 C CNN
F 3 "~" H 6250 4450 50  0001 C CNN
	1    6250 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5300 4950 5300 5050
Wire Wire Line
	5700 4950 5300 4950
Wire Wire Line
	5550 5250 5550 5650
Connection ~ 5550 5250
Wire Wire Line
	5700 5250 5550 5250
$Comp
L Device:CP C4
U 1 1 63876129
P 5700 5100
F 0 "C4" H 5818 5146 50  0001 L CNN
F 1 "10Uf" H 5500 5000 50  0000 L CNN
F 2 "cnc3018-PCB:my2pin" H 5738 4950 50  0001 C CNN
F 3 "~" H 5700 5100 50  0001 C CNN
	1    5700 5100
	-1   0    0    1   
$EndComp
Wire Notes Line
	3550 5100 5450 5100
$Comp
L Device:R R1
U 1 1 626031D2
P 5350 7950
F 0 "R1" V 5420 7996 50  0001 C CNN
F 1 "10K" V 5200 7950 50  0000 C TNN
F 2 "cnc3018-PCB:myResistor" V 5280 7950 50  0001 C CNN
F 3 "~" H 5350 7950 50  0001 C CNN
	1    5350 7950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5800 7950 5550 7950
$Comp
L cnc3018_Library:SD_TYP0 M4
U 1 1 626031D9
P 6100 7650
F 0 "M4" V 5900 7750 50  0000 L CNN
F 1 "SD_TYP0" H 6050 7800 50  0000 L CNN
F 2 "cnc3018-PCB:my6Pin" H 5900 7800 50  0001 C CNN
F 3 "" H 5900 7800 50  0001 C CNN
	1    6100 7650
	0    1    1    0   
$EndComp
Wire Wire Line
	5300 6050 5300 6600
Wire Wire Line
	5300 7750 5800 7750
Wire Wire Line
	4700 6050 4700 6900
Wire Wire Line
	4600 7650 5800 7650
Wire Wire Line
	5800 7850 4200 7850
Wire Wire Line
	5400 6500 5400 7450
Wire Wire Line
	5400 7450 5800 7450
Connection ~ 5400 6500
Connection ~ 5300 6600
Wire Wire Line
	5200 6700 5200 6050
Wire Wire Line
	4900 6800 4900 6050
Connection ~ 4700 6900
Connection ~ 4600 7000
Wire Wire Line
	4600 7000 4600 7650
$Comp
L cnc3018_Library:MY_DISPLAY_CONN J12
U 1 1 62B1C242
P 5900 6900
F 0 "J12" V 5991 6622 50  0000 R CNN
F 1 "MY_DISPLAY_CONN" V 5900 6622 50  0000 R CNN
F 2 "cnc3018-PCB:my8Pin" H 5900 6650 50  0001 C CNN
F 3 "" H 5900 6650 50  0001 C CNN
	1    5900 6900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5400 6500 5800 6500
Wire Wire Line
	5300 6600 5800 6600
Wire Wire Line
	5200 6700 5800 6700
Wire Wire Line
	4900 6800 5800 6800
Wire Wire Line
	4700 6900 5800 6900
Wire Wire Line
	4600 7000 5800 7000
Wire Wire Line
	4500 7100 5800 7100
Wire Wire Line
	4700 7550 5200 7550
Wire Wire Line
	5400 5050 5550 5050
Connection ~ 5550 5050
Wire Wire Line
	5550 5050 5550 5250
Wire Wire Line
	5550 7950 5500 7950
Connection ~ 5550 7950
Wire Wire Line
	5200 7950 5200 7550
Connection ~ 5200 7550
Wire Wire Line
	5200 7550 5800 7550
Wire Wire Line
	2350 3950 2750 3950
$Comp
L Connector_Generic:Conn_01x01 J9
U 1 1 618A718B
P 2900 4150
F 0 "J9" V 3000 4150 50  0000 C CNN
F 1 "5VB" V 2900 4350 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 2900 4150 50  0001 C CNN
F 3 "~" H 2900 4150 50  0001 C CNN
	1    2900 4150
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J8
U 1 1 618A7185
P 2750 4150
F 0 "J8" V 2850 4150 50  0000 C CNN
F 1 "5VA" V 2750 4050 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 2750 4150 50  0001 C CNN
F 3 "~" H 2750 4150 50  0001 C CNN
	1    2750 4150
	0    1    1    0   
$EndComp
Wire Wire Line
	2900 3950 3600 3950
Wire Wire Line
	3600 3950 3600 4100
Wire Wire Line
	4300 3350 4300 5050
Wire Wire Line
	4000 3800 4000 5050
Wire Wire Line
	5650 3800 5650 3350
Wire Wire Line
	5750 3900 5750 3350
Wire Wire Line
	5100 4450 6050 4450
Wire Wire Line
	5200 4750 6050 4750
Wire Wire Line
	5550 4550 6050 4550
Connection ~ 5550 4550
Wire Wire Line
	5550 4550 5550 4850
Wire Wire Line
	5550 4850 6050 4850
Connection ~ 5550 4850
Wire Wire Line
	5550 4850 5550 5050
Wire Wire Line
	5900 4650 5900 5550
Wire Wire Line
	5900 5550 5400 5550
Connection ~ 5400 5550
$Comp
L Connector_Generic:Conn_01x01 J5
U 1 1 6458F534
P 2700 4850
F 0 "J5" H 2900 4850 50  0000 R CNN
F 1 "WS2812" H 2950 4850 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 2700 4850 50  0001 C CNN
F 3 "~" H 2700 4850 50  0001 C CNN
	1    2700 4850
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J6
U 1 1 6462A066
P 2700 4950
F 0 "J6" H 2900 4950 50  0000 R CNN
F 1 "5V" H 3000 4950 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 2700 4950 50  0001 C CNN
F 3 "~" H 2700 4950 50  0001 C CNN
	1    2700 4950
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J7
U 1 1 64676964
P 2700 5050
F 0 "J7" H 2900 5050 50  0000 R CNN
F 1 "GND" H 2950 5050 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 2700 5050 50  0001 C CNN
F 3 "~" H 2700 5050 50  0001 C CNN
	1    2700 5050
	-1   0    0    1   
$EndComp
Connection ~ 3600 4950
Wire Wire Line
	3600 4950 3600 5050
Connection ~ 3300 5050
Wire Wire Line
	3300 5050 3300 5550
Wire Wire Line
	2900 5050 3300 5050
$Comp
L Connector_Generic:Conn_01x01 J2
U 1 1 6480A73B
P 2700 4450
F 0 "J2" H 2900 4450 50  0000 R CNN
F 1 "SERVO" H 2950 4450 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 2700 4450 50  0001 C CNN
F 3 "~" H 2700 4450 50  0001 C CNN
	1    2700 4450
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J3
U 1 1 6480A741
P 2700 4550
F 0 "J3" H 2900 4550 50  0000 R CNN
F 1 "5V" H 3000 4550 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 2700 4550 50  0001 C CNN
F 3 "~" H 2700 4550 50  0001 C CNN
	1    2700 4550
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 J4
U 1 1 6480A747
P 2700 4650
F 0 "J4" H 2900 4650 50  0000 R CNN
F 1 "GND" H 2950 4650 50  0000 L CNN
F 2 "cnc3018-PCB:my1pin" H 2700 4650 50  0001 C CNN
F 3 "~" H 2700 4650 50  0001 C CNN
	1    2700 4650
	-1   0    0    1   
$EndComp
Wire Wire Line
	2900 4650 3300 4650
Connection ~ 3300 4650
Wire Wire Line
	3300 4650 3300 5050
Wire Wire Line
	2900 4550 3600 4550
Connection ~ 3600 4550
Wire Wire Line
	3600 4550 3600 4950
Wire Wire Line
	2350 3350 2350 3950
Wire Wire Line
	3300 2600 3300 3700
Wire Wire Line
	3250 3700 3300 3700
Connection ~ 3300 3700
Wire Wire Line
	3300 3700 3300 4650
Wire Wire Line
	2950 3700 2900 3700
Wire Wire Line
	2900 3700 2900 3950
Connection ~ 2900 3950
Wire Wire Line
	5800 7200 3400 7200
Wire Wire Line
	3400 7200 3400 4950
Connection ~ 3400 4950
Wire Wire Line
	3400 4950 2900 4950
Wire Wire Line
	3600 3700 4400 3700
Wire Wire Line
	4400 5050 4400 3700
Connection ~ 4400 3700
Wire Wire Line
	4400 3700 5050 3700
Wire Wire Line
	4500 5050 4500 3800
Wire Wire Line
	4500 3800 5650 3800
Wire Wire Line
	4600 5050 4600 3900
Wire Wire Line
	4600 3900 5750 3900
Wire Wire Line
	4750 2150 4850 2150
Connection ~ 5150 3500
Wire Wire Line
	5150 3500 5550 3500
$Comp
L Connector_Generic:Conn_01x01 V7
U 1 1 64BA6C96
P 4000 6400
F 0 "V7" V 3964 6312 50  0001 R CNN
F 1 "v31" H 4300 6400 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 4000 6400 50  0001 C CNN
F 3 "~" H 4000 6400 50  0001 C CNN
	1    4000 6400
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 V12
U 1 1 64BD4C7E
P 4500 7300
F 0 "V12" V 4464 7212 50  0001 R CNN
F 1 "v51" H 4750 7300 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 4500 7300 50  0001 C CNN
F 3 "~" H 4500 7300 50  0001 C CNN
	1    4500 7300
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 V13
U 1 1 64BD4C84
P 4500 7400
F 0 "V13" V 4464 7312 50  0001 R CNN
F 1 "v52" H 4750 7400 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 4500 7400 50  0001 C CNN
F 3 "~" H 4500 7400 50  0001 C CNN
	1    4500 7400
	-1   0    0    1   
$EndComp
Connection ~ 4200 6400
Wire Wire Line
	4200 6400 4200 6050
Connection ~ 4200 6500
Wire Wire Line
	4200 6500 4200 6400
$Comp
L Connector_Generic:Conn_01x01 V11
U 1 1 64BCE68B
P 4400 6850
F 0 "V11" V 4364 6762 50  0001 R CNN
F 1 "v42" H 4800 6850 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 4400 6850 50  0001 C CNN
F 3 "~" H 4400 6850 50  0001 C CNN
	1    4400 6850
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 V10
U 1 1 64BCE685
P 4400 6750
F 0 "V10" V 4364 6662 50  0001 R CNN
F 1 "v41" H 4800 6750 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 4400 6750 50  0001 C CNN
F 3 "~" H 4400 6750 50  0001 C CNN
	1    4400 6750
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 V9
U 1 1 64BC4C38
P 4000 6500
F 0 "V9" V 3964 6412 50  0001 R CNN
F 1 "v32" H 4300 6500 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 4000 6500 50  0001 C CNN
F 3 "~" H 4000 6500 50  0001 C CNN
	1    4000 6500
	-1   0    0    1   
$EndComp
Wire Wire Line
	4200 7850 4200 6500
Wire Wire Line
	4500 6050 4500 7100
Connection ~ 4600 6750
Wire Wire Line
	4600 6750 4600 6850
Connection ~ 4600 6850
Wire Wire Line
	4600 6850 4600 7000
Wire Wire Line
	4600 6050 4600 6750
Wire Wire Line
	4700 6900 4700 7300
Connection ~ 4700 7300
Wire Wire Line
	4700 7300 4700 7400
Connection ~ 4700 7400
Wire Wire Line
	4700 7400 4700 7550
Wire Wire Line
	5300 6600 5300 7750
Wire Wire Line
	5550 3500 5550 4550
Wire Wire Line
	4550 3100 4550 3500
Wire Wire Line
	4700 5050 4700 4700
Wire Wire Line
	4700 4700 3700 4700
Wire Wire Line
	3700 4700 3700 4600
Wire Wire Line
	3700 4450 2900 4450
Wire Wire Line
	2900 4850 3200 4850
Wire Wire Line
	3200 6150 3900 6150
Wire Wire Line
	3900 6150 3900 6050
$Comp
L Connector_Generic:Conn_01x01 V18
U 1 1 64C87C02
P 3800 4200
F 0 "V18" V 3764 4112 50  0001 R CNN
F 1 "v82" H 4100 4200 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 3800 4200 50  0001 C CNN
F 3 "~" H 3800 4200 50  0001 C CNN
	1    3800 4200
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 V17
U 1 1 64C87C08
P 3800 4100
F 0 "V17" V 3764 4012 50  0001 R CNN
F 1 "v81" H 4100 4100 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 3800 4100 50  0001 C CNN
F 3 "~" H 3800 4100 50  0001 C CNN
	1    3800 4100
	1    0    0    -1  
$EndComp
Connection ~ 3600 4100
Wire Wire Line
	3600 4100 3600 4200
Connection ~ 3600 4200
Wire Wire Line
	3600 4200 3600 4550
$Comp
L Connector_Generic:Conn_01x01 V19
U 1 1 64CAAC74
P 4850 1950
F 0 "V19" V 4814 1862 50  0001 R CNN
F 1 "v02" H 5050 1950 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 4850 1950 50  0001 C CNN
F 3 "~" H 4850 1950 50  0001 C CNN
	1    4850 1950
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x01 V20
U 1 1 64CB1F20
P 4950 1950
F 0 "V20" V 4914 1862 50  0001 R CNN
F 1 "v03" H 5150 1950 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 4950 1950 50  0001 C CNN
F 3 "~" H 4950 1950 50  0001 C CNN
	1    4950 1950
	0    -1   -1   0   
$EndComp
Connection ~ 4850 2150
Wire Wire Line
	4850 2150 4950 2150
Connection ~ 4950 2150
Wire Wire Line
	4950 2150 5050 2150
Wire Wire Line
	5100 4450 5100 5050
Wire Wire Line
	5200 4750 5200 5050
Wire Wire Line
	1950 1600 1950 1800
$Comp
L Connector_Generic:Conn_01x01 V14
U 1 1 64CF9A05
P 5750 5750
F 0 "V14" V 5714 5662 50  0001 R CNN
F 1 "v13" H 6050 5750 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 5750 5750 50  0001 C CNN
F 3 "~" H 5750 5750 50  0001 C CNN
	1    5750 5750
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 V6
U 1 1 64CF9A0B
P 5750 5650
F 0 "V6" V 5714 5562 50  0001 R CNN
F 1 "v12" H 6050 5650 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 5750 5650 50  0001 C CNN
F 3 "~" H 5750 5650 50  0001 C CNN
	1    5750 5650
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 V16
U 1 1 64D00DBF
P 5750 5950
F 0 "V16" V 5714 5862 50  0001 R CNN
F 1 "v15" H 6050 5950 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 5750 5950 50  0001 C CNN
F 3 "~" H 5750 5950 50  0001 C CNN
	1    5750 5950
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 V15
U 1 1 64D00DC5
P 5750 5850
F 0 "V15" V 5714 5762 50  0001 R CNN
F 1 "v14" H 6050 5850 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 5750 5850 50  0001 C CNN
F 3 "~" H 5750 5850 50  0001 C CNN
	1    5750 5850
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 V22
U 1 1 64D08161
P 5750 6150
F 0 "V22" V 5714 6062 50  0001 R CNN
F 1 "v17" H 6050 6150 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 5750 6150 50  0001 C CNN
F 3 "~" H 5750 6150 50  0001 C CNN
	1    5750 6150
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 V21
U 1 1 64D08167
P 5750 6050
F 0 "V21" V 5714 5962 50  0001 R CNN
F 1 "v16" H 6050 6050 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 5750 6050 50  0001 C CNN
F 3 "~" H 5750 6050 50  0001 C CNN
	1    5750 6050
	1    0    0    -1  
$EndComp
Connection ~ 5550 5750
Wire Wire Line
	5550 5750 5550 5850
Connection ~ 5550 5850
Wire Wire Line
	5550 5850 5550 5950
Connection ~ 5550 5950
Wire Wire Line
	5550 5950 5550 6050
Connection ~ 5550 6050
Wire Wire Line
	5550 6050 5550 6150
Connection ~ 5550 6150
Wire Wire Line
	5350 3350 5350 3000
Wire Wire Line
	5350 3000 5750 3000
Wire Wire Line
	5750 3000 5750 2650
Connection ~ 5750 2650
Wire Wire Line
	3900 3350 3900 3000
Wire Wire Line
	3900 3000 4300 3000
Wire Wire Line
	4300 3000 4300 2650
Connection ~ 4300 2650
Connection ~ 5550 5650
Wire Wire Line
	5550 5650 5550 5750
Wire Wire Line
	5550 6150 5550 6250
$Comp
L Connector_Generic:Conn_01x01 V24
U 1 1 64D32573
P 5750 6350
F 0 "V24" V 5714 6262 50  0001 R CNN
F 1 "v19" H 6050 6350 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 5750 6350 50  0001 C CNN
F 3 "~" H 5750 6350 50  0001 C CNN
	1    5750 6350
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 V23
U 1 1 64D32579
P 5750 6250
F 0 "V23" V 5714 6162 50  0001 R CNN
F 1 "v18" H 6050 6250 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 5750 6250 50  0001 C CNN
F 3 "~" H 5750 6250 50  0001 C CNN
	1    5750 6250
	1    0    0    -1  
$EndComp
Connection ~ 5550 6250
Wire Wire Line
	5550 6250 5550 6350
Connection ~ 5550 6350
Wire Wire Line
	5550 6350 5550 7950
Wire Wire Line
	4800 6050 4800 5550
Wire Wire Line
	3300 5550 4100 5550
Connection ~ 4800 5550
$Comp
L Connector_Generic:Conn_01x01 V25
U 1 1 64D602CC
P 3900 4600
F 0 "V25" V 3864 4512 50  0001 R CNN
F 1 "v101" H 4200 4600 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 3900 4600 50  0001 C CNN
F 3 "~" H 3900 4600 50  0001 C CNN
	1    3900 4600
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x01 V26
U 1 1 64D602D2
P 3900 4500
F 0 "V26" V 3864 4412 50  0001 R CNN
F 1 "v102" H 4200 4500 50  0000 R CNN
F 2 "cnc3018-PCB:my1pin" H 3900 4500 50  0001 C CNN
F 3 "~" H 3900 4500 50  0001 C CNN
	1    3900 4500
	1    0    0    -1  
$EndComp
Connection ~ 3700 4500
Wire Wire Line
	3700 4500 3700 4450
Connection ~ 3700 4600
Wire Wire Line
	3700 4600 3700 4500
Wire Wire Line
	3200 4850 3200 6150
Wire Wire Line
	4100 5050 4100 5550
Connection ~ 4100 5550
Wire Wire Line
	5400 6050 5400 6500
Wire Wire Line
	5400 6050 5400 5550
Connection ~ 5400 6050
$Comp
L cnc3018_Library:ESP32_DEV U1
U 1 1 6184012B
P 4400 5600
F 0 "U1" H 4250 5800 50  0000 L CNN
F 1 "ESP32_DEV" H 4850 5800 50  0000 R CNN
F 2 "cnc3018-PCB:ESP32-DEV" H 4050 5700 50  0001 C CNN
F 3 "" H 4050 5700 50  0001 C CNN
	1    4400 5600
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 5550 5400 5550
Wire Wire Line
	4100 5550 4800 5550
Wire Wire Line
	6050 4350 5900 4350
Wire Wire Line
	5900 4350 5900 4650
Connection ~ 5900 4650
$EndSCHEMATC
