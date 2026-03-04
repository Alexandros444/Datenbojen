import time, datetime


class state:
    cycles_per_day = 10
    battery_max_capacity_Wh = 30
    battery_current_Energy_kJ = 50
    num_batteries=1
    num_solar_cells=1
    sleep_power_mA=20
    measuring_power_mA=100
    sending_power_mA=150
    measuring_time_s=60
    sending_time_s=60
    solar_spring_Wh=1
    solar_summer_Wh=1
    solar_autumn_Wh=1
    solar_winter_Wh=1
    day = 0
    houres = 0
    minutes = 0

    def sim_step(self) -> "state":
        
        
        
        



