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
            # 'solar_Wh': 1,
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
        self.fig1.canvas.manager.set_window_title('Batterie-Ladezustand')
        plt.subplots_adjust(left=0.1, bottom=0.15, right=0.95, top=0.9)

        # Solar Power Plot (solar only, no overproduction shading here)
        self.fig2, self.ax2 = plt.subplots(figsize=(12, 6))
        self.fig2.canvas.manager.set_window_title('Solarertrag / Energieverbrauch')
        plt.subplots_adjust(left=0.1, bottom=0.15, right=0.95, top=0.9)

        # Power Consumption Plot (separate window)
        self.fig3, self.ax3 = plt.subplots(figsize=(12, 6))
        self.fig3.canvas.manager.set_window_title('Energieverbrauch')
        plt.subplots_adjust(left=0.1, bottom=0.15, right=0.95, top=0.9)

        self.ax3 = self.ax2.twinx()

        # System State and Energy Balance Plot (kept as a separate window)
        self.fig4, self.ax4 = plt.subplots(figsize=(12, 6))
        self.fig4.canvas.manager.set_window_title('Systemzustand und Leistungsbilanz')
        plt.subplots_adjust(left=0.1, bottom=0.15, right=0.95, top=0.9)
    
    def setup_control_window(self):
        """Create a separate Tkinter window for parameter controls"""
        self.control_window = tk.Tk()
        self.control_window.title("Solarsimulation: Parameter")
        self.control_window.geometry("800x750")
        
        # Create scrollable frame
        canvas = tk.Canvas(self.control_window)
        scrollbar = ttk.Scrollbar(self.control_window, orient="vertical", command=canvas.yview)
        scrollable_frame = ttk.Frame(canvas)
        
        # keep a reference to the window item so we can force the inner frame width
        def _on_scrollable_config(e):
            canvas.configure(scrollregion=canvas.bbox("all"))
        scrollable_frame.bind("<Configure>", _on_scrollable_config)     
        inner_window = canvas.create_window((0, 0), window=scrollable_frame, anchor="nw")
        # make the inner frame match the canvas width so children can expand
        def _on_canvas_config(event):
            canvas.itemconfig(inner_window, width=event.width)
        canvas.bind("<Configure>", _on_canvas_config)
        canvas.configure(yscrollcommand=scrollbar.set)
        # ensure at least the first column can expand horizontally
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
            # ('solar_Wh', 0.1, 10.0, 0.1, False),
            ('simulation_days', 1, 100, 1, True),
            ('start_day', 0, 99, 1, True)
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
            value_label = ttk.Label(scrollable_frame, textvariable=value_var, width=10)
            value_label.grid(row=row, column=1, padx=10, pady=8)

            # Special handling for start_day: replace slider with Prev/Next ±5 buttons
            if param == 'start_day':
                # store a small record so other code can still find value_var
                self.sliders[param] = {
                    'var': None,
                    'value_var': value_var,
                    'slider': None,
                    'is_integer': is_integer
                }

                def change_start(delta):
                    # clamp start_day between 0 and simulation_days-1
                    max_start = max(0, int(self.params.get('simulation_days', 1)) - 10)
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
                             variable=slider_var, orient='horizontal', length=250)
            slider.grid(row=row, column=2, padx=10, pady=8)

            # Store references and bind update (only for display, not simulation)
            self.sliders[param] = {
                'var': slider_var,
                'value_var': value_var,
                'slider': slider,
                'is_integer': is_integer
            }

            # Bind update function (only updates display, not simulation)
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

            slider_var.trace('w', make_display_update_func(param))
            row += 1

        # sun_days_label = ttk.Label(scrollable_frame, text="Sun Days Wh (per day)", font=("Arial", 12, "bold"))
        # sun_days_label.grid(row=row, column=0, columnspan=3, pady=(20, 5))
        # row += 1

        self.sun_days_Wh_entries = []
        self.sun_days_Wh : list[float] = [1]  # Start with one day

        # --- start: separate horizontal scrollable box for day entries ---
        sun_frame = ttk.LabelFrame(scrollable_frame, text="Solarverlauf - Wh pro Tag")
        sun_frame.grid(row=row, column=0, columnspan=8, sticky="ew", padx=5, pady=4)

        # allow the sun_canvas to expand horizontally (do not force small fixed width)
        sun_canvas = tk.Canvas(sun_frame, height=80)
        h_scroll = ttk.Scrollbar(sun_frame, orient="horizontal", command=sun_canvas.xview)
        sun_canvas.configure(xscrollcommand=h_scroll.set)

        # Inner frame that will hold the day entries side-by-side
        inner_days_frame = ttk.Frame(sun_canvas)
        inner_window = sun_canvas.create_window((0, 0), window=inner_days_frame, anchor="nw")

        def _on_inner_config(event):
            # update scrollregion to match inner frame size
            sun_canvas.configure(scrollregion=sun_canvas.bbox("all"))
        inner_days_frame.bind("<Configure>", _on_inner_config)

        # Optional: make wheel with Shift scroll horizontally
        def _on_mousewheel(event):
            if event.state & 0x0001:  # Shift pressed -> horizontal
                sun_canvas.xview_scroll(-1 * int(event.delta / 120), "units")
        sun_canvas.bind_all("<MouseWheel>", _on_mousewheel)

        sun_canvas.pack(fill="both", expand=True, side="top")
        h_scroll.pack(fill="x", side="bottom")
        # --- end horizontal box setup ---

        def update_sun_days_entries():
            # remove existing widgets in inner_days_frame
            for child in inner_days_frame.winfo_children():
                child.destroy()
            self.sun_days_Wh_entries.clear()

            # create one entry per day, evenly spaced horizontally
            for i, val in enumerate(self.sun_days_Wh):
                # container to allow labeling and consistent spacing
                cell = ttk.Frame(inner_days_frame, width=100, height=60)
                cell.grid_propagate(False)
                cell.grid(row=0, column=i, padx=8, pady=6)

                lbl = ttk.Label(cell, text=f"Tag {i}", anchor="center")
                lbl.pack(side="top", fill="x", padx=2)
                entry = ttk.Entry(cell, width=5, justify="center")
                entry.insert(0, str(val))
                entry.pack(side="top", pady=2)
                self.sun_days_Wh_entries.append(entry)

            # make sure canvas scrollregion is updated
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

        # Buttons placed below the LabelFrame (still inside the main scrollable_frame)
        btn_frame = ttk.Frame(scrollable_frame)
        btn_frame.grid(row=row + 1, column=0, columnspan=3, sticky="w", pady=(4, 10))

        add_btn = ttk.Button(btn_frame, text="+ Tag", command=add_day)
        remove_btn = ttk.Button(btn_frame, text="- Tag", command=remove_day)
        add_btn.pack(side="left", padx=(0, 6))
        remove_btn.pack(side="left")

        update_sun_days_entries()
        row += 2  # account for sun_frame + buttons

        def read_sun_day_values():
            new = []
            for entry in self.sun_days_Wh_entries:
                try:
                    v = float(entry.get())
                except Exception:
                    v = self.params.get('solar_Wh', 1.0)
                new.append(v)
            # ensure at least one day
            self.sun_days_Wh = new or [self.params.get('solar_Wh', 1.0)]

        def update_simulation():
            # Read sun_days_Wh from entries (fallback to slider solar_Wh if invalid)
            read_sun_day_values()
            self.simulate_and_plot()

        update_btn = ttk.Button(scrollable_frame, text="Simulation aktualisieren",
                                command=update_simulation)
        update_btn.grid(row=row, column=3, columnspan=3, pady=20, ipady=20)
        
        # Add style to button
        style = ttk.Style()
        style.configure('TButton')
        
        canvas.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")
    
    def generate_solar_power(self, time_minutes, sun_days_Wh : np.ndarray):
        """Generate solar power based on normal distribution between 6-22h"""
        # Solar generation follows normal distribution centered at 14h (2 PM)
        minutes_per_day = 24 * 60
        time_hours = time_minutes / 60
        daily_time = time_hours % 24
        u = 13
        v = 2
        solar_curve = 1 / np.sqrt(2*np.pi*v**2) * np.exp(- ((daily_time - u) ** 2) / (2*v**2)) # Why div by 60? Idk
        day_power = np.sum(solar_curve[slice(0, minutes_per_day)])
        assert np.round(day_power,2) == 60, f"Solar Curve must be total area 60 since we calculate from min to hours for one Day, {day_power=}"
        # for i in daily_time[slice(0, minutes_per_day)]:
        #     print(i)
        # print(len(time_hours), len(daily_time), len(solar_curve), minutes_per_day, day_power)
        # exit()
        # solar_curve = np.where((daily_time >= 6) & (daily_time <= 22), solar_curve, 0)

        assert len(sun_days_Wh), "sun Days Wh must not be empty"
        days : int = int(len(time_minutes) / minutes_per_day)
        # Pad Sun Days Wh repeating its own values
        if len(sun_days_Wh) < days:
            sun_days_Wh = np.tile(sun_days_Wh, int(days/len(sun_days_Wh)) + 1)
        for day in range(days):
            solar_curve[slice(day * minutes_per_day, day * minutes_per_day + minutes_per_day)] *= sun_days_Wh[day] * self.params["num_solar_cells"]

            solar_power_day_integral = np.round(np.sum(solar_curve[slice(day * minutes_per_day, day * minutes_per_day + minutes_per_day)]),2)
            solar_power_day_calc = sun_days_Wh[day] * self.params["num_solar_cells"] * 60
            assert solar_power_day_integral == solar_power_day_calc, f"Solar Power Missmatch {solar_power_day_integral} != {solar_power_day_calc}"
        # else:
        #     solar_curve = solar_curve * self.params['solar_Wh'] * self.params['num_solar_cells']

        #     # Integral über Watt Minuten deswegen mit Faktor 60 korrigieren auf Wattstundenden
        #     solar_power_day_integral = np.round(np.sum(solar_curve[slice(0, minutes_per_day)]),2)
        #     solar_power_day_calc = self.params["solar_Wh"] * self.params["num_solar_cells"] * 60
        #     assert solar_power_day_integral == solar_power_day_calc, f"Solar Power Missmatch {solar_power_day_integral} != {solar_power_day_calc}"



        # # Normalize for a single day
        # one_day = np.linspace(0, 24, minutes_per_day)
        # one_day_curve = np.exp(-0.5 * ((one_day - 14) / 4) ** 2)
        # one_day_curve = np.where((one_day >= 6) & (one_day <= 22), one_day_curve, 0)
        # daily_integral = np.trapezoid(one_day_curve, dx=1/60)
        # if daily_integral > 0:
        #     solar_curve = solar_curve * (self.params['solar_Wh'] * self.params['num_solar_cells']) / daily_integral
        # day_power = np.sum(solar_curve[slice(0, minutes_per_day)])
        # day_hours = time_hours[slice(0, minutes_per_day)]
        # print(day_hours, solar_curve)
        # assert day_power == self.params['solar_Wh'] * self.params['num_solar_cells'], f"Solar normal Distribution {day_power} != {self.params['solar_Wh'] * self.params['num_solar_cells']} Defined Power"
        return solar_curve
    
    def simulate_system(self):
        """Simulate the solar system over multiple days"""
        days = int(self.params['simulation_days'])
        dt_minutes = 1  # 1-minute resolution
        total_minutes = days * 24 * 60
        
        # Time arrays
        time_minutes = np.arange(0, total_minutes, dt_minutes)
        time_hours = time_minutes / 60
        
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
        # daily_time = time_hours % 24
        solar_power = self.generate_solar_power(time_minutes, np.array(self.sun_days_Wh))
        
        # Initialize battery
        total_battery_capacity = self.params['battery_capacity_Wh'] * self.params['num_batteries']
        current_battery = total_battery_capacity * 0.5  # Start at 50% charge
        
        # Simulate each minute
        for i, t_min in enumerate(time_minutes):
            day_minute = t_min % (24 * 60)  # Minute within the day
            
            # Determine system state based on cycles
            if cycles_per_day > 0:
                cycle_position = (day_minute % cycle_interval_minutes)
                measuring_time_min = self.params['measuring_time_s'] / 60
                sending_time_min = self.params['sending_time_s'] / 60
                
                if cycle_position < measuring_time_min:
                    system_state[i] = 1  # Measuring
                    power_consumption[i] = self.params['measuring_power_W']
                elif cycle_position < measuring_time_min + sending_time_min:
                    system_state[i] = 2  # Sending
                    power_consumption[i] = self.params['sending_power_W']
                else:
                    system_state[i] = 0  # Sleep
                    power_consumption[i] = self.params['sleep_power_W']
            else:
                system_state[i] = 0  # Always sleep if no cycles
                power_consumption[i] = self.params['sleep_power_W']
            
            # Calculate energy balance for this minute
            solar_energy_min = solar_power[i] / 60  # Convert W to Wh for 1 minute
            consumed_energy_min = power_consumption[i] / 60  # Convert W to Wh for 1 minute
            
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
            # highlight the unused capacity region above current battery up to capacity
            self.ax1.fill_between(time_plot, 0, data['total_battery_capacity'],
                                  where=overproduction_mask, alpha=0.25, color='green',
                                  label='Überproduktion (Betterie voll)')

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
        # self.ax2.legend(loc='upper right')
        self.ax2.grid(True, alpha=0.3)


        # Plot 3: Power Consumption (separate window)
        self.ax3.set_ylabel('Energieverbrauch (W)', fontsize=12)
        self.ax3.plot(time_plot, data['power_consumption'][mask], color='red', linewidth=.5, linestyle='--',
                      label='Energieverbrauch (W)')
        self.ax3.yaxis.set_label_position("right")
        self.ax3.fill_between(time_plot, 0, data["power_consumption"][mask], color="red", alpha=0.25)
        # self.ax3.set_xlabel('Zeit (Stunden)', fontsize=12)
        # self.ax3.set_title('Energieverbrauch', fontsize=14, fontweight='bold')
        # self.ax3.legend(loc='upper left')
        # self.ax3.grid(True, alpha=0.3)

        lines2, labels2 = self.ax2.get_legend_handles_labels()
        lines3, labels3 = self.ax3.get_legend_handles_labels()
        self.ax3.legend(lines2 + lines3, labels2 + labels3, loc="upper right")

        self.fig2.tight_layout()

        # Plot 4: System State and Energy Balance (kept similar to before)
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
        self.fig1.canvas.draw()
        self.fig2.canvas.draw()
        self.fig3.canvas.draw()
        self.fig4.canvas.draw()

        print(f"Simulation updated for {days_to_show} starting day {start_day} days")  # Debug feedback
    
    def show(self):
        """Display the interactive plot and control windows"""
        # Show all windows
        plt.show(block=False)  # Non-blocking show for matplotlib
        self.control_window.mainloop()  # This will block until control window is closed

# Create and run the simulation
if __name__ == "__main__":
    simulator = SolarSystemSimulator()
    simulator.show()