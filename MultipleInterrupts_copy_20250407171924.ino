#include <WiFiNINA.h>
#include <WiFiSSLClient.h>
#include <ArduinoHttpClient.h>

// WiFi credentials
char wifiSSID[] = "AK";
char wifiPassword[] = "vasu@8556";

// Firebase Realtime Database configuration
char firebaseHost[] = "ledcontrol-8e81e-default-rtdb.firebaseio.com";
int firebasePort = 443;

// Database paths for LED controls
String pathBlueLED = "/leds/blue.json";
String pathGreenLED = "/leds/green.json";
String pathRedLED = "/leds/red.json";

// Assign pins for LEDs
const int pinBlue = 2;
const int pinGreen = 3;
const int pinRed = 4;

// Create WiFi and HTTP client instances
WiFiSSLClient sslClient;
HttpClient httpClient = HttpClient(sslClient, firebaseHost, firebasePort);

void setup() {
  Serial.begin(9600);

  // Configure LED pins as outputs
  pinMode(pinBlue, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinRed, OUTPUT);

  // Attempt WiFi connection
  WiFi.begin(wifiSSID, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Trying to connect to WiFi...");
    delay(1000);
  }

  Serial.println("WiFi Connected Successfully!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Continuously update LED states from Firebase
  updateLEDState(pathBlueLED, pinBlue);
  updateLEDState(pathGreenLED, pinGreen);
  updateLEDState(pathRedLED, pinRed);
  
  delay(1000); // Check every second
}

void updateLEDState(String dbPath, int ledPin) {
  httpClient.get(dbPath);
  int responseCode = httpClient.responseStatusCode();
  String payload = httpClient.responseBody();
  payload.trim(); // Remove any extra spaces/newlines

  Serial.print("Requesting: ");
  Serial.println(dbPath);
  Serial.print("HTTP Status: ");
  Serial.println(responseCode);
  Serial.print("Payload: ");
  Serial.println(payload);

  if (responseCode == 200) {
    payload.replace("\"", ""); // Remove quotes if present

    if (payload == "true") {
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }
  } else {
    Serial.println("Error fetching data from Firebase");
  }
}
