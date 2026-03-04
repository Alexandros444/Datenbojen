"""Plot Voltage & Current (one figure) and Energy (another) from a CSV file.

Assumes CSV columns (with or without leading comment lines):
Time(s),Voltage(V),Current(A),Power(W),Capacity(Ah),Energy(Wh)

Usage:
	python System_Power_Plot.py [path/to/SystemTestLong1.csv]

If no path is provided, the script will try to read 'SystemTestLong1.csv' in the
same directory as this script.
"""

import sys
from pathlib import Path
import csv
import matplotlib.pyplot as plt


def read_system_csv(path):
	"""Read the CSV and return columns as lists: time, voltage, current, energy.

	The reader will skip initial comment lines that start with '#' and will try
	to parse the header to find column indices. If no header is present it will
	assume the standard order: Time,Voltage,Current,Power,Capacity,Energy.
	"""
	time = []
	voltage = []
	current = []
	energy = []

	with open(path, 'r', newline='') as f:
		# peek lines to skip commented header lines
		reader = csv.reader(f)
		headers = None
		for row in reader:
			if not row:
				continue
			# join row to string to check for leading comment
			line = ','.join(row).strip()
			if line.startswith('#'):
				# remove leading '#' and check if it's header-like
				maybe = line.lstrip('#').strip()
				if 'Time' in maybe and 'Voltage' in maybe:
					headers = [h.strip() for h in maybe.split(',')]
					break
				else:
					continue
			else:
				# first non-comment row: could be header or data
				if any(s.isalpha() for s in row[0]):
					headers = [h.strip() for h in row]
				else:
					# no header, this is data: reset file pointer and read with defaults
					f.seek(0)
				break

		# Determine how to read the rest depending on headers
		f.seek(0)
		reader = csv.reader(f)
		# skip initial comment lines
		for row in reader:
			if not row:
				continue
			line = ','.join(row).strip()
			if line.startswith('#'):
				# if header is embedded in comment, extract it and continue
				maybe = line.lstrip('#').strip()
				if 'Time' in maybe and 'Voltage' in maybe:
					headers = [h.strip() for h in maybe.split(',')]
				continue
			# now row is first non-comment row
			if headers is None and any(c.isalpha() for c in ''.join(row)):
				headers = [h.strip() for h in row]
				# continue to next rows for data
				break
			else:
				# no header; we'll parse using defaults below
				break

		# Create another reader from the file to parse numeric rows
		f.seek(0)
		reader = csv.reader(f)
		for row in reader:
			if not row:
				continue
			line = ','.join(row).strip()
			if line.startswith('#'):
				continue
			# if this row looks like header, skip it
			if headers is not None and all(h.replace(' ', '').isalpha() for h in row):
				# header row
				continue
			# parse numeric values, allow possible whitespace
			try:
				vals = [v.strip() for v in row]
				# If there are more columns due to stray commas, truncate to 6
				# if len(vals) < 6:
				# 	# try to skip rows that aren't data
				# 	continue
				t = float(vals[0])
				vlt = float(vals[1])
				i = float(vals[2])
				en = float(vals[5])
			except Exception:
				try:
					vals = [v.strip() for v in row]
					# If there are more columns due to stray commas, truncate to 6
					if len(vals) < 5:
						# try to skip rows that aren't data
						continue
					t = float(vals[0])
					vlt = float(vals[1])
					i = float(vals[2])
					en = float(vals[4])
				except Exception:
					# Skip lines which cant be red
					continue

			time.append(t)
			voltage.append(vlt)
			current.append(i)
			energy.append(en)

	return time, voltage, current, energy


