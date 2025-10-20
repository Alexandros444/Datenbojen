import re
import os
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.widgets import CheckButtons

# Read and safely concatenate CSV files (some files may miss columns)
files = [
	'received_data_old.csv',
	'received_data_tmp.csv',
	'received_data.csv'
]

dfs = []
for f in files:
	path = os.path.join(os.path.dirname(__file__), f)
	try:
		dfs.append(pd.read_csv(path, low_memory=False))
	except FileNotFoundError:
		# ignore missing files
		continue

if not dfs:
	raise SystemExit("No input files found.")

df = pd.concat(dfs, ignore_index=True, sort=False)

# Parse timestamp
df['timestamp'] = pd.to_datetime(df['timestamp'], errors='coerce')
df = df.dropna(subset=['timestamp'])

# Temperature: numeric and remove invalid marker -1000
if 'temperature' in df.columns:
	df['temperature'] = pd.to_numeric(df['temperature'], errors='coerce')
	df = df[df['temperature'] != -1000]
else:
	df['temperature'] = pd.NA

# Battery-Voltage: extract numeric mV from strings like "3769 mV - not Charging"
def parse_batt_mv(val):
	if pd.isna(val):
		return pd.NA
	# accept numeric already present
	s = str(val)
	m = re.search(r'(-?\d+)\s*mV', s)
	if m:
		return int(m.group(1))
	# fallback: any number
	m2 = re.search(r'(-?\d+)', s)
	return int(m2.group(1)) if m2 else pd.NA

if 'Battery-Voltage' in df.columns:
	df['Battery_mV'] = df['Battery-Voltage'].apply(parse_batt_mv)
else:
	df['Battery_mV'] = pd.NA

# Signal-Quality: extract numeric dBm (e.g. "< -115 dBm" -> -115)
def parse_signal_dbm(val):
	if pd.isna(val):
		return pd.NA
	s = str(val)
	m = re.search(r'(-?\d+)', s)
	return int(m.group(1)) if m else pd.NA

if 'Signal-Quality' in df.columns:
	df['Signal_dBm'] = df['Signal-Quality'].apply(parse_signal_dbm)
else:
	df['Signal_dBm'] = pd.NA

# TDS and Current_mA: convert to numeric if present
for col, newcol in [('TDS', 'TDS'), ('Current_mA', 'Current_mA')]:
	if col in df.columns:
		df[newcol] = pd.to_numeric(df[col], errors='coerce')
	else:
		df[newcol] = pd.NA

# set index
df.set_index('timestamp', inplace=True)
df.sort_index(inplace=True)

# Prepare plotting: combine into one plot with multiple y-axes
metrics = [
	('temperature', 'Temperature (°C)'),
	('Battery_mV', 'Battery Voltage (mV)'),
	('Signal_dBm', 'Signal Quality (dBm)'),
	('TDS', 'TDS'),
	('Current_mA', 'Current (mA)')
]

# Create single figure and host axis
# compute offsets so additional twinx axes stay inside the figure
pos_offset = 0.05  # smaller offset between twin spines
total_extra_width = pos_offset * (len(metrics) - 1)
fig, host = plt.subplots(figsize=(14, 6))
# expand right margin dynamically to accommodate extra y-axes, and leave space at top for buttons
fig.subplots_adjust(right=0.75 + total_extra_width, top=0.90)
 
# base axis (host) plus one twinx per extra metric
axes = [host]
for i in range(1, len(metrics)):
	# create a new twin axis and shift its spine to the right
	ax = host.twinx()
	# position offset in axes coordinates (increase for each extra axis)
	pos = 1.0 + pos_offset * (i - 1)
	ax.spines["right"].set_position(("axes", pos))
	# make sure background doesn't cover host
	ax.set_frame_on(True)
	ax.patch.set_visible(False)
	axes.append(ax)
 
# colors for clarity
colors = ['tab:blue', 'tab:orange', 'tab:green', 'tab:red', 'tab:purple']
 
# Plot each metric on its assigned axis
lines = []
labels = []
for ax, (col, label), color in zip(axes, metrics, colors):
	series = pd.to_numeric(df[col], errors='coerce') if col in df.columns else pd.Series(dtype=float, index=df.index)
	if series.notna().any():
		l, = ax.plot(df.index, series, 'o-', markersize=3, color=color, label=label, alpha=0.8)
		lines.append(l)
		labels.append(label)
	ax.set_ylabel(label, color=color)
	ax.tick_params(axis='y', colors=color)
 
# X axis and overall formatting
host.set_xlabel('Timestamp')
host.set_title('Sensor Data Overview (combined)')
host.xaxis.set_tick_params(rotation=25)
host.grid(True, axis='x', linestyle=':', alpha=0.5)
 
# combined legend (clickable) + CheckButtons for toggling
leg = None
if lines:
	leg = host.legend(lines, labels, loc='upper left', bbox_to_anchor=(0, 1.02))
	# make legend entries pickable and store reference to original line
	for legline, origline in zip(leg.get_lines(), lines):
		legline.set_picker(5)  # tolerance in points
		legline._origline = origline

	def on_pick(event):
		# toggle visibility of the corresponding original line
		legline = event.artist
		origline = getattr(legline, "_origline", None)
		if origline is None:
			return
		vis = not origline.get_visible()
		origline.set_visible(vis)
		legline.set_alpha(1.0 if vis else 0.2)
		fig.canvas.draw_idle()

	fig.canvas.mpl_connect('pick_event', on_pick)

	# Add compact horizontal CheckButtons at the bottom-right
	# Use short display labels and keep mapping to full labels for callbacks
	display_labels = ['C°', 'mV', 'dBm', 'TDS', 'mA']
	# reserve a bit more bottom margin so buttons don't overlap x-axis
	fig.subplots_adjust(right=0.75 + total_extra_width, top=0.90, bottom=0.10)
	# coords: [left, bottom, width, height] in figure fraction (bottom-right)
	rax = fig.add_axes([0.8, 0.01, 0.16, 0.15])
	# create horizontal CheckButtons (orientation kw may be supported on your MPL version)
	check = CheckButtons(rax, display_labels, [l.get_visible() for l in lines])
	# tighten label font to fit horizontally
	for txt in check.labels:
		txt.set_fontsize(9)
	# remove axis decorations behind the widget
	rax.set_xticks([])
	rax.set_yticks([])
	rax.set_frame_on(False)
 
	def cb_func(label):
		# map short display label to metric index
		idx = display_labels.index(label)
		line = lines[idx]
		vis = not line.get_visible()
		line.set_visible(vis)
		# update legend appearance if present
		if leg is not None:
			leg.get_lines()[idx].set_alpha(1.0 if vis else 0.2)
		fig.canvas.draw_idle()
 
	check.on_clicked(cb_func)
 
plt.tight_layout()
plt.show()