/*
 * Lucas Berbesson for La Fabrique DIY
 *
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> 
#include <EEPROM.h>
#include "play.h"
WiFiUDP Udp;
char  replyPacekt[64] = "";
IPAddress ipBroadCast(255, 255, 255, 255);
unsigned int localUdpPort = 8000;
boolean sendUdpPacket = true;
int knockReading = 0;      // variable to store the value read from the sensor pin
const int knockSensor = A0; // the piezo is connected to analog pin 0
const int threshold = 15;  // threshold value to decide when the detected sound is a knock or not
int score = 0;
unsigned long last_bounce = 0;
boolean playing = false;
boolean firstBounce = true;
unsigned long previousMillis = 0;
#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server = ESP8266WebServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

            // send message to client
            webSocket.sendTXT(num, "Ready!");
        }
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);

            if(payload[0] == '#') {
                // we get RGB data

                // decode rgb data
                uint32_t rgb = (uint32_t) strtol((const char *) &payload[1], NULL, 16);

            }

            break;
    }

}

void setup() {
    USE_SERIAL.begin(115200);
     WiFiManager wifiManager;
    // wifiManager.resetSettings();
     wifiManager.autoConnect("Ping Pong");
     Serial.println("connected...yeey :)");
     Serial.println( WiFi.SSID().c_str());
     Serial.println(WiFi.psk().c_str());
    //String ssid1 = WiFi.SSID().c_str();
    wifiManager.setConfigPortalTimeout(180);

    // start webSocket server
     Udp.begin(localUdpPort);
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    if(MDNS.begin("esp8266")) {
        USE_SERIAL.println("MDNS responder started");
        USE_SERIAL.println(WiFi.localIP());
    }
IPAddress myIP = WiFi.localIP();
    String myStr = String(myIP[0]) + "." +
                   String(myIP[1]) + "." +
                   String(myIP[2]) + "." +
                   String(myIP[3]);

  USE_SERIAL.println(myStr);  
String msg = "{\"ip\":\"";
       msg+= myStr;
       msg+= "\", \"port\":\"80\", \"name\":\"ping-pong\"}";
 msg.toCharArray(replyPacekt, 64);      
  USE_SERIAL.println(msg);      
    // handle index
    server.on("/", []() {
        // send index.html
        String s = MAIN_page;
        server.send(200, "text/html", s);
    });
    server.begin();
    // Add service to MDNS
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);
}

void loop() {
  if(sendUdpPacket == true)
    {
      Udp.beginPacket(ipBroadCast, localUdpPort);
    Udp.write(replyPacekt);
    Udp.endPacket();
    USE_SERIAL.println(replyPacekt);
    sendUdpPacket = false;
    }
    webSocket.loop();
    server.handleClient();
    knockReading = analogRead(A0);
    delay(1);
    unsigned long currentMillis = millis();
    // No bounce during more than 2 seconds is GAME OVER
    if (currentMillis - last_bounce > 2000 && playing == true) {
      webSocket.sendTXT(0,"Game over");
      score = -1;
      playing=false;
      firstBounce = true;
      delay(50);
    }
    if ((knockReading >= threshold) && firstBounce == true)
       { 
         score = score + 1;
      playing = true;
      last_bounce = millis();
      String thisString = String(score);
      webSocket.sendTXT(0,thisString);
      previousMillis = currentMillis;
      firstBounce = false;
      delay(50);
       }
    // If there is a bounce, send a message to websocket
    if ((knockReading >= threshold) && (currentMillis - previousMillis) > 300 && firstBounce == false) {
      score = score + 1;
      playing = true;
      last_bounce = millis();
      String thisString = String(score);
      webSocket.sendTXT(0,thisString);
      previousMillis = currentMillis;
      delay(50);
    } 
   
}