def plot_voltage_current(time, voltage, current, median=None, cycles=None, out_path=None):
	fig, ax1 = plt.subplots(figsize=(10, 5))
	ax1.plot(time, voltage, 'tab:blue', label='Spannung')
	ax1.set_xlabel('Zeit in s')
	ax1.set_ylabel('Spannung in V', color='tab:blue')
	ax1.tick_params(axis='y', labelcolor='tab:blue')

	ax2 = ax1.twinx()
	ax2.plot(time, current, 'tab:red', label='Strom', alpha=0.4, linestyle=':')
	ax2.set_ylabel('Strom in A', color='tab:red')
	ax2.tick_params(axis='y', labelcolor='tab:red')


	if median is not None:
		ax2.plot(time,median, color="tab:red", label="Strom arith. Mittel (2s)", alpha=0.8)

	# Optionally plot a rolling median series (1s window) if provided.
	# if median is not None:
	# 	try:
	# 		ax2.plot(time, median, color='magenta', label='Median(1s)', linewidth=1.25, alpha=0.9)
	# 		# draw threshold line for median-based sending detection at 0.2 A
	# 		ax2.axhline(0.2, color='black', linestyle='--', linewidth=0.9, alpha=0.7, label='Median thresh 0.2 A')
	# 	except Exception:
	# 		pass

	# combine legends from both axes
	lines_1, labels_1 = ax1.get_legend_handles_labels()
	lines_2, labels_2 = ax2.get_legend_handles_labels()
	ax1.legend(lines_1 + lines_2, labels_1 + labels_2, loc='upper right')

	plt.title('Energieverbrauch')
	plt.grid(True, which='both', linestyle='--', linewidth=0.5)

	# Shade detected cycles (measurement / sending / sleep)
	if cycles:
		t = time
		first = True
		for idx, c in enumerate(cycles):
			s = c['start_idx']
			ss = c['send_start_idx']
			e = c['end_idx']
			valid = c.get('valid', True)
			try:
				t_s = t[s]
				t_ss = t[ss]
				t_e = t[e]
			except Exception:
				continue
			# Decide colors; unknown cycles get greyed out
			if valid:
				meas_color = 'gold'
				send_color = 'salmon'
				sleep_color = 'lightgrey'
				meas_label = 'Measuring' if first else None
				send_label = 'Sending' if first else None
				sleep_label = 'Sleep' if first else None
			else:
				meas_color = 'darkgrey'
				send_color = 'grey'
				sleep_color = 'lightgrey'
				meas_label = 'Unknown' if first else None
				send_label = None
				sleep_label = None

			# Measuring zone
			ax1.axvspan(t_s, t_ss, color=meas_color, alpha=0.25, label=meas_label)
			# Sending zone
			ax1.axvspan(t_ss, t_e, color=send_color, alpha=0.25, label=send_label)
			# Sleep zone: until next cycle start or end of data
			if idx + 1 < len(cycles):
				next_s = cycles[idx + 1]['start_idx']
				try:
					t_next = t[next_s]
					ax1.axvspan(t_e, t_next, color=sleep_color, alpha=0.25, label=sleep_label)
				except Exception:
					pass
			first = False

			# # Add horizontal phase label bars at top of plot for clarity
			# try:
			# 	ymin, ymax = ax1.get_ylim()
			# 	height = ymax - ymin
			# 	label_y = ymin + height * 0.98
			# 	for idx, c in enumerate(cycles):
			# 		s = c['start_idx']
			# 		ss = c['send_start_idx']
			# 		e = c['end_idx']
			# 		valid = c.get('valid', True)
			# 		try:
			# 			t_s = t[s]
			# 			t_ss = t[ss]
			# 			t_e = t[e]
			# 		except Exception:
			# 			continue
			# 		# text labels centered in each zone
			# 		if valid:
			# 			ax1.text((t_s + t_ss) / 2.0, label_y, 'Messen', ha='center', va='top', fontsize=8, color='black', bbox=dict(facecolor='gold', alpha=0.15, edgecolor='none'))
			# 			ax1.text((t_ss + t_e) / 2.0, label_y, 'Senden', ha='left', va='top', fontsize=8, color='black', bbox=dict(facecolor='salmon', alpha=0.15, edgecolor='none'))
			# 		# else:
			# 		# 	ax.text((t_s + t_e) / 2.0, label_y, 'Unknown', ha='center', va='top', fontsize=8, color='black', bbox=dict(facecolor='darkgrey', alpha=0.15, edgecolor='none'))
			# 		# sleep label between cycles
			# 		if idx + 1 < len(cycles):
			# 			next_s = cycles[idx + 1]['start_idx']
			# 			try:
			# 				t_next = t[next_s]
			# 				ax1.text((t_e + t_next) / 2.0, label_y, 'Schlaf', ha='center', va='top', fontsize=8, color='black', bbox=dict(facecolor='lightgrey', alpha=0.15, edgecolor='none'))
			# 			except Exception:
			# 				pass
			# except Exception:
			# 	pass

	plt.tight_layout()
	if out_path:
		fig.savefig(out_path)
	return fig


