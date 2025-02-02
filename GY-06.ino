#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <Base64.h>
#include <ESP8266WebServer.h>

// Twilio Credentials
const String twilio_account_sid = "*********";
const String twilio_auth_token = "***********";
const String twilio_phone_number = "**********";
const String target_phone_number = "+91********";

// WiFi Credentials
const char* ssid = "********";
const char* password = "*******";

// Accelerometer Pin (X-axis of ADXL335)
const int xPin = A0; // X-axis connected to A0 (the only analog pin on ESP8266)

// Threshold value (these may need adjustment based on calibration)
const int ACCEL_THRESHOLD = 600; 

// GPS Data
String latitude = "N/A";
String longitude = "N/A";
bool vibrationDetected = false; // Flag to check if vibration was detected

// Web Server
ESP8266WebServer server(80); // Create a web server on port 80

void setup() {
  Serial.begin(9600);
  connectToWiFi();
  
  // Setup web server
  server.on("/", handleRoot);      // Route for the root / page
  server.on("/gps", handleGPSData); // Route for /gps page
  server.begin();                   // Start the server
  Serial.println("HTTP server started");
}

void loop() {
  checkAcceleration();  // Check for vibration or acceleration changes
  delay(1000);          // Check every 1 second
  server.handleClient();  // Handle web server clients
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to WiFi");

  // Print the IP address
  Serial.print("ESP8266 IP Address: ");
  Serial.println(WiFi.localIP());
}


// Function to check acceleration on the X-axis only
void checkAcceleration() {
  int xValue = analogRead(xPin);  // Read X-axis value

  Serial.print("X-axis: ");
  Serial.println(xValue);
  delay(5000);

  // Check if the X-axis value exceeds the threshold (indicating a sudden movement or impact)
  if (abs(xValue) > ACCEL_THRESHOLD) {
    Serial.println("Acceleration threshold exceeded, sending SMS alert...");
    
    // Set vibration detected flag to true
    vibrationDetected = true;
    
    // Call the GPS handling function to process GPS data
    handleGPSData();
    
    // Send SMS alert
    sendSMSAlert();
  }
}

// Function to handle the root web page
void handleRoot() {
  Serial.println("Handling root endpoint request...");

  // Only show GPS data if vibration has been detected
  if (vibrationDetected) {
    // Create HTML page displaying latitude and longitude
    String htmlPage = "<html><body><h1>GPS Logger Data</h1>";
    htmlPage += "<p>Latitude: " + latitude + "</p>";
    htmlPage += "<p>Longitude: " + longitude + "</p>";
    htmlPage += "</body></html>";

    server.send(200, "text/html", htmlPage); // Send HTML response
    Serial.println("Root endpoint response with GPS data sent.");
  } else {
    // Display a message if no vibration has been detected
    String htmlPage = "<html><body><h1>No Vibration Detected</h1>";
    htmlPage += "<p>GPS data will be displayed once excessive vibration is detected.</p>";
    htmlPage += "</body></html>";
    
    server.send(200, "text/html", htmlPage); // Send HTML response
    Serial.println("Root endpoint response without GPS data sent.");
  }
}

// Function to handle GPS data from a POST request
void handleGPSData() {
  Serial.println("Handling GPS data endpoint request...");

  if (!vibrationDetected) {
    Serial.println("Vibration not detected, skipping GPS data handling.");
    return;  // Skip handling GPS data if no vibration was detected
  }

  if (server.method() == HTTP_POST) {
    String body = server.arg("plain"); // Get the raw POST body
    Serial.println("Received POST data: " + body);

    // Extract latitude and longitude from body (e.g., "lat=value&lon=value")
    int latIndex = body.indexOf("lat=") + 4;
    int lonIndex = body.indexOf("lon=") + 4;
    String newLatitude = body.substring(latIndex, body.indexOf("&", latIndex));
    String newLongitude = body.substring(lonIndex);

    // Update GPS data
    latitude = newLatitude;
    longitude = newLongitude;

    // Print updated GPS data to Serial Monitor
    Serial.println("Updated GPS data:");
    Serial.println("Latitude: " + latitude);
    Serial.println("Longitude: " + longitude);
    
    server.send(200, "text/plain", "GPS data received"); // Send response
  } else {
    Serial.println("Invalid request method for GPS data.");
    server.send(400, "text/plain", "Bad Request"); // Handle bad request
  }
}

// Function to send an SMS alert via Twilio
void sendSMSAlert() {
  WiFiClientSecure client;
  client.setInsecure(); // Disable SSL certificate verification
  
  const char* host = "api.twilio.com";
  const int httpsPort = 443;
  String url = "/2010-04-01/Accounts/" + twilio_account_sid + "/Messages.json";
  String payload = "To=" + target_phone_number + "&From=" + twilio_phone_number + "&Body=Acceleration Detected!\nLatitude: " + latitude + "\nLongitude: " + longitude;

  Serial.println("Connecting to Twilio...");
  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection to Twilio failed");
    return;
  }

  String auth = twilio_account_sid + ":" + twilio_auth_token;
  String encodedAuth = base64::encode(auth);
  
  Serial.println("Sending POST request to Twilio...");
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Authorization: Basic " + encodedAuth + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded\r\n" +
               "Content-Length: " + payload.length() + "\r\n\r\n" +
               payload);

  Serial.println("Alert sent to Twilio.");
  client.stop();
}
