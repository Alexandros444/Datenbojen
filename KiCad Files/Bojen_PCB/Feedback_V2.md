



## IO Expander
- [x] Silkscreen missing
- [x] Holes to small
- Weitere Pins zugänglich machen

## Sim Module
- [x] Silkscreen missing
- [ ] Mosfet DOES NOT WORK, Wenn ESP aus, wird gpio auf GND gezogen,  ~1.1V gemessen Spannungsteiler innenwiderstand berechnen? Auf Nicht inv. wechseln


## TMP Sensor
- [x] Beschriftung Falsch "- + D" -> "- D +"


## GSM Module
- [x] Mehr Pufferkondensatoren
    - 100u, 10u
- [x] PWR Mosfet Skip
- [x] RST Pin auf GPIO für Hard Reset


## Power
- [x] Puffer Kondensatoren
    - 100u, 10u


## Kosten
- JST - XH - 5€
- Pin Headers - 2€
- DS18B20

# ESP
- [x] GPIO Label Silkscreen TODO / Label Pin Header
- [x] USB-C kann man nicht einstecken wenn Sensoren verbaut. ggf. (Sensoren Tiefer legen) oder esp mehr nach unten
- [o] USB verbindung rausführen, aber USB - TTL benötigt (not worth it)


# Mount Holes
- Mehr Clearence
- [x] Besonders bei den ESP-GPIO Pins zu dem mount vom Waveshare solar board


# Solar Modul
- Power reroute backup, Battery voltage for development


# Sensoren
- [x] Sensoren Distanz zu Board anschlüssen sind zu kurz für Steckverbindungen
- ADC Pins beschriften Bei Sensoren A0-A3


# Board
- Alle Pins rausbringen (oben Vcc anschlüsse etc.)
- [x] Alle GPIO Pin header für extension board
- GPIO Default connected pins (everywhere solder work minimizing)

# LED
- Braucht kein Widerstand für Data weil, ESP 3.3V

# Timer
- [x] Buffer Capacitor