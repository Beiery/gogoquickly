/*Desktop Train Controller
	===========================================
	--Board: Arduino DUE
	--Author: TSDArthur
	--Pages: http://github.com/tsdarthur
	--Version: 1.0
	--Simulator: OpenBVE
	===========================================
	--Note:
	External Interrupts:
	All pins.
	===========================================
	--Devices:
	Type:0.SWITCH_C -> CHANGE
	Type:1.SWITCH_F -> FALLING
	Type:3.ENCODER -> CHANGE (in developing)
	===========================================
	--Train:
	0.SPEED -> INT
	1.REVERSER -> BOOL
	2.POWER -> U8
	3.BRAKE -> U8
	4.SIGNAL -> INT
	5.SIGNAL_DISTANCE -> INT
	6.SPEED_LIMIT -> INT
	7.HORN -> BOOL
	8.SPEED_CONST -> BOOL
	9.MASTER_KEY -> BOOL
	===========================================
	Never mind scandal and liber!
*/

#include "DueTimer.h"

#define SWITCH_C 0
#define SWITCH_F 1
#define ENCODER 2
#define SPEED_MIN 0
#define SPEED_MAX 400
#define REVERSER_FORWARD 1
#define REVERSER_NEUTRAL 0
#define REVERSER_BACKWARD -1
#define POWER_MIN 0
#define POWER_MAX 4
#define BRAKE_MIN 0
#define BRAKE_MAX 8
#define SIGNAL_RED 0
#define SIGNAL_YELLOW 1
#define SIGNAM_GREEN 2
#define SIGNAL_DISTANCE_N1 2000
#define SIGNAL_DISTANCE_N2 1500
#define SIGNAL_DISTANCE_N3 1000
#define SIGNAL_DISTANCE_N4 500
#define SIGNAL_PASS 0
#define SPEED_LIMIT_MIN 0
#define SPEED_LIMIT_DEF 30
#define SPEED_LIMIT_MAX 400
#define SPEED_CONST_MIN 0
#define SPEED_CONST_DEF 30
#define SPEED_CONST_MAX 400
#define MASTER_KEY_OFF 0
#define MASTER_KEY_ON 1
//
#define DEVICE_NUMBER 8
#define DEVICE_TYPE_NUMBER 3
//
typedef void (*funcPoint)();
//
/*
	===========================================
	--Interrupts Map:
	--edit and see Interrupts.h
		default:
		PowerUp:interrupt0
		PowerDown:interrupt1
		ReserverForward:interrupt2
		ReserverBackward:interrupt3
		Horn:interrupt4
		SpeedConst:interrupt5
		Emergency:interrupt6
		MasterKey:interrupt7
	===========================================
*/

void interrupt0();
void interrupt1();
void interrupt2();
void interrupt3();
void interrupt4();
void interrupt5();
void interrupt6();
void interrupt7();

//change device type here
int deviceType[DEVICE_NUMBER] = {SWITCH_F, SWITCH_F, SWITCH_C, SWITCH_C, SWITCH_C, SWITCH_C, SWITCH_C, SWITCH_C};
//change functions here
funcPoint Interrputs[DEVICE_NUMBER] = {interrupt0, interrupt1, interrupt2, interrupt3, interrupt4, interrupt5, interrupt6, interrupt7};
//all use PULL_UP gpio mode
int devicePins[DEVICE_NUMBER];

class ControllerDevices
{
public:
	void Initialize(int devicePins[], int deviceType[], funcPoint Interrupts[])
	{
		//define gpio mode
		for (int i = 0; i < DEVICE_NUMBER; i++)
			pinMode(devicePins[i], INPUT_PULLUP);
		//attach Interrupts
		for (int i = 0; i < DEVICE_NUMBER; i++)
			attachInterrupt(devicePins[i], Interrupts[i], deviceType[i] == SWITCH_F ? FALLING : CHANGE);
	}
	//get state
	int GetDeviceState(int deviceID, int devicePins[])
	{
		return digitalRead(devicePins[deviceID]);
	}
}