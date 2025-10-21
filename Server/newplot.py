#!/usr/bin/env python3
"""
Liest 'received_data.csv' ein und zeichnet Temperatur ("Temp C") und Batteriespannung über der Zeit.
Speichert das Diagramm als 'temp_voltage_plot.png' im selben Ordner.
"""

from pathlib import Path
import re
import sys
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.dates import DateFormatter

HERE = Path(__file__).parent
CSV_PATH = HERE / 'received_data.csv'
OUT_PNG = HERE / 'temp_voltage_plot.png'

if not CSV_PATH.exists():
    print(f"CSV-Datei nicht gefunden: {CSV_PATH}")
    sys.exit(1)

# CSV einlesen
df = pd.read_csv(CSV_PATH, parse_dates=['timestamp'])

# Batteriespannung (mV) aus der Spalte 'Battery' extrahieren
def extract_batt_mv(s):
    if pd.isna(s):
        return None
    m = re.search(r"(\d+(?:\.\d+)?)\s*mV", str(s))
    if m:
        return float(m.group(1))
    m2 = re.search(r"(-?\d+(?:\.\d+)?)", str(s))
    if m2:
        return float(m2.group(1))
    return None

# Spalten prüfen
if 'Temp C' not in df.columns:
    print("Spalte 'Temp C' nicht gefunden.")
    sys.exit(1)
if 'Battery' not in df.columns:
    print("Spalte 'Battery' nicht gefunden.")
    sys.exit(1)

# Batteriespannung berechnen
df['battery_mv'] = df['Battery'].apply(extract_batt_mv)
df['battery_v'] = df['battery_mv'] / 1000.0

# Nach Zeit sortieren
df = df.sort_values('timestamp')

# Plot-Stil neutral halten (kein farbiger Hintergrund)
plt.style.use('default')
fig, ax1 = plt.subplots(figsize=(12, 6), facecolor='white')

# Temperatur
ax1.plot(df['timestamp'], df['Temp C'], color='tab:red', marker='o', markersize=4, label='Temperatur (°C)')
ax1.set_xlabel('Zeit (Tag Stunde)')
ax1.set_ylabel('Temperatur (°C)', color='tab:red')
ax1.tick_params(axis='y', labelcolor='tab:red')

# Batteriespannung (zweite Achse)
ax2 = ax1.twinx()
ax2.plot(df['timestamp'], df['battery_v'], color='tab:blue', marker='x', markersize=4, label='Batteriespannung (V)')
ax2.set_ylabel('Batteriespannung (V)', color='tab:blue')
ax2.tick_params(axis='y', labelcolor='tab:blue')
ax2.set_ylim(0, 4.2)

# Zeitformat: dd hh
date_formatter = DateFormatter("%d. %Hh")
ax1.xaxis.set_major_formatter(date_formatter)

# Automatische Formatierung der Datumsbeschriftungen
fig.autofmt_xdate()

# Titel und Legende
plt.title('Temperatur und Batteriespannung über Zeit')
lines_1, labels_1 = ax1.get_legend_handles_labels()
lines_2, labels_2 = ax2.get_legend_handles_labels()
ax1.legend(lines_1 + lines_2, labels_1 + labels_2, loc='lower left')

plt.tight_layout()
plt.grid()
plt.savefig(OUT_PNG, facecolor='white')
print(f"Diagramm gespeichert unter: {OUT_PNG}")

# Optional: interaktiv anzeigen
try:
    plt.show()
except Exception:
    pass
