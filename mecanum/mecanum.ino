#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);

//================ WIFI =================

const char* ssid = "MECANUM_ROBOT";
const char* pass = "12345678";

//============== PINI MOTOARE ==============

// Stânga Față
#define SF1 D4
#define SF2 D3

// Stânga Spate
#define SS1 D2
#define SS2 D1

// Dreapta Față
#define DF1 D5
#define DF2 D6

// Dreapta Spate
#define DS1 D7
#define DS2 D8

// PWM comun
#define PWM_PIN D0

//==========================================

int limitSpeed(int v)
{
  if (v > 255) v = 255;
  if (v < -255) v = -255;
  return v;
}

// Controlează doar direcția
void motor(int a, int b, int viteza)
{
  viteza = limitSpeed(viteza);

  if (viteza > 0)
  {
    digitalWrite(a, HIGH);
    digitalWrite(b, LOW);
  }
  else if (viteza < 0)
  {
    digitalWrite(a, LOW);
    digitalWrite(b, HIGH);
  }
  else
  {
    digitalWrite(a, LOW);
    digitalWrite(b, LOW);
  }
}

void drive(int x, int y, int r)
{
  // Corecție rotație
  r = -r;

  // Formula mecanum

  int sf = y + x + r;
  int df = y - x - r;

  int ss = y - x + r;
  int ds = y + x - r;

  // Normalizare

  int maxim = max(abs(sf),
              max(abs(df),
              max(abs(ss), abs(ds))));

  if (maxim > 255)
  {
    sf = sf * 255 / maxim;
    df = df * 255 / maxim;
    ss = ss * 255 / maxim;
    ds = ds * 255 / maxim;
  }

  // PWM comun

  int pwm = max(abs(sf),
            max(abs(df),
            max(abs(ss), abs(ds))));

  analogWrite(PWM_PIN, pwm);

  // Direcția motoarelor

  motor(SF1, SF2, sf);
  motor(DF1, DF2, df);
  motor(SS1, SS2, ss);
  motor(DS1, DS2, ds);
}

void handleCMD()
{
  int x = server.arg("x").toInt();
  int y = server.arg("y").toInt();
  int r = server.arg("r").toInt();

  drive(x, y, r);

  server.send(200, "text/plain", "OK");
}

void setup()
{
  Serial.begin(115200);

  pinMode(SF1, OUTPUT);
  pinMode(SF2, OUTPUT);

  pinMode(SS1, OUTPUT);
  pinMode(SS2, OUTPUT);

  pinMode(DF1, OUTPUT);
  pinMode(DF2, OUTPUT);

  pinMode(DS1, OUTPUT);
  pinMode(DS2, OUTPUT);

  pinMode(PWM_PIN, OUTPUT);

  analogWriteRange(255);
  analogWriteFreq(1000);
  analogWrite(PWM_PIN, 0);

  WiFi.softAP(ssid, pass);

  Serial.println();
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/cmd", handleCMD);

  server.begin();

  Serial.println("Server pornit.");
}

void loop()
{
  server.handleClient();
}