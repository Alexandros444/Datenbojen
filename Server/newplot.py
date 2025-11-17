#!/usr/bin/env python3
"""
Liest 'received_data.csv' ein und zeichnet Temperatur ("Temp C") und Batteriespannung über der Zeit.
Berechnet zusätzlich die mittlere, minimale und maximale Zeit zwischen zwei Messungen.
Zeichnet außerdem die Median-Spannung als Linie ein.
Speichert das Diagramm als 'temp_voltage_plot.png' im selben Ordner.
"""

from pathlib import Path
import re
import sys
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.dates import DateFormatter

HERE = Path(__file__).parent
CSV_PATH = HERE / 'received_data_adjusted.csv'
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
if 'battery_v' not in df.columns:
    print("Spalte 'Battery' nicht gefunden.")
    sys.exit(1)

# # Batteriespannung berechnen
# df['battery_mv'] = df['Battery'].apply(extract_batt_mv)
# df['battery_v'] = df['battery_mv'] / 1000.0

# Nach Zeit sortieren
df = df.sort_values('timestamp')

# Zeitdifferenzen berechnen
if len(df) > 1:
    time_diffs = df['timestamp'].diff().dropna()
    mean_delta = time_diffs.mean()
    min_delta = time_diffs.min()
    max_delta = time_diffs.max()

    def fmt_td(td):
        """Formatiere timedelta als s / min / h"""
        sec = td.total_seconds()
        return f"{sec:.2f} s ({sec/60:.2f} min, {sec/3600:.3f} h)"

    print("Zeit zwischen Messungen:")
    print(f"   Mittelwert: {fmt_td(mean_delta)}")
    print(f"   Minimum:    {fmt_td(min_delta)}")
    print(f"   Maximum:    {fmt_td(max_delta)}")
else:
    print("Nicht genügend Datenpunkte, um Zeitdifferenzen zu berechnen.")

# Median der Spannung
median_voltage = df['battery_v'].median()
print(f"Median der Batteriespannung: {median_voltage:.3f} V")
# Print slope of median voltage over time

slope_total = (df['battery_v'].iloc[-1] - df['battery_v'].iloc[0])
slope = slope_total / ((df['timestamp'].iloc[-1] - df['timestamp'].iloc[0]).total_seconds() / 3600)
print(f"Spannungsänderung gesamt: {slope_total:.6f} V")
print(f"Spannungsänderung über Zeit: {slope:.6f} V/h")

# Plot-Stil neutral halten (kein farbiger Hintergrund)
plt.style.use('default')
fig, ax1 = plt.subplots(figsize=(12, 6), facecolor='white')

# Temperatur
ax1.plot(df['timestamp'], df['Temp C'], color='tab:red', label='Temperatur (°C)')
ax1.set_xlabel('Zeit (Tag. Stunde)')
ax1.set_ylabel('Temperatur (°C)', color='tab:red')
ax1.tick_params(axis='y', labelcolor='tab:red')

# Batteriespannung (zweite Achse)
ax2 = ax1.twinx()
ax2.plot(df['timestamp'], df['battery_v'], color='tab:blue', label='Batteriespannung (V)')
ax2.axhline(median_voltage, color='gray', linestyle='--', linewidth=1.5,
            label=f'Median Spannung = {median_voltage:.2f} V')
ax2.set_ylabel('Batteriespannung (V)', color='tab:blue')
ax2.tick_params(axis='y', labelcolor='tab:blue')
ax2.set_ylim(0, 4.2)

# Zeitformat: dd hh
date_formatter = DateFormatter("%d. %Hh")
ax1.xaxis.set_major_formatter(date_formatter)

# Automatische Formatierung der Datumsbeschriftungen
fig.autofmt_xdate()

# Titel und Legende (unten links)
plt.title('Temperatur und Batteriespannung über der Zeit')
lines_1, labels_1 = ax1.get_legend_handles_labels()
lines_2, labels_2 = ax2.get_legend_handles_labels()
ax1.legend(lines_1 + lines_2, labels_1 + labels_2, loc='lower left')

plt.tight_layout()
plt.savefig(OUT_PNG, facecolor='white')
print(f"Diagramm gespeichert unter: {OUT_PNG}")

# Optional: interaktiv anzeigen
try:
    plt.show()
except Exception:
    pass
