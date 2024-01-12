#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <RCSwitch.h>

#ifndef STASSID
#define STASSID "ASIWYFA_slow"
#define STAPSK  "8720AHaL7708AeMP"
#endif
#define SERVER_IP "192.168.1.116"

// http://192.168.1.116/m/0
// http://192.168.1.116/json

const int led = 13;
const int baud = 115200;
const int cmd_delay = 100;
int rcInPin = 2;  // D4

RCSwitch mySwitch = RCSwitch();

const char* ssid = STASSID;
const char* password = STAPSK;

const char* ALL_OFF = "/m/0";
const char* LIGHTS_OFF = "/m/1";
const char* ELECTRONICS_OFF = "/m/2";
const char* LIGHTS_ON = "/m/3";
const char* TV_DAY = "/m/4";
const char* TV_NIGHT = "/m/5";
const char* VINYL = "/m/6";

//ESP8266WebServer server(port);

String IP2String() {
  IPAddress address = WiFi.localIP();
  return String() + address[0] + "." + address[1] + "." + address[2] + "." + address[3];
}

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);
  Serial.begin(baud);

  Serial.println("Establish Wifi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  Serial.println("Establish RC Receive...");
  mySwitch.enableReceive(rcInPin);  // Receiver on interrupt 0 => that is pin #2; 1 => pin #3
  
  Serial.println("Setup done!");

  digitalWrite(led, 0);
}

void request(String suburl) {
  for (int counter = 0; counter < 10 ; ++counter)
  {
    if ((WiFi.status() == WL_CONNECTED)) {
  
      WiFiClient client;
      HTTPClient http;
  
      String url = String("http://") + SERVER_IP + suburl;
      Serial.print("[HTTP] begin... ");
      
      Serial.print(url + "\n");
      http.begin(client, url); //HTTP
      //http.addHeader("Content-Type", "application/json");
  
      Serial.print("[HTTP] ...\n");
      int httpCode = http.GET();
  
      if (httpCode > 0) {
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          Serial.printf("[HTTP] success\n");
          http.end();
          return;
        }
        else {
          Serial.printf("[HTTP] code: %d\n", httpCode);
        }
      } else {
        Serial.printf("[HTTP] failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      
      http.end();
      Serial.printf("[HTTP] done.\n");
    }
  }
}

void loop() {  
  if (mySwitch.available()) {
    int val = mySwitch.getReceivedValue();
    switch(val)
    {
      //ALL_OFF
      //LIGHTS_OFF
      //ELECTRONICS_OFF
      //LIGHTS_ON
      //TV_DAY
      //TV_NIGHT
      //VINYL      
      case 1361: Serial.println("A ON"); request(ALL_OFF); break;
      case 1364: Serial.println("A OFF"); request(ALL_OFF); break;
      case 4433: Serial.println("B ON"); request(LIGHTS_ON); break;
      case 4436: Serial.println("B OFF"); request(LIGHTS_OFF); break;
      case 5201: Serial.println("C ON"); request(TV_DAY); break;
      case 5204: Serial.println("C OFF"); request(VINYL); break;
      case 5393: Serial.println("D ON"); request(TV_NIGHT); break;
      case 5396: Serial.println("D OFF"); request(VINYL); break;
      default: Serial.println(val); break;
    }
    mySwitch.resetAvailable();
  }
}