def plot_energy(time, energy, cycles=None, der_override=None, out_path=None):
	fig, ax = plt.subplots(figsize=(10, 4))
	ax.plot(time, energy, color='tab:green', label='Energie')
	ax.set_xlabel('Zeit in s')
	ax.set_ylabel('Energie in Wh', color='tab:green')
	ax.tick_params(axis='y', labelcolor='tab:green')
	ax.set_title('Akkumulierter Energieverbrauch')
	ax.grid(True, which='both', linestyle='--', linewidth=0.5)

	# derivative (dE/dt) on a twin y-axis
	import numpy as np

	time_arr = np.array(time)
	energy_arr = np.array(energy)
	# compute gradient (Wh/s)
	if der_override is not None:
		der_smooth = der_override
	else:
		if len(time_arr) > 1:
			der = np.gradient(energy_arr, time_arr)
		else:
			der = np.zeros_like(energy_arr)

		# smooth derivative with small moving average to reduce noise
		def smooth(x, w=5):
			if w <= 1:
				return x
			kernel = np.ones(w) / w
			return np.convolve(x, kernel, mode='same')

		der_smooth = smooth(der, w=5)

	# Instead of plotting the derivative line, mark rapid-change points on the energy curve
	import numpy as np

	time_arr = np.array(time)
	energy_arr = np.array(energy)
	if len(time_arr) > 1:
		der = np.gradient(energy_arr, time_arr)
	else:
		der = np.zeros_like(energy_arr)

	# smooth derivative
	def smooth(x, w=5):
		if w <= 1:
			return x
		kernel = np.ones(w) / w
		return np.convolve(x, kernel, mode='same')

	der_smooth_local = smooth(der, w=5)

	mean_der = np.nanmean(der_smooth_local)
	std_der = np.nanstd(der_smooth_local)
	der_thresh = mean_der + max(3 * std_der, 1e-6)

	# indices where rate-of-change is significantly high
	rapid_idx = np.where(der_smooth_local > der_thresh)[0]

	# combine legends (only energy legend)
	lines_1, labels_1 = ax.get_legend_handles_labels()
	ax.legend(lines_1, labels_1, loc='upper right')

	plt.tight_layout()
	if out_path:
		fig.savefig(out_path)
	# Mark rapid-change points on energy curve
	# (User requested removal of rapid-change markers — nothing drawn here)

	# Shade cycles similarly to voltage/current plot
	if cycles:
		t = time
		first = True
		for idx, c in enumerate(cycles):
			s = c['start_idx']
			ss = c['send_start_idx']
			e = c['end_idx']
			valid = c.get('valid', True)
			try:
				t_s = t[s]
				t_ss = t[ss]
				t_e = t[e]
			except Exception:
				continue
			if valid:
				ax.axvspan(t_s, t_ss, color='gold', alpha=0.25, label='Measuring' if first else None)
				ax.axvspan(t_ss, t_e, color='salmon', alpha=0.25, label='Sending' if first else None)
			else:
				ax.axvspan(t_s, t_e, color='darkgrey', alpha=0.25, label='Unknown' if first else None)
			if idx + 1 < len(cycles):
				next_s = cycles[idx + 1]['start_idx']
				try:
					t_next = t[next_s]
					ax.axvspan(t_e, t_next, color='lightgrey', alpha=0.25, label='Sleep' if first else None)
				except Exception:
					pass
			first = False

		# Add horizontal phase label bars at top of plot for clarity
		try:
			ymin, ymax = ax.get_ylim()
			height = ymax - ymin
			label_y = ymin + height * 0.98
			for idx, c in enumerate(cycles):
				s = c['start_idx']
				ss = c['send_start_idx']
				e = c['end_idx']
				valid = c.get('valid', True)
				try:
					t_s = t[s]
					t_ss = t[ss]
					t_e = t[e]
				except Exception:
					continue
				# text labels centered in each zone
				if valid:
					ax.text((t_s + t_ss) / 2.0, label_y, 'Messen', ha='center', va='top', fontsize=8, color='black', bbox=dict(facecolor='gold', alpha=0.15, edgecolor='none'))
					ax.text((t_ss + t_e) / 2.0, label_y, 'Senden', ha='left', va='top', fontsize=8, color='black', bbox=dict(facecolor='salmon', alpha=0.15, edgecolor='none'))
				# else:
				# 	ax.text((t_s + t_e) / 2.0, label_y, 'Unknown', ha='center', va='top', fontsize=8, color='black', bbox=dict(facecolor='darkgrey', alpha=0.15, edgecolor='none'))
				# sleep label between cycles
				if idx + 1 < len(cycles):
					next_s = cycles[idx + 1]['start_idx']
					try:
						t_next = t[next_s]
						ax.text((t_e + t_next) / 2.0, label_y, 'Schlaf', ha='center', va='top', fontsize=8, color='black', bbox=dict(facecolor='lightgrey', alpha=0.15, edgecolor='none'))
					except Exception:
						pass
		except Exception:
			pass

	return fig


