#include "Adafruit_GFX.h"
#include "WROVER_KIT_LCD.h"
#include <WiFi.h>
#include "images.h"
#include <SPIFFS.h>

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
    bool imageUpload = false;                // flag to check if an image is being uploaded
    File imageFile;                          // file object to hold the uploaded image

    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client
        char c = client.read();             // read a byte

        if (c == '\n') {                    // if the byte is a newline character
          if (currentLine.length() == 0) {
            // Handle HTTP request
            // ...

            if (imageUpload) {
              saveImage(imageFile);
              displayImage("/uploaded_image.jpg");
              imageUpload = false;
            }

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else {
            // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character
          currentLine += c;      // add it to the end of the currentLine

          // Check if an image is being uploaded
          if (currentLine.startsWith("Content-Type: image/jpeg")) {
            imageUpload = true;
            imageFile = SPIFFS.open("/uploaded_image.jpg", FILE_WRITE);
            if (!imageFile) {
              Serial.println("Failed to create image file");
              client.println("HTTP/1.1 500 Internal Server Error");
              client.println();
              client.stop();
              return;
            }
          }
        }
      }

      // Write the received bytes to the image file if an image is being uploaded
      if (imageUpload && client.available()) {
        imageFile.write(client.read());
      }
    }

    // close the connection
    client.stop();
  }
}

void saveImage(File file) {
  if (!file) {
    Serial.println("Failed to open file for saving");
    return;
  }

  String filename = "/uploaded_image.jpg";  // Specify the filename to save the image

  File outputFile = SPIFFS.open(filename, FILE_WRITE);
  if (!outputFile) {
    Serial.println("Failed to create output file");
    file.close();
    return;
  }

  while (file.available()) {
    outputFile.write(file.read());
  }

  outputFile.close();
  file.close();

  Serial.print("Image saved as: ");
  Serial.println(filename);
}

void displayImage(const char* filename) {
  File imageFile = SPIFFS.open(filename, FILE_READ);
  if (!imageFile) {
    Serial.println("Failed to open image file");
    return;
  }

  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);  // Clear the screen before displaying the image
  tft.drawJpgFile(imageFile, 0, 0, tft.width(), tft.height());

  imageFile.close();
}
