import json
import numpy as np
import matplotlib.pyplot as plt

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

# --------------------- Load data  ----------------------
with open("solar_data.json") as f:
    data = json.load(f)

series_day_energy = []

def parse_hour_min(t):
    """
    Parses a time string in 'H:M' format and returns the time in hours as a float.
    Example: '8:30' -> 8.5
    """
    h, m = map(int, t.split(':'))
    return h + m / 60.0

def houre_min_to_str(t : float):
    return f"{int(t)}:{int(np.round((t % 1) * 60))}"

def get_time_diff(time_start: str, time_end: str):
    """
    Returns the difference in hours between two time strings (e.g., '8:30', '34:20').
    """
    start = parse_hour_min(time_start)
    end = parse_hour_min(time_end)
    return houre_min_to_str(end - start)


def print_stats_for_series(series):
    last_time = "00:00"
    last_val : np.float32 = np.round(0.0,2)

    template = "{0:<8} {1:<8} {2:<8} {3:<8} {4:<8}" # column widths: 8, 10, 15, 7, 10
    print(template.format("i", "Time", "Wh", "ΔT", "ΔWh"))
    for i,time in enumerate(series):
        print(template.format(i, time, series[time], get_time_diff(last_time,time), np.round(series[time]-last_val,decimals=2)))

        last_time = time
        last_val = np.round(series[time],2)

    print("\n")


for series in data:

    print_stats_for_series(series)

    items = sorted(series.items(), key=lambda x: parse_hour_min(x[0]))
    day_energies = {}

    # 1) Assign first measurement value fully to day 0 if <24h
    first_time = parse_hour_min(items[0][0])
    first_value = items[0][1]
    if first_time <= 24:
        day_energies[0] = first_value

    # 2) Then compute the delta between successive readings
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

# --------------------- Plotting ------------------------
all_days = sorted(set().union(*series_day_energy))
n_series = len(series_day_energy)
bar_width = 0.8 / n_series
x = range(len(all_days))

fig, ax = plt.subplots()

for i, days in enumerate(series_day_energy):
    heights = [days.get(d, 0) for d in all_days]
    positions = [p + i * bar_width for p in x]
    ax.bar(positions, heights, width=bar_width, label=f"Messreihe {i+1}")

ax.set_xticks([p + (n_series - 1)*bar_width/2 for p in x])
ax.set_xticklabels([f"Tag {d}" for d in all_days])
ax.set_ylabel("Energie in Wh")
# ax.set_xlabel("Tag")
ax.set_title("Solarzellen Leistungsmessungen")
ax.legend()
plt.show()
