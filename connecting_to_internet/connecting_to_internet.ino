#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Arduino_JSON.h>
#include <ArduinoJson.h>

// const char* ssid = "OPPO F19 Pro+";
// const char* password = "Wills@22";

const char* ssid = "My PC";
const char* password = "TEST1234";

//Your Domain name with URL path or IP address with path
// String serverName = "http://192.168.0.145:3000";
// String serverName = "http://192.168.137.245:3000";
String serverName = "http://192.168.137.42:8081";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;



int ledPin = 13;  // GPIO13---D7 of NodeMCU
WiFiServer server(80);
int RelayPin = 1;
String myIP;
String updateBoardAddress = serverName + "/updateBoardAddress";

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(RelayPin, OUTPUT);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Connect to WiFi network
  Serial.println("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println("Use this URL to connect: ");
  Serial.println("http://");
  Serial.println(WiFi.localIP());
  myIP = WiFi.localIP().toString();
  Serial.println("/");
}

void loop() {

  digitalWrite(RelayPin, LOW);
  delay(3000);

  // Let's turn off the relay...
  digitalWrite(RelayPin, HIGH);
  delay(3000);

  // Send an HTTP POST request depending on timerDelay
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {

      StaticJsonDocument<300> JSONbuffer;  //Declaring static JSON buffer
      // JsonObject JSONencoder;

      JSONbuffer["switch_board_uuid"] = 1;
      JSONbuffer["switch_board_ip"] = myIP;

      String jsonPayload;
      serializeJson(JSONbuffer, jsonPayload);


      WiFiClient client1;
      HTTPClient http1;  //Declare object of class HTTPClient

      http1.begin(client1, updateBoardAddress.c_str());     //Specify request destination
      http1.addHeader("Content-Type", "application/json");  //Specify content-type header

      int httpCode1 = http1.POST(jsonPayload);  //Send the request
      String payload = http1.getString();       //Get the response payload


      WiFiClient client;
      HTTPClient http;

      String serverPath = serverName + "/getBoardDetails";


      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());

      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.println("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);

        JSONVar myObject = JSON.parse(payload);

        // JSON.typeof(jsonVar) can be used to get the type of the var
        if (JSON.typeof(myObject) == "undefined") {
          Serial.println("Parsing input failed!");
          return;
        }

        Serial.println("JSON object = ");
        Serial.println(myObject);

        JSONVar keys = myObject.keys();


        for (int i = 0; i < keys.length(); i++) {
          JSONVar value = myObject[keys[i]];
          Serial.println(keys[i]);
          Serial.println(" = ");
          Serial.println(value);

          String vale = JSON.stringify(keys[i]);
          Serial.println(" vals = ");
          Serial.println(vale);
          String vv = "\"data\"";
          Serial.println(vv);

          if (vale.equalsIgnoreCase(vv)) {
            JSONVar dataKeys = value.keys();
            Serial.println("yes");

            for (int j = 0; j < dataKeys.length(); j++) {
              JSONVar value2 = value[dataKeys[j]];
              Serial.println(dataKeys[j]);
              Serial.println(" = ");
              Serial.println(value2);
              if (JSON.stringify(dataKeys[j]) == "\"isLightStatus\"") {
                if (value2) {
                  Serial.println(" on = ");
                  digitalWrite(ledPin, HIGH);
                  value = HIGH;
                } else {
                  Serial.println(" off = ");
                  digitalWrite(ledPin, LOW);
                  value = LOW;
                }
              }
            }

          } else {
            Serial.println("no");
          }
          // sensorReadingsArr[i] = double(value);
        }


      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  // Check if a client has connected
  WiFiClient client = server.available();

  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }



  // // Read the first line of the request
  // String request = client.readStringUntil('\r');
  // Serial.println(request);
  // client.flush();

  // // Match the request

  // int value = LOW;
  // if (request.indexOf("/MY_LED=ON") != -1) {
  //   digitalWrite(ledPin, HIGH);
  //   value = HIGH;
  // }
  // if (request.indexOf("/MY_LED=OFF") != -1) {
  //   digitalWrite(ledPin, LOW);
  //   value = LOW;
  // }

  // Set ledPin according to the request
  //digitalWrite(ledPin, value);

  // Return the response
  // client.println("HTTP/1.1 200 OK");
  // client.println("Content-Type", "application/json");  //Specify content-type header
  //  StaticJsonDocument<300> JSONbuffer;  //Declaring static JSON buffer
  //     // JsonObject JSONencoder;

  //     JSONbuffer["switch_board_uuid"] = 1;
  //     JSONbuffer["switch_board_ip"] = myIP;

  //     String jsonPayload;
  //   serializeJson(JSONbuffer, jsonPayload);
  //   client.println(jsonPayload);


  StaticJsonDocument<300> JSONbuffer;  //Declaring static JSON buffer
  // JsonObject JSONencoder;

  JSONbuffer["switch_board_uuid"] = 1;
  JSONbuffer["switch_board_ip"] = myIP;

  String jsonPayload;
  serializeJson(JSONbuffer, jsonPayload);


  client.println("HTTP/1.1 200 OK");
  // client.println("POST /rods/airlo/firstmodule HTTP/1.1 200 OK");

  // client.println("POST /rods/airlo/firstmodule/ HTTP/1.1");


  // client.println("POST /rods/airlo/firstmodule/ HTTP/1.1");
  // client.println("Host: iotsystem.synology.me:314<http://iotsystem.synology.me:314>");
  client.println("Cache-Control: no-cache");
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonPayload.length());
  client.println();
  client.println(jsonPayload);

  // client.println("");  //  do not forget this one
  // client.println("<!DOCTYPE HTML>");
  // client.println("<html>");

  // client.print("Led is now: ");

  // if (value == HIGH) {
  //   client.print("On");
  // } else {
  //   client.print("Off");
  // }
  // client.println("<br><br>");
  // client.println("<a href=\"/LED=ON\"\"><button>On </button></a>");
  // client.println("<a href=\"/LED=OFF\"\"><button>Off </button></a><br />");
  // client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  // Serial.println("");
}
