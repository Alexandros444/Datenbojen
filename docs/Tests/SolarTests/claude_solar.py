import numpy as np
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider
import matplotlib.dates as mdates
from datetime import datetime, timedelta
import tkinter as tk
from tkinter import ttk

class SolarSystemSimulator:
    def __init__(self):
        # Default parameters
        self.params = {
            'cycles_per_day': 5,
            'battery_capacity_Wh': 30,
            'num_batteries': 1,
            'sleep_power_W': 0.1,
            'measuring_power_W': 1,
            'measuring_time_s': 60,
            'sending_power_W': 10,
            'sending_time_s': 60,
            'num_solar_cells': 1,
            'simulation_days': 30,
            'start_day': 0
        }
        
        # Create figure windows
        self.create_plot_windows()
        
        # Setup control window
        self.setup_control_window()
        
        # Initial simulation and plot
        self.simulate_and_plot()
    
    def create_plot_windows(self):
        """Create separate windows for each plot"""
        # Battery Energy Plot (shows over- and underproduction)
        self.fig1, self.ax1 = plt.subplots(figsize=(12, 6))
        try:
            self.fig1.canvas.manager.set_window_title('Batterie-Ladezustand')
        except Exception:
            pass
        self.fig1.tight_layout()

        # Solar Power Plot (solar only, no overproduction shading here)
        self.fig2, self.ax2 = plt.subplots(figsize=(12, 6))
        try:
            self.fig2.canvas.manager.set_window_title('Solarertrag / Energieverbrauch')
        except Exception:
            pass
        self.fig2.tight_layout()

        # Power Consumption Plot (separate window)
        self.fig3, self.ax3 = plt.subplots(figsize=(12, 6))
        try:
            self.fig3.canvas.manager.set_window_title('Energieverbrauch')
        except Exception:
            pass
        self.fig3.tight_layout()

        # System State and Energy Balance Plot (kept as a separate window)
        self.fig4, self.ax4 = plt.subplots(figsize=(12, 6))
        try:
            self.fig4.canvas.manager.set_window_title('Systemzustand und Leistungsbilanz')
        except Exception:
            pass
        self.fig4.tight_layout()
    
    def setup_control_window(self):
        """Create a separate Tkinter window for parameter controls"""
        self.control_window = tk.Tk()
        self.control_window.title("Solarsimulation: Parameter")
        self.control_window.geometry("900x750")
        
        # Create scrollable frame
        canvas = tk.Canvas(self.control_window)
        scrollbar = ttk.Scrollbar(self.control_window, orient="vertical", command=canvas.yview)
        scrollable_frame = ttk.Frame(canvas)
        
        def _on_scrollable_config(e):
            canvas.configure(scrollregion=canvas.bbox("all"))
        scrollable_frame.bind("<Configure>", _on_scrollable_config)     
        inner_window = canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
        def _on_canvas_config(event):
            canvas.itemconfig(inner_window, width=event.width)
        canvas.bind("<Configure>", _on_canvas_config)
        canvas.configure(yscrollcommand=scrollbar.set)
        for c in range(0, 8):
            try:
                scrollable_frame.grid_columnconfigure(c, weight=2)
            except Exception:
                pass
        scrollable_frame.grid_columnconfigure(0, weight=1)
        
        # Parameter configurations: (name, min, max, step, is_integer)
        param_configs = [
            ('cycles_per_day', 1, 20, 1, True),
            ('battery_capacity_Wh', 5, 100, 5, True),
            ('num_batteries', 1, 5, 1, True),
            ('sleep_power_W', 0.01, 1.0, 0.01, False),
            ('measuring_power_W', 0.1, 5.0, 0.1, False),
            ('measuring_time_s', 10, 300, 10, True),
            ('sending_power_W', 1, 50, 1, True),
            ('sending_time_s', 10, 300, 10, True),
            ('num_solar_cells', 1, 10, 1, True),
            ('simulation_days', 1, 200, 1, True),
            ('start_day', 0, 199, 1, True)
        ]
        
        # Deutsche Bezeichnungen für Parameter
        param_display_names = {
            'cycles_per_day': 'Zyklen pro Tag',
            'battery_capacity_Wh': 'Batteriekapazität (Wh)',
            'num_batteries': 'Anzahl Batterien',
            'sleep_power_W': 'Ruheleistung (W)',
            'measuring_power_W': 'Messleistung (W)',
            'measuring_time_s': 'Messdauer (s)',
            'sending_power_W': 'Sendeleistung (W)',
            'sending_time_s': 'Sendedauer (s)',
            'num_solar_cells': 'Anzahl Solarzellen',
            'simulation_days': 'Simulations-Tage',
            'start_day': 'Start-Tag'
        }
        
        self.sliders = {}
        row = 0
        
        for param, min_val, max_val, step, is_integer in param_configs:
            # Label
            display = param_display_names.get(param, param.replace('_', ' ').title())
            label = ttk.Label(scrollable_frame, text=display)
            label.grid(row=row, column=0, sticky='w', padx=10, pady=8)

            # Current value label
            value_var = tk.StringVar()
            if is_integer:
                value_var.set(f"{int(self.params[param])}")
            else:
                value_var.set(f"{self.params[param]:.2f}")
            value_label = ttk.Label(scrollable_frame, textvariable=value_var, width=12)
            value_label.grid(row=row, column=1, padx=10, pady=8)

            # Special handling for start_day: Prev/Next ±5 buttons
            if param == 'start_day':
                self.sliders[param] = {
                    'var': None,
                    'value_var': value_var,
                    'slider': None,
                    'is_integer': is_integer
                }

                def change_start(delta):
                    # clamp start_day between 0 and simulation_days-1
                    max_start = max(0, int(self.params.get('simulation_days', 1)) - 1)
                    new = int(self.params.get('start_day', 0)) + delta
                    new = max(0, min(new, max_start))
                    self.params['start_day'] = new
                    self.sliders[param]['value_var'].set(f"{new}")
                    # refresh plots immediately
                    self.simulate_and_plot()

                prev_btn = ttk.Button(scrollable_frame, text="<< -5 Tage", command=lambda d=-5: change_start(d))
                next_btn = ttk.Button(scrollable_frame, text="+5 Tage >>", command=lambda d=5: change_start(d))
                prev_btn.grid(row=row, column=2, padx=6, pady=8, sticky='w')
                next_btn.grid(row=row, column=3, padx=6, pady=8, sticky='w')

                row += 1
                continue
            
            # Slider for all other params
            slider_var = tk.DoubleVar(value=self.params[param])
            slider = ttk.Scale(scrollable_frame, from_=min_val, to=max_val, 
                             variable=slider_var, orient='horizontal', length=300)
            slider.grid(row=row, column=2, padx=10, pady=8)

            # Store references and bind update
            self.sliders[param] = {
                'var': slider_var,
                'value_var': value_var,
                'slider': slider,
                'is_integer': is_integer
            }

            # Bind update function (updates display and params)
            def make_display_update_func(p):
                def update_func(*args):
                    val = self.sliders[p]['var'].get()
                    if self.sliders[p]['is_integer']:
                        val = int(round(val))
                        self.sliders[p]['value_var'].set(f"{val}")
                        self.params[p] = val
                    else:
                        self.sliders[p]['value_var'].set(f"{val:.2f}")
                        self.params[p] = val
                return update_func

            # modern trace_add if available, else older trace
            try:
                slider_var.trace_add('write', make_display_update_func(param))
            except Exception:
                slider_var.trace('w', make_display_update_func(param))
            row += 1

        self.sun_days_Wh_entries = []
        self.sun_days_Wh : list[float] = [1]  # Start with one day

        # --- start: separate horizontal scrollable box for day entries ---
        sun_frame = ttk.LabelFrame(scrollable_frame, text="Solarverlauf - Wh pro Tag")
        sun_frame.grid(row=row, column=0, columnspan=8, sticky="ew", padx=5, pady=4)

        sun_canvas = tk.Canvas(sun_frame, height=80)
        h_scroll = ttk.Scrollbar(sun_frame, orient="horizontal", command=sun_canvas.xview)
        sun_canvas.configure(xscrollcommand=h_scroll.set)

        inner_days_frame = ttk.Frame(sun_canvas)
        inner_window = sun_canvas.create_window((0, 0), window=inner_days_frame, anchor="nw")

        def _on_inner_config(event):
            sun_canvas.configure(scrollregion=sun_canvas.bbox("all"))
        inner_days_frame.bind("<Configure>", _on_inner_config)

        def _on_mousewheel(event):
            if event.state & 0x0001:
                sun_canvas.xview_scroll(-1 * int(event.delta / 120), "units")
        sun_canvas.bind_all("<MouseWheel>", _on_mousewheel)

        sun_canvas.pack(fill="both", expand=True, side="top")
        h_scroll.pack(fill="x", side="bottom")

        def update_sun_days_entries():
            for child in inner_days_frame.winfo_children():
                child.destroy()
            self.sun_days_Wh_entries.clear()
            for i, val in enumerate(self.sun_days_Wh):
                cell = ttk.Frame(inner_days_frame, width=100, height=60)
                cell.grid_propagate(False)
                cell.grid(row=0, column=i, padx=8, pady=6)
                lbl = ttk.Label(cell, text=f"Tag {i}", anchor="center")
                lbl.pack(side="top", fill="x", padx=2)
                entry = ttk.Entry(cell, width=6, justify="center")
                entry.insert(0, str(val))
                entry.pack(side="top", pady=2)
                self.sun_days_Wh_entries.append(entry)
            sun_canvas.update_idletasks()
            sun_canvas.configure(scrollregion=sun_canvas.bbox("all"))

        def add_day():
            read_sun_day_values()
            self.sun_days_Wh.append(self.sun_days_Wh[-1] if self.sun_days_Wh else 1.0)
            update_sun_days_entries()

        def remove_day():
            read_sun_day_values()
            if len(self.sun_days_Wh) > 1:
                self.sun_days_Wh.pop()
                update_sun_days_entries()

        btn_frame = ttk.Frame(scrollable_frame)
        btn_frame.grid(row=row + 1, column=0, columnspan=3, sticky="w", pady=(4, 10))

        add_btn = ttk.Button(btn_frame, text="+ Tag", command=add_day)
        remove_btn = ttk.Button(btn_frame, text="- Tag", command=remove_day)
        add_btn.pack(side="left", padx=(0, 6))
        remove_btn.pack(side="left")

        update_sun_days_entries()
        row += 2

        def read_sun_day_values():
            new = []
            for entry in self.sun_days_Wh_entries:
                try:
                    v = float(entry.get())
                except Exception:
                    v = 1.0
                new.append(v)
            self.sun_days_Wh = new or [1.0]

        def update_simulation():
            read_sun_day_values()
            # Ensure start_day is not out of range after changing simulation_days
            max_start = max(0, int(self.params.get('simulation_days', 1)) - 1)
            if self.params['start_day'] > max_start:
                self.params['start_day'] = max_start
                if 'start_day' in self.sliders and self.sliders['start_day']['value_var'] is not None:
                    self.sliders['start_day']['value_var'].set(f"{max_start}")
            self.simulate_and_plot()

        update_btn = ttk.Button(scrollable_frame, text="Simulation aktualisieren",
                                command=update_simulation)
        update_btn.grid(row=row, column=3, columnspan=3, pady=20, ipady=10)
        
        style = ttk.Style()
        style.configure('TButton')
        
        canvas.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")
    
    def generate_solar_power(self, time_minutes, sun_days_Wh : np.ndarray):
        """
        Generate solar power per-minute array (W). Each day will sum to:
            sum_{minutes in day} solar_power = sun_days_Wh[day] * num_solar_cells * 60
        (Because later we use solar_energy_min = solar_power / 60 -> Wh per minute.)
        """
        minutes_per_day = 24 * 60
        total_minutes = len(time_minutes)
        days = int(total_minutes / minutes_per_day)
        # Build one-day normalized Gaussian curve (minute resolution)
        one_day_minutes = np.arange(0, minutes_per_day)
        # center at 14:00 -> 14*60 minutes / 60 = 14 hours; we'll use hours for Gaussian
        one_day_hours = one_day_minutes / 60.0
        mu = 14.0
        sigma = 2.0  # hours
        gauss = np.exp(-0.5 * ((one_day_hours - mu) / sigma) ** 2)
        # zero outside 6..22 hours
        mask_daylight = (one_day_hours >= 6) & (one_day_hours <= 22)
        gauss *= mask_daylight.astype(float)
        s = np.sum(gauss)
        if s <= 0:
            # fallback flat distribution between 6 and 22
            gauss = mask_daylight.astype(float)
            s = np.sum(gauss)
        # normalize so sum(gauss) == 1
        gauss = gauss / np.sum(gauss)

        # Prepare full-length array by tiling the normalized day and scaling per day's Wh
        solar_curve = np.zeros(total_minutes)
        # Expand sun_days_Wh to at least 'days' length
        if len(sun_days_Wh) < days:
            repeats = int(np.ceil(days / len(sun_days_Wh)))
            sun_days = np.tile(sun_days_Wh, repeats)[:days]
        else:
            sun_days = sun_days_Wh[:days]

        for d in range(days):
            day_start = d * minutes_per_day
            day_end = day_start + minutes_per_day
            # desired per-day total in "W-minutes" is Wh * 60
            target_w_min = sun_days[d] * self.params.get('num_solar_cells', 1) * 60.0
            solar_curve[day_start:day_end] = gauss * target_w_min

        return solar_curve
    
    def simulate_system(self):
        """Simulate the solar system over multiple days"""
        days = int(self.params['simulation_days'])
        dt_minutes = 1  # 1-minute resolution
        total_minutes = days * 24 * 60
        
        # Time arrays
        time_minutes = np.arange(0, total_minutes, dt_minutes)
        time_hours = time_minutes / 60.0
        
        # Initialize arrays
        battery_energy = np.zeros(len(time_minutes))
        solar_power = np.zeros(len(time_minutes))
        power_consumption = np.zeros(len(time_minutes))
        system_state = np.zeros(len(time_minutes), dtype=int)  # 0=sleep, 1=measure, 2=send
        energy_balance = np.zeros(len(time_minutes))
        
        # Calculate cycle timing
        cycles_per_day = int(self.params['cycles_per_day'])
        cycle_interval_minutes = (24 * 60) / cycles_per_day if cycles_per_day > 0 else 24 * 60
        
        # Generate solar power profile
        solar_power = self.generate_solar_power(time_minutes, np.array(self.sun_days_Wh))
        
        # Initialize battery
        total_battery_capacity = float(self.params['battery_capacity_Wh']) * float(self.params['num_batteries'])
        current_battery = total_battery_capacity * 0.5  # Start at 50% charge
        
        # Simulate each minute
        for i, t_min in enumerate(time_minutes):
            day_minute = t_min % (24 * 60)  # Minute within the day
            
            # Determine system state based on cycles
            if cycles_per_day > 0:
                cycle_position = (day_minute % cycle_interval_minutes)
                measuring_time_min = float(self.params['measuring_time_s']) / 60.0
                sending_time_min = float(self.params['sending_time_s']) / 60.0
                
                if cycle_position < measuring_time_min:
                    system_state[i] = 1  # Measuring
                    power_consumption[i] = float(self.params['measuring_power_W'])
                elif cycle_position < measuring_time_min + sending_time_min:
                    system_state[i] = 2  # Sending
                    power_consumption[i] = float(self.params['sending_power_W'])
                else:
                    system_state[i] = 0  # Sleep
                    power_consumption[i] = float(self.params['sleep_power_W'])
            else:
                system_state[i] = 0  # Always sleep if no cycles
                power_consumption[i] = float(self.params['sleep_power_W'])
            
            # Calculate energy balance for this minute
            solar_energy_min = solar_power[i] / 60.0  # W -> Wh for 1 minute
            consumed_energy_min = power_consumption[i] / 60.0  # W -> Wh for 1 minute
            
            # Update battery
            net_energy = solar_energy_min - consumed_energy_min
            current_battery += net_energy
            
            # Clamp battery to valid range
            if current_battery > total_battery_capacity:
                current_battery = total_battery_capacity  # Excess energy is wasted
            elif current_battery < 0:
                current_battery = 0  # System shuts down or draws from grid
            
            battery_energy[i] = current_battery
            energy_balance[i] = net_energy
        
        return {
            'time_minutes': time_minutes,
            'time_hours': time_hours,
            'battery_energy': battery_energy,
            'solar_power': solar_power,
            'power_consumption': power_consumption,
            'system_state': system_state,
            'energy_balance': energy_balance,
            'total_battery_capacity': total_battery_capacity
        }
    
    def simulate_and_plot(self):
        """Run simulation and update all plots"""
        print("Updating simulation...")  # Debug feedback
        data = self.simulate_system()
        
        # Clear previous plots
        self.ax1.clear()
        self.ax2.clear()
        self.ax3.clear()
        self.ax4.clear()

        # Convert time to hours for plotting
        time_hours = data['time_hours']
        days_to_show = min(int(self.params['simulation_days']), 10)  # show up to 10 days
        start_day = int(self.params["start_day"])
        start_hour = start_day * 24
        end_hour = days_to_show * 24 + start_hour
        mask = (time_hours >= start_hour) & (time_hours <= end_hour)
        time_plot = time_hours[mask]

        # Plot 1: Battery Energy Level (show over- and underproduction here)
        self.ax1.plot(time_plot, data['battery_energy'][mask], 'b-', linewidth=2, label='Batterieenergie')
        self.ax1.axhline(y=data['total_battery_capacity'], color='r', linestyle='--',
                         alpha=0.7, label=f'Batteriekapazität ({data["total_battery_capacity"]:.1f} Wh)')
        self.ax1.axhline(y=0, color='k', linestyle='--', alpha=0.5)

        # Underproduction: battery at (near) 0
        underproduction_mask = data['battery_energy'][mask] <= 0.01
        if np.any(underproduction_mask):
            self.ax1.fill_between(time_plot, 0, data['total_battery_capacity'],
                                    where=underproduction_mask, alpha=0.25, color='red',
                                    label='Unterproduktion (Batterie leer)')

        # Overproduction: solar > consumption while battery is (nearly) full -> wasted energy
        overproduction_mask = (data['solar_power'][mask] > data['power_consumption'][mask]) & \
                              (data['battery_energy'][mask] >= data['total_battery_capacity'] * 0.99)

        if np.any(overproduction_mask):
            self.ax1.fill_between(time_plot, 0, data['total_battery_capacity'],
                                  where=overproduction_mask, alpha=0.25, color='green',
                                  label='Überproduktion (Batterie voll)')

        self.ax1.set_ylabel('Energie (Wh)', fontsize=12)
        self.ax1.set_xlabel('Zeit (Stunden)', fontsize=12)
        self.ax1.set_title('Batterie-Ladezustand', fontsize=14, fontweight='bold')
        self.ax1.legend(loc='upper right')
        self.ax1.grid(True, alpha=0.3)

        # Plot 2: Solar Power (no overproduction shading here)
        self.ax2.plot(time_plot, data['solar_power'][mask], color='orange', linewidth=1, label='Solarleistung (W)')
        self.ax2.fill_between(time_plot, 0, data["solar_power"][mask], color="orange", alpha=0.25)
        self.ax2.set_ylabel('Solarleistung (W)', fontsize=12)
        self.ax2.set_title('Solarertrag / Energieverbrauch', fontsize=14, fontweight='bold')
        self.ax2.set_xlabel('Zeit (Stunden)', fontsize=12)
        self.ax2.grid(True, alpha=0.3)

        # Plot 3: Power Consumption (separate window)
        self.ax3.set_ylabel('Energieverbrauch (W)', fontsize=12)
        self.ax3.plot(time_plot, data['power_consumption'][mask], color='red', linewidth=.75, linestyle='--',
                      label='Energieverbrauch (W)')
        self.ax3.yaxis.set_label_position("right")
        self.ax3.fill_between(time_plot, 0, data["power_consumption"][mask], color="red", alpha=0.25)

        lines2, labels2 = self.ax2.get_legend_handles_labels()
        lines3, labels3 = self.ax3.get_legend_handles_labels()
        # combine legends if any
        if labels2 or labels3:
            self.ax3.legend(lines2 + lines3, labels2 + labels3, loc="upper right")

        self.fig2.tight_layout()
        self.fig3.tight_layout()

        # Plot 4: System State and Energy Balance
        state_colors = ['lightblue', 'lightgreen', 'lightcoral']
        state_labels = ['Ruhezustand', 'Messen', 'Senden']
        for state in range(3):
            state_mask = (data['system_state'][mask] == state)
            if np.any(state_mask):
                max_val = np.max(np.abs(data['energy_balance'][mask] * 60)) * 1.1
                if max_val == 0:
                    max_val = 1
                self.ax4.fill_between(time_plot, -max_val, max_val, where=state_mask,
                                      alpha=0.3, color=state_colors[state],
                                      label=f'{state_labels[state]}')
        self.ax4.plot(time_plot, data['energy_balance'][mask] * 60, 'purple', linewidth=2,
                      label='Leistungsbilanz (W)')
        self.ax4.axhline(y=0, color='k', linestyle='--', alpha=0.7, linewidth=1)
        self.ax4.set_ylabel('Leistung (W)', fontsize=12)
        self.ax4.set_xlabel('Zeit (Stunden)', fontsize=12)
        self.ax4.set_title('Systemzustand und Leistungsbilanz', fontsize=14, fontweight='bold')
        self.ax4.legend(loc='upper right')
        self.ax4.grid(True, alpha=0.3)

        # Set x-axis formatting for all plots
        axes = [(self.ax1, self.fig1), (self.ax2, self.fig2), (self.ax3, self.fig3), (self.ax4, self.fig4)]
        for ax, fig in axes:
            ax.set_xlim(start_hour, end_hour)
            for day in range(days_to_show + 1):
                ax.axvline(x=day * 24 + start_hour, color='gray', linestyle=':', alpha=0.5)
            ticks = []
            labels = []
            for day in range(days_to_show + 1):
                for hour in [0, 12]:
                    if day * 24 + start_hour + hour <= end_hour:
                        ticks.append(day * 24 + start_hour + hour)
                        if hour == 0:
                            labels.append(f'Tag {day + start_day}\n')
                        else:
                            labels.append('12h')
            ax.set_xticks(ticks)
            ax.set_xticklabels(labels, rotation=0, ha='center')

        # Refresh all plots
        try:
            self.fig1.canvas.draw_idle()
            self.fig2.canvas.draw_idle()
            self.fig3.canvas.draw_idle()
            self.fig4.canvas.draw_idle()
        except Exception:
            # fallback draw
            self.fig1.canvas.draw()
            self.fig2.canvas.draw()
            self.fig3.canvas.draw()
            self.fig4.canvas.draw()

        print(f"Simulation updated for {days_to_show} starting day {start_day} days")  # Debug feedback
    
    def show(self):
        """Display the interactive plot and control windows"""
        plt.show(block=False)  # Non-blocking show for matplotlib
        self.control_window.mainloop()  # This will block until control window is closed

# Create and run the simulation
if __name__ == "__main__":
    simulator = SolarSystemSimulator()
    simulator.show()
