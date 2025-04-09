# Raspberry Setup
## General
- Usr: riversync
- pwd: 55555spreeart$
- [Backup] (https://raspberrystreet.com/learn/how-to-backup-raspberrypi-sdcard)

## Motion - Kamerastream auf Webserver
- [Tutorial](https://www.instructables.com/How-to-Make-Raspberry-Pi-Webcam-Server-and-Stream-/)
- Deamon Mode off! Wegen Systemd
- Current Locations:
    - `/etc/motion/motion.conf`
    - `/var/log/motion/motion.log`
    - `/var/lib/motion/`
- Camera Webstream 8080
- Camera Config 8081

## Ngrok - Erreichbarkeit vom Internet
- [Tutorial auch Nginx](https://medium.com/@herman.daniel/raspberry-pi-secured-webcam-streaming-with-motion-nginx-and-ngrok-eb31c46ecedd)
- Config wo? `ngrok config check`
- Current Locations:
    - `/home/riversync/.config/ngrok/ngrok.yml`
- Website: [https://partly-suitable-parrot.ngrok-free.app/](https://partly-suitable-parrot.ngrok-free.app/)
- SSH login über TCP [Ngrok](https://dashboard.ngrok.com/endpoints?sortBy=updatedAt&orderBy=desc)
- Forward tcp an 22 (SSH)
- Forwarded jetzt http an 80 also nginx
- old
    - Forwarded http an localhost 8000 (icecast)


## Icecast - Webserver (Audio)
- Relay Camera Webserver
- Current Location:
    - `/etc/icecast2/icecast.xml`
- Usr: admin
- pwd: 55555
- Relay 8000 -> 8081 Cam Stream
    - Relay in icecast.xml auf 8081 port von Camserver
- Host Webserver on 8000


## Darkice - Audio Stream
- [Tutorial auch Icecast](https://www.hackster.io/Shilleh/stream-audio-from-raspberry-pi-to-local-computer-02c7f0)
- Current Location:
    - `/etc/darkice.cfg`
- Automatic Execution rc-local service:
    - `/etc/rc.local`
- Send Data to Icecast in config


## MJPG-Streamer
- [Tutorial](https://github.com/TechsPassion/RaspberryPi/blob/main/Webcam_Streaming.md)
- [Github](https://github.com/jacksonliam/mjpg-streamer/tree/master)
- Location mjpg_config
    - `/etc/default/mjpg_streamer`
- Location mjpg_stream
    - `/usr/local/bin/mjpg-streamer/mjpg-streamer-experimental/`
    - scripts für die Service Datei (jetzt unter `/etc/systemd/system/mjpg-streamer.service`)
    - www für Webserver
- System Service hosted Website
- auf port 8888



## Nginx - Lokaler Webserver
- Current Location: 
    - `/etc/nginx/sites-available/node`
- Login No in Use 
    - Usr: pi
    - pwd: raspberry
- Linkt von http (80) auf 8888 bzw 8888?actioon=stream für später wenn man die config nicht mehr von außen erreichen soll


## Control.sh
- Easy managable config
- has to be updated for the mjpg-server