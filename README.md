![Build-Ram-USB-HID](https://github.com/SystemsCyber/SSS2/workflows/Build-Ram-USB-HID/badge.svg?branch=Ram-USB-HID)

# SSS2

The code base for the Teensy 3.6 based Smart Sensor Simulator 2. This SSS2 is primarily designed to simulate sensors for heavy vehicle electronic control units; however, it can be used for many other things. It is a multitool for vehicle systems. It can be used in a forensic context to simulate the presence of a vehicle and reduce the number of fault codes present when turning on the system again.

# Software Design

There are three sets of files needed to make the SSS2 work. All the software in this repository is for Arduino.

1. Board Definitions. This file declares the pin settings and defines the functions needed to communicate with the chips on board. For example, there is a difference between the SSS2-R02, SSS-R03, and SSS-R05 boards in the way the chip select pin is accessed for the digital potentiometers. This file is `SSS2_board_defs_rev_X.h` (TODO: a definitions file is needed for the Rev2 board.)
2. Settings processor. This file sets up the function calls for all of the commands. This is decoupled from the communications module to enable different ways of communicating with the SSS2. This file is `SSS2_functions.h`
3. Communications Interface. This the main wrapper file that provides an interface between the user and the SSS2 functions. The primary method of communications is Serial over USB, but CAN, LIN, J1708, WiFi (with the ESP8266) or others are enabled by customizing this main file. This file is usually called `SSS2_communications_over_serial.ino` and contains functions for all SSS2 variants. This means the lower level files need to tolerate differences.

## Board Definitions

The Schematics for the boards are available in the docs directory. The revision of the board is indicated in the serial number: SSS2-R0X, where X is the board revision. A define statment is at the top of the `SSS2_boards_defs.h` file to use in the programs. The following constants are defined:

`SSS2_BOARD_REVISION` - an integer representing the board revision.

### Pin Defintions

All pin defintions are `const int8_t` data types so they can take on a value of -1. If the pins are not used for a particular board revision, they should be set to -1.
|Pin Name|Description|
|--------|-----------|
|`greenLEDpin` | output for the pin that drives the green led.|
|`redLEDpin` | output for the pin that drives the red led.|
|`CSCANPin` | output for the chip select pin for the Microchip MCP2515 Stand alone CAN transciever over SPI.|
|`PWMPins` | output pin array for pulse width modulated signal providing the input for an opamp that converts the 3.3V signal to a 5V signal.|
|`CSconfigAPin` | output pin for the chip select pin for the first Analog Devices ADG1414 SPI controlled CMOS switching chip.|
|`CSconfigBPin` | output pin for the chip select pin for the other Analog Devices ADG1414 SPI controlled CMOS switching chip.|
|`buttonPin` | input pin that is pulled up for the push button input.|
|`encoderAPin` | input pin used for the quadrature knob.|
|`encoderBPin` | input pin used for the quadrature knob.|
|`IH2Pin` |output pin to drive a high current, high side switch from the Infineon BTM7710 driver chip.|
|`IL1Pin`| output pin to drive a high current, low side switch from the Infineon BTM7710 driver chip.|
|`IL2Pin` | output pin to drive a high current, low side switch from the Infineon BTM7710 driver chip.|
|`ignitionCtlPin` | output pin to drive an N-Channel MOSFET that drives a relay solenoid to switch 12 V.|
|`analogInPins` | An input pin array for the Analog to Digital Converter.|

This file defines a `setPinModes()` function to use when initializing the board. It sets all the pin modes and writes default values to them. This is what turns on the red LED at the beginning.

The revision 5 hardware has an MCP2515 CAN controller on SPI1. #define spi_readwrite SPI1.transfer

### Settings Variables

There are SPI connected potentiometers in the SSS that all have thier own settings. We define the number of these with `numSPIpots` which is usually 16. Then we can define arrays that holds all the potenetiometer settings. Since these potentiometers have settings for both the wiper position and the terminal connections, there are two arrays: `SPIpotWiperSettings` and `SPIpotTCONSettings`. These SPI potentiometers are labeled U1 through U16 in the schematics.

There are I2C connected potentiometers in the SSS2 Rev 3 and greater boards. These potentiometers use addressing to access the chip controls. The MCP45HV51 digital potentiometer can have up to 4 unique addresses on one I2C line. Three of these potentiometers are used for outward facing ports and the other is used to adjust the voltage on the high current regulator. The arrays that stores these settings are `I2CpotWiperSettings` and `I2CPotTCONSettings`.

There are also settings for all the switch or boolean variables set up. The PWM values and DAC values are given as well.

## SSS2 Functions

The SSS2_functions file is a header file that defines all the functions needed to convert a String command into a setting output. There are 2 setting strings that are used: `commandPrefix` and `commandString`. The command prefix is used but the communications module to call a function. The `commandString` variable is parsed by the function to determine what it needs to do. All command entries end with a newline character ("/n").

**X,Y** where X is one of the SSS2 Settings and Y is its value.
|Function | Description |
|---------|-------------|
| **AI,X**| where X is a 0 = off or 1 = on. This turns on and off the analog input display.|
| **B0,X**| Sets the baudrate for the first FlexCAN controller. If X is ommitted, then the current CAN bit rate is returned. Typical baudrates are 250000, 666000, 500000, and 125000.|
| **B1,X**| Sets the baudrate for the second FlexCAN controller. If X is ommitted, then the current CAN bit rate is returned.|
| **BMCP,X**| Sets the baudrate for the MCP2515 controller. If X is ommitted or is not valid, then the current CAN bit rate is returned. Possible baudrates are: 250000, 500000, 666666, 125000, 1000000, 5000, 10000, 20000, 31520, 333333, 40000, 50000, 80000, 100000, 200000.
| **DB,**| Displays the baud rates. |
| **CANCOMP,X**| Turns on the ability for the SSS2 to respond to component ID requests. The value of X is binary: 0 = do not respond or 1 = respond to J1939 request messages.|
| **ID,**| Request the unique Chip ID from the Freescale K66 chip on the Teensy 3.6.|
| **C0,X**| where X is a 0 or 1. When X is 0, it turns off the display of received CAN messages from the first built-in controller, which is J1939. When X is > 0, it turns on the serial stream. The CAN messages can arrive faster than the USB stack can send.|
| **C1,X**| where X is a 0 or 1. When X is 0, it turns off the display of received CAN messages from the first built-in controller, which is J1939. When X is > 0, it turns on the serial stream.|
| **GO,i,X**| Starts and stops the can message in the _i_-th slot of the threading queue. The value of X is binary: 0 = stop message or 1 = start message.|
| **SP,X**| Sets the shortest period in milliseconds for a periodic CAN transmission. This sets the lower bound of the interthread CAN messages. |
| **STARTCAN,**| Enables all CAN messages that are setup to start. This is like sending the command `GO,i,1` for all i.|
| **STOPCAN,**| Disables the transmission of all CAN messages. This is like sending the command `GO,i,0` for all i.|
| **STOPCAN,**| Disables the transmission of all CAN messages. This is like sending the command `GO,i,0` for all i.|
| **CLEARCAN,**| Removes all CAN messages setup with the SM command from the transmitting thread. All messages will be destroyed and need reloaded. |
| **STATS,**| Displays the statistics from the FlexCAN controller for both CAN channels.|
| **CLEARSTATS,**| Resets the FlexCAN statisitics.|
| **CI,_string_**| Display or change SSS2 Component Information. The component information (_string_) should be similar to `SYNER*SSS2-00*XXXX*UNIVERSAL`. If there are not more than 12 characters for the component ID, then the command will just display the current setting. This command also saves the new component ID in EEPROM for non-volitile storage. |
| **LS,** |List Settings. Displays all the numbered settings in the current configuration.|
| **CANNAME,X**| Display the name of the CAN transmit thread as it was entered in the `SM` command.|
| **CANSIZE,**| List the number of CAN transmitting threads. The maximum is 1024. |
| **THREADS,**| Display a list of all indexes and names for the CAN transmitting threads.|
| **SOFT,**| Display the firmware version running on the SSS2 unit.|
| **J1708,X**| Set the streaming of J1708 traffic to 1 = straming on or 0 = streaming off. The format of the displayed messages are `J1708 Milliseconds MID PID DATA OK` where OK is the result of the checksum.|
| **RELOAD,**| Resets the threads that send CAN messages.|
| **TIME,X**| Sets the time on the SSS2 to _X_, where _X_ the UNIX timestamp (number of seconds from 1 Jan 1970). If _X_ is omitted, then the current time is returned in a human readable form. |
| **GETTIME,**| Displays the time on the SSS2 as a UNIX timestamp (number of seconds from 1 Jan 1970).|
| **LIN,X**| Toggles the streaming display of LIN messages. This streams the data on the LIN bus to the USB Serial. The value of X is binary: 0 = stop streaming message. or 1 = start streaming messages.|
| **SENDLIN,X**| Toggles the build in LIN message for a shifter lever on DDEC13 ECUs. The value of X is binary: 0 = disable LIN or 1 = enable LIN.|
| **CANSEND,message**| Send a 1 time CAN message with the following structure for _message_: C,XXXX,YYYY <br> _C_ is the channel number, 0 = CAN0 and 1 = CAN <br> _XXXX_ is the CAN ID in hex. If XXXX is greater than 0x7FF, then it will use an extended id. The most significant bit of the UINT32 is masked away, so if you want an 29-bit ID with a value of 5, then enter the ID field as '80000005'. For an 11-bit ID, just use '5'. <br> _YYYY_ are the message data in an even number hex digits (up to 16 nibble characters). |

**SM,name,i,n,j,c,p,d,t,e,ID,DLC,b1,b2,b3,b4,b5,b6,b7,b8** Setup a CAN message with the fiollowing arguments. All arguments are required.
|Variable|Alias|Description|
|--------|-----|-----------|
| _name_ | Name|is a string to describe the transmitting thread. Suggested naming scheme would use language from J1939. For example: " CCVS1 from Instrument Cluster."|
| _i_ | index| the CAN message index from 0 to 1024. If the position in the array exists, the command will overwrite that spot. If not, it will create the next new one. This means, you can tell i to be 1000, but is may return 1.|
| _n_ |num*messages| the number of total sub messages.|
|\_j* | sub*index|the CAN message sub index. Default should be 0. This places the message in a list the gets sent as a group.|
|\_c* | channel| the CAN Channel 0 or 1.|
|_p_ | tx*period| the period of transmission from one mesage to the next in milliseconds.|
|\_d* | tx*delay| the delay between repeating the message groups from start to start.|
|\_t* | stop*after_count| the total number of message groups to send. Set to 0 for disabling this count.|
|\_e*| temp*txmsg.ext| the extended ID flag set to 0 for 11-bit IDs or 1 for 29-bit IDs.|
|\_ID*| temp*txmsg.id| the CAN ID HEX characters, (e.g. 18FEF100).|
|\_DLC*| temp*txmsg.len| the data length code.|
|\_b1* through _b8_ | temp_txmsg.buf[i]| the data bytes in HEX. |

### Examples:

Enter the command

`SM,Test,0,1,0,0,100,0,0,1,18FEF100,8,DE,AD,BE,EF,02,03,04,05`

to set up a message in the threading queue. This will be the first message in the list. It is set up to last forever at a rate of 100 milliseconds between each message. Enter `GO,0,1` to start the CAN message broadcasting. The following command will overwrite the message in the queue and change its values.

`SM,Fred,0,1,0,0,100,0,0,1,18FEF10B,8,DE,AD,BE,EF,06,07,08,09`

Let's say we want to modify the previous message and transmit it at a rate of 25 msec.

`SM,Joe,0,1,0,0,25,0,0,1,18FEF10B,8,DE,AD,BE,EF,06,07,08,09`

Let's add a toggle to the last digit of the messag that cycles back and forth for each message. To do this, we'll keep the same rate and ID. We will add a sub_index and increase the number of messages in the single transmit thread. We'll slow it down a little too.

`SM,Mary,0,2,1,0,250,0,0,1,18FEF10B,8,DE,AD,BE,EF,06,07,08,0A`

Let's take the toggling message and wait 2 seconds before toggling again. This is done with the tx_delay.

`SM,Chuck,0,2,1,0,250,2500,0,1,18FEF10B,8,DE,AD,BE,EF,06,07,08,0A`

We can add another message to the same sequence by increasing the number of messages to 3 and putting the data in the sub_index 2.

`SM,0,3,2,0,250,2500,0,1,18FEF10B,8,DE,AD,BE,EF,06,07,08,0B`

The CAN messages will be broadcast in sequence with an intermessage timing defined by the period, then wait until the delay takes place. If the delay is less than the time needed for all the messages to broadcast, then it is effectively not used. For example,

`SM,0,4,3,0,250,700,0,1,18FEF10B,8,DE,AD,BE,EF,06,07,08,0C`

adds another message for a total of 4. So witth 250 msec between each message, it takes 1 second to get through all 4 messages and the delay doesn't have any influence.

To send a one time request for component ID in J1939, use the following command to set up the message thread,

`SM,1,1,0,0,0,0,1,1,18EAFFFA,3,EB,FE,00`

Keep in mind that this just sets up the message. You have to send a command `GO,1,1` which set the message in slot 1 to run. If we wanted to request component ID 3 times 10 ms apart, then do the same thing 5 seconds later and stop after 3 spurts, you would use the commands:

`SM,0,1,0,0,5,5000,9,1,18EAFFFA,3,EB,FE,00`

`SM,0,2,1,0,5,5000,9,1,18EAFFFA,3,EB,FE,00`

`SM,0,3,2,0,5,5000,9,1,18EAFFFA,3,EB,FE,00`

`GO,0,1`

Finally, if we wanted to set up a thread to send component ID once. A Component ID may be `SYNER*SSS2-03*XXXX*UNIVERSAL`. This is 28 characters. We will need to set up a broacast annoucement message (BAM) and send the message with 4 CAN Frames. Referencing the J1939-21 document, we have to set up a Transfer Protocol - Connection Management (TP.CM) message before sending the data using 4 Transfer Protocol - Data Transfer (TP.DT) messages. The following setup commands are sent to the SSS2:

`SM,0,5,0,0,10,0,5,1,1CECFFFA,8,20,1C,0,4,FF,EB,FE,0`, which is the connection managmement message.

`SM,0,5,1,0,10,0,5,1,1CEBFFFA,8,01,53,59,4E,45,52,2A,53`

`SM,0,5,2,0,10,0,5,1,1CEBFFFA,8,02,53,53,32,2D,30,33,2A`

`SM,0,5,3,0,10,0,5,1,1CEBFFFA,8,03,58,58,58,58,2A,55,4E`

`SM,0,5,4,0,10,0,5,1,1CEBFFFA,8,04,49,56,45,52,53,41,4C`

This loads the component ID into the transmit thread queue. The thread in the thread queue in position 0 needs to be enabled to transmit. The can be done by sending the command `GO,0,1`. The enable flag could also be set during a J1939 receive message when the request message for Component ID is received.

### Default CAN messages

0. Component ID
1.

### CanThread Class

The program needs to be able to set up the transmission of groups of messages. It uses the Thread libary to set up 3 controls:

1. `can_thread_controller` is the main thread control that gets called on an interupt every and during every loop. This guarantees CAN delivery

2. `CanGroupThread` is a container for the threads for 1 or more CAN messages.

## SSS2 Settings

The following are the enumerated settings for the SSS2. These settings are set using a lookup table, which is a long if-then-else structure. Teh settings are set when the command `setSetting` is called. The function is declared as:

`int16_t setSetting(uint8_t settingNum, int settingValue, bool debugDisplay)`

where `settingNum` is from the enumerated list below and the `settingValue` is the value for which to set the setting. The `debugDisplay` provides additional serial console ouptut from the executed command. The function returns the value of the setting as a confirmation it was set. The list of settings is as follows:

1. Digital Potentiometer 1 Wiper, Port 1 (J24-1)
2. Digital Potentiometer 2 Wiper, Port 2 (J24-2)
3. Digital Potentiometer 3 Wiper, Port 3 (J24-3)
4. Digital Potentiometer 4 Wiper, Port 4 (J24-4)
5. Digital Potentiometer 5 Wiper, Port 5 (J24-5)
6. Digital Potentiometer 6 Wiper, Port 6 (J24-6)
7. Digital Potentiometer 7 Wiper, Port 7 (J24-7)
8. Digital Potentiometer 8 Wiper, Port 8 (J24-8)
9. Digital Potentiometer 9 Wiper, Port 9 (J24-9)
10. Digital Potentiometer 10 Wiper, Port 10 (J24-10)
11. Digital Potentiometer 11 Wiper, Port 11 (J24-11)
12. Digital Potentiometer 12 Wiper, Port 12 (J24-12)
13. Digital Potentiometer 13 Wiper, Port 13 (J18-11)
14. Digital Potentiometer 14 Wiper, Port 14 (J18-12)
15. Digital Potentiometer 15 Wiper, Port 15 (J24-15)
16. Digital Potentiometer 16 Wiper, Port 16 (J24-16)
17. Vout2-A, Port 18 (J18- 2)
18. Vout2-B, Port 19 (J18- 3)
19. Vout2-C, Port 20 (J18- 4)
20. Vout2-D, Port 21 (J18- 5)
21. Vout2-E, Port 22 (J18- 6)
22. Vout2-F, Port 23 (J18- 7)
23. Vout2-G, Port 24 (J18- 8)
24. Vout2-H, Port 25 (J18- 9)
25. U1 & U2 P0A, Ports 1 and 2
26. U3 & U4 P0A, Ports 3 and 4
27. U5 & U5 P0A, Ports 5 and 6
28. U7 & U2 P0A, Ports 7 and 8
29. U9 & U10 P0A, Ports 9 and 10
30. U11 & U12 P0A, Ports 11 and 12
31. U13 & U14 P0A, Ports 13 and 14
32. U15 & U16 P0A, Ports 15 and 16
33. PWM 1 Connect, Ports 13 (J24-13) and 31 (J18-15)
34. PWM 2 Connect, Ports 14 (J24-14) and 32 (J18-16)
35. PWM 3 Connect, Port 27 (J18-10)
36. PWM 4 Connect, Port 17 (J18-1)
37. Port 10 or 19, (J24-10)
38. Port 15 or 18, (J24-15)
39. CAN1 or J1708, (J24-17 & J24-18)
40. Port 31 & 32 or CAN2, (J18-15 and J18-16)
41. CAN0 Termination Resistor, R44
42. CAN1 Termination Resistor, R45
43. CAN2 Termination Resistor, R46
44. LIN Master Pullup Resistor, R59
45. 12V Out 1 (H-Bridge), Port 26 (J18-10)
46. 12V Out 2 (H-Bridge), Port 11 (J24-11)
47. Ground Out 1 (H-Bridge), Port 17 (J18- 1)
48. Ground Out 2 (H-Bridge), Port 12 (J24-12)
49. High Voltage Adjustable Output, (J24-19 and J18-11)
50. Ignition Relay, (J24-20)
51. Dig. Pot. 1 Terminal Connect, Port 1 (J24- 1)
52. Dig. Pot. 2 Terminal Connect, Port 2 (J24- 2)
53. Dig. Pot. 3 Terminal Connect, Port 3 (J24- 3)
54. Dig. Pot. 4 Terminal Connect, Port 4 (J24- 4)
55. Dig. Pot. 5 Terminal Connect, Port 5 (J24- 5)
56. Dig. Pot. 6 Terminal Connect, Port 6 (J24- 6)
57. Dig. Pot. 7 Terminal Connect, Port 7 (J24- 7)
58. Dig. Pot. 8 Terminal Connect, Port 8 (J24- 8)
59. Dig. Pot. 9 Terminal Connect, Port 9 (J24- 9)
60. Dig. Pot. 10 Terminal Connect, Port 10 (J24-10)
61. Dig. Pot. 11 Terminal Connect, Port 11 (J24-11)
62. Dig. Pot. 12 Terminal Connect, Port 12 (J24-12)
63. Dig. Pot. 13 Terminal Connect, Port 13 (J18-11)
64. Dig. Pot. 14 Terminal Connect, Port 14 (J18-12)
65. Dig. Pot. 15 Terminal Connect, Port 15 (J24-15)
66. Dig. Pot. 16 Terminal Connect, Port 16 (J24-16)
67. PWM1 Connect, Port 13 (J24-13)
68. PWM2 Connect, Port 14 (J24-14)
69. PWM3 Connect, Port 27 (J18-10)
70. PWM4 Connect, Port 17 (J18- 1)
71. LIN to Shield Connect, (J10- 5)
72. LIN to Port 16 Connect, Port 16 (J24-16)
73. U28 (U1-U8) P0A Enable, (J24-1 to J24-8)
74. U31 (U9-U16) P0A Enable, (J24-9 to J24-16)
75. Digital Potentiometer 28 Wiper, Port 28 (J18-12)
76. Digital Potentiometer 29 Wiper, Port 29 (J18-13)
77. Digital Potentiometer 30 Wiper, Port 30 (J18-14)
78. Dig. Pot. 28 Terminal Connect, Port 28 (J18-12)
79. Dig. Pot. 29 Terminal Connect, Port 29 (J18-13)
80. Dig. Pot. 30 Terminal Connect, Port 30 (J18-14)
81. PWM 1 Frequency, Port 13 (J24-13)
82. PWM 2 Frequency, Port 14 (J24-14)
83. PWM 3 Frequency, Port 27 (J18-10)
84. PWM 4 Frequency, Port 17 (J18-1)
85. PWM 5 Frequency, Port 1 (J24-1)
86. PWM 6 Frequency, Port 2 (J24-2)
87. PWM 5 Value, Port 1 (J24-1)
88. PWM 6 Value, Port 2 (J24-2)
89. PWM 5 Connect, Port 1 (J24-1)
90. PWM 6 Connect, Port 2 (J24-2)
91. CAN1 Connect, Ports 3 and 4 (J24-3,4)
    Some setting take binary values of 0 or 1. For example, if the SSS2 receives a serial command that says `50,1` it will interpret that to set setting number 50 to true, which means to close the ignition key switch relay. Similarly, the command `45,0` will turn off the 12V output on J18-10. There is a `fastSetSetting` function that gets called whenever the communication manager receives a digit. This processes the command without the `debugDisplay` flag, so the serial output says `SET X,Y` where X is the setting number and Y is the current or new setting value.

- Binary Settings
- Terminal Settings
- Digital Potentiometer Settings
- Voltage Output Settings
- PWM Frequency Settings
- PWM Duty Cycle Settings

# SSS2 Firmware — PlatformIO Project (Teensy 3.6)

Converted from the original Arduino IDE project for use with PlatformIO.

## Project Structure

```
SSS2_PlatformIO/
├── platformio.ini              ← PlatformIO config (board, libs, flags)
├── src/
│   ├── main.cpp                ← Main firmware (was SSS2_Firmware.ino)
│   ├── SSS2_functions.h        ← Core functions header
│   ├── SSS2_board_defs_rev_5.h ← Board pin definitions (Rev 5 - default)
│   └── SSS2_board_defs_rev_3.h ← Board pin definitions (Rev 3)
└── lib/
    ├── FlexCAN_Library/        ← Teensy CAN bus library
    ├── MCP23017/               ← Adafruit MCP23017 I/O expander (i2c_t3 version)
    ├── MCP_CAN_lib/            ← MCP2515 SPI CAN libraryalternate between formated md and text in vs code
    ├── OneButton/              ← Button debounce library
    ├── ArduinoThread/          ← Thread management library
    └── TeensyID/               ← Teensy unique ID library
```

## Setup & Build

1. Install [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) (VS Code extension recommended)
2. Open this folder in VS Code
3. PlatformIO will auto-install dependencies from `platformio.ini`
4. Click **Build** (✓) or run: `pio run`
5. To upload: `pio run --target upload`

## Dependencies Auto-Installed by PlatformIO

| Library | Source                   |
| ------- | ------------------------ |
| Encoder | paulstoffregen/Encoder   |
| Time    | paulstoffregen/Time      |
| i2c_t3  | github.com/nox771/i2c_t3 |

## Bundled Local Libraries (in `lib/`)

| Library         | Notes                                              |
| --------------- | -------------------------------------------------- |
| FlexCAN_Library | Teensy FlexCAN driver                              |
| MCP23017        | Modified Adafruit lib using i2c_t3 instead of Wire |
| MCP_CAN_lib     | MCP2515 SPI CAN driver                             |
| OneButton       | Single-button state machine                        |
| ArduinoThread   | Cooperative threading                              |
| TeensyID        | Reads Teensy chip UID                              |

## J1939 Proprietary A Service Interface (PGN 0xEF00)

The SSS2 exposes a remote-control service layer over the J1939 bus using **PGN 0xEF00
(Proprietary A, PDU1)**. Any tool on the bus can read or write SSS2 settings by sending an
8-byte CAN frame addressed to the target SSS2's source address (e.g. **0x80**).

The service is **always listening** — it works even when the SSS2 ignition output is off,
which allows a remote tool to turn ignition on over CAN.

---

### 1. J1939 29-bit CAN ID

J1939 uses extended (29-bit) CAN IDs. For a PDU1 message the bits are:

```
Bit 28–26   Bit 25   Bit 24   Bits 23–16   Bits 15–8   Bits 7–0
──────────  ───────  ───────  ───────────  ──────────  ────────
PRIORITY    RSVD     DP       PF (0xEF)    DA           SA
 3 bits      1 bit    1 bit    8 bits       8 bits       8 bits
```

| Field    | Meaning                     | Request value                             | Response value        |
| -------- | --------------------------- | ----------------------------------------- | --------------------- |
| PRIORITY | Message priority (0=high)   | 6 (0b110)                                 | 6 (0b110)             |
| DP       | Data Page                   | 0                                         | 0                     |
| PF       | PDU Format — identifies PGN | 0xEF (Proprietary A)                      | 0xEF                  |
| DA       | Destination Address         | SSS2's SA (e.g. 0x80) or 0xFF (broadcast) | requester's SA        |
| SA       | Source Address (sender)     | your tool's SA (e.g. 0x52)                | SSS2's SA (e.g. 0x80) |

**Example request CAN ID** — tool SA=0x52 targeting SSS2 SA=0x80:

```
Priority=6, DP=0, PF=0xEF, DA=0x80, SA=0x52
→ 29-bit ID = 0x18EF8052
```

**Example response CAN ID** — SSS2 (SA=0x80) replying to tool SA=0x52:

```
Priority=6, DP=0, PF=0xEF, DA=0x52, SA=0x80
→ 29-bit ID = 0x18EF5280
```

---

### 2. Payload Layout (8 bytes)

Every request and response is exactly 8 bytes:

```
Byte index:  0       1           2          3          4         5    6    7
             ┌───────┬───────────┬──────────┬──────────┬─────────┬────┬────┬────┐
Request:     │  CMD  │ SETTING # │ VALUE lo │ VALUE hi │  0xFF   │ FF │ FF │ FF │
             └───────┴───────────┴──────────┴──────────┴─────────┴────┴────┴────┘
             ┌───────┬───────────┬──────────┬──────────┬─────────┬────┬────┬────┐
Response:    │ 0x80  │ SETTING # │ VALUE lo │ VALUE hi │ STATUS  │ FF │ FF │ FF │
             └───────┴───────────┴──────────┴──────────┴─────────┴────┴────┴────┘
```

#### Byte 0 — Command (request) / Response marker

| Value  | Constant                | Meaning                        |
| ------ | ----------------------- | ------------------------------ |
| `0x01` | `J1939_SVC_SET_SETTING` | Write a setting value          |
| `0x02` | `J1939_SVC_GET_SETTING` | Read the current setting value |
| `0x80` | `J1939_SVC_RESPONSE`    | SSS2 response marker (byte 0)  |

#### Byte 1 — Setting number

An integer from **1 to 92** that selects which SSS2 hardware parameter to read or write.
This maps directly to the `settingNum` argument of `setSetting()`.
Use the `LS` serial command to list all setting names and their numbers.

#### Bytes 2–3 — Value (signed 16-bit, little-endian)

The setting value as a signed 16-bit integer, **low byte first**:

```
Byte 2 = value & 0xFF          (least-significant byte)
Byte 3 = (value >> 8) & 0xFF   (most-significant byte)
```

- In a **request**: the value to write (SET_SETTING) or `0xFF 0xFF` as don't-care (GET_SETTING).
- In a **response**: the value that is now active after the operation.

**Examples:**

| Decimal value | Byte 2 | Byte 3 |
| ------------- | ------ | ------ |
| 0             | 0x00   | 0x00   |
| 1             | 0x01   | 0x00   |
| 255           | 0xFF   | 0x00   |
| 256           | 0x00   | 0x01   |
| -1            | 0xFF   | 0xFF   |

#### Byte 4 — Status (response only; 0xFF in requests)

| Value  | Constant                 | Meaning                               |
| ------ | ------------------------ | ------------------------------------- |
| `0x00` | `J1939_SVC_STATUS_OK`    | Success                               |
| `0x01` | `J1939_SVC_IGN_REQUIRED` | Ignition is off; request blocked      |
| `0x02` | `J1939_SVC_OUT_OF_RANGE` | Setting number is 0 or ≥ 93 (invalid) |

#### Bytes 5–7 — Reserved

Always `0xFF`. Ignore on receive.

---

### 3. Ignition-Off Rules

The SSS2 enforces a whitelist when ignition is off:

| Command                 | Cold-start allowed?                |
| ----------------------- | ---------------------------------- |
| SET setting 50, value 1 | **Yes** — this turns ignition ON   |
| All other SET requests  | No — returns `IGN_REQUIRED (0x01)` |
| GET setting 50          | **Yes** — read ignition state      |
| All other GET requests  | No — returns `IGN_REQUIRED (0x01)` |

Setting **50** is the ignition relay control (0 = off, 1 = on).

---

### 4. Complete Examples

All examples use tool SA = **0x52** (82 decimal), SSS2 SA = **0x80** (example).

#### Turn ignition ON (allowed cold)

```
Request:
  CAN ID : 0x18EF8052  (extended)
  Payload: 01 32 01 00 FF FF FF FF
           ── ── ───── ── ── ── ──
           │  │  │     │  reserved
           │  │  │     └─ status (don't-care in request)
           │  │  └─── value = 0x0001 = 1  (LE: low=01 high=00)
           │  └────── setting 50 (0x32)
           └───────── CMD = SET_SETTING (0x01)

Response:
  CAN ID : 0x18EF5280  (extended)
  Payload: 80 32 01 00 00 FF FF FF
                        ──
                        status = OK (0x00) → ignition is now ON
```

#### Read ignition state (GET)

```
Request:
  CAN ID : 0x18EF8052
  Payload: 02 32 FF FF FF FF FF FF
           ── ──
           │  └─ setting 50 (0x32)
           └──── CMD = GET_SETTING (0x02)

Response:
  CAN ID : 0x18EF5280
  Payload: 80 32 01 00 00 FF FF FF
                 ─────
                 value = 1 → ignition is ON
```

#### Write a setting while ignition is ON (e.g. setting 1, value 128)

```
Request:
  CAN ID : 0x18EF8052
  Payload: 01 01 80 00 FF FF FF FF
              ── ─────
              │  value = 128 (LE: low=0x80, high=0x00)
              └─ setting 1

Response:
  CAN ID : 0x18EF5280
  Payload: 80 01 80 00 00 FF FF FF
                        ──
                        status = OK
```

#### Attempt to write while ignition is OFF (blocked)

```
Request:
  CAN ID : 0x18EF8052
  Payload: 01 01 80 00 FF FF FF FF   (setting 1, value 128)

Response:
  CAN ID : 0x18EF5280
  Payload: 80 01 00 00 01 FF FF FF
                        ──
                        status = IGN_REQUIRED (0x01)
```

#### Invalid setting number (out of range)

```
Request:
  CAN ID : 0x18EF8052
  Payload: 01 00 00 00 FF FF FF FF   (setting 0 — invalid)

Response:
  CAN ID : 0x18EF5280
  Payload: 80 00 00 00 02 FF FF FF
                        ──
                        status = OUT_OF_RANGE (0x02)
```

#### Broadcast message (DA = 0xFF)

The SSS2 also responds to broadcast requests (DA = 0xFF in the CAN ID):

```
Request CAN ID : 0x18EFFF52   (DA=0xFF = broadcast)
```

All other bytes follow the same rules as a unicast request.

---

### 5. Quick Reference Card

```
┌─────────────────────────────────────────────────────────────────┐
│             SSS2 J1939 Proprietary A — Quick Reference          │
├──────────────────────┬──────────────────────────────────────────┤
│ SSS2 Source Address  │ e.g. 0x80 (depends on unit)              │
│ PGN                  │ 0xEF00  (PF = 0xEF, PDU1)               │
│ CAN frame type       │ Extended (29-bit ID)                     │
│ Payload length       │ Always 8 bytes                           │
├──────────────────────┼──────────────────────────────────────────┤
│ CMD SET_SETTING      │ 0x01                                     │
│ CMD GET_SETTING      │ 0x02                                     │
│ Response marker      │ 0x80 (byte 0 of every response payload) │
├──────────────────────┼──────────────────────────────────────────┤
│ STATUS OK            │ 0x00                                     │
│ STATUS IGN_REQUIRED  │ 0x01                                     │
│ STATUS OUT_OF_RANGE  │ 0x02                                     │
├──────────────────────┼──────────────────────────────────────────┤
│ Setting range        │ 1 – 92 (inclusive)                       │
│ Cold-start whitelist │ SET setting 50 value 1 only              │
│ Value encoding       │ Signed int16, little-endian (bytes 2–3)  │
└──────────────────────┴──────────────────────────────────────────┘
```

---

## Changes from Original Arduino Project

- `SSS2_Firmware.ino` → `src/main.cpp` with `#include <Arduino.h>` added at top
- `SSS2_functions.h` → `#include <Arduino.h>` added at top
- All libraries moved to `lib/` (no need to install manually into Arduino libraries folder)
- `ENCODER_OPTIMIZE_INTERRUPTS` and `USE_SPI1` moved to `build_flags` in `platformio.ini`
- Added J1939 CAN message listening in `loop()` — incoming frames on CAN0 and CAN1 are parsed for J1939 Network Management (address claim) and Proprietary A service requests via `is_j1939_proprietary_a_for_us()` and `handle_j1939_service()`
- J24:17-18 default changed from CAN1 (`CAN1Switch = true`) to J1708/J1939 mode (`CAN1Switch = false`) in Rev 5 board defs, making J1939 the default protocol on that connector
- Added interrupt-safe guards (`noInterrupts()`/`interrupts()`) around all `bus.write()` calls in `loop()` to prevent ring buffer corruption with the 1ms `CANTimer` interrupt
