#include "arduino_secrets.h"          // Contains WiFi credentials (SSID and password)
#include <WiFiS3.h>                   // Library for WiFi connectivity
#include <MQTTClient.h>              // Library for MQTT communication
#include <ArduinoJson.h>             // Library to parse and handle JSON data
#include <AccelStepper.h>            // Library to control stepper motors

// Initialize two stepper motors with STEP and DIR pins
AccelStepper stepper1(1, 3, 4);
AccelStepper stepper2(1, 5, 6);

const int zeroPositionPin = 9;       // Pin connected to zero position sensor
const int enableDrivers = 7;         // Pin to enable stepper drivers
bool driversEnabled = false;         // Flag to track driver state

// MQTT configuration
const char MQTT_BROKER_ADRRESS[] = "SECRET_ADDRESS";
const int MQTT_PORT = 1883;
const char MQTT_CLIENT_ID[] = "dogfeeder";
const char SUBSCRIBE_TOPIC[] = "dogfeeder/receive";

WiFiClient network;                  // Network client
MQTTClient mqtt = MQTTClient(256);   // MQTT client with 256-byte buffer

#define MAX_QUEUE_SIZE 24            // Maximum number of feed requests

// Struct to store feeding request info
struct FeedRequest {
  int angle;     // Angle for the rotating stepper
  int amount;    // Amount of food to dispense
};

// Circular queue for feeding requests
FeedRequest feedQueue[MAX_QUEUE_SIZE];
int queueStart = 0;
int queueEnd = 0;
bool queueFull = false;
bool isFeeding = false;
FeedRequest currentFeed;
bool hasCurrentFeed = false;

// Add a feed request to the queue
void enqueue(int angle, int amount) {
  if (queueFull) return;
  feedQueue[queueEnd] = {angle, amount};
  queueEnd = (queueEnd + 1) % MAX_QUEUE_SIZE;
  if (queueEnd == queueStart) queueFull = true;
}

// Retrieve the next feed request from the queue
bool dequeue(FeedRequest &req) {
  if (queueStart == queueEnd && !queueFull) return false;
  req = feedQueue[queueStart];
  queueStart = (queueStart + 1) % MAX_QUEUE_SIZE;
  queueFull = false;
  return true;
}

void setup() {
  Serial.begin(9600);

  // Configure stepper speeds and acceleration
  stepper1.setMaxSpeed(200);
  stepper1.setAcceleration(100);
  stepper2.setMaxSpeed(200);
  stepper2.setAcceleration(100);

  // Setup pin modes
  pinMode(zeroPositionPin, INPUT);
  pinMode(enableDriver1, OUTPUT);
  digitalWrite(enableDrivers, LOW);     // Initially disable drivers
  zeroStepper();                        // Zero Food Chute

  // Connect to WiFi
  while (WiFi.begin(SECRET_SSID, SECRET_PASSWORD) != WL_CONNECTED) {
    Serial.print("Connecting to WiFi...");
    delay(5000);
  }
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP());

  // Setup MQTT
  mqtt.begin(MQTT_BROKER_ADRRESS, MQTT_PORT, network);
  mqtt.onMessage(messageHandler);

  while (!mqtt.connect(MQTT_CLIENT_ID)) {
    Serial.print(".");
    delay(500);
  }

  mqtt.subscribe(SUBSCRIBE_TOPIC);
  Serial.println("MQTT connected and subscribed.");
}

void loop() {
  mqtt.loop();  // Maintain MQTT connection

  // Enable drivers if queue has pending jobs
  if (!driversEnabled && (queueStart != queueEnd || queueFull)) {
    digitalWrite(enableDrivers, HIGH);
    driversEnabled = true;
    delay(5000);  // Wait for motor drivers to stabilize
  }

  // Start next feed if idle
  if (!isFeeding && !hasCurrentFeed) {
    if (dequeue(currentFeed)) {
      Serial.println("Starting next feed...");
      stepper2.moveTo(currentFeed.angle);  // Rotate to dog bowl
      stepper2.runToPosition();            // Wait until rotation finishes
      stepper1.moveTo(currentFeed.amount + stepper1.currentPosition());  // Dispense
      isFeeding = true;
      hasCurrentFeed = true;
    }
  }

  // Dispensing process
  if (isFeeding) {
    stepper1.run();  // Non-blocking move
    if (stepper1.distanceToGo() == 0) {
      isFeeding = false;  // Done dispensing
    }
  }

  // Reset after feed
  if (!isFeeding && hasCurrentFeed) {
    hasCurrentFeed = false;
  }

  // Power down drivers if idle and queue is empty
  if (driversEnabled && queueStart == queueEnd && !queueFull && !isFeeding && !hasCurrentFeed) {
    digitalWrite(enableDrivers, LOW);
    driversEnabled = false;
  }
}

// Convert dog name to corresponding angle (3x for gear ratio)
int getDogAngle(const char* dogName) {
  if (strcmp(dogName, "dog1data") == 0) return 0 * 3;
  if (strcmp(dogName, "dog2data") == 0) return 90 * 3;
  if (strcmp(dogName, "dog3data") == 0) return 180 * 3;
  if (strcmp(dogName, "dog4data") == 0) return 270 * 3;
  if (strcmp(dogName, "dog5data") == 0) return 0 * 3;
  if (strcmp(dogName, "dog6data") == 0) return 90 * 3;
  if (strcmp(dogName, "dog7data") == 0) return 180 * 3;
  if (strcmp(dogName, "dog8data") == 0) return 270 * 3;
  return -1;
}

// Handle incoming MQTT messages
void messageHandler(String &topic, String &payload) {
  Serial.println(payload);
  if (payload == "prime") {
    stepper1.runSpeed(100);
  } else if (payload == "endPrime") {
    stepper1.stop();
  } else {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, payload);
  if (error) return;

  int amount = doc["amount"];
  if (amount <= 0) return;

  if (doc["dogName"].is<const char*>()) {
    const char* dogName = doc["dogName"];
    int angle = getDogAngle(dogName);
    if (angle >= 0) enqueue(angle, amount);
  } else if (doc["dogName"].is<JsonArray>()) {
    JsonArray names = doc["dogName"].as<JsonArray>();
    for (const char* name : names) {
      int angle = getDogAngle(name);
      if (angle >= 0) enqueue(angle, amount);
    }
  }
}
}

// Move the stepper motor to the zero (home) position
void zeroStepper() {
  stepper.setSpeed(100);
  while (digitalRead(zeroPositionPin) == LOW) {
    stepper2.runSpeed();
  }
  stepper.setCurrentPosition(0);
}