def detect_cycles_and_phases(time, current, energy, sleep_thresh=0.0125, active_thresh=0.0125, min_measure_spike_s=20.0):
	"""Detect cycles and mark measurement/sending/sleep phases.

	Returns a list of dicts: {start_idx, send_start_idx, end_idx}
	Indices are inclusive start, exclusive end for plotting spans.
	"""
	import numpy as np

	t = np.array(time)
	i = np.array(current)
	e = np.array(energy)

	high = i > active_thresh
	# find rising edges: cycle starts where high is True and previous is False
	prev = np.concatenate((np.array([False]), high[:-1]))
	starts = np.where(np.logical_and(high, np.logical_not(prev)))[0]

	cycles = []
	# precompute derivative and smooth
	if len(t) > 1:
		der = np.gradient(e, t)
	else:
		der = np.zeros_like(e)

	# smooth derivative
	def smooth(x, w=5):
		if w <= 1:
			return x
		kernel = np.ones(w) / w
		return np.convolve(x, kernel, mode='same')

	der_smooth = smooth(der, w=5)

	mean_der = np.nanmean(der_smooth)
	std_der = np.nanstd(der_smooth)
	# threshold for energy derivative spike detection
	der_thresh = mean_der + max(3 * std_der, 1e-6)

	for s in starts:
		# find end: first index after s where high becomes False
		after = high[s:]
		rel_ends = np.where(after == False)[0]
		if rel_ends.size > 0:
			end = s + rel_ends[0]
		else:
			end = len(t) - 1

		send_start = None
		# first try: median-current rule after min_measure_spike_s
		min_time = t[s] + min_measure_spike_s
		min_idx = np.searchsorted(t, min_time)
		# define window length for median check (1 second)
		median_window_s = 2
		# convert to sample counts by finding indices where t <= t + median_window_s
		found_median = False
		for idx_candidate in range(min_idx, end + 1):
			# find window end index where time <= time[idx_candidate] + median_window_s
			window_end = np.searchsorted(t, t[idx_candidate] + median_window_s)
			if window_end <= idx_candidate:
				continue
			med = np.median(i[idx_candidate:window_end])
			if med >= 0.20:
				send_start = int(idx_candidate)
				found_median = True
				break

		# fallback to derivative-based detection if median rule didn't find send_start
		if not found_median:
			cand_idx = np.where((t >= min_time) & (np.arange(len(t)) <= end))[0]
			if cand_idx.size > 0:
				# find first index where der_smooth exceeds der_thresh
				big = np.where(der_smooth[cand_idx] > der_thresh)[0]
				if big.size > 0:
					send_start = int(cand_idx[big[0]])

		# fallback: if no send_start found, but there is a second hump in energy after s
		if send_start is None:
			# try to detect a local maxima in energy after s
			if end - s > 3:
				seg_e = e[s:end+1]
				rel_max = np.argmax(seg_e)
				send_start = s + rel_max
				# ensure it's at least a few seconds after s
				if t[send_start] < min_time:
					# don't accept a send start earlier than the minimum measurement window
					send_start = None

		# enforce measurement-first: ensure send_start is at or after min_time
		min_idx = np.searchsorted(t, min_time)
		if send_start is None or send_start < min_idx:
			# prefer min_idx if within the detected end bound
			if min_idx <= end:
				send_start = int(min_idx)
			else:
				# no sending detected within this cycle window; set send_start to end
				send_start = int(end)

		# if still None, set send_start = s (degenerate)
		if send_start is None:
			send_start = s

		# determine validity of cycle: measurement must be at least min_measure_spike_s
		# and must be followed by a send (i.e., send window not degenerate)
		valid = True
		# measurement duration
		meas_dur = t[send_start] - t[s] if send_start >= s else 0.0
		if meas_dur < min_measure_spike_s:
			valid = False

		# ensure there is a sending segment of some duration
		if send_start >= end:
			# no sending region inside this cycle
			valid = False

		cycles.append({
			'start_idx': int(s),
			'send_start_idx': int(send_start),
			'end_idx': int(end),
			'valid': bool(valid),
			'meas_dur_s': float(meas_dur),
		})

	return cycles, der_smooth


