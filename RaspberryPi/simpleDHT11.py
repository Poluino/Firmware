import gpiozero
import Adafruit_DHT
import requests
import time
while True:
    time.sleep(9)
    h, t = Adafruit_DHT.read_retry(11, 4)
    weatherstation_url = 'https://beta.api.weatherstationproject.com/data/update'
    Message = {
        "token": "token",
        "rainfall": "-255",
        "wind_speed": "-255" ,
        "wind_direction": "-255",
        "soil_moisture": "-255",
        "lux": "-255",
        "uv_index": "-255",
        "temperature": t,
        "pressure": "-255",
        "humidity": h,
    }
    try:
        requests.post(weatherstation_url, json=Message)
        print('request sent')
    except Exception as E:
        print(E)`
