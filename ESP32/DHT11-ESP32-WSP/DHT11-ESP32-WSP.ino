/*

    * * *   ESP32 Weather Station Project   * * * 
    
    This sketch gets data from a DHT11 temperature and humidity sensor
    and posts it to weatherstationproject.com.
    
    It can be modified to collect and send these types of data:
    - Temperature
    - Humidity
    - Pressure
    - Rainfall
    - Wind Speed
    - Wind Direction
    - Soil Moisture
    - Pressure
    - Lux
    - UV Index
     
    Required Libraries:
    - ArduinoJson                 by Benoît Blanchon        https://github.com/bblanchon/ArduinoJson
    - DHT sensor library          by Adafruit Industries    https://github.com/adafruit/DHT-sensor-library
    - ESP32 Arduino HTTPClient    by Expressif Systems      https://github.com/espressif/arduino-esp32/tree/master/libraries/HTTPClient

    Hardware connections: 
    - Connect DHT11 sensor to GPIO pin 5
    
*/


// ------------------------------------------------------------------
//                        Importing libraries 
// ------------------------------------------------------------------
 
#include <Arduino.h>            
#include <ArduinoJson.h>      // JSON parsing and serializing
#include "DHT.h"              // DHT sensor library
#include <Wire.h>             // For I2C serial communication
#include "WiFi.h"             // Handles Wi-Fi (IEEE 802.11 b/g/n) connections
#include <HTTPClient.h>       // Handles HTTP resquests and responses

// ------------------------------------------------------------------
//               General variables and config stuff 
// ------------------------------------------------------------------

// Wi-Fi Network credentials 
const char* ssid     = "MyNetwork";
const char* password = "MyPassword";

// Just API things...

// Go to https://api.weatherstationproject.com/ to get the most recent API link!
const String API_URL = "https://api.weatherstationproject.com/api/v2/private/update_data.php";
 
// Go to Account --> Boards on https://weatherstationproject.com to get your board token!
const char* token = "board_token_goes_here!"; 

// Initializing the HTTP client
HTTPClient http;
 
// Timer for how often to post to website
long lastTime;
// Timer for 10 minutes
long timerDelay = 600000000;                // In microseconds! (10^-6 seconds)
 
#define DHTTYPE DHT11                       // DHT 11 sensor 
#define DHTPIN 5                            // Connect DHT11 to GPIO 5!

// Variables for sensor measurements
float temperature;
float humidity;
String temp_string;
String humidity_string;
boolean sent_data_success;
 
DHT dht(DHTPIN, DHTTYPE);                   // DHT sensor object



// ------------------------------------------------------------------
//                   Getting sensor measurements 
// ------------------------------------------------------------------



void getData() {
  
  temperature = dht.readTemperature();          // reads temperature  (Celsius)
  humidity = dht.readHumidity();                // reads air humidity (relative percentage)
  // Convert sensor readings into strings       
  temp_string = String(temperature);
  humidity_string = String(humidity);
 
}


// ------------------------------------------------------------------
//              Sending sensor data to the WSP website
// ------------------------------------------------------------------


 
void sendData(){
  
  http.begin(API_URL);                                    
  http.addHeader("Content-Type", "application/json");             // This header specifies that we're sending a JSON file
  
  StaticJsonDocument<200> json_data;                              // Building a JSON object with the sensor data
                                                                  // NOTE: All unused values *must* be posted as -255!
  json_data["token"] = token;
  json_data["temperature"]    = temp_string;
  json_data["humidity"]       = humidity_string;
  json_data["pressure"]       = "-255";
  json_data["rainfall"]       = "-255";
  json_data["wind_speed"]     = "-255";
  json_data["wind_direction"] = "-255";
  json_data["soil_moisture"]  = "-255";
  json_data["pressure"]       = "-255";
  json_data["lux"]            = "-255";
  json_data["uv_index"]       = "-255";
 
  String requestBody;
  serializeJson(json_data, requestBody);                          // Converting the JSON object into a String
  
  int httpResponseCode = http.POST(requestBody);                  // Sending an HTTP POST request + getting the HTTP response status code
  Serial.println(String("Response Code: ") + httpResponseCode);   // Printing the status code to the Serial Monitor
  Serial.println(http.getString());                               // Printing the HTTP response content (JSON) to the Serial Monitor 
  
  lastTime = esp_timer_get_time();                                // Using the ESP32's timer to get the current time (in micros!)
  http.end();               
                                        
}
 


// ------------------------------------------------------------------
//                       Connecting to Wi-Fi
// ------------------------------------------------------------------


void wifi_connect() {
  
  WiFi.begin(ssid, password);                                   // Connects to Wi-Fi using the provided network credentials 
 
  while (WiFi.status() != WL_CONNECTED) {                       // If the ESP32 couldn't connect to Wi-Fi...
      delay(500);                                               // ... keep trying  ¯\_(ツ)_/¯
      Serial.print(".");                                    
  }

  Serial.println("");
  Serial.println("Wi-Fi: Connected");                           // Once the loop is broken, the ESP32 is now connected to Wi-Fi!
  Serial.println("IP address: ");                               
  Serial.println(WiFi.localIP());                               // Printing the ESP32's local IP address to the Serial Monitor 
  
}                                                                 


// ------------------------------------------------------------------
//                      Setup and Initialization
// ------------------------------------------------------------------


 
void setup() {
  
  Serial.begin(115200);         // Standard baud rate for the ESP32 is 115200 bauds per second
  dht.begin();                  // Initializes the DHT library
                                                   
  // As soon as the ESP32 is plugged in, we take our measurements, connect to Wi-Fi and send our data!
  
  getData();                    // Gets data!
  wifi_connect();               // Connects to Wi-Fi!
  sendData();                   // Sends data! 
  
}



// ------------------------------------------------------------------
//                        Main program loop
// ------------------------------------------------------------------


 
void loop() {
  
  long time_start = esp_timer_get_time();       // Getting time at the start of this loop iteration
  
  // Check if the ESP32 is connected to Wi-Fi
  if (WiFi.status() == WL_CONNECTED){           // If the ESP32 is connected to Wi-Fi...
    long time = time_start - lastTime;          // Time difference between this loop iteration at the last data collection event
    if (time >= timerDelay){                    // If time exceeds timerDelay, we collect our data and send it again
      getData();                                // Gets data!
      sendData();                               // Sends data!
    }
  } else {                                      // If the ESP32 isn't connected to Wi-Fi... (or if the connection is lost)
    Serial.println("Wi-Fi: Not connected");     // Print a message on the Serial Monitor to alert the user
    wifi_connect();                             // Attempt to reconnect
  }
  
}
