/*
  WiFlyTCP.h - WiFly Library for Arduino
*/


#include "ESP8266_TCP.h"

/*
 * two-wire constructor.
 * Sets which wires should control the motor.
 */
ESP8266_TCP::ESP8266_TCP() {

}

void ESP8266_TCP::begin(Stream *serial, Stream *serialDebug, int pinReset)
{
	pinMode(pinReset, OUTPUT);
	digitalWrite(pinReset, HIGH);
	this->pinReset = pinReset;
	this->serial = serial;
	this->serialDebug = serialDebug;
	this->isDebug = true;
	this->TCPConnected = false;
	this->clientId = -1;
	this->clientMessage = "";
	this->runningState = WIFI_STATE_UNAVAILABLE;
	delay(2000);
	//debug.begin(serialport);
	//this->baudrate = baudrate;      		// which step the motor is on
	//this->serialport = serialport;
  //this->serialport.begin(serialport);
}

void ESP8266_TCP::begin(Stream *serial, int pinReset)
{
	pinMode(pinReset, OUTPUT);
	digitalWrite(pinReset, HIGH);
	this->pinReset = pinReset;
	this->serial = serial;
	this->isDebug = false;
	this->TCPConnected = false;
	this->clientId = -1;
	this->clientMessage = "";
	this->runningState = WIFI_STATE_UNAVAILABLE;
	delay(2000);
	//debug.begin(serialport);
	//this->baudrate = baudrate;      		// which step the motor is on
	//this->serialport = serialport;
  //this->serialport.begin(serialport);
}

bool ESP8266_TCP::test() {
	clearBuffer();
	write("AT");
	String data = readData();
    debugPrintln(data);
	if(data.equals("")) {
    	debugPrintln("RESET");
		hardReset();
		return true;
	}
	data = readData();
    debugPrintln(data);
	if(data.equals("busy now ...")) {
    	debugPrintln("RESET");
		hardReset();
		return true;
	}
	return readData().equals("OK"); 
}

void ESP8266_TCP::reset() {
	clearBuffer();
	write("AT+RST");
	waitingForReset();
}

void ESP8266_TCP::hardReset() {
	digitalWrite(this->pinReset, LOW);
	delay(1000);
	digitalWrite(this->pinReset, HIGH);
	waitingForHardReset();
}

void ESP8266_TCP::waitingForReset() {
	while(true) {
		if(available() > 0) {
			String data = readData(1000);
		    debugPrintln(data);
			if(data.substring(1, 4).equals("ets")) {
    			debugPrintln("RESET");
				delay(2000);
				clearBuffer();
				return;
			}
		}
	}
}

void ESP8266_TCP::waitingForReset(unsigned long timeout) {
	unsigned long t = millis();
	while(millis() - t < timeout) {
		if(available() > 0) {
			String data = readData(1000);
			if(data.substring(1, 4).equals("ets")) {
    			debugPrintln("RESET");
				delay(2000);
				clearBuffer();
				return;
			}
		}
	}
}

void ESP8266_TCP::waitingForHardReset() {
	unsigned long t = millis();
	while(true) {
		if(available() > 0) {
			String data = readData(1000);
			if(data.equals("ready")) {
				debugPrintln("RESET");
				delay(2000);
				clearBuffer();
				return;
			}
		}
	}
}

String ESP8266_TCP::connectAccessPoint(String ssid, String pass) {
	setMode(WIFI_MODE_STATION);
	write("AT+CWJAP=\"" + ssid + "\",\"" + pass + "\"");
	delay(2000);
	debugPrintln(readData());
	debugPrintln(readData());
	debugPrintln(readData());
	waitingForReset(1000);
	String ip = waitingForJoin();
	debugPrintln(readData());
	debugPrintln(ip);
	return ip;
}

String ESP8266_TCP::waitingForJoin() {
	unsigned long timeout = 20000;
	unsigned long t = millis();
	while(millis() - t < timeout) {
		clearBuffer();
		write("AT+CIFSR");
		readData();
		String data = readData();
		if(!data.equals("") && !data.equals("ERROR") && !data.equals("busy now ...")) {
			debugPrintln(data);
			return data;
		}
		debugPrint(".");
		delay(500);
	}
	return "0.0.0.0";
}

bool ESP8266_TCP::setMode(int mode) {
	clearBuffer();
	write("AT+CWMODE=" + String(mode));
	delay(500);
	debugPrintln(readData());
	String data = readData();
	if(data.equals("no change")) {
		debugPrintln("No Change");
		return true;
	} else if(data.equals("OK")) {
		reset();
		debugPrintln("OK");
		return true;
	}
	data = readData();
	debugPrintln(data);
	if(data.equals("OK")) {
		reset();
		debugPrintln("OK");
		return true;
	}
	return false; 
}

