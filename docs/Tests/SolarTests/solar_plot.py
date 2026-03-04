import json
import numpy as np
import matplotlib.pyplot as plt
import csv

def parse_hour_min(t):
    h, m = map(int, t.split(':'))
    return h + m / 60.0

def distribute_energy(start_h, end_h, delta_wh):
    """
    Split energy between day boundaries.
    """
    result = {}
    current = start_h
    while current < end_h:
        day = int(current // 24)
        next_boundary = (day + 1) * 24
        segment_end = min(end_h, next_boundary)
        total_interval = end_h - start_h
        hours_in_segment = segment_end - current
        portion = (hours_in_segment / total_interval) if total_interval > 0 else 0
        result[day] = result.get(day, 0) + (delta_wh * portion)
        current = segment_end
    return result

# --------------------- Load data ----------------------
with open("solar_data.json") as f:
    data = json.load(f)

series_day_energy = []

def houre_min_to_str(t: float):
    return f"{int(t)}:{int(np.round((t % 1) * 60))}"

def get_time_diff(time_start: str, time_end: str):
    start = parse_hour_min(time_start)
    end = parse_hour_min(time_end)
    diff = end - start
    return houre_min_to_str(diff if diff >= 0 else 0)

def print_and_collect_stats(series, idx):
    """
    Prints and collects statistics for one Messreihe.
    Returns a list of tuples for CSV export.
    """
    last_time = "00:00"
    last_val = np.round(0.0, 2)
    rows = []

    template = "{0:<8} {1:<8} {2:<8} {3:<8} {4:<8}"
    print(f"\nMessreihe {idx + 1}")
    print(template.format("i", "Time", "Wh", "ΔT", "ΔWh"))

    for i, time in enumerate(series):
        delta_t = get_time_diff(last_time, time)
        delta_wh = np.round(series[time] - last_val, decimals=2)
        print(template.format(i, time, series[time], delta_t, delta_wh))
        rows.append([f"Messreihe {idx + 1}", i, time, series[time], delta_t, delta_wh])
        last_time = time
        last_val = np.round(series[time], 2)

    print("\n")
    return rows


# --------------------- Process Data ----------------------
all_detail_rows = []

for idx, series in enumerate(data):
    detail_rows = print_and_collect_stats(series, idx)
    all_detail_rows.extend(detail_rows)

    items = sorted(series.items(), key=lambda x: parse_hour_min(x[0]))
    day_energies = {}

    first_time = parse_hour_min(items[0][0])
    first_value = items[0][1]
    if first_time <= 24:
        day_energies[0] = first_value

    prev_time = first_time
    prev_val = first_value

    for time_str, val in items[1:]:
        t = parse_hour_min(time_str)
        delta = val - prev_val
        split = distribute_energy(prev_time, t, delta)
        for d, e in split.items():
            day_energies[d] = day_energies.get(d, 0) + e
        prev_time = t
        prev_val = val

    series_day_energy.append(day_energies)

# --------------------- Compute Averages ----------------------
averages = []
for i, days in enumerate(series_day_energy):
    avg = np.mean(list(days.values())) if days else 0
    averages.append(avg)

overall_avg = np.mean(averages) if averages else 0

# --------------------- Save to CSV ----------------------
# 1) Averages
with open("solar_averages.csv", "w", newline="", encoding="utf-8") as f:
    writer = csv.writer(f)
    writer.writerow(["Messreihe", "Durchschnitt pro Tag [Wh]"])
    for i, avg in enumerate(averages):
        writer.writerow([f"Messreihe {i+1}", round(avg, 2)])
    writer.writerow([])
    writer.writerow(["Gesamtdurchschnitt", round(overall_avg, 2)])

# 2) Detailed per-measurement data
with open("solar_series_details.csv", "w", newline="", encoding="utf-8") as f:
    writer = csv.writer(f)
    writer.writerow(["Messreihe", "i", "Time", "Wh", "ΔT", "ΔWh"])
    writer.writerows(all_detail_rows)

print("\n--- CSV Files Written ---")
print(" -> solar_averages.csv (Averages per series + overall)")
print(" -> solar_series_details.csv (All per-measurement data)\n")

# --------------------- Plot 1: Original ----------------------
all_days = sorted(set().union(*series_day_energy))
n_series = len(series_day_energy)
bar_width = 0.8 / n_series
x = range(len(all_days))

fig, ax = plt.subplots()

for i, days in enumerate(series_day_energy):
    heights = [days.get(d, 0) for d in all_days]
    positions = [p + i * bar_width for p in x]
    ax.bar(positions, heights, width=bar_width, label=f"Messreihe {i+1}")

ax.set_xticks([p + (n_series - 1) * bar_width / 2 for p in x])
ax.set_xticklabels([f"Tag {d}" for d in all_days])
ax.set_ylabel("Energie in Wh")
ax.set_title("Solarzellen Leistungsmessungen")
ax.legend()

# --------------------- Plot 2: Transparent/Hollow with Averages ----------------------
fig2, ax2 = plt.subplots()

for i, days in enumerate(series_day_energy):
    heights = [days.get(d, 0) for d in all_days]
    positions = [p + i * bar_width for p in x]
    ax2.bar(positions, heights, width=bar_width, edgecolor=f"C{i}", fill=False, linewidth=1.5, label=f"Messreihe {i+1}")
    ax2.axhline(y=averages[i], color=f"C{i}", linestyle="--", linewidth=1.2)

ax2.axhline(y=overall_avg, color="black", linestyle=":", linewidth=2, label="Gesamtdurchschnitt")

ax2.set_xticks([p + (n_series - 1) * bar_width / 2 for p in x])
ax2.set_xticklabels([f"Tag {d}" for d in all_days])
ax2.set_ylabel("Energie in Wh")
ax2.set_title("Solarzellen Leistungsmessungen (Durchschnittsanalyse)")
ax2.legend()

plt.show()