def export_cycle_csv(time, voltage, current, energy, cycles, out_path=None):
	"""Export CSV with per-cycle stats: start/end times, durations, mean power per phase, total energy consumed."""
	import numpy as np
	import csv

	t = np.array(time)
	v = np.array(voltage)
	i = np.array(current)
	e = np.array(energy)
	p = v * i

	rows = []
	unknown_rows = []
	for idx, c in enumerate(cycles):
		s = c['start_idx']
		ss = c['send_start_idx']
		end = c['end_idx']

		t_start = float(t[s])
		t_send = float(t[ss])
		t_end = float(t[end])

		dur_measure = float(t_send - t_start) if t_send > t_start else 0.0
		dur_send = float(t_end - t_send) if t_end >= t_send else 0.0
		# sleep duration until next cycle or 0 if last
		if idx + 1 < len(cycles):
			next_s = cycles[idx + 1]['start_idx']
			t_next = float(t[next_s])
			dur_sleep = float(t_next - t_end) if t_next > t_end else 0.0
		else:
			dur_sleep = 0.0

		mean_power_measure = float(np.nanmean(p[s:ss])) if ss > s else 0.0
		mean_power_send = float(np.nanmean(p[ss:end+1])) if end >= ss else 0.0

		# compute energies by integrating power over time (Wh)
		# measurement energy (integrate power over time -> W*s)
		t_seg_meas = t[s:ss]
		p_seg_meas = p[s:ss]
		if p_seg_meas.size >= 2:
			energy_meas_Ws = float(np.trapz(p_seg_meas, t_seg_meas))
		elif p_seg_meas.size == 1:
			# fallback: approximate as constant power over meas duration
			meas_duration = float(t[ss-1] - t[s]) if ss - 1 >= s else 0.0
			energy_meas_Ws = float(p_seg_meas[0] * meas_duration) if meas_duration > 0 else 0.0
		else:
			energy_meas_Ws = 0.0

		# sending energy
		t_seg_send = t[ss:end+1]
		p_seg_send = p[ss:end+1]
		if p_seg_send.size >= 2:
			energy_send_Ws = float(np.trapz(p_seg_send, t_seg_send))
		elif p_seg_send.size == 1:
			send_duration = float(t[end] - t[ss]) if end >= ss else 0.0
			energy_send_Ws = float(p_seg_send[0] * send_duration) if send_duration > 0 else 0.0
		else:
			energy_send_Ws = 0.0

		# sleep energy (between end and next cycle start)
		if idx + 1 < len(cycles):
			next_s = cycles[idx + 1]['start_idx']
			if (end + 1) < next_s:
				t_seg_sleep = t[end+1:next_s]
				p_seg_sleep = p[end+1:next_s]
			else:
				t_seg_sleep = np.array([])
				p_seg_sleep = np.array([])
		else:
			t_seg_sleep = np.array([])
			p_seg_sleep = np.array([])

		if p_seg_sleep.size >= 2:
			energy_sleep_Ws = float(np.trapz(p_seg_sleep, t_seg_sleep))
			mean_power_sleep = float(np.nanmean(p_seg_sleep))
		elif p_seg_sleep.size == 1:
			# if single sample in sleep, approximate duration from indices if next cycle exists
			if idx + 1 < len(cycles):
				next_s = cycles[idx + 1]['start_idx']
				sleep_duration = float(t[next_s] - t[end]) if next_s > end else 0.0
			else:
				sleep_duration = 0.0
			energy_sleep_Ws = float(p_seg_sleep[0] * sleep_duration) if sleep_duration > 0 else 0.0
			mean_power_sleep = float(p_seg_sleep[0])
		else:
			energy_sleep_Ws = 0.0
			mean_power_sleep = 0.0

		# total energy for cycle
		total_energy_cycle_Ws = energy_meas_Ws + energy_send_Ws + energy_sleep_Ws

		# cycle duration includes measure + send + sleep
		cycle_duration_s = float(dur_measure + dur_send + dur_sleep)

		row = {
			'cycle': idx + 1,
			't_start_s': t_start,
			't_send_s': t_send,
			't_end_s': t_end,
			'dur_measure_s': dur_measure,
			'dur_send_s': dur_send,
			'dur_sleep_s': dur_sleep,
			# mean powers reported in mW
			'mean_power_measure_mW': mean_power_measure * 1000.0,
			'mean_power_send_mW': mean_power_send * 1000.0,
			'mean_power_sleep_mW': mean_power_sleep * 1000.0,
			# total energies reported in Ws (W*s)
			'total_energy_meas_Ws': energy_meas_Ws,
			'total_energy_send_Ws': energy_send_Ws,
			'total_energy_sleep_Ws': energy_sleep_Ws,
			'total_energy_cycle_Ws': total_energy_cycle_Ws,
			'cycle_duration_s': cycle_duration_s,
		}

		# round numeric values to 0.1 decimal where appropriate
		for k, v in list(row.items()):
			if isinstance(v, float):
				row[k] = round(v, 1)

		if not c.get('valid', True):
			unknown_rows.append(row)
		else:
			rows.append(row)

	if out_path:
		keys = [
			'cycle', 't_start_s', 't_send_s', 't_end_s',
			'dur_measure_s', 'dur_send_s', 'dur_sleep_s',
			'mean_power_measure_mW', 'mean_power_send_mW', 'mean_power_sleep_mW',
			'total_energy_meas_Ws', 'total_energy_send_Ws', 'total_energy_sleep_Ws', 'total_energy_cycle_Ws',
			'cycle_duration_s'
		]
		with open(out_path, 'w', newline='') as f:
			writer = csv.DictWriter(f, fieldnames=keys)
			writer.writeheader()
			for r in rows:
				writer.writerow(r)

	return rows, unknown_rows


