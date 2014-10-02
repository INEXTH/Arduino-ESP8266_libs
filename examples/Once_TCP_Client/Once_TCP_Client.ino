/*
 *
 *	Client side for Once Connection per message (Connect > Send > Close) 
 *
 */

#include <ESP8266_TCP.h>

// ESP8266 Class
ESP8266_TCP wifi;

// Target Access Point
#define ssid         "ESP_AP"
#define pass         "123456789"

// TCP Server IP and port
#define serverIP    "192.168.43.1"
#define serverPort  2000

// Connect this pin to CH_PD pin on ESP8266
#define PIN_RESET    6

// Pin that connected to button to send any message
#define PIN_SEND     8

void setup()
{
  delay(3000);
  
  // Set pin for send command to input mode
  pinMode(PIN_SEND, INPUT);
  
  
  // We use Serial1 to interface with ESP8266 
  // and use Serial to debugging
  Serial.begin(9600);
  Serial1.begin(115200);
  wifi.begin(&Serial1, &Serial, PIN_RESET);
  
  /* If your board has only 1 serial port
   * or you didn't need to debugging, try this.
   *
   * Serial.begin(115200);
   * wifi.begin(&Serial, PIN_RESET);
   *
   */
  
  // Check that ESP8266 is available
  if(wifi.test()) 
  {
	// Connect to target Access Point
    String ip = connectAP();
  } 
  else 
  {
	// ESP8266 isn't available
    Serial.println("Check module connection and restart to try again..."); 
    while(true);
  }
}

void loop()
{
  // Check for any data has coming to ESP8266
  int dataState = wifi.isNewDataComing(WIFI_CLIENT);
  if(dataState != WIFI_NEW_NONE) {
    if(dataState == WIFI_NEW_CONNECTED) {
	  // Connected with TCP Server Side
	  // Send a message to TCP Server Side
      wifi.send("12345678");
    } else if(dataState == WIFI_NEW_DISCONNECTED) {
	  // Disconnected from TCP Server Side
      Serial.println("Disconnected");
    } else if(dataState == WIFI_NEW_SEND_OK) {
	  // Message transfer has successfully
      Serial.println("SENT!!!!");
    } else if(dataState == WIFI_NEW_ALREADY_CONNECT) {
	  // Already connected with TCP Server Side
      Serial.println("Already Connect!!");
    } 
  }
  
  // When button for connect to TCP Server Side was pressed
  if(!digitalRead(PIN_SEND)) {
    // Send message to TCP Server Side and waiting for 1 sec
    Serial.println("Connect!!");
    wifi.connectTCP(serverIP, serverPort);
    delay(1000);
  }
  
  delay(50);
}

// Access Point Connection Function that you can loop connect to Access Point until successful
String connectAP() 
{
  String ip = "0.0.0.0";
  while(ip.equals("0.0.0.0")) 
  {
    String ip = wifi.connectAccessPoint(ssid, pass);
    if(!ip.equals("0.0.0.0")) 
    {
      break;
    } 
  }
  return ip;
}
