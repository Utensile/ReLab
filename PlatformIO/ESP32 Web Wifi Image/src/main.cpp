#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include "SPIFFS.h"
#include "FS.h"
#include "Adafruit_GFX.h"
#include "WROVER_KIT_LCD.h"

#define MAXimg 20

//dobbiamo trovare un modo per connetterci al wifi a scelta.

//Se non lo troviamo amen
const char* ssid = "RE:Lab";
const char* password = "Interact2019!";

char* imagePath[MAXimg] = {"/zanas.jpg", "/mario.jpg", "/beacon.jpg", "/black_hole.jpg", "/earth.jpg", "/orbit.jpg", "/rocket.jpg", "/rover.jpg"};
char* imgNameChar[MAXimg];

String imgName[MAXimg];
const int w = 320;
const int h = 240;
int imgN = 8;
int pos = 0;
int nUpload = 0;
bool inizio = true;
WROVER_KIT_LCD tft;

AsyncWebServer server(80);


void removeElement(char* arr[], int& size, int index) {
  if (index < 0 || index >= size) {
    // Invalid index, do nothing
    return;
  }
  // Shift elements after the index
  for (int i = index; i < size - 1; i++) {
    arr[i] = arr[i + 1];
  }
  // Reduce the size of the array
  size--;
}

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
  
  tft.fillScreen(WROVER_WHITE);

  // Connessione alla rete WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    tft.setCursor(w/2-70, h/2-15);
    tft.setTextColor(WROVER_BLACK);
    tft.setTextSize(1);
    tft.println("Connettendomi alla rete...");
    // Serial.println("Connessione in corso alla rete WiFi...");
  }

  Serial.println("Connessione WiFi stabilita");
  Serial.print("Indirizzo IP: ");
  Serial.println(WiFi.localIP());

  tft.fillScreen(WROVER_NAVY);
  tft.setCursor(w/2-90, h/2-30);
  tft.setTextSize(2);
  tft.setTextColor(WROVER_WHITE);
  tft.println("digita su Google: ");
  tft.setCursor(w/2-120, h/2);
  tft.setTextSize(3);
  tft.println(WiFi.localIP());


  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while (file) {
    // Stampa il nome di ogni file nella directory radice
    if (file.name() != "") {
      Serial.println(file.name());
      file = root.openNextFile();
    }
    else {
      Serial.println("Problema");
      break;
    }
  }
  root.close();

  

  // Configurazione del server web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, "/index.html", "text/html");
    tft.drawJpgFile(SPIFFS, imagePath[pos], 0, 0);


  });

  server.on("/H", HTTP_GET, [](AsyncWebServerRequest* request) {
    pos = (pos + 1) % imgN;
    request->send(SPIFFS, "/index.html", "text/html");
    tft.drawJpgFile(SPIFFS, imagePath[pos], 0, 0);
  });
  server.on("/L", HTTP_GET, [](AsyncWebServerRequest* request) {

    pos -= 1;
    if (pos < 0)
      pos = imgN - 1;
    request->send(SPIFFS, "/index.html", "text/html");
    tft.drawJpgFile(SPIFFS, imagePath[pos], 0, 0);
  });

  server.on("/image.jpg", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(SPIFFS, imagePath[pos], "image/jpeg");
  });



  server.on("/D", HTTP_GET, [](AsyncWebServerRequest* request) {
    tft.fillScreen(WROVER_RED);
    tft.setCursor(w/2-80, h/2-15);
    tft.setTextColor(WROVER_WHITE);  
    tft.setTextSize(2);
    tft.println("Image Deleted");
    int size = sizeof(imagePath) / sizeof(imagePath[0]);
    int size2 = sizeof(imgName) / sizeof(imgName[0]);
    removeElement(imagePath, size, pos);
    removeElement(imgNameChar, size2, pos);
    delay(500);
    tft.drawJpgFile(SPIFFS, imagePath[pos], 0,0);

    //imgN -= 1;
    //pos += 1;
    //request->send(SPIFFS, "/index.html", "text/html");
  });

  // Gestione della richiesta di caricamento immagine
  server.on(
    "/upload",
    HTTP_POST,
    [](AsyncWebServerRequest* request) {
      AsyncWebServerResponse* response = request->beginResponse(200, "text/plain", "Caricamento immagine completato");
      response->addHeader("Access-Control-Allow-Origin", "*");
      request->send(response);
    },
    [](AsyncWebServerRequest* request, String filename, size_t index, uint8_t* data, size_t len, bool final) {

      if (!index) {
        imgName[nUpload] = "/img" + String(nUpload) + ".jpg";
        imgNameChar[nUpload] = new char[imgName[nUpload].length() + 1];
        strcpy(imgNameChar[nUpload], imgName[nUpload].c_str());
        Serial.println(imgNameChar[nUpload]);
        File file = SPIFFS.open(imgNameChar[nUpload], FILE_WRITE);
        if (!file) {
          Serial.println("Errore nell'apertura del file");
          delete[] imgNameChar[nUpload];
          return;
        }
        file.write(data, len);
        file.close();
      }
      else {
        // Caricamento successivo del file
        File file = SPIFFS.open(imgNameChar[nUpload], FILE_APPEND);
        if (!file) {
          Serial.println("Errore nell'apertura del file");
          delete[] imgNameChar[nUpload];
          return;
        }
        file.write(data, len);
        file.close();
      }
      if (final) {
        Serial.println("Caricamento immagine completato");
        tft.drawJpgFile(SPIFFS, imgNameChar[nUpload], 0, 0);
        imgN += 1;
        pos = imgN - 1; //Metti ultimo num array
        imagePath[pos] = imgNameChar[nUpload];
        nUpload += 1;
      }
    }
  );

  // Inizializzazione del server
  server.begin();
}

void loop() {
  // Codice di gestione delle altre operazioni, se necessario

}
