/*
 *  This code integrates ESP8266 with the temperature (DHT11), and WebSocket as Client. 
 */
// Libraries 
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <LiquidCrystal.h>
#include "DHT.h"
#include <Arduino_JSON.h>



// Temperature sensor 
#define DHTPIN D7  // digital pin 
#define DHTTYPE DHT11 //DHT type 
DHT dht(DHTPIN, DHTTYPE); 
float previousTemperature=0;

// Liquid Crystal Display 
const int rs = D0, en = D1, d4 = D2, d5 = D3, d6 = D4, d7 = D5; //assign digital pin 
LiquidCrystal lcd(rs, en, d4, d5, d6, d7); 



ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define USE_SERIAL Serial1
// Wi-Fi credentials
const char* ssid = "SSID"; // Replace with your 2.4GHz Wi-Fi network name
const char* password = "PASSWORD"; // Replace with your Wi-Fi password



void setup() {

  USE_SERIAL.begin(115200);

  initLCD(); //Initialize LCD 
  dht.begin();
  previousTemperature = getFahrenheit(); // initialize previous temperature

  
  WiFiMulti.addAP(ssid, password); 

  // Wait for Network to connect
  while(WiFiMulti.run() != WL_CONNECTED) {
    lcdRow1("Connecting..."); 
    delay(100);
  }

  lcdRow1("Connected");
  lcdRow2(WiFi.localIP().toString()); 
  

  // server address, port and URL
  webSocket.begin("address", PORT#, "/Path");

  // event handler
  webSocket.onEvent(webSocketEvent);


  // try ever 5000 again if connection has failed
  //webSocket.setReconnectInterval(5000);
  
  // start heartbeat (optional)
  // ping server every 15000 ms
  // expect pong from server within 3000 ms
  // consider connection disconnected if pong is not received 2 times
  //webSocket.enableHeartbeat(15000, 3000, 2);
lcdClearRow(0);
}

void loop() {
  webSocket.loop();
  lcdRow1(String(getFahrenheit()));
       // Check if temperature has changed
      if (!isSameTemperature()) {
        // Prepare the JSON data for the updated temperature
        //String jsonData = String(getFahrenheit());

        JSONVar doc;
        doc["sensor"] = "DHT11";
        doc["fahrenheit"] =  String(getFahrenheit()); 
        doc["celsius"] = String(getCelsius());
        doc["humidity"] = String(getHumidity()); 
        
        // Convert JSON object to string
        String jsonString = JSON.stringify(doc);
        
        // Send the updated temperature data to the WebSocket server
        webSocket.sendTXT(jsonString);
        
        // Optional: Debug message
        //USE_SERIAL.printf("Temperature sent: %s\n", jsonData.c_str());
      }
 
}


//*************************Start: LCD functions*********************************************************
void initLCD() {
  const int contrast = 50; 
  analogWrite(D6, contrast); //
  lcd.begin(16, 2); //Sepecify the LCD's number of columns and rows. 
}

void lcdRow1(String data)
{
  lcd.setCursor(0, 0); 
  lcd.print(data); 
}

void lcdRow2(String data)
{
  lcd.setCursor(0, 1); 
  lcd.print(data);
}

void lcdClearRow(int row)
{
  lcd.setCursor(0, row); 
  for(int n =0; n < 15; n++)
  {
    lcd.print(" ");
  }
}

//*************************End: LCD function*********************************************************


//*************************Start: DHT11 function*********************************************************

float getFahrenheit() {
  return dht.readTemperature(true);
}
float getCelsius() {
  return dht.readTemperature();
}
float getHumidity() {
  return dht.readHumidity();
}

bool isSameTemperature() {
  float currentTemperature = getFahrenheit();
  if (previousTemperature != currentTemperature) {
    previousTemperature = currentTemperature;  // Update the previous temperature
    return false;  // Temperature has changed
  }
  return true;  // No change in temperature
}

bool SensorReadFailed(){

  if(isnan(getHumidity()) || isnan(getCelsius()) || isnan(getFahrenheit()))
  {
    return true;
  }
  return false;
}
//*************************End: DHT11 function*********************************************************



//*************************Start: webSocketEvent function*********************************************************

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

  bool isSame = previousTemperature == dht.readTemperature(true)? true: false; 
  if(isSame)
  {
    previousTemperature = dht.readTemperature(true);
  }
  //String jsonData =String(getFahrenheit()); 
  
  switch(type) {
    case WStype_DISCONNECTED:
      USE_SERIAL.printf("[WSc] Disconnected!\n");
      break;
    case WStype_CONNECTED: {
      USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

      // send message to server when Connected
      webSocket.sendTXT("Hello from Esp8266 Connected");
    }
      break;
    case WStype_TEXT:
      USE_SERIAL.printf("[WSc] get text: %s\n", payload);

      // Prepare JSON data to send
      // Prepare JSON data to send
//    String jsonData = "{\"sensorName\": \"DHT11\","
//                     + "\"fahrenheit\": " + String(getFahrenheit()) + ","
//                     + "\"celsius\": " + String(getCelsius()) + ","
//                     + "\"humidity\": " + String(getHumidity()) + "}";

    // Send the JSON data to the server
 
      break;
    case WStype_BIN:
      USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);

      // send data to server
      // webSocket.sendBIN(payload, length);
      break;
        case WStype_PING:
            // pong will be send automatically
            USE_SERIAL.printf("[WSc] get ping\n");
            break;
        case WStype_PONG:
            // answer to a ping we send
            USE_SERIAL.printf("[WSc] get pong\n");
            break;
    }

}
//*************************End: webSocketEvent function*********************************************************
