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
L Device:LED D?
U 1 1 6161AFC5
P 2900 2800
F 0 "D?" H 2800 2900 50  0000 C CNN
F 1 "LED" H 2950 2900 50  0000 C CNN
F 2 "" H 2900 2800 50  0001 C CNN
F 3 "~" H 2900 2800 50  0001 C CNN
	1    2900 2800
	1    0    0    -1  
$EndComp
$Comp
L Device:D_Photo D?
U 1 1 6161B5B7
P 2950 3050
F 0 "D?" H 2800 2950 50  0000 C CNN
F 1 "D_Photo" H 3050 2950 50  0000 C CNN
F 2 "" H 2900 3050 50  0001 C CNN
F 3 "~" H 2900 3050 50  0001 C CNN
	1    2950 3050
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J?
U 1 1 6161BBEF
P 3150 2400
F 0 "J?" H 3230 2442 50  0000 L CNN
F 1 "Conn_01x03" H 3230 2351 50  0000 L CNN
F 2 "" H 3150 2400 50  0001 C CNN
F 3 "~" H 3150 2400 50  0001 C CNN
	1    3150 2400
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R?
U 1 1 6161C011
P 2400 2800
F 0 "R?" H 2470 2846 50  0000 L CNN
F 1 "100" V 2400 2800 50  0000 C CNN
F 2 "" V 2330 2800 50  0001 C CNN
F 3 "~" H 2400 2800 50  0001 C CNN
	1    2400 2800
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 6161C310
P 2400 3050
F 0 "R?" H 2470 3096 50  0000 L CNN
F 1 "220" V 2400 3050 50  0000 C CNN
F 2 "" V 2330 3050 50  0001 C CNN
F 3 "~" H 2400 3050 50  0001 C CNN
	1    2400 3050
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_POT RV?
U 1 1 6161CD8F
P 2000 3050
F 0 "RV?" H 1931 3096 50  0000 R CNN
F 1 "50K" V 2000 3050 50  0000 C CNN
F 2 "" H 2000 3050 50  0001 C CNN
F 3 "~" H 2000 3050 50  0001 C CNN
	1    2000 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3050 2600 3050 2800
Connection ~ 3050 2800
Wire Wire Line
	3050 2800 3050 3050
Wire Wire Line
	3150 2600 3150 2650
Wire Wire Line
	2650 3300 2650 3050
Wire Wire Line
	2650 3050 2550 3050
Wire Wire Line
	2750 3050 2650 3050
Connection ~ 2650 3050
Wire Wire Line
	2150 3050 2250 3050
Wire Wire Line
	2000 2900 2000 2650
Wire Wire Line
	3250 2600 3250 3300
Wire Wire Line
	2650 3300 3250 3300
Wire Wire Line
	2250 2800 2250 2650
Wire Wire Line
	2000 2650 2250 2650
Connection ~ 2250 2650
Wire Wire Line
	2250 2650 3150 2650
Wire Wire Line
	2550 2800 2750 2800
$EndSCHEMATC