bool ESP8266_TCP::setAP(String ssid, String pass, int channel) {
	clearBuffer();
	write("AT+CWSAP=\"" + ssid + "\",\"" + pass + "\"," + String(channel) + ",4");
	debugPrintln(readData());
	delay(4000);
	debugPrintln(readData());
	String data = readData();
	debugPrintln(data);
	return data.equals("OK"); 
}

bool ESP8266_TCP::isNewAPSetting(String ssid, String pass, int channel) {
	clearBuffer();
	write("AT+CWSAP?");
	debugPrintln(readData());
	String data = readData();
	debugPrintln(data);
	data = data.substring(8, data.length());

	byte ssidLength = findChar(data, 0, '\"');
	String ssidTmp = data.substring(0, ssidLength);
	byte passLength = findChar(data, ssidLength + 3, '\"');
	String passTmp = data.substring(ssidLength + 3, passLength);
	byte channelLength = findChar(data, passLength + 2, ',');
	int channelTmp = data.substring(passLength + 2, channelLength).toInt();

	if(ssidTmp.equals(ssid) && passTmp.equals(pass) && channelTmp == channel) {
		return true;
	} 
	return false;
}

bool ESP8266_TCP::setMux(int mux) {
	clearBuffer();
	write("AT+CIPMUX=" + String(mux));
	debugPrintln(readData());
	String data = readData();
	debugPrintln(data);
	if(data.equals("no change") || data.equals("OK")) 
		return true;
	data = readData();
	debugPrintln(data);
	return data.equals("OK"); 
}

bool ESP8266_TCP::enableTCPServer(int port) {
	clearBuffer();
	write("AT+CIPSERVER=1," + String(port));
	debugPrintln(readData());
	String data = readData();
	debugPrintln(data);
	if(data.equals("no change")) 
		return true;
	data = readData();
	debugPrintln(data);
	return data.equals("OK"); 
}

void ESP8266_TCP::openAccessPoint(String ssid, String pass, int channel) {
	if(!isNewAPSetting(ssid, pass, channel)) {
		debugPrintln("SET NEW AP");
		setAP(ssid, pass, channel);
		reset();
	}
	setMode(WIFI_MODE_AP);
}

bool ESP8266_TCP::closeTCPServer() {
	clearBuffer();
	write("AT+CIPSERVER=0");
	debugPrintln(readData());
	debugPrintln(readData());
	String data = readData();
	debugPrintln(data);
	return data.equals("OK");
}
/*
bool ESP8266_TCP::isTCPEnabled() {
	clearBuffer();
	write("AT+CIPMUX?");
	debugPrintln(readData());
	String data = readData();
	debugPrintln(data);
	return data.equals("+CIPMUX:1"); 
}*/

void ESP8266_TCP::closeTCPConnection() {
	delay(1000);
	clearBuffer();
	write("AT+CIPCLOSE");
	debugPrintln(readData());
}

void ESP8266_TCP::closeTCPConnection(int id) {
	delay(1000);
	clearBuffer();
	write("AT+CIPCLOSE=" + String(id));
	debugPrintln(readData());
}

void ESP8266_TCP::openTCPServer(int port, int timeout) {
	setMux(WIFI_MUX_MULTI);
	enableTCPServer(port);
	setTCPTimeout(timeout);
}

bool ESP8266_TCP::setTCPTimeout(int timeout) {
	clearBuffer();
	write("AT+CIPSTO=" + String(timeout));
	debugPrintln(readData());
	debugPrintln(readData());
	String data = readData();
	debugPrintln(data);
	return data.equals("OK");
}

void ESP8266_TCP::connectTCP(String ip, int port) {
	clearBuffer();
	write("AT+CIPSTART=\"TCP\",\"" + ip + "\"," + String(port) );
	debugPrintln(readData());

	setRunningState(WIFI_STATE_CONNECT);
}

void ESP8266_TCP::waitingForTCPConnection() {

}

void ESP8266_TCP::setRunningState(int state) {
	this->runningState = state;
}

int ESP8266_TCP::getRunningState() {
	return this->runningState;
}

void ESP8266_TCP::flush() {
	this->serial->flush();
}

