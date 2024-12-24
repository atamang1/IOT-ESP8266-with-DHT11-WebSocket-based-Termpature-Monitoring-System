# ESP8266 with DHT11 WebSocket-based Temperature Monitoring System 

This project uses an ESP8266 microcontroller with a DHT11 temperature and humidity sensor, which communicates with a WebSocket hosted on an ASP.NET Web API server. The Angular frontend displays real-time temperature data from the DHT11 module. 

## Overview
The system allows real-time temerature data to be collected from the DHT11 sensor connected to the ESP8266 and transmitted via WebSocket to  a backend ASP.NET Web API server. The server then broadcasts the data to specific connected WebSocket clients. The Angular frontend continuously receives updates and displays the current temperature.

## Components
* ESP8266: A Wifi-enabled microcontroller used to read temperature and humidity data from DHT11 module and send it to the WebSocket Server
* DHT11 module: A temperature and humidity sensor that communicates with the ESP8266 to provide enviromental data
* ASP.NET Web API: A WebSocket server that listens for incoming data from the ESP8266 and forwards it to connected frontend client.
* Angular: The frontend framework used to display the live temerature and humidity data


## Hardware Wiring 
| ESP8266    | LCD |  
| -------- | ------- | 
| Vin      |    VDD |        
| GND      | VSS     |        
| D6      | V0    |        
| D0      |    RS |        
| GND      | RW     |        
| D1    | E    |        
| D2      |    D4 |        
| D3      | D5     |        
| D4    | D6    |        
| D5      |    D7 |        
| Vin      | A     |        
| GND    | K    |        

| ESP8266    | DHT11 |  
| -------- | ------- | 
| 3V3      |    + |        
| GND      | -     |        
| D7      |  S    |        

![circuit_diagram](https://github.com/user-attachments/assets/5eef5beb-3b71-4577-8f00-378eeb6574fd)


 ## Front-end - swipe display
 
Display 1 shows temperature in Fahrenheit scale: 
![front-end-f](https://github.com/user-attachments/assets/5032bf38-3d38-4051-bc9f-46cf83461d40)

Display 2 shows temperature in Celcius
![front-end-c](https://github.com/user-attachments/assets/a5a23d07-e1d3-4c2d-9c24-fa0e3ba5330c)

Display 3 shows humidity 
![front-end-h](https://github.com/user-attachments/assets/156e7044-4280-4a94-b0d3-390c8327e550)


## Running the Project 
#### Step 1: Upload the ESP8266 code 
* Use Ardunio IDE to upload the ESP8266 code to your board. Ensure it's connected to the same network as the WebSocket server
#### Step 2: Start the ASP.NET Web API Server
* Open your ASp.NET Web API project in visual Studio
* Run the server on http (ESP8266 doesn't support secure websocket)
#### Step 3: Open up the Angular frontend to visual studio code or desire IDE
* install dependencies:
```bash
npm install
```
* run it :
```bash
ng serve
```










 
