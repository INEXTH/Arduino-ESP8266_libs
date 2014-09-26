/*
 *
 *	Server side for Simple Connection (Always Connected) 
 *
 */

#include <ESP8266_TCP.h>

// ESP8266 Class
ESP8266_TCP wifi;

// Target Access Point
#define ssid         "ESP_AP"
#define pass         "123456789"

// Connect this pin to reset pin on ESP8266
#define PIN_RESET    6

void setup()
{
  delay(3000);
  
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
	
	// Open TCP Server on port 2000 and 30 seconds for connection timeout (Max 2880)
    wifi.openTCPServer(2000, 30);
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
  int dataState = wifi.isNewDataComing(WIFI_SERVER);
  if(dataState != WIFI_NEW_NONE) {
    if(dataState == WIFI_NEW_CONNECTED) {
	  // Connected with TCP Client Side
      Serial.println("Status : Connected");
    } else if(dataState == WIFI_NEW_DISCONNECTED) {
	  // Disconnected from TCP Client Side
      Serial.println("Status : Disconnected");
    } else if(dataState == WIFI_NEW_MESSAGE) {
	  // Got a message from TCP Client Side
      Serial.println("ID : " + String(wifi.getId()));
      Serial.println("Message : " + wifi.getMessage());
    } else if(dataState == WIFI_NEW_SEND_OK) {
	  // Message transfer has successful
      Serial.println("SENT!!!!");
    } 
  }
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