int ESP8266_TCP::isNewDataComing(byte type) {
	if(type == WIFI_CLIENT) {
		String data = read();
		if(!data.equals("")) {
			if(data.substring(2, 6).equals("+IPD")) {
				int lastPosition = findChar(data, 7, ':');
				int length = data.substring(7, lastPosition).toInt() - 1;
				lastPosition += 2;
				this->clientMessage = data.substring(lastPosition, lastPosition + length);
				return WIFI_NEW_MESSAGE;
			} else if(data.substring(3, 6).equals("ets")) {
				//debugPrintln("RESET");
				delay(2000);
				clear();
				return WIFI_NEW_RESET;
			} else if(data.substring(6, 12).equals("Unlink") 
					|| (data.substring(9, 15).equals("Unlink") &&  data.substring(2, 7).equals("ERROR"))) {
				//debugPrintln("Disconnected");
				this->TCPConnected = false;
				setRunningState(WIFI_STATE_UNAVAILABLE);
				return WIFI_NEW_DISCONNECTED;
			} else if(data.substring(2, 4).equals("OK") && data.substring(6, 12).equals("Linked")) {
				setRunningState(WIFI_STATE_IDLE);
				//debugPrintln("Connected");
				this->TCPConnected = true;
				return WIFI_NEW_CONNECTED;
			} else if(data.substring(2, 9).equals("SEND OK")) {
				setRunningState(WIFI_STATE_IDLE);
				//debugPrintln("Sent!!");
				return WIFI_NEW_SEND_OK;
			} else if(data.substring(0, 14).equals("ALREAY CONNECT") 
					&& data.substring(16, 17).equals("OK") 
					&& data.substring(19, 24).equals("Unlink")) {
				this->TCPConnected = false;
				setRunningState(WIFI_STATE_UNAVAILABLE);
				return WIFI_NEW_DISCONNECTED;
			} else if(data.substring(0, 14).equals("ALREAY CONNECT")) {
				return WIFI_NEW_ALREADY_CONNECT;
			} else {
				//debugPrintln("******");
				//debugPrintln(data);
				this->clientMessage = data;
				return WIFI_NEW_ETC;
			}
		}
		return WIFI_NEW_NONE;
	} else if(type == WIFI_SERVER) {
		String data = read();
		if(!data.equals("")) {
			if(data.substring(2, 6).equals("+IPD")) {
				this->clientId = data.substring(7, 8).toInt();
				int lastPosition = findChar(data, 9, ':');
				int length = data.substring(9, lastPosition).toInt();

				if(data.charAt(lastPosition + 1) == '\n') {
					this->clientMessage = data.substring(lastPosition + 2, lastPosition + length + 1);
				} else {
					this->clientMessage = data.substring(lastPosition + 1, lastPosition + length);
				}
				
				//this->clientMessage = data.substring(lastPosition + 1, lastPosition + length);
				return WIFI_NEW_MESSAGE;
			} else if(data.substring(3, 6).equals("ets")) {
				//debugPrintln("RESET");
				delay(2000);
				clear();
				return WIFI_NEW_RESET;
			} else if((data.length() == 1 && data.charAt(0) == 0x0A) 
					|| (data.substring(2, 4).equals("OK") && data.substring(6, 12).equals("Unlink"))) {
				//debugPrintln("Disconnected");
				this->TCPConnected = false;
				setRunningState(WIFI_STATE_UNAVAILABLE);
				return WIFI_NEW_DISCONNECTED;
			} else if(data.substring(0, 4).equals("Link")
					|| data.substring(2, 4).equals("Link") 
					|| data.substring(5, 11).equals("Link")) {
				setRunningState(WIFI_STATE_IDLE);
				//debugPrintln("Connected");
				this->TCPConnected = true;
				return WIFI_NEW_CONNECTED;
			} else if(data.substring(2, 9).equals("SEND OK")) {
				delay(2000);
				setRunningState(WIFI_STATE_IDLE);
				//debugPrintln("Sent!!");
				return WIFI_NEW_SEND_OK;
			} else {
				//debugPrintln("******");
				//debugPrintln(data);
				//debugPrintln(data.substring(2, 9));
				debugPrintln("******");
				//debugPrintln(data);
				this->clientMessage = data;
				debugPrintln(data.substring(2, 4));
				debugPrintln(data.substring(6, 12));
				return WIFI_NEW_ETC;
			}
		}
		return WIFI_NEW_NONE;

		/*
			String data = read();
			if(!data.equals("")) {
				if(data.substring(2, 6).equals("+IPD")) {
					this->clientId = data.substring(7, 8).toInt();
					int lastPosition = findChar(data, 9, ':');
					int length = data.substring(9, lastPosition).toInt();
					lastPosition += 1;
					this->clientMessage = data.substring(lastPosition + 1, lastPosition + length);
					return true;
				} else if(data.substring(3, 6).equals("ets")) {
					debugPrintln("RESET");
					delay(2000);
					clear();
					return true;
				} else if(data.substring(6,12).equals("Unlink")) {
					debugPrintln("Disconnected");
					this->TCPConnected = false;
					return true;
				} else if(data.substring(0, 4).equals("Link")) {
					debugPrintln("Connected");
					this->TCPConnected = true;
					//getClientList();
					return true;
				} else {
					debugPrintln("ETC");
					debugPrintln(data);
					debugPrintln(data.substring(0, 3));
					return true;

				}
			}
			*/
		return false;
	}
}

