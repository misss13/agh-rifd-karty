#include <SPI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include "UART.h"

#define LED_BUILTIN 2

//eprom zajmiemy stanami więc wsm zmiana hasła troche mi nie siada pozatym dane trzeba gdzieś trzymać 
const char *ssid = "yourAP";
const char *password = "yourPassword";

IPAddress PageIP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
WiFiServer server(80);

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);

  UART::init();
  UART::println("AP constructing");

  WiFi.softAP(ssid, password);
  //IPAddress myIP = WiFi.softAPIP();
  server.begin();

  UART::println("Server UP");
}

void loop() {
  UART::tick();
  WiFiClient client = server.available();

  if (client) {                       
    UART::println("New client connected");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.print("<button onclick=\"window.location.href='/prog'\">Tryb programowania</button><br/>");
            client.print("<button onclick=\"window.location.href='/once'\">Otworz drzwi raz</button><br/>");
            client.print("<button onclick=\"window.location.href='/multi'\">Otworz drzwi na kilka minut</button><br/>");
            client.print("<button onclick=\"window.location.href='/bis'\">Tryb bistabilny</button><br/>"); //po przyłożeniu karty drzwi się zamukają
            client.print("<button onclick=\"window.location.href='/list'\">Lista kart</button><br/>");
            
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
        if (currentLine.endsWith("GET /prog")) {
          digitalWrite(LED_BUILTIN, HIGH);
          //func
        }
        if (currentLine.endsWith("GET /once")) {
          digitalWrite(LED_BUILTIN, LOW);
          //func
        }
        if (currentLine.endsWith("GET /multi")) {
          digitalWrite(LED_BUILTIN, HIGH); 
          //func
        }
        if (currentLine.endsWith("GET /bis")) {
          digitalWrite(LED_BUILTIN, LOW);
          //func
        }
        if (currentLine.endsWith("GET /list")) {
          digitalWrite(LED_BUILTIN, HIGH); 
          //func
        }
      }
    }
    client.stop();
    UART::println("Client Disconnected.");
  }
}