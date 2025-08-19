#include "sensors_module.h"

void sensors_module::begin(adc_module* adc) {
  this->adc = adc; // Set the ADC module for the sensors
  ds = OneWire(DS18B20_PIN);

  Serial.println("Initializing sensors...");

  gravityTds.begin(TDS_ADC_PIN, adc, TDS_EEPROM_ADDRESS); // Initialize TDS sensor
  ph.begin(); 

  // Sauerstoffsensor initialisieren
  if (DO.begin(DO_ADC_PIN, adc, DO_EEPROM_ADDRESS))
    Serial.println("EEPROM für den DO-Sensor geladen");

  // DO.cal();


  // DO.cal_clear();
  
  Serial.println("Sensors initialized");
}

// float tdsValue = 0;  // hier feste Werte von TDS-Messwerte definiert (Variable für den TDS-Wert)
// float phValue = 0; // berechneter ph-Wert

void sensors_module::print()
{
  // Serial.println("Reading sensors...");
  // Prüfe, ob serielle Daten für DO-Kalibrierung vorliegen
  if (Serial.available() > 0)
    DO.ask_manual_calibration();

  temperature = readTemperature(); // Temperatur vom DS18B20 abrufen

  if (temperature != -1000)
  {                                            // Prüfen, ob die Temperatur gültig ist
    gravityTds.setTemperature(temperature);    // set the temperature and execute temperature compensation
    gravityTds.update();                       // sample and calculate
    tdsValue = gravityTds.getTdsValue(); // then get the value (TDS-Wert in ppm)

    // ADC Depth 4096 lieber in #define ADC_RESOLUTION variable
    // pH-Sensor auslesen. Hier wird der analoge Wert eingelesen, in eine Spannung (in mV) umgerechnet
    float ph_voltage = adc->readVoltage(PH_ADC_PIN) * 1000; // Umrechnung in mV

    // Den gemessenen Spannungswert und die Temperatur an die pH-Berechnung übergeben
    phValue = ph.readPH(ph_voltage, temperature);

    // Trübheitssensor auslesen
    turbidity = readTurbidity(temperature);

    // Sauerstoffsensor (DO-Sensor) auslesen
    doValue = DO.read_do_percentage();

    // pH-Kalibrierung: Diese Funktion verarbeitet serielle Kommandos zur Kalibrierung
    ph.calibration(ph_voltage, temperature);

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
    Serial.println("Error reading temperature!");

  delay(1000); // 1 Sekunde warten, bevor die nächste Messung erfolgt
}

// Funktion und Kallibrierung des Trübheitsensor
float sensors_module::readTurbidity(float temperature) {
  float trb_voltage = adc->readVoltage(TRB_ADC_PIN);

  // Temperaturkompensation
  float slope = -0.0075;                                   // Spannungsabfall pro Grad Celsius. (4.1 V - 4.4 V) / 40 Grad Celsius
  float baselinePure = 4.4 + slope * (temperature - 10.0); // Berechnung der Basisspannung bzw der erwarteten Spannung

  // Berechne die Differenz zwischen der erwarteten Basisspannung und der gemessenen Spannung
  float voltageDifference = baselinePure - trb_voltage;

  // Umrechnung in NTU: Wichtig hier!! Annahme ist das 0.5 V Unterschied, entspricht bei 1000 NTU ein Faktor 2000 NTU/V.
  // Generelle Annahme die getroffen wurde, ist das wir einen linearen Spannungsabfall bei steigender Temperatur haben, was aus dem Datenblatt des Sensors zu entnehmen ist.
  // Daher wurde diese Annahmen getroffen für die Berechnung in NTU. Die Formel ist nicht exakt aber im Intervall des Sensors bei 4.1 +- 0.3V für Temperatur 10 bis 50 Grad Celsius.
  float ntu = voltageDifference * 2000.0;

  // Falls Messrauschen negative NTU ergibt, setze NTU = 0
  if (ntu < 0)
    ntu = 0;

  return ntu;
}

// Funktion zum Auslesen des DS18B20-Temperatursensors
float sensors_module::readTemperature() {
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
