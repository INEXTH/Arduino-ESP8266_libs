#include <ESP8266_TCP.h>

ESP8266_TCP wifi;

#define ssid        "ESP8266_AP"
#define pass        "00000000"

#define serverIP    "192.168.4.1"
#define serverPort  2000

#define PIN_RESET    2

#define SW_0         13
#define SW_1         12
#define SW_2         11
#define SW_3         10
#define SW_4         9
#define SW_5         8
#define SW_6         7
#define SW_7         6

#define LED_0        A2
#define LED_1        A3
#define LED_2        A4
#define LED_3        A5

int sw_pin[8] = { 
  SW_0, SW_1, SW_2, SW_3, SW_4, SW_5, SW_6, SW_7 };

boolean sw[8] = { 
  false, false, false, false, false, false, false, false };

void setup()
{
  delay(3000);

  pinMode(SW_0, INPUT);
  pinMode(SW_1, INPUT);
  pinMode(SW_2, INPUT);
  pinMode(SW_3, INPUT);
  pinMode(SW_4, INPUT);
  pinMode(SW_5, INPUT);
  pinMode(SW_6, INPUT);
  pinMode(SW_7, INPUT);

  pinMode(LED_0, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);

  digitalWrite(LED_0, HIGH);
  digitalWrite(LED_1, HIGH);
  digitalWrite(LED_2, HIGH);
  digitalWrite(LED_3, HIGH);

  Serial.begin(115200);
  wifi.begin(&Serial, PIN_RESET);

  if(wifi.test()) 
  {
    digitalWrite(LED_0, LOW);
    String ip = connectAP();
    digitalWrite(LED_0, HIGH);
  } 
  else 
  {
    digitalWrite(LED_3, LOW);
    while(true);
  }
}

void loop()
{
  int dataState = wifi.isNewDataComing(WIFI_CLIENT);
  if(dataState != WIFI_NEW_NONE) {
    if(dataState == WIFI_NEW_CONNECTED) {
      digitalWrite(LED_1, LOW);
    } 
    else if(dataState == WIFI_NEW_DISCONNECTED) {
      digitalWrite(LED_1, HIGH);
    } 
    else if(dataState == WIFI_NEW_MESSAGE) {
      if(wifi.getMessage().equals("update")) {
        update();
      }
    } 
    else if(dataState == WIFI_NEW_SEND_OK) {
      digitalWrite(LED_2, HIGH);
    } 
  }

  if(isSwitchChanged()) {
    update();
  }

  if(wifi.getRunningState() == WIFI_STATE_UNAVAILABLE) {
    delay(500);
    wifi.connectTCP(serverIP, serverPort);
    delay(500);
  }

  delay(50);
}

boolean isSwitchChanged() {
  for(int i = 0 ; i < 8 ; i++) {
    if(digitalRead(sw_pin[i]) != sw[i]) {
      digitalWrite(LED_2, LOW);
      return true;  
    }
  }
  return false;
}

void update() {
  String str = "";
  for(int i = 0 ; i < 8 ; i++) {
    boolean state = digitalRead(sw_pin[i]);
    sw[i] = state;
    str += state;
    if(i < (8 - 1)) {
      str += ",";
    }
  }
  wifi.send(str);
  delay(500);  
}

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

