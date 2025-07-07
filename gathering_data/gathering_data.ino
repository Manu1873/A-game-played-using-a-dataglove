#include <UbiBuilder.h>
#include <UbiConstants.h>
#include <UbiHttp.h>
#include <UbiProtocol.h>
#include <UbiProtocolHandler.h>
#include <UbiTcp.h>
#include <UbiTypes.h>
#include <UbiUdp.h>
#include <UbiUtils.h>
#include <Ubidots.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ESP8266WiFi.h>

#include "secrets.h" // Store sensitive information 


//ThingSpeak details
//unsigned long myChannelNumber = SECRET_CH_ID; 
//const char *myWriteAPIKey = SECRET_WRITE_APIKEY;

char ssid[] = SECRET_SSID;   //network SSID
char pass[] = SECRET_PASS;   //network password
WiFiClient wifiClient;
Ubidots ubidotsClient(TOKEN);


// MPU6050 instance
Adafruit_MPU6050 mpu;     
double gestureValue = 0.0;
String currentGesture; // Variable to store the gesture label

void setup() {
  Serial.begin(115200);
  
  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1);
  }
  
  // Connect to WiFi
  WiFi.begin(SECRET_SSID, SECRET_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Mapping gesture labels to numeric values
    if (currentGesture == "left") {
        gestureValue = 1.0;
    } else if (currentGesture == "right") {
        gestureValue = 2.0;
    } else if (currentGesture == "up") {
        gestureValue = 3.0;
    } else if (currentGesture == "down") {
        gestureValue = 4.0;
    }
  
  // Fetch accelerometer and gyroscope data
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  // Send data to Ubidots
    ubidotsClient.add("gesture_label", gestureValue); // The gesture label
    ubidotsClient.add("acc_x", accel.acceleration.x);
    ubidotsClient.add("acc_y", accel.acceleration.y);
    ubidotsClient.add("acc_z", accel.acceleration.z);
    ubidotsClient.add("gyro_x", gyro.gyro.x);
    ubidotsClient.add("gyro_y", gyro.gyro.y);
    ubidotsClient.add("gyro_z", gyro.gyro.z);
    
    ubidotsClient.send();  // Send all the variables

    delay(1000);  // Adjust this delay for the rate at which you want to gather data
}