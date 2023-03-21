#include <Wire.h>
#include <Ultrasonic.h>
#include <WiFi.h>

// Firebase ESP32 library
#include <FirebaseESP32.h>

#define WIFI_SSID "WIFINAME"
#define WIFI_PASSWORD "WIFIPASSWORD"


//Token Helper
#include "addons/TokenHelper.h"

//RTD PAyload Helper
#include "addons/RTDBHelper.h"

// Defining the Digital Pin of On Board LED.
#define On_Board_LED 2

// Insert Firebase project API Key
#define API_KEY "API_KEY"

// Insert Firebase Database Url
#define DATABASE_URL "DATABASE_URL"

// Defining Firebase Data .
FirebaseData fbdo;

// Defining firebase Auth.
FirebaseAuth auth;

// Defining firebase Config.
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
const long sendDataIntervalMillis = 5000; 


bool signupOK = false;

float store_random_Float_Val;
int store_random_Int_Val;

//TrigPin and echoPin port on ESP32
const int trigPin = 5;
const int echoPin = 18;
Ultrasonic ultrasonic(trigPin, echoPin);

void setup() {
  Serial.begin(115200);
  Serial.println();

  //PinMode of trigPin and echoPin on ESP32
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

//Setting up Wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connection........");
  Serial.print("Connecting to : ");
  Serial.println(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");

//Used to send a trigger signal from a microcontroller to a sensor module
    digitalWrite(trigPin, HIGH);
    delay(250);
    digitalWrite(trigPin, LOW);
    delay(250);
  }
  digitalWrite(trigPin, LOW);
  Serial.println();
  Serial.print("Successfully connected to : ");
  Serial.println(WIFI_SSID);
  
  Serial.println("***********************************");


  // Assigning api key.
  config.api_key = API_KEY;

  // Assigning the Database Url.
  config.database_url = DATABASE_URL;

  // Signing up new user
  Serial.println();
  Serial.println("Sign up......");
  Serial.print("Sign up new user..... ");
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  Serial.println("******************");
  
  
  config.token_status_callback = tokenStatusCallback; 
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

}

void loop() {
  long duration, distance;
  String Parking_Status, Concat;

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > sendDataIntervalMillis || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
//Preparing the trigPin for the trigger pulse by setting it to a logic LOW state
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
//Sending the trigger pulse to the ultrasonic sensor module by setting the trigPin to a logic HIGH state for 10 microseconds and then setting it back to LOW. 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  //Measures the duration of the pulse
  duration = pulseIn(echoPin, HIGH);
  //Calculates the Distance
  distance = (duration / 2) / 29.1;
  Serial.println(distance);
  
//According to the condition, prints and sends the distance and status of the parking spot to Firebase Database
  if (distance <=10) {
     Serial.println(distance);
    Serial.println("Spot Occupied");
    Parking_Status="Not Available";
    Firebase.RTDB.setString(&fbdo, "Test/Parking_Status", Parking_Status);
    Firebase.RTDB.setInt(&fbdo, "Test/Distance", distance);
  } else {
     Serial.println(distance);
    Serial.println("Spot Available");
    Parking_Status="Available";
    Firebase.RTDB.setString(&fbdo, "Test/Parking_Status", Parking_Status);
     Firebase.RTDB.setInt(&fbdo, "Test/Distance", distance);
  }

  digitalWrite(trigPin, HIGH);

  
    
    digitalWrite(trigPin, LOW);
    Serial.println("*****************************************");

  delay(2000);
}
}