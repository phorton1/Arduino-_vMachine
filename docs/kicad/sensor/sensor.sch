EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
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
L Device:LED D1
U 1 1 6161AFC5
P 2900 2750
F 0 "D1" H 3150 2750 50  0000 C CNN
F 1 "IR Emit" H 2750 2850 50  0000 C CNN
F 2 "cnc3018-PCB:my2pin" H 2900 2750 50  0001 C CNN
F 3 "~" H 2900 2750 50  0001 C CNN
	1    2900 2750
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J1
U 1 1 6161BBEF
P 3150 1950
F 0 "J1" V 3150 2300 50  0000 L CNN
F 1 "Conn_01x03" H 3230 1901 50  0001 L CNN
F 2 "cnc3018-PCB:my3pin" H 3150 1950 50  0001 C CNN
F 3 "~" H 3150 1950 50  0001 C CNN
	1    3150 1950
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R1
U 1 1 6161C011
P 2400 2750
F 0 "R1" V 2400 2300 50  0000 L CNN
F 1 "100" V 2400 2750 50  0000 C CNN
F 2 "cnc3018-PCB:myResistor" V 2330 2750 50  0001 C CNN
F 3 "~" H 2400 2750 50  0001 C CNN
	1    2400 2750
	0    1    1    0   
$EndComp
Wire Wire Line
	2150 3000 2250 3000
Wire Wire Line
	2550 2750 2750 2750
Wire Wire Line
	2250 2750 2150 2750
Wire Wire Line
	2150 2750 2150 3000
Connection ~ 2150 2750
Wire Wire Line
	2650 3500 2750 3500
$Comp
L Amplifier_Operational:LM358 U1
U 3 1 6166B823
P 3050 3600
F 0 "U1" H 4100 3500 50  0001 L CNN
F 1 "LM393" H 4100 3400 50  0001 L CNN
F 2 "cnc3018-PCB:myDip8" H 3050 3600 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm2904-n.pdf" H 3050 3600 50  0001 C CNN
	3    3050 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 3250 2950 3250
Wire Wire Line
	2950 3300 2950 3250
Connection ~ 2950 3250
Wire Wire Line
	2950 3250 2550 3250
Wire Wire Line
	2400 3400 2400 3700
Wire Wire Line
	2250 3250 2150 3250
Wire Wire Line
	2150 3250 2150 3000
Connection ~ 2150 3000
Wire Wire Line
	2150 3250 2150 3900
Wire Wire Line
	2150 3900 2950 3900
Connection ~ 2150 3250
$Comp
L Device:LED D3
U 1 1 616972E4
P 3300 3150
F 0 "D3" V 3350 3450 50  0000 R CNN
F 1 "LED" V 3250 3450 50  0000 R CNN
F 2 "cnc3018-PCB:my2pin" H 3300 3150 50  0001 C CNN
F 3 "~" H 3300 3150 50  0001 C CNN
	1    3300 3150
	0    1    1    0   
$EndComp
$Comp
L Device:D_Photo D2
U 1 1 6161B5B7
P 2850 3000
F 0 "D2" H 3050 3000 50  0000 C CNN
F 1 "IR_Detect" H 2900 2900 50  0000 C CNN
F 2 "cnc3018-PCB:my2pin" H 2800 3000 50  0001 C CNN
F 3 "~" H 2800 3000 50  0001 C CNN
	1    2850 3000
	-1   0    0    1   
$EndComp
$Comp
L Device:R R3
U 1 1 616BC923
P 3300 2750
F 0 "R3" V 3400 2700 50  0000 L CNN
F 1 "220" V 3300 2750 50  0000 C CNN
F 2 "cnc3018-PCB:myResistor" V 3230 2750 50  0001 C CNN
F 3 "~" H 3300 2750 50  0001 C CNN
	1    3300 2750
	1    0    0    -1  
$EndComp
Connection ~ 3050 2750
Wire Wire Line
	3050 2450 2800 2450
Wire Wire Line
	2500 2450 2150 2450
Connection ~ 2150 2450
Wire Wire Line
	2150 2450 2150 2600
Connection ~ 3050 2450
Wire Wire Line
	2150 2250 2150 2450
Wire Wire Line
	3450 3600 3450 3450
Wire Wire Line
	3300 2900 3300 3000
Wire Wire Line
	3300 3300 3300 3450
Wire Wire Line
	3300 3450 3450 3450
Connection ~ 3450 3450
Wire Wire Line
	3050 2750 3050 3000
Connection ~ 3050 3000
Wire Wire Line
	3050 3000 3050 3250
$Comp
L Device:R R2
U 1 1 6161C310
P 2400 3000
F 0 "R2" V 2400 3350 50  0000 L CNN
F 1 "10K" V 2400 3000 50  0000 C CNN
F 2 "cnc3018-PCB:myResistor" V 2330 3000 50  0001 C CNN
F 3 "~" H 2400 3000 50  0001 C CNN
	1    2400 3000
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2750 3000 2650 3000
Wire Wire Line
	2650 3000 2550 3000
Connection ~ 2650 3000
Wire Wire Line
	2650 3000 2650 3500
Wire Wire Line
	3300 2600 2150 2600
Connection ~ 2150 2600
Wire Wire Line
	2150 2600 2150 2750
Wire Wire Line
	3250 2250 3250 2150
Wire Wire Line
	2150 2250 3250 2250
Wire Wire Line
	3450 2300 3150 2300
Wire Wire Line
	3150 2300 3150 2150
$Comp
L Device:R_POT RV1
U 1 1 6161CD8F
P 2400 3250
F 0 "RV1" V 2400 2900 50  0000 R CNN
F 1 "10K" V 2400 3250 50  0000 C CNN
F 2 "cnc3018-PCB:myTrimPot" H 2400 3250 50  0001 C CNN
F 3 "~" H 2400 3250 50  0001 C CNN
	1    2400 3250
	0    1    1    0   
$EndComp
$Comp
L Amplifier_Operational:LM358 U1
U 2 1 616580A4
P 3050 3600
F 0 "U1" H 3200 3350 50  0001 C CNN
F 1 "LM393" H 3050 3600 50  0000 C CNN
F 2 "" H 3050 3600 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm2904-n.pdf" H 3050 3600 50  0001 C CNN
	2    3050 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3350 3600 3450 3600
Wire Wire Line
	2400 3700 2750 3700
Wire Wire Line
	3050 2450 3050 2750
Wire Wire Line
	3050 2150 3050 2450
$Comp
L Device:C C?
U 1 1 6167CA95
P 2650 2450
F 0 "C?" V 2650 1800 50  0000 C CNN
F 1 "100nf" V 2600 2250 50  0000 C CNN
F 2 "" H 2688 2300 50  0001 C CNN
F 3 "~" H 2650 2450 50  0001 C CNN
	1    2650 2450
	0    1    1    0   
$EndComp
Wire Wire Line
	3450 2300 3450 3450
$EndSCHEMATC
