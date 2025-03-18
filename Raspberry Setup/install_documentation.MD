# Raspberry Setup
## General
- Usr: riversync
- pwd: 55555

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
- Forwarded http an localhost 8000 (icecast)
- Forward tcp an 22 (SSH)


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


---
---

## Nginx - Lokaler Webserver (obsolete)
- Den nginx server kann man eig, weglassen, ngrok kann direkt auf den webcam Stream oder icecast2 linken
- Current Location: 
    - `/etc/nginx/sites-available/node`
- Usr: pi
- pwd: raspberry