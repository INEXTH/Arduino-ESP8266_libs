#include <ipst.h>
#include <ESP8266_TCP.h>

ESP8266_TCP wifi;

#define ssid        "ESP8266_AP"
#define pass        "00000000"
#define channel     5

#define PIN_RESET    17

void setup()
{
  delay(3000);

  glcdClear();
  glcdMode(3);
  for(int i = 0 ; i < 8 ; i++) {           
    glcdFillCircle((20 * i) + 10 , 50, 5, GLCD_WHITE);
  }

  Serial.begin(9600);
  Serial1.begin(115200);
  wifi.begin(&Serial1, &Serial, PIN_RESET);

  if(wifi.test()) 
  {
    wifi.openAccessPoint(ssid, pass, channel);
    //String ip = connectAP();
    wifi.openTCPServer(2000, 30);
  } 
  else 
  {
    Serial.println("Check module connection and restart to try again..."); 
    while(true);
  }
}

void loop()
{  
  int dataState = wifi.isNewDataComing(WIFI_SERVER);
  if(dataState != WIFI_NEW_NONE) {
    if(dataState == WIFI_NEW_CONNECTED) {
      wifi.send(0, "update");
    } 
    else if(dataState == WIFI_NEW_MESSAGE) {
      setDisplay(wifi.getMessage());
    } 
  }
}

String connectAP() 
{
  String ip = "0.0.0.0";
  while(ip.equals("0.0.0.0")) 
  {
    ip = wifi.connectAccessPoint(ssid, pass);
    if(!ip.equals("0.0.0.0")) 
    {
      break;
    } 
  }
  return ip;
}

void setDisplay(String str) {
  char state[8];

  for(int i = 0 ; i < 8 ; i++) {           
    state[i] = str.charAt(2 * i);
  }  

  for(int i = 0 ; i < 8 ; i++) {           
    glcdFillCircle((20 * i) + 10 , 50, 5, (state[i] == '1') ? GLCD_RED : GLCD_WHITE);
  }
}


