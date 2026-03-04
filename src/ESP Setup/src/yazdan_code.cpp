
// #include <OneWire.h>
#include <Arduino.h>
#include <OneWire.h>
#include <EEPROM.h>

#include "do_grav.h"
#include "yazdan_code.h"

// DS18B20 Setup
#define DS18B20_PIN 27
// const int DS18B20_PIN =2; // DS18B20 Signalpin auf digitalem Pin 2
OneWire ds(DS18B20_PIN); // Initialisierung des DS18B20

// TDS-Sensor Setup
#define TDS_SENSOR_PIN 14
GravityTDS gravityTds;

// pH-Sensor Setup
#define PH_PIN 13
DFRobot_PH ph;

// Trübheitssensor Setup
#define TURBIDITY_PIN 12

// Sauerstoffsensor Setup (DO-Sensor)
#define DO_SENSOR_PIN 2
Gravity_DO DO = Gravity_DO(DO_SENSOR_PIN); // Der Sauerstoffsensor ist an Pin 15 angeschlossen

#define ADC_RESOLUTION 4096

// float tdsValue = 0;  // hier feste Werte von TDS-Messwerte definiert (Variable für den TDS-Wert)
// float phValue = 0; // berechneter ph-Wert

// Funktion zum Parsen von Kalibrierungsbefehlen für den DO-Sensor
void ask_manual_calibration()
{
#define USER_INPUT_BUFFER_LEN 32
  char user_input[USER_INPUT_BUFFER_LEN] = {0};

  size_t bytes_read = Serial.readBytesUntil('\r', user_input, sizeof(user_input));
  if (bytes_read <= 0)
    return;

  strupr(user_input);
  String cmd = String(user_input);
  if (cmd.startsWith("CAL"))
  {
    int index = cmd.indexOf(",");
    if (index != -1)
    {
      String param = cmd.substring(index + 1, cmd.length());
      if (param.equals("CLEAR"))
      {
        DO.cal_clear();
        Serial.println("DO Sensorkalibrierung gelöscht");
      }
    }
    else
    {
      DO.cal();
      Serial.println("DO Sensor kalibriert");
    }
  }
}

void sensors_init()
{
  Serial.println("Initializing sensors...");
  // TDS-Sensor Konfiguration bzw initialisieren
  gravityTds.setPin(TDS_SENSOR_PIN);
  gravityTds.setAref(3.3);                // reference voltage on ADC, default 3.3V on ESP32
  gravityTds.setAdcRange(ADC_RESOLUTION); // 1024 for 10bit ADC;4096 for 12bit ADC
  gravityTds.begin();                     // initialization

  // pH-Sensor Konfiguration bzw initialisierung
  ph.begin();

  // Kalibrierungshinweis für den DO-Sensor ausgeben
  Serial.println(F("Gib 'CAL' ein, um den DO-Sensor (100% Sättigung in Luft) zu kalibrieren."));
  Serial.println(F("Gib 'CAL,CLEAR' ein, um die DO-Kalibrierung zu löschen."));

  // Sauerstoffsensor initialisieren
  if (DO.begin())
  {
    Serial.println("EEPROM für den DO-Sensor geladen");
  }
  Serial.println("Sensors initialized");
}

