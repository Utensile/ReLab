#include "Adafruit_GFX.h"
#include "WROVER_KIT_LCD.h"
#include <WiFi.h>
#include "image1.h"
#include "mario.h"
#include "zanas.h"

#define IMG_NUM 3
int pos=2;
WROVER_KIT_LCD tft;
const char* ssid     = "RE:Lab";
const char* password = "Interact2019!";

const int len[IMG_NUM];
const uint8_t img[IMG_NUM][100000];
len[0]=image1_len; img[0]=image_1;
len[1]=mario_len; img[1]=mario;
len[2]=zanas_len; img[2]=zanas;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);
  tft.drawJpg(img[2], len[2]);
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
            client.print("<body><div class=\"button-container\"><a href=\"/L\" type=\"button\">Previous Image</a><a href=\"/H\" type=\"button\">Next Image</a></div>");

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

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          pos++;
          pos=pos%IMG_NUM;
        }
        if (currentLine.endsWith("GET /L")) {
          pos--; 
          if(pos<0)
            pos=0;             // GET /L turns the LED off
        }
        ttft.setRotation(1);
        tft.drawJpg(img[pos], len[pos]);
      }
    }
    // close the connection:
    client.stop();
  }
}
