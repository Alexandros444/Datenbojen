
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

df['battery_mv'] = df['Battery'].apply(extract_batt_mv)
df['battery_v'] = df['battery_mv'] / 1000.0

# reverse Battery voltage array but not the timestamps
df['battery_v'] = df['battery_v'].values[::-1]
df['battery_v'] -= 1


# # Nach Zeit sortieren
# df = df.sort_values('timestamp')

# Write values back into csv name_adjusted.csv
df.to_csv(HERE / 'received_data_adjusted.csv', index=False)