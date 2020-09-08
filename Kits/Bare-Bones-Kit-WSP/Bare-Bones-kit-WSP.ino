/*
THSI SKETCH IS NOT DONE AND WILL NOW WORK!!!! The code for the BME280 sensor needs to be added

This sketch is for a ESP32 and works with our Bare Bones Kit
What it does:
This sketch gets data from a BME280 temperature, humidity, and pressure sensor
and posts it to weatherstationproject.com

It can be modified to collect and send these types of data:
-Temperature
-Humidity
-Pressure
-Rainfall
-Wind Speed
-Wind Direction
-Soil Moisture
-Pressure
-Lux
-UV Index

Required Libraries:
ArduinoJson By bartoz Bielawski
DHT sensor library By Adafruit
AduinoHttpClient By Arduino

*/


#include <Arduino.h>
/*JSON Library*/
#include <ArduinoJson.h>
/*I2C Library*/
#include <Wire.h>
/*Wifi*/
#include "WiFi.h"
#include <HTTPClient.h>
// wifi name and password so the esp32 can connect to it
const char* SSID = "wifi_ssid";
const char* PASSWORD = "wifi_pass";
int connect_attempts = 0;
int max_attempts = 10;
const char* token = "board_token"; //get board token from website
const String API_URL = "https://api.weatherstationproject.com/api/v2/private/update_data.php";
HTTPClient http;

//timer for how often to post data to website
long lastTime;
//set for 10 minutes
long timerDelay = 600000000; //in micros

float temperature;
float humidity;
float pressure;
String temp_string;
String humidity_string;
String pressure_string;
boolean sent_data_success;


void getData() {
  //Read sensors
  temperature = ;
  humidity = ;
  pressure = ;
  //Turn sensor values into strings
  temp_string = String(temperature);
  humidity_string = String(humidity);
  pressure_string = String(pressure);
}

void sendData(){
  http.begin(API_URL);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<200> json_data;
  // Add values in the document
  json_data["token"] = token;
  json_data["temperature"] = temp_string;
  json_data["humidity"] = humidity_string;
  //all values nut used must be posted as -255
  json_data["pressure"] = "-255";
  json_data["rainfall"] = "-255";
  json_data["wind_speed"] = "-255";
  json_data["wind_direction"] = "-255";
  json_data["soil_moisture"] = "-255";
  json_data["pressure"] = "-255";
  json_data["lux"] = "-255";
  json_data["uv_index"] = "-255";

  String requestBody;
  serializeJson(json_data, requestBody);
  //send data to website and get server response
  int httpResponseCode = http.POST(requestBody);
  Serial.println(httpResponseCode);
  Serial.println(http.getString());
  if (httpResponseCode == 200) {
      lastTime = esp_timer_get_time();
  } else {
      lastTime = esp_timer_get_time();
  }
  http.end();
}


void setup() {
  Serial.begin(9600);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    if (connect_attempts >= max_attempts) {
      Serial.println("Unable to connect to the wifi!");
      Serial.println("Board will not be");
      Serial.println("connected to the");
      Serial.println("internet!");
      delay(2000);
      break;
    }
    else {
      Serial.println("Attempting to connect to wifi");
      connect_attempts = connect_attempts + 1;
    }
  }
  //get and send data as soon as esp32 is plugged in
  getData();
  sendData();
}

void loop() {
  long time_start = esp_timer_get_time();
  //check if wifi is connected
  if (WiFi.status() == WL_CONNECTED){
    long time = time_start - lastTime;
    //check if delay is done
    if (time >= timerDelay){
      //get and send data
      getData();
      sendData();
    }
  } else {
    Serial.println("Wifi: Not connected");
  }
}