void sensors_loop()
{
  Serial.println("Reading sensors...");
  // Prüfe, ob serielle Daten für DO-Kalibrierung vorliegen
  if (Serial.available() > 0)
  {
    ask_manual_calibration();
  }

  float temperature = readTemperature(); // Temperatur vom DS18B20 abrufen

  if (temperature != -1000)
  {                                            // Prüfen, ob die Temperatur gültig ist
    gravityTds.setTemperature(temperature);    // set the temperature and execute temperature compensation
    gravityTds.update();                       // sample and calculate
    float tdsValue = gravityTds.getTdsValue(); // then get the value (TDS-Wert in ppm)

    // ADC Depth 4096 lieber in #define ADC_RESOLUTION variable
    // pH-Sensor auslesen. Hier wird der analoge Wert eingelesen, in eine Spannung (in mV) umgerechnet
    float voltage = (analogRead(PH_PIN) * 3300) / ADC_RESOLUTION;

    // Den gemessenen Spannungswert und die Temperatur an die pH-Berechnung übergeben
    float phValue = ph.readPH(voltage, temperature);

    // Trübheitssensor auslesen
    float turbidity = readTurbidity(temperature);

    // Sauerstoffsensor (DO-Sensor) auslesen
    float doValue = DO.read_do_percentage();

    // pH-Kalibrierung: Diese Funktion verarbeitet serielle Kommandos zur Kalibrierung
    ph.calibration(voltage, temperature);

    // Ergebnisse ausgeben
    Serial.print("Temperature:");
    Serial.print(temperature, 2); // Temperatur mit 2 Dezimalstellen ausgeben
    Serial.println("°C");

    Serial.print("TDS Value:");
    Serial.print(tdsValue, 0); // TDS-Wert ohne Dezimalstellen ausgeben
    Serial.println("ppm");

    Serial.print("ph Value:");
    Serial.println(phValue, 2); // pH-Wert mit 2 Dezimalstellen ausgeben

    Serial.print("Trübung:");
    Serial.print(turbidity);
    Serial.println("NTU");

    Serial.print("DO (Oxygen):");
    Serial.print(doValue);
    Serial.println("%");

    Serial.println("---------------------");
  }
  else
  {
    Serial.println("Error reading temperature!");
  }

  delay(1000); // 1 Sekunde warten, bevor die nächste Messung erfolgt
}

// Funktion und Kallibrierung des Trübheitsensor
float readTurbidity(float temperature)
{
  int turbRaw = analogRead(TURBIDITY_PIN);
  float voltage = turbRaw / 4096.0 * 3.3; // Umrechnung in Volt

  // Temperaturkompensation
  float slope = -0.0075;                                   // Spannungsabfall pro Grad Celsius. (4.1 V - 4.4 V) / 40 Grad Celsius
  float baselinePure = 4.4 + slope * (temperature - 10.0); // Berechnung der Basisspannung bzw der erwarteten Spannung

  // Berechne die Differenz zwischen der erwarteten Basisspannung und der gemessenen Spannung
  float voltageDifference = baselinePure - voltage;

  // Umrechnung in NTU: Wichtig hier!! Annahme ist das 0.5 V Unterschied, entspricht bei 1000 NTU ein Faktor 2000 NTU/V.
  // Generelle Annahme die getroffen wurde, ist das wir einen linearen Spannungsabfall bei steigender Temperatur haben, was aus dem Datenblatt des Sensors zu entnehmen ist.
  // Daher wurde diese Annahmen getroffen für die Berechnung in NTU. Die Formel ist nicht exakt aber im Intervall des Sensors bei 4.1 +- 0.3V für Temperatur 10 bis 50 Grad Celsius.
  float ntu = voltageDifference * 2000.0;

  // Falls Messrauschen negative NTU ergibt, setze NTU = 0
  if (ntu < 0)
  {
    ntu = 0;
  }

  return ntu;
}

// Funktion zum Auslesen des DS18B20-Temperatursensors
float readTemperature()
{
  // Temperatur vom DS18B20 abrufen
  byte data[12];
  byte addr[8];

  // Adresse des Sensors suchen
  if (!ds.search(addr))
  {
    ds.reset_search(); // Suche zurücksetzen, wenn kein weiterer Sensor gefunden
    return -1000;      // Fehlerwert zurückgeben
  }

  // Prüfen, ob die CRC gültig ist
  if (OneWire::crc8(addr, 7) != addr[7])
  {
    Serial.println("CRC check failed!");
    return -1000;
  }

  // Prüfen, ob es sich um einen DS18B20 handelt
  if (addr[0] != 0x10 && addr[0] != 0x28)
  {
    Serial.println("Device not recognized!");
    return -1000;
  }

  // Messung starten
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // Starte die Temperaturkonvertierung mit parasitärer Stromversorgung

  delay(750); // Wartezeit für die Temperaturmessung

  // Daten aus dem Scratchpad lesen
  ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Scratchpad lesen

  for (int i = 0; i < 9; i++)
  {
    data[i] = ds.read();
  }

  ds.reset_search(); // Suche zurücksetzen, um weitere Sensoren zu finden

  // Temperatur aus dem Scratchpad-Daten berechnen
  int16_t rawTemperature = (data[1] << 8) | data[0]; // Zusammenfügen von MSB und LSB
  float TemperatureSum = rawTemperature / 16.0;      // Umrechnung auf Grad Celsius

  return TemperatureSum;
}
