/*
  WiFlyTCP.h - WiFly Library for Arduino
*/
// ensure this library description is only included once
#ifndef _ESP8266_TCP_H_
#define _ESP8266_TCP_H_


#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Stream.h>
#include <avr/pgmspace.h>
#include <IPAddress.h>

#define WIFI_MODE_STATION			0x01
#define WIFI_MODE_AP				0x02
#define WIFI_MODE_BOTH				0x03

#define WIFI_MUX_SINGLE				0x00
#define WIFI_MUX_MULTI				0x01

#define WIFI_TCP_DISABLE			0x00
#define WIFI_TCP_ENABLE				0x01

#define WIFI_CLIENT					0x00
#define WIFI_SERVER					0x01

#define WIFI_STATE_IDLE				0x00
#define WIFI_STATE_UNAVAILABLE		0x01
#define WIFI_STATE_SEND				0x02
#define WIFI_STATE_CONNECT			0x03

#define WIFI_NEW_NONE				0x00
#define WIFI_NEW_MESSAGE			0x01
#define WIFI_NEW_CONNECTED			0x02
#define WIFI_NEW_DISCONNECTED		0x03
#define WIFI_NEW_SEND_OK			0x04
#define WIFI_NEW_SEND_ERROR			0x05
#define WIFI_NEW_RESET				0x06
#define WIFI_NEW_ALREADY_CONNECT	0x07
#define WIFI_NEW_ETC				0x08


// library interface description

class ESP8266_TCP {

	public:
  
	ESP8266_TCP();
	void begin(Stream *serial, Stream *serialDebug, int pinReset);
	void begin(Stream *serial, int pinReset);


	bool test();
	void reset();
	void hardReset();

	bool closeTCPServer();
	//bool isTCPEnabled();
	void openTCPServer(int port, int timeout);
	void connectTCP(String ip, int port);
	void closeTCPConnection();
	void closeTCPConnection(int id);

	int getRunningState();

	int getId();
	String getMessage();
	void clearNewMessage();

	String connectAccessPoint(String ssid, String pass);
	void openAccessPoint(String ssid, String pass, int channel);
	
	int isNewDataComing(byte type);

	bool send(String message);
	bool send(int id, String message);

	private:	

	void waitingForReset();
	void waitingForReset(unsigned long timeout);
	void waitingForHardReset();

	String waitingForJoin();
	void waitingForTCPConnection();

	bool setMode(int mode);
	bool setMux(int mux);

	bool enableTCPServer(int port);
	bool setTCPTimeout(int timeout);

	void flush();

	//bool waitForSendStatus();

	void setRunningState(int state);

	void debugPrintln(String str);
	void debugPrint(String str);

	bool setAP(String ssid, String pass, int channel);
	bool isNewAPSetting(String ssid, String pass, int channel);


	void clearBuffer();
	int findChar(String str, int start, char c);
	void clear();

	void write(String str);
	
	int available();

	String getClientList();

	String read();
	String readData();
	String readData(unsigned long timeout);
	String readTCPData();

	bool TCPConnected;
	bool TCPEnable;
	bool isDebug;

	int pinReset;
	
	int clientId;
	String clientMessage;
	
	Stream *serial;
	Stream *serialDebug;

	int runningState;
};

#endif

