/*
    The Weather Station Project's Firmware for the BareBones Kit.
    This file is protected by the GNU General Public License V -3.0
    More information about the license of this file can be found in the 
    LICENSE file on our GitHub Repository.
    For More information about the GNU General Public License V - 3.0,
    Visit their website at https://www.gnu.org/licenses/gpl-3.0.en.html

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/*
    Edited on 9th September 2020
      by Dhruv Sujatha
*/

/*
    This sketch is written to work with the ESP32 with the BME280 Sensor.
    This sketch gets data from a BME280 Environmental Sensor
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
    ArduinoJson By Bartoz Bielawski
    BME280 Sensor library By Adafruit Industries
    HTTPClient By Arduino
    WiFi By Arduino
*/


// Lets first include the required libraries to use in our code.

#include <Arduino.h> //Include this is you are using VS Code or similar Editors. Arduino IDE Doesn't need this.
#include <ArduinoJson.h> //This library is used to create json document which we will sent to the website using an API.
#include <Adafruit_BME280.h> //This is a library contributed by the Adafruit Industries which helps us interface with the BME280 sensor
#include <HTTPClient.h> //This Library is used to make HTTP Requests such as POST.
#include <WiFi.h> //This Library is used to connect the ESP32 to your WiFi Network

HTTPClient http;

Adafruit_BME280 bme;

float temperature;
float humidity;
float pressure;

long timerDelay = 60000;//Enter the Delay time (in milliseconds) here. It defines how often the ESP32 sends data to the website. 

String API_URL = "Paste the most recent API URL here."; //Visit our website to get the Most recent version of the API URL

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  WiFi.begin("Your SSID", "Your Password"); //Make sure to put in your WiFi SSID and Password
  Serial.println("Connecting...");

  while (WiFi.status() != WL_CONNECTED) {  // To check if the ESP32 is connected to your WiFi Network
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED){
    Serial.println("Connected to :");
    Serial.println(WiFi.SSID()); //Prints out the SSID
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP()); //Prints out the IP Address of the ESP32 in your Network
  }

  if (!bme.begin()) { //This step initializes the BME280 sensor and if the sensor isn't recognized, it throws an Error Message 
    Serial.println(F("Could not find a valid BME280 sensor, check wiring!"));
    while (1) delay(10);
  }
}

void loop(){

  temperature = bme.readTemperature(); //Retrieves the Temperature Data from the BME280.
  pressure = bme.readPressure(); //Retrieves the Pressure Data from the BME280.
  humidity = bme.readHumidity(); //Retrieves the Humidity Data from the BME280.

  String str_temperature = String(temperature); //Making a String Variable hold the Temperature data for further use.
  String str_pressure = String(pressure); //Making a String Variable hold the Pressure data for further use.
  String str_humidity = String(humidity); //Making a String Variable hold the Humidity data for further use.

  Serial.print("[HTTPS] begin...\n");
  if (http.begin("https://api.weatherstationproject.com/api/v2/private/update_data.php")) {  // HTTPS connection between the ESP32 and the Website is established.

  http.addHeader("Content-Type", "application/json"); //A Header is added to the Connection
  }

  StaticJsonDocument<200> json_data; //The JSON Document which will be used to send the data to the website is created
  // Add values in the document
  //
  json_data["token"] = "Your Board's Token"; //Paste your board's token here form your account page on our Website.
  json_data["rainfall"] = "-255";            //These are the parameters supported by our API. As the Barebones Kit Only comes with
  json_data["wind_speed"] = "-255";          //Temperature, Pressure, and Humidity, we are using the value "-255" to indicate that 
  json_data["wind_direction"] = "-255";      //the respective sensors are not connected / used.
  json_data["soil_moisture"] = "-255";
  json_data["pressure"] = "-255";
  json_data["lux"] = "-255";
  json_data["uv_index"] = "-255";               
  json_data["temperature"] = str_temperature; //We are using the String variable created earlier here to add it to the JSON document
  json_data["pressure"] = str_pressure;
  json_data["humidity"] = str_humidity;

  String requestBody;
  serializeJson(json_data, requestBody); //Finalizes the JSON document and makes it ready for transfer.


  int httpResponseCode = http.POST(requestBody); //Handles the POST Request to the API
  Serial.println(httpResponseCode); // Prints out the Response Code of the API. Useful to determine if the code is working correctly.
  String payload = http.getString(); 
  Serial.println(payload); // Prints out the Response paylode of the API. Gives a more human readable version of the Response Code.

  http.end(); //Ends the HTTP Connection to the website as the work is done.
  
  delay(timerDelay); //Starts the delay which was initialized in the start of the code. After the set time, the board sends data to the website again.

}
