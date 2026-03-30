# 🌡️ Smart Thermostat — Automatic Indoor Comfort Control

A Raspberry Pi-based system for **automatic temperature regulation and humidity monitoring** inside a room, with a remote web interface accessible from any device on the network.

---

## 📋 Overview

This project implements a closed-loop automatic control system that:
- Reads **temperature and humidity** in real time via a DHT22 sensor
- Compares the current temperature to a **user-defined setpoint**
- Activates a **heating or cooling device** (simulated via red/blue LEDs) to maintain the desired comfort range
- Exposes a **web UI** (served by NGINX + PHP-FPM on the Pi) for remote monitoring and control
- Logs all readings to **CSV files** (daily & monthly) accessible online

---

## 🛠️ Hardware

| Component | Role |
|---|---|
| Raspberry Pi (any model with GPIO) | Main controller |
| DHT22 / AM2301 sensor | Temperature & humidity sensing |
| GPIO T-Board + Breadboard + Jumpers | Prototyping connections |
| Red LED | Simulates heating device (heat pump / boiler) |
| Blue LED | Simulates cooling device (AC / fan) |

---

## 💻 Software Stack

| Layer | Technology |
|---|---|
| Sensor driver | C + `DHTXXD.h` library |
| Control algorithm | C (reads `reqtemp` file, drives GPIO) |
| Web server | NGINX |
| Backend | PHP-FPM |
| Frontend | HTML + JavaScript (Gauge widget) |
| Data logging | CSV (daily logs + monthly averages) |

---

## ⚙️ How It Works

### 1. Sensor Reading
The DHT22 sends 40-bit packets over a single GPIO data line every 40 µs:

```
Byte 1: Humidity (integer)
Byte 2: Humidity (decimal)
Byte 3: Temperature (integer)
Byte 4: Temperature (decimal)
Byte 5: Checksum
```

The C program decodes these packets using `DHTXXD.h` and writes the values to `data.json`.

### 2. Control Logic
- The user's desired temperature is stored in a `reqtemp` file
- A **±0.5°C dead band** prevents oscillation around the setpoint
- Outside the dead band → red LED on (heating) or blue LED on (cooling)

### 3. Web Interface
- A JavaScript script polls `data.json` every **3 seconds** to update the live display
- A colour-coded **gauge widget** shows thermal comfort level at a glance
- Users set a new target temperature via an input box; PHP writes it to `reqtemp`
- Daily CSV logs are served from the `/dailylogs/` folder on the web server

---

## 🖥️ UI Features

- 🌡️ Live temperature & humidity display
- 🎨 Dynamic gauge with comfort-indicating colours
- 🎯 Remote setpoint input (from any browser on the network)
- 📊 Downloadable daily/monthly CSV temperature history

---

## 🚀 Setup

### Prerequisites
```bash
sudo apt update
sudo apt install nginx php-fpm gcc git
```

### Clone & Build
```bash
git clone https://github.com/bosne28/thermostatPi.git
cd smart-thermostat-pi
gcc thermostat.c -o thermostat -lDHTXXD
```

### Configure NGINX
Point the web root to the project's `www/` folder and enable PHP-FPM — see `nginx.conf` in the repo for the full configuration.

### Run
```bash
# Single read (debug)
./thermostat

# Continuous read every 5 seconds
./thermostat 5
```

---

## 📁 Project Structure

```
smart-thermostat-pi/
├── thermostat.c       # Sensor reading + control algorithm
├── www/
│   ├── index.php      # Web UI
│   ├── settemp.php    # Handles setpoint input
│   ├── data.json      # Live sensor data (updated by C program)
│   └── dailylogs/     # CSV temperature history (served online)
├── nginx.conf         # NGINX configuration example
└── README.md
```

---

## 📚 References

- [Raspberry Pi Foundation](https://www.raspberrypi.com/)
- [DHTXXD Library](https://github.com/nethoncho)
- [NGINX Documentation](https://www.theserverside.com/)

---

## 📄 License

MIT License — see [LICENSE](LICENSE) for details.
