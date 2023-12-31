EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "PS/2 C128 Keyboard Adapter"
Date "2021-10-09"
Rev ""
Comp "YTM Enteprises"
Comment1 "ytm@elysium.pl"
Comment2 "Maciej Witkowiak"
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L promicro:ProMicro U3
U 1 1 615F6F02
P 8250 3700
F 0 "U3" H 8250 4737 60  0000 C CNN
F 1 "ProMicro" H 8250 4631 60  0000 C CNN
F 2 "promicro:ProMicro" H 8350 2650 60  0001 C CNN
F 3 "" H 8350 2650 60  0000 C CNN
	1    8250 3700
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0101
U 1 1 615FF623
P 4650 2050
F 0 "#PWR0101" H 4650 1900 50  0001 C CNN
F 1 "VCC" H 4665 2223 50  0000 C CNN
F 2 "" H 4650 2050 50  0001 C CNN
F 3 "" H 4650 2050 50  0001 C CNN
	1    4650 2050
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0102
U 1 1 615FFEB0
P 9100 2050
F 0 "#PWR0102" H 9100 1900 50  0001 C CNN
F 1 "VCC" H 9115 2223 50  0000 C CNN
F 2 "" H 9100 2050 50  0001 C CNN
F 3 "" H 9100 2050 50  0001 C CNN
	1    9100 2050
	1    0    0    -1  
$EndComp
$Comp
L mt8808:MT8808 U2
U 1 1 6160A262
P 4650 3300
F 0 "U2" H 4650 2237 60  0000 C CNN
F 1 "MT8808" H 4650 2343 60  0000 C CNN
F 2 "Housings_DIP:DIP-28_W15.24mm_Socket" H 4600 4000 60  0001 C CNN
F 3 "https://www.mouser.com/pdfdocs/Mt8808_DataSheet.PDF" H 4600 4000 60  0001 C CNN
	1    4650 3300
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR0104
U 1 1 61602A0F
P 4750 4650
F 0 "#PWR0104" H 4750 4400 50  0001 C CNN
F 1 "GND" H 4755 4477 50  0000 C CNN
F 2 "" H 4750 4650 50  0001 C CNN
F 3 "" H 4750 4650 50  0001 C CNN
	1    4750 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	4750 4450 4750 4650
Wire Wire Line
	4550 4150 4550 4450
Wire Wire Line
	4550 4450 4750 4450
Connection ~ 4750 4450
Wire Wire Line
	4650 2450 4650 2050
Wire Wire Line
	8950 3250 9100 3250
Wire Wire Line
	9100 3250 9100 2550
$Comp
L power:GND #PWR0105
U 1 1 61605744
P 7150 4600
F 0 "#PWR0105" H 7150 4350 50  0001 C CNN
F 1 "GND" H 7155 4427 50  0000 C CNN
F 2 "" H 7150 4600 50  0001 C CNN
F 3 "" H 7150 4600 50  0001 C CNN
	1    7150 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	7550 3150 7150 3150
Wire Wire Line
	7150 3150 7150 3250
Wire Wire Line
	7550 3250 7150 3250
Connection ~ 7150 3250
Wire Wire Line
	7150 3250 7150 4600
Wire Wire Line
	7550 3950 5850 3950
Wire Wire Line
	5850 3950 5850 3450
Wire Wire Line
	5850 3450 5350 3450
Wire Wire Line
	5750 4050 5750 3550
Wire Wire Line
	5750 3550 5350 3550
Wire Wire Line
	5750 4050 7550 4050
Wire Wire Line
	8950 4050 9050 4050
Wire Wire Line
	9050 4050 9050 4200
Wire Wire Line
	9050 4200 5350 4200
Wire Wire Line
	5350 4200 5350 4000
$Comp
L Connector:Conn_01x04_Male J2
U 1 1 6160B785
P 7350 2100
F 0 "J2" V 7412 2244 50  0000 L CNN
F 1 "SERIAL" V 7503 2244 50  0000 L CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04_Pitch2.54mm" H 7350 2100 50  0001 C CNN
F 3 "~" H 7350 2100 50  0001 C CNN
	1    7350 2100
	0    1    1    0   
$EndComp
Wire Wire Line
	7550 2950 7350 2950
Wire Wire Line
	7350 2950 7350 2300
Wire Wire Line
	7550 3050 7450 3050
Wire Wire Line
	7450 3050 7450 2300
Wire Wire Line
	7250 2300 7250 2550
Wire Wire Line
	7250 2550 9100 2550
Connection ~ 9100 2550
Wire Wire Line
	9100 2550 9100 2050
Wire Wire Line
	7150 2300 7150 3150
Wire Wire Line
	6500 3000 5350 3000
Wire Wire Line
	7550 3450 5950 3450
