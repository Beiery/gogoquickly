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
	1.REVERSER -> INT
	2.POWER -> U8
	3.BRAKE -> U8
	4.SIGNAL -> INT
	5.SIGNAL_DISTANCE -> INT
	6.SPEED_LIMIT -> INT
	7.HORN -> BOOL
	8.SPEED_CONST -> INT
	9.MASTER_KEY -> BOOL
	===========================================
	Never mind scandal and liber!
*/

#include "DueTimer.h"
//
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
#define SIGNAL_DISTANCE_DE 0
#define SIGNAL_PASS 0
#define HORN_OFF 0
#define HORN_ON 1
#define SPEED_LIMIT_MIN 0
#define SPEED_LIMIT_DEF 30
#define SPEED_LIMIT_MAX 400
#define SPEED_CONST_MIN 0
#define SPEED_CONST_DEF 30
#define SPEED_CONST_MAX 400
#define MASTER_KEY_OFF 0
#define MASTER_KEY_ON 1
//Devices
#define DEVICE_NUMBER 8
#define DEVICE_TYPE_NUMBER 3
#define ACTIVE 0
#define NO_ACTIVE 1
#define NO_READY 0
#define READY 1
//Train
#define TRAIN_DATA_NUMBER 10
#define _INT 0
#define _BOOL 1
#define SPEED 0
#define REVERSER 1
#define POWER 2
#define BRAKE 3
#define SIGNAL 4
#define SIGNAL_DISTANCE 5
#define SPEED_LIMIT 6
#define HORN 7
#define SPEED_CONST 8
#define MASTER_KEY 9
//HMI
#define HMI_SCRIPT_NUM 11
#define HMI_END_SYM 10
//PC
#define FILTER '|'
#define START_SYM '#'
#define END_SYM '!'
#define RECIEVE_DELAY 10
#define SEND_DELAY 10
#define TIMER_TICK 100
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
const int deviceType[DEVICE_NUMBER] = {SWITCH_F, SWITCH_F, SWITCH_C, SWITCH_C, SWITCH_C, SWITCH_C, SWITCH_C, SWITCH_C};
//change functions here
const funcPoint Interrputs[DEVICE_NUMBER] = {interrupt0, interrupt1, interrupt2, interrupt3, interrupt4, interrupt5, interrupt6, interrupt7};
//all use PULL_UP gpio mode
const int devicePins[DEVICE_NUMBER] = {30, 31, 32, 33, 34, 35, 36, 37};
//train data id
const int dataDefault[TRAIN_DATA_NUMBER] = {SPEED_MIN, REVERSER_NEUTRAL, POWER_MIN, BRAKE_MIN, SIGNAL_RED, SIGNAL_DISTANCE_DE, SPEED_LIMIT_DEF, HORN_OFF, SPEED_CONST_MIN, MASTER_KEY_OFF};
const int dataType[TRAIN_DATA_NUMBER] = {_INT, _INT, _INT, _INT, _INT, _INT, _INT, _BOOL, _INT, _BOOL};
//communication
const String HMIScript[HMI_SCRIPT_NUM] = { "spd.val=", "reserver.val=", "pwr.val=", "brake.val=", "sig.val=", "sigdis.val=", "spdlim.val=", "horn.val=", "speedconst.val=", "mstkey.val=", "0xFF"};
const int HMIMap[TRAIN_DATA_NUMBER] = {SPEED, REVERSER, POWER, BRAKE, SIGNAL, SIGNAL_DISTANCE, SPEED_LIMIT, HORN, SPEED_CONST, MASTER_KEY};

class DevicesManager
{
public:
	Initialize(int devicePins[], const int deviceType[], const funcPoint Interrupts[])
	{
		//define gpio mode
		for (int i = 0; i < DEVICE_NUMBER; i++)
			pinMode(devicePins[i], INPUT_PULLUP);
		//attach Interrupts
		for (int i = 0; i < DEVICE_NUMBER; i++)
			attachInterrupt(devicePins[i], Interrupts[i], deviceType[i] == SWITCH_F ? FALLING : CHANGE);
	}
	//get state
	int GetDeviceState(int deviceID, const int devicePins[])
	{
		return digitalRead(devicePins[deviceID]) ? ACTIVE : NO_ACTIVE;
	}
	//check state
	int IsReady(const int devicePins[])
	{
		for (int i = 0; i < DEVICE_NUMBER; i++)
			if (GetDeviceState(i, devicePins) == ACTIVE)return NO_READY;
		return READY;
	}
};

class TrainManager
{
public:
	int trainData[TRAIN_DATA_NUMBER];
	Initialize(const int dataDefault[])
	{
		//set default data to Train Manager
		for (int i = 0; i < TRAIN_DATA_NUMBER; i++)
			trainData[i] = dataDefault[i];
	}
	//!!for INT use SetData
	//!!for BOOL use SetReversal
	void SetData(int dataID, int value)
	{
		trainData[dataID] = value;
	}

	void SetReversal(int dataID)
	{
		trainData[dataID] = !trainData[dataID];
	}
	//get train data
	int GetData(int dataID)
	{
		return trainData[dataID];
	}
};

class CommunicationManager
{
public:
	Initialize()
	{
		Serial.begin(115200);
		Serial1.begin(115200);
	}
}