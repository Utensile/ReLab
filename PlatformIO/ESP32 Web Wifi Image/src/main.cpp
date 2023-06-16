#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"
#include "FS.h"
#include "Adafruit_GFX.h"
#include "WROVER_KIT_LCD.h"

#define MAXimg 20

const char* ssid = "RE:Lab";
const char* password = "Interact2019!";

const char* imagePath[MAXimg] ={"/zanas.jpg", "/mario.jpg", "/beacon.jpg", "/black_hole.jpg", "/earth.jpg", "/orbit.jpg", "/rocket.jpg", "/rover.jpg"};
const char* imgNameChar[MAXimg];

String imgName[MAXimg];

int imgN=8;
int pos = 0;
int nUpload = 0;
WROVER_KIT_LCD tft;

AsyncWebServer server(80);




void setup() {
  Serial.begin(115200);
  

  // Montaggio della memoria SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Errore nel montaggio di SPIFFS");
    return;
  }
  //inizializzazione canvas
  tft.begin();
  tft.setRotation(1);
  tft.drawJpgFile(SPIFFS, imagePath[pos], 0, 0);
  // Connessione alla rete WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
   // Serial.println("Connessione in corso alla rete WiFi...");
  }

  Serial.println("Connessione WiFi stabilita");
  Serial.print("Indirizzo IP: ");
  Serial.println(WiFi.localIP());
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while (file) {
    // Stampa il nome di ogni file nella directory radice
    if(file.name() != ""){
      Serial.println(file.name());
      file = root.openNextFile();
    }
    else{
      Serial.println("Probleam");
    }
  }
  root.close();
  
  // Configurazione del server web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/H", HTTP_GET, [](AsyncWebServerRequest *request){
    pos=(pos+1)%imgN;
    request->send(SPIFFS, "/index.html", "text/html");
    tft.drawJpgFile(SPIFFS, imagePath[pos], 0, 0);
  });
  server.on("/L", HTTP_GET, [](AsyncWebServerRequest *request){
    
    pos-=1;
    if(pos<0)
      pos=imgN-1;
    request->send(SPIFFS, "/index.html", "text/html");
    tft.drawJpgFile(SPIFFS, imagePath[pos], 0, 0);
  });

  server.on("/image.jpg", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, imagePath[pos], "image/jpeg");
  });


  

  // Gestione della richiesta di caricamento immagine
  server.on(
    "/upload",
    HTTP_POST,
    [](AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "Caricamento immagine completato");
      response->addHeader("Access-Control-Allow-Origin", "*");
      request->send(response);
    },
    [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {

      if (!index) {
        imgName[nUpload] = "/img" + String(nUpload) + ".jpg";
        imgNameChar[nUpload] = imgName[nUpload].c_str();
        Serial.println(imgNameChar[nUpload]);
        File file = SPIFFS.open(imgNameChar[nUpload], FILE_WRITE);
        if (!file) {
          Serial.println("Errore nell'apertura del file");
          return;
        }
        file.write(data, len);
        file.close();
       } else {
        // Caricamento successivo del file
        File file = SPIFFS.open(imgNameChar[nUpload], FILE_APPEND);
        if (!file) {
          Serial.println("Errore nell'apertura del file");
          return;
        }
        file.write(data, len);
        file.close();
      }
      if (final) {
        Serial.println("Caricamento immagine completato");
        tft.drawJpgFile(SPIFFS, imgNameChar[nUpload], 0, 0);
        imgN+=1;
        pos=imgN-1; //Metti ultimo num array
        imagePath[pos]=imgNameChar[nUpload];
        nUpload+=1;
      }
    }
  );

  // Inizializzazione del server
  server.begin();
}

void loop() {
  // Codice di gestione delle altre operazioni, se necessario

}