int ESP8266_TCP::findChar(String str, int start, char c) {
	for(int i = start ; i < str.length() ; i++) {
		if(str.charAt(i) == c)
			return i;
	}
	return -1;
}

int ESP8266_TCP::getId() {
	return this->clientId;
}

String ESP8266_TCP::getMessage() {
	return this->clientMessage;
}

void ESP8266_TCP::clearBuffer() {
	while(available() > 0) {
		serial->read();
	}
}

void ESP8266_TCP::clear() {
	clearBuffer();
	this->clientId = -1;
	this->clientMessage = "";
	this->TCPConnected = false;	
}

void ESP8266_TCP::debugPrintln(String str) {
	if(this->isDebug)
		serialDebug->println(str);
}

void ESP8266_TCP::debugPrint(String str) {
	if(this->isDebug)
		serialDebug->print(str);
}

String ESP8266_TCP::readData() {
    String data = "";
    while(available() > 0) {
        char r = serial->read();
        if (r == '\n') {
            return data;
        } else if(r == '\r') {
        } else {
            data += r;  
        }
    }
    return "";
}

void ESP8266_TCP::write(String str) {
	this->serial->println(str);
	flush();
	delay(50);
}

bool ESP8266_TCP::send(String message) {
	if(getRunningState() == WIFI_STATE_IDLE) {
		debugPrintln("Send : " + message);
		write("AT+CIPSEND=" + String(message.length() + 1));
		serial->print(message + " ");
		flush();
		debugPrintln(readData());
		debugPrintln(readData());
		setRunningState(WIFI_STATE_SEND);
		return true;
	}
	return false;
}

bool ESP8266_TCP::send(int id, String message) {
	if(getRunningState() == WIFI_STATE_IDLE) {
		debugPrintln("ID : " + String(id) + " Send : " + message);
		clearBuffer();
		write("AT+CIPSEND=" + String(id) + "," + String(message.length() + 1));
		serial->print(message);
		flush();
		debugPrintln(readData());
		debugPrintln(readData());
		setRunningState(WIFI_STATE_SEND);
		return true;
	}
	return false;
	//waitForSendStatus();
}

/*
bool ESP8266_TCP::waitForSendStatus() {
	unsigned long timeout = 10000;
	unsigned long t = millis();
	while(millis() - t < timeout) {
		if(available()) {
			String status = readData(100);
			if(status.equals("SEND OK") || status.equals("OK")) {
				return true;
			} else if(!status.equals("") && !status.substring(0, 2).equals("AT") 
					&& !status.substring(0, 1).equals(">")) {
				return false;
			}
		}
	}
	return false;
}*/

void ESP8266_TCP::printClientList() {
	clearBuffer();
	write("AT+CWLIF");
	delay(1000);
	debugPrintln(readData(100));
	debugPrintln(readData(100));
	debugPrintln(readData(100));
	debugPrintln(readData(100));
	debugPrintln(readData(100));
	debugPrintln(readData(100));
}

int ESP8266_TCP::available() {
	return this->serial->available();
}

String ESP8266_TCP::read() {
	String data = readTCPData();
	if(data.equals("Unlink")) {
		this->TCPConnected = false;
		clearBuffer();
		return "";
	}
	return data;
}

String ESP8266_TCP::readData(unsigned long timeout) {
    String data = "";
	unsigned long t = millis();
    while(millis() - t < timeout) {
        if(available() > 0) { 
            char r = serial->read();
            if (r == '\n') {
                return data;
            } else if(r == '\r') {
            } else {
                data += r;  
                t = millis();
            }
        }
    }
    return "";
}

String ESP8266_TCP::readTCPData() {
	unsigned long timeout = 100;
	unsigned long t = millis();
    String data = "";
    while(millis() - t < timeout) {
    	if(available() > 0) {
	        char r = serial->read();
	        if(data.equals("Unlink")) {
				return data;
	        } else {
	            data += r;  
                t = millis();
	        }
	    }
    }
    return data;
}
