#include "Adafruit_GFX.h"
#include "WROVER_KIT_LCD.h"
#include <WiFi.h>
#include <SPIFFS.h>

int IMG_NUM = 8;
int pos = 0;
WROVER_KIT_LCD tft;
const char* ssid = "RE:Lab";
const char* password = "Interact2019!";

const char* links[IMG_NUM] = {"https://i.ibb.co/7rX5Gvj/zanas.jpg", "https://i.ibb.co/SrrjXQW/mario.png", "https://i.ibb.co/bryjPHV/beacon.jpg", "https://i.ibb.co/fYKMXN9/black-hole.jpg", "https://i.ibb.co/znvswRm/earth.jpg", "https://i.ibb.co/Z1xZmPW/orbit.jpg", "https://i.ibb.co/rfX1QLZ/rocket.jpg", "https://i.ibb.co/N6K4ct3/rover.jpg"};
const char* imagePath[IMG_NUM] ={"/zanas.jpg", "/mario.jpg", "/beacon.jpg", "/black_hole.jpg", "/earth.jpg", "/orbit.jpg", "/rocket.jpg", "/rover.jpg"};

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS init failed");
    return;
  } 
  Serial.println("SPIFFS init finished");
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  
  while (file) {
    Serial.println(file.name());
    file = root.openNextFile();
  }
  tft.begin();
  tft.setRotation(1);
  tft.drawJpgFile(SPIFFS, imagePath[0], 0, 0);
  Serial.begin(115200);

  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client
        char c = client.read();             // read a byte

        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("<style>body{font-family: Arial, sans-serif; text-align: center; padding: 40px 20px;}a{-webkit-appearance: button; -moz-appearance: button; appearance: button; text-decoration: none; color: initial; color: #fff; background-color: #4CAF50; border: none; padding: 15px 75px; font-size: 30px; border-radius: 5px; cursor: pointer; transition: background-color 0.3s ease;}.button-container{display: flex; justify-content: center; gap: 10px; margin-top: 10vh; margin-bottom: 40px;}a:hover{background-color: #45a049;}.upload-container {display: flex; justify-content: center; align-items: center; margin-bottom: 20px;} .upload-container input[type=\"file\"] {display: none;} .upload-button {color: #fff; background-color: #d9e718; border: none; padding: 10px 20px; font-size: 25px; border-radius: 4px; cursor: pointer; transition: background-color 0.3s ease;} .upload-button:hover {background-color: #71790d;}</style>");
            client.print("<body><div class=\"button-container\"><a href=\"/L\" type=\"button\">Previous Image</a><a href=\"/H\" type=\"button\">Next Image</a></div><div class=\"upload-container\"><input type=\"file\" id=\"file-upload\"><label for=\"file-upload\" class=\"upload-button\">Upload Image</label></div><br><br><img src=\"");
            client.print(links[pos]);
            client.print("\"></body>");

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /H")) {
          pos++;
          pos = pos % IMG_NUM;
          tft.setRotation(1);
          tft.drawJpgFile(SPIFFS, imagePath[pos], 0, 0);
        }
        if (currentLine.endsWith("GET /L")) {
          pos--;
          if (pos < 0)
            pos = IMG_NUM - 1;
          tft.setRotation(1);
          tft.drawJpgFile(SPIFFS, imagePath[pos], 0, 0);
        }

      }
    }
    client.stop();
  }
}