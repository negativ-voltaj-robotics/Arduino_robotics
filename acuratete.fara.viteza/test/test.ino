#include <QTRSensors.h>

QTRSensors qtr;

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

void setup()
{
  Serial.begin(115200);

  qtr.setTypeRC();
  qtr.setSensorPins(
    (const uint8_t[]){2, 3, 4, 5, 6, 7, 8, 9},
    SensorCount);

  Serial.println("Calibrare...");

  // Calibrare
  for (uint16_t i = 0; i < 300; i++)
  {
    qtr.calibrate();
    delay(5);
  }

  Serial.println("Gata!");
}

void loop()
{
  uint16_t position = qtr.readLineBlack(sensorValues);

  // Afișează valorile senzorilor
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }

  Serial.print("Pos=");
  Serial.println(position);

  delay(100);
}