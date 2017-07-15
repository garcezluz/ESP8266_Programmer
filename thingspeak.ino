/**
 * \file
 *       ESP8266 RESTful Bridge example
 * \author
 *       Tuan PM <tuanpm@live.com>
 *       
 *       Adaptado DaLuz
 */

#include <SoftwareSerial.h>
#include <espduino.h>
#include <rest.h>
//#include "DHT.h"

//dht DHT;

#define RESETESP  4

SoftwareSerial debugPort(2, 3); // RX, TX

ESP esp(&Serial, &debugPort, 4);

REST rest(&esp);

boolean wifiConnected = false;
int x=10,y=30;
void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if(res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if(status == STATION_GOT_IP) {
      debugPort.println("WIFI CONNECTED");
     
      wifiConnected = true;
    } else {
      wifiConnected = false;
    }
    
  }
}

void setup() {
//  Serial.begin(115200);
//  debugPort.begin(115200);
pinMode(RESETESP, OUTPUT);

  Serial.begin(19200);
  debugPort.begin(19200);

  digitalWrite(RESETESP, LOW);
  delay(250);
  digitalWrite(RESETESP, HIGH);
  delay(250);
  
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while(!esp.ready());

  debugPort.println("ARDUINO: setup rest client");
  if(!rest.begin("api.thingspeak.com")) {
    debugPort.println("ARDUINO: failed to setup rest client");
    while(1);
  }

  /*setup wifi*/
  debugPort.println("ARDUINO: setup wifi");
  esp.wifiCb.attach(&wifiCb);

  esp.wifiConnect("LSERF","lserflab25x");
  debugPort.println("ARDUINO: system started");
}

void loop() {
  char response[266];
  esp.process();
  if(wifiConnected) {
    //int chk = DHT.read11(7);
    if(1){
      char buff[64];
      char str_hum[6]="", str_temp[6]="";
      x+=3;
      y+=5;
      dtostrf(x, 4, 2, str_hum);
      dtostrf(y, 4, 2, str_temp);
      rest.begin("api.thingspeak.com");
      sprintf(buff, "/update?api_key=110UYAPC7KQ3WQ2Q&field1=%s&field2=%s", str_hum, str_temp);
      debugPort.println(buff);
      rest.get((const char*)buff);
      debugPort.println("ARDUINO: send get");

      if(rest.getResponse(response, 266) == HTTP_STATUS_OK){
        debugPort.println("ARDUINO: GET successful");
        debugPort.println(response);

/*
      rest.begin("api.pushbullet.com");
      rest.setContentType("application/json");
      rest.setHeader("Access-Token: <>\r\n");
      sprintf(buff, "{\"type\": \"note\", \"title\": \"Alerta:\", \"body\": \"Status = [indat]\"}");
      debugPort.println(buff);
      rest.post("/v2/users/me", (const char*)buff);
*/        
      }
      delay(30000);
      
    } else {
      debugPort.print("error,\r\n"); 
    }
/*  
  char data_buf[256];
  rest.begin("api.pushbullet.com", 443, true);
  rest.setContentType("application/json");
  rest.setHeader("Authorization: Bearer o.6jVIavnzQHAxdbvQYOjaWLT5PTp6RHTv\r\n");
  sprintf(data_buf, "{\"type\": \"note\", \"title\": \"Alarme Solidario:\", \"body\": \"Status = [%d]\"}",x+y);
  debugPort.println(data_buf);
  rest.post("/v2/pushes", (const char*)data_buf);
  debugPort.println("ARDUINO: send post");
*/
    
    
  }
  
  
}
