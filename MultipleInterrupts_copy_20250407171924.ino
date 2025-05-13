#include <WiFiNINA.h>
#include <WiFiSSLClient.h>
#include <ArduinoHttpClient.h>

// WiFi =
char wifiSSID[] = "AK";
char wifiPassword[] = "vasu@8556";


char firebaseHost[] = "ledcontrol-8e81e-default-rtdb.firebaseio.com";
int firebasePort = 443;


String pathBlueLED = "/leds/blue.json";
String pathGreenLED = "/leds/green.json";
String pathRedLED = "/leds/red.json";

// LED pins
const int pinBlue = 2;
const int pinGreen = 3;
const int pinRed = 4;

// Track previous LED states
String prevBlueState = "";
String prevGreenState = "";
String prevRedState = "";

// Create clients
WiFiSSLClient sslClient;
HttpClient httpClient = HttpClient(sslClient, firebaseHost, firebasePort);

 //Define callback function type
typedef void (*LEDCallback)(int ledPin, const String &state);


void setup() {
  Serial.begin(9600);
  pinMode(pinBlue, OUTPUT);
  pinMode(pinGreen, OUTPUT);
  pinMode(pinRed, OUTPUT);

  // Connect to WiFi
  WiFi.begin(wifiSSID, wifiPassword);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(1000);
  }

  Serial.println("Connected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}


void loop() {
  // Check and call callback only if state changes
  checkLEDStateChange(pathBlueLED, pinBlue, prevBlueState, handleLEDChange);
  checkLEDStateChange(pathGreenLED, pinGreen, prevGreenState, handleLEDChange);
  checkLEDStateChange(pathRedLED, pinRed, prevRedState, handleLEDChange);

  delay(1000);  // Poll every second
}

 //callback trigger function 
void checkLEDStateChange(String dbPath, int ledPin, String &prevState, LEDCallback callback) {
  httpClient.get(dbPath);
  int statusCode = httpClient.responseStatusCode();
  String response = httpClient.responseBody();

  response.trim();
  response.replace("\"", ""); // remove quotes

  Serial.print("Checking path: ");
  Serial.println(dbPath);
  Serial.print("Status: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);

  if (statusCode == 200 && response != prevState) {
    prevState = response;
    callback(ledPin, response); // invoke callback function
  }
}

// === The callback function ===
void handleLEDChange(int ledPin, const String &state) {
  if (state == "true") {
    digitalWrite(ledPin, HIGH);
    Serial.print("LED ON at pin ");
  } else {
    digitalWrite(ledPin, LOW);
    Serial.print("LED OFF at pin ");
  }
  Serial.println(ledPin);
}

  
  
