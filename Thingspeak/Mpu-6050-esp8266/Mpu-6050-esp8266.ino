#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include "secrets.h" // Store sensitive information 

Adafruit_MPU6050 mpu;

//ThingSpeak details
unsigned long myChannelNumber = SECRET_CH_ID; 
const char *myWriteAPIKey = SECRET_WRITE_APIKEY;

char ssid[] = SECRET_SSID;   //network SSID
char pass[] = SECRET_PASS;   //network password
WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(100);

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    delay(5000);
  }
  Serial.println("\nConnected to Wi-Fi.");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);
}

void loop() {
  // Get sensor readings
    sensors_event_t accel, gyro, temp;
    mpu.getEvent(&accel, &gyro, &temp);

  // Connect or reconnect to Wi-Fi
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("Reconnecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      delay(5000);
    }
    Serial.println("Connected.");
  }

  //{0:left, 1:right, 3:up, 4:down}

  // Prepare data to send to ThingSpeak
  ThingSpeak.setField(1, accel.acceleration.x); // Accelerometer X
  ThingSpeak.setField(2, accel.acceleration.y); // Accelerometer Y
  ThingSpeak.setField(3, accel.acceleration.z); // Accelerometer Z
  ThingSpeak.setField(4, gyro.gyro.x);          // Gyroscope X
  ThingSpeak.setField(5, gyro.gyro.y);          // Gyroscope Y
  ThingSpeak.setField(6, gyro.gyro.z);          // Gyroscope Z
  ThingSpeak.setField(7, 0);                    // gesture

  // Write data to ThingSpeak
  int httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (httpCode == 200) {
    Serial.println("Channel write successful.");
  } else {
    Serial.println("Problem writing to channel. HTTP error code " + String(httpCode));
  }

  // Wait 0.5 seconds to update again 
  delay(500);
}
