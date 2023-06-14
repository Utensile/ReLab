#include "Adafruit_GFX.h"
#include "WROVER_KIT_LCD.h"
#include <WiFi.h>
#include "images.h"

#define IMG_NUM 8
int pos=0;
WROVER_KIT_LCD tft;
const char* ssid     = "RE:Lab";
const char* password = "Interact2019!";

const int len[IMG_NUM]      = {zanas_len, mario_len, beacon_len, black_hole_len, earth_len, orbit_len, rocket_len, rover_len};
const uint8_t* img[IMG_NUM] = {zanas, mario, beacon, black_hole, earth, orbit, rocket, rover};

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);
  tft.drawJpg(img[0], len[0]);
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

  if (client) {                             // if you get a client,          // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then                  // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character a response:
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("<style>body{font-family: Arial, sans-serif; text-align: center; padding: 40px 20px;}");
            client.print("a{-webkit-appearance: button; -moz-appearance: button; appearance: button; text-decoration: none; color: initial; color: #fff; background-color: #4CAF50; border: none; padding: 15px 75px; font-size: 30px; border-radius: 5px; cursor: pointer; transition: background-color 0.3s ease;}.button-container{display: flex; justify-content: center; gap: 10px; margin-top: 10vh; margin-bottom: 40px;}a:hover{background-color: #45a049;}</style>");
            client.print("<body><div class=\"button-container\"><a href=\"/L\" type=\"button\">Previous Image</a><a href=\"/H\" type=\"button\">Next Image</a></div><div class=\"upload-container\"><input type=\"file\" id=\"file-upload\"><label for=\"file-upload\" class=\"upload-button\">Upload Image</label></div></body>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        if (currentLine.endsWith("GET /H")) {
          pos++;
          pos=pos%IMG_NUM;
          tft.setRotation(1);
          tft.drawJpg(img[pos], len[pos]);
        }
        if (currentLine.endsWith("GET /L")) {
          pos--; 
          if(pos<0)
            pos=IMG_NUM-1;
          tft.setRotation(1);
          tft.drawJpg(img[pos], len[pos]);
          client.print("<img src=\"\">")
        }
        
      }
    }
    // close the connection:
    client.stop();
  }
}
