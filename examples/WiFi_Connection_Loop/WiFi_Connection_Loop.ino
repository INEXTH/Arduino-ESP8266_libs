/*
 *
 *	Reconnect to target Access Point until successful
 *
 */

#include <ESP8266_TCP.h>

// ESP8266 Class
ESP8266_TCP wifi;

// Target Access Point
#define ssid         "ESP_AP"
#define pass         "123456789"

// Connect this pin to CH_PD pin on ESP8266
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
    // Connect to Access Point
    String ip = connectAP();
  } 
  else 
  {
	// ESP8266 isn't available
    Serial.println("Check module connection and restart to try again..."); 
  }
}

void loop()
{

}

// Access Point Connection Function that you can loop connect to Access Point until successful
String connectAP() 
{
  String ip = "0.0.0.0";
  while(ip.equals("0.0.0.0")) 
  {
    ip = wifi.connectAccessPoint(ssid,pass);
    if(!ip.equals("0.0.0.0")) 
    {
      break;
    } 
  }
  return ip;
}