Wire Wire Line
	5950 3450 5950 2900
Wire Wire Line
	5950 2900 5350 2900
Wire Wire Line
	7550 3550 6050 3550
Wire Wire Line
	6050 3550 6050 2800
Wire Wire Line
	6050 2800 5350 2800
Wire Wire Line
	7550 3650 6150 3650
Wire Wire Line
	6150 3650 6150 2700
Wire Wire Line
	6150 2700 5350 2700
Wire Wire Line
	7550 3750 6250 3750
Wire Wire Line
	6250 3750 6250 2600
Wire Wire Line
	6250 2600 5350 2600
Wire Wire Line
	7550 3850 6350 3850
Wire Wire Line
	6350 3850 6350 2500
Wire Wire Line
	6350 2500 5350 2500
Connection ~ 7150 3150
Wire Wire Line
	3950 2500 3700 2500
Wire Wire Line
	3700 2500 3700 3300
Wire Wire Line
	1950 3300 3700 3300
Wire Wire Line
	1950 3300 1950 3100
$Comp
L Connector_Generic:Conn_02x13_Top_Bottom J1
U 1 1 615DF75F
P 2150 2500
F 0 "J1" H 2200 3317 50  0000 C CNN
F 1 "KBD" H 2200 3226 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x13_Pitch2.54mm" H 2150 2500 50  0001 C CNN
F 3 "http://oldcomputer.info/8bit/c128/51.gif" H 2150 2500 50  0001 C CNN
	1    2150 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 3000 1850 3000
Wire Wire Line
	1850 3000 1850 3400
Wire Wire Line
	1850 3400 3950 3400
Wire Wire Line
	3950 3500 1800 3500
Wire Wire Line
	1800 3500 1800 2900
Wire Wire Line
	1800 2900 1950 2900
Wire Wire Line
	3950 3600 1750 3600
Wire Wire Line
	1750 3600 1750 2800
Wire Wire Line
	1750 2800 1950 2800
Wire Wire Line
	3950 3700 1650 3700
Wire Wire Line
	1650 3700 1650 2300
Wire Wire Line
	1650 2300 1950 2300
Wire Wire Line
	3950 3800 1550 3800
Wire Wire Line
	1550 3800 1550 2600
Wire Wire Line
	1550 2600 1950 2600
Wire Wire Line
	3950 3900 1450 3900
Wire Wire Line
	1450 3900 1450 2500
Wire Wire Line
	1450 2500 1950 2500
Wire Wire Line
	3950 4000 1350 4000
Wire Wire Line
	1350 4000 1350 2400
Wire Wire Line
	1350 2400 1950 2400
Wire Wire Line
	3950 4100 1250 4100
Wire Wire Line
	1250 4100 1250 2700
Wire Wire Line
	1250 2700 1950 2700
$Comp
L power:GND #PWR02
U 1 1 61661F66
P 2850 1700
F 0 "#PWR02" H 2850 1450 50  0001 C CNN
F 1 "GND" H 2855 1527 50  0000 C CNN
F 2 "" H 2850 1700 50  0001 C CNN
F 3 "" H 2850 1700 50  0001 C CNN
	1    2850 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	2850 1700 1950 1700
Wire Wire Line
	1950 1700 1950 1900
Wire Wire Line
	8950 3950 9200 3950
Wire Wire Line
	1150 4950 1150 2100
Wire Wire Line
	1150 2100 1950 2100
$Comp
L power:VCC #PWR01
U 1 1 61668DFA
P 1800 1800
F 0 "#PWR01" H 1800 1650 50  0001 C CNN
F 1 "VCC" H 1815 1973 50  0000 C CNN
F 2 "" H 1800 1800 50  0001 C CNN
F 3 "" H 1800 1800 50  0001 C CNN
	1    1800 1800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1950 2200 1800 2200
Wire Wire Line
	1800 2200 1800 1800
$Comp
L Connector:Mini-DIN-6 J3
U 1 1 6161A8C7
P 10250 3400
F 0 "J3" H 10250 3767 50  0000 C CNN
F 1 "Mini-DIN-6" H 10250 3676 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06_Pitch2.54mm" H 10250 3400 50  0001 C CNN
F 3 "http://service.powerdynamics.com/ec/Catalog17/Section%2011.pdf" H 10250 3400 50  0001 C CNN
	1    10250 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	8950 3050 9250 3050
Wire Wire Line
	9250 3050 9250 3400
$Comp
L power:GND #PWR0103
U 1 1 61601FB4
P 9250 3400
F 0 "#PWR0103" H 9250 3150 50  0001 C CNN
F 1 "GND" H 9255 3227 50  0000 C CNN
F 2 "" H 9250 3400 50  0001 C CNN
F 3 "" H 9250 3400 50  0001 C CNN
	1    9250 3400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR04
