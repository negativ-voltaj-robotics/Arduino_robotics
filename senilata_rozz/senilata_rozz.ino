#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "ROBOT ROZZ";
const char* password = "12345678";

ESP8266WebServer server(80);

// Motoare
#define IN1 D1
#define IN2 D2
#define IN3 D5
#define IN4 D6

// Extra
#define L1PIN D7
#define R1PIN D8

unsigned long lastCmd = 0;

void stopAll() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  digitalWrite(L1PIN, LOW);
  digitalWrite(R1PIN, LOW);
}

void handleCmd() {

  String cmd = server.arg("c");

  lastCmd = millis();

  stopAll();

  // Fata si spatele sunt inversate

  if (cmd == "f") {
    digitalWrite(IN2, HIGH);
    digitalWrite(IN4, HIGH);
  }

  if (cmd == "b") {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN3, HIGH);
  }

  if (cmd == "l") {
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
  }

  if (cmd == "r") {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN4, HIGH);
  }

  if (cmd == "l1")
    digitalWrite(L1PIN, HIGH);

  if (cmd == "r1")
    digitalWrite(R1PIN, HIGH);

  server.send(200, "text/plain", "OK");
}

void setup() {

  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(L1PIN, OUTPUT);
  pinMode(R1PIN, OUTPUT);

  stopAll();

  WiFi.softAP(ssid, password);

  server.on("/cmd", handleCmd);

  server.begin();

  Serial.println(WiFi.softAPIP());
}

void loop() {

  server.handleClient();

  // Opreste motoarele daca nu mai primeste comenzi
  if (millis() - lastCmd > 1000000) {
    stopAll();
  }
}