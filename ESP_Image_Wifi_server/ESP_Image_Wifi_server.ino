/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA2 encryption. For insecure
 WEP or WPA, change the Wifi.begin() call and use Wifi.setMinSecurity() accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

#include <WiFi.h>

const char* ssid     = "RE:Lab";
const char* password = "Interact2019!";

WiFiServer server(80);

void setup()
{
    Serial.begin(115200);
    pinMode(5, OUTPUT);      // set the LED pin mode

    delay(10);

    // We start by connecting to a WiFi network

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

void loop(){
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
          Serial.println("Going to next image...");          // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          Serial.println("Going to previous image...");              // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
  }
}