def main(argv=None):
	argv = argv or sys.argv[1:]
	if argv:
		csv_path = Path(argv[0]) if argv[0] else Path(__file__).with_name('SystemTestLong1.csv')
	else:
		csv_path = Path(__file__).with_name('SystemTestLong1.csv')

	if not csv_path.exists():
		print(f"CSV file not found: {csv_path}")
		return 2

	time, voltage, current, energy = read_system_csv(csv_path)
	if not time:
		print('No data parsed from CSV.')
		return 3

	# detect cycles and derivative for marking phases
	cycles, der_smooth = detect_cycles_and_phases(time, current, energy)

	# compute 1s rolling median of current for inspection
	try:
		import numpy as np
		t_arr = np.array(time)
		i_arr = np.array(current)
		median_window_s = 1.0
		median_series = np.zeros_like(i_arr)
		# for each sample, compute median of samples within [t, t+median_window_s)
		for idx in range(len(t_arr)):
			end_idx = np.searchsorted(t_arr, t_arr[idx] + median_window_s)
			if end_idx <= idx:
				median_series[idx] = i_arr[idx]
			else:
				median_series[idx] = float(np.median(i_arr[idx:end_idx]))
	except Exception:
		median_series = None

	out_dir = csv_path.parent
	v_i_png = out_dir / (csv_path.stem + '_voltage_current.png')
	energy_png = out_dir / (csv_path.stem + '_energy.png')
	summary_png = out_dir / (csv_path.stem + '_cycle_summary.png')

	plot_voltage_current(time, voltage, current, median=median_series, cycles=cycles, out_path=str(v_i_png))
	plot_energy(time, energy, cycles=cycles, der_override=der_smooth, out_path=str(energy_png))

	csv_summary = out_dir / (csv_path.stem + '_cycle_summary.csv')
	rows, unknown_rows = export_cycle_csv(time, voltage, current, energy, cycles, out_path=str(csv_summary))

	# if there are unknown cycles, write them to a separate CSV for inspection
	if unknown_rows:
		csv_unknown = out_dir / (csv_path.stem + '_unknown_cycles.csv')
		import csv as _csv
		keys = [
			'cycle', 't_start_s', 't_send_s', 't_end_s',
			'dur_measure_s', 'dur_send_s', 'dur_sleep_s',
			'mean_power_measure_mW', 'mean_power_send_mW', 'mean_power_sleep_mW',
			'total_energy_meas_Ws', 'total_energy_send_Ws', 'total_energy_sleep_Ws', 'total_energy_cycle_Ws',
			'cycle_duration_s'
		]
		with open(csv_unknown, 'w', newline='') as f:
			writer = _csv.DictWriter(f, fieldnames=keys)
			writer.writeheader()
			for r in unknown_rows:
				writer.writerow(r)
		print(f'Wrote: {v_i_png}\nWrote: {energy_png}\nWrote: {csv_summary}\nWrote: {csv_unknown}')
	else:
		print(f'Wrote: {v_i_png}\nWrote: {energy_png}\nWrote: {csv_summary}')

	plt.show()


if __name__ == '__main__':
	raise SystemExit(main())