U 1 1 61638C42
P 10700 3950
F 0 "#PWR04" H 10700 3700 50  0001 C CNN
F 1 "GND" H 10705 3777 50  0000 C CNN
F 2 "" H 10700 3950 50  0001 C CNN
F 3 "" H 10700 3950 50  0001 C CNN
	1    10700 3950
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR03
U 1 1 6163953D
P 9750 2850
F 0 "#PWR03" H 9750 2700 50  0001 C CNN
F 1 "VCC" H 9765 3023 50  0000 C CNN
F 2 "" H 9750 2850 50  0001 C CNN
F 3 "" H 9750 2850 50  0001 C CNN
	1    9750 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	9950 3400 9750 3400
Wire Wire Line
	9750 3400 9750 2850
Wire Wire Line
	10550 3400 10700 3400
Wire Wire Line
	10700 3400 10700 3950
Wire Wire Line
	6500 4300 9300 4300
Wire Wire Line
	9300 4300 9300 3850
Wire Wire Line
	9300 3850 8950 3850
Wire Wire Line
	10550 3300 10700 3300
Wire Wire Line
	10700 3300 10700 2400
Wire Wire Line
	10700 2400 7050 2400
Wire Wire Line
	7050 2400 7050 3350
Wire Wire Line
	7050 3350 7550 3350
Text Label 10550 3300 0    50   ~ 0
PS2CLK
Wire Wire Line
	10550 3500 10550 3750
Wire Wire Line
	10550 3750 8950 3750
Text Label 10550 3500 0    50   ~ 0
PS2DATA
Text Label 1850 2100 0    50   ~ 0
~NMI
$Comp
L Transistor_BJT:2N3904 Q1
U 1 1 61654E20
P 8250 5150
F 0 "Q1" H 8441 5196 50  0000 L CNN
F 1 "2N3904" H 8441 5105 50  0000 L CNN
F 2 "TO_SOT_Packages_THT:TO-92_Inline_Narrow_Oval" H 8450 5075 50  0001 L CIN
F 3 "https://www.fairchildsemi.com/datasheets/2N/2N3904.pdf" H 8250 5150 50  0001 L CNN
	1    8250 5150
	-1   0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 61655627
P 8150 5550
F 0 "#PWR05" H 8150 5300 50  0001 C CNN
F 1 "GND" H 8155 5377 50  0000 C CNN
F 2 "" H 8150 5550 50  0001 C CNN
F 3 "" H 8150 5550 50  0001 C CNN
	1    8150 5550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8150 4950 1150 4950
Wire Wire Line
	8150 5550 8150 5350
$Comp
L Device:R R1
U 1 1 616753CE
P 8800 5150
F 0 "R1" V 8593 5150 50  0000 C CNN
F 1 "10K" V 8684 5150 50  0000 C CNN
F 2 "Resistors_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 8730 5150 50  0001 C CNN
F 3 "~" H 8800 5150 50  0001 C CNN
	1    8800 5150
	0    1    1    0   
$EndComp
Wire Wire Line
	8650 5150 8450 5150
Wire Wire Line
	8950 5150 9200 5150
Wire Wire Line
	9200 3950 9200 5150
Text Label 7450 2350 0    50   ~ 0
RX
Text Label 7350 2350 0    50   ~ 0
TX
Text Label 7250 2350 0    50   ~ 0
5V
Text Label 7150 2350 0    50   ~ 0
G
Wire Wire Line
	6500 3000 6500 4300
Wire Wire Line
	3950 3100 3500 3100
Wire Wire Line
	3500 3100 3500 1900
Wire Wire Line
	3500 1900 2450 1900
Wire Wire Line
	3950 3000 3400 3000
Wire Wire Line
	3400 3000 3400 2000
Wire Wire Line
	3400 2000 2450 2000
Wire Wire Line
	3950 2900 3300 2900
Wire Wire Line
	3300 2900 3300 2100
Wire Wire Line
	3300 2100 2450 2100
Wire Wire Line
	3950 2800 3200 2800
Wire Wire Line
	3200 2800 3200 2200
Wire Wire Line
	3200 2200 2450 2200
Wire Wire Line
	3950 2700 3100 2700
Wire Wire Line
	3100 2700 3100 2300
Wire Wire Line
	3100 2300 2450 2300
Wire Wire Line
	3950 2600 3000 2600
Wire Wire Line
	3000 2600 3000 2400
Wire Wire Line
	3000 2400 2450 2400
Wire Wire Line
	2900 3200 2900 2500
Wire Wire Line
	2900 3200 3950 3200
Wire Wire Line
	2900 2500 2450 2500
$EndSCHEMATC
