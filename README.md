# 🌡️ Smart Thermostat — Automatic Indoor Comfort Control

A Raspberry Pi-based system for **automatic temperature regulation and humidity monitoring** inside a room, with a remote web interface accessible from any device on the network.

---

## 📋 Overview

This project implements a closed-loop automatic control system that:
- Reads **temperature and humidity** in real time via a DHT22 sensor
- Compares the current temperature to a **user-defined setpoint**
- Activates a **heating or cooling device** (simulated via red/blue LEDs) to maintain the desired comfort range
- Exposes a **web UI** for remote monitoring and control from any browser
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
| Control algorithm | C (`test_DHTXXD.c`) |
| Web server | NGINX + PHP-FPM |
| Frontend | HTML + CSS + JavaScript |
| Data logging | CSV + JSON |

---

## ⚙️ How It Works

### 1. Sensor Reading
The DHT22 sends 40-bit packets over a single GPIO data line:

```
Byte 1: Humidity (integer)
Byte 2: Humidity (decimal)
Byte 3: Temperature (integer)
Byte 4: Temperature (decimal)
Byte 5: Checksum
```

The C program decodes these via `DHTXXD.h` and writes live values to `homify.local/data.json`.

### 2. Control Logic
- The user's desired temperature is stored in a `reqtemp` file on the server
- A **±0.5°C dead band** prevents oscillation around the setpoint
- Outside the dead band → red LED on (heating) or blue LED on (cooling)

### 3. Web Interface
- `printData.js` polls `data.json` every **3 seconds** to update the live display
- A colour-coded **gauge widget** shows thermal comfort level at a glance
- Users set a new target temperature via an input box; `index.php` writes it to `reqtemp`
- Daily/monthly CSV logs are served from the web root and can be downloaded directly

---

## 🖥️ UI Features

- 🌡️ Live temperature & humidity display
- 🎨 Dynamic gauge with comfort-indicating colours
- 🎯 Remote setpoint input from any device on the network
- 📊 Downloadable daily/monthly CSV temperature history (`daylog.csv`, `monthlog.csv`)

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
cd thermostatPi
gcc test_DHTXXD.c -o thermostat
```

### Run
```bash
# Single read (debug)
./thermostat

# Continuous read every N seconds
./thermostat 5
```

---

## 📁 Project Structure

```
thermostatPi/
├── test_DHTXXD.c       # Sensor reading + GPIO control algorithm
├── homify.local/
│   ├── index.html      # Main web UI
│   ├── index2.html     # Secondary UI page
│   ├── index.php       # Handles setpoint input (writes reqtemp)
│   ├── printData.js    # Live data polling & gauge update
│   ├── style.css       # Styles
│   ├── styles.css      # Styles
│   ├── data.json       # Live sensor output (updated by C program)
│   ├── historylog.json # History data (JSON format)
│   ├── daylog.csv      # Current day temperature log
│   ├── monthlog.csv    # Monthly temperature averages
│   ├── bar.png         # UI assets
│   ├── hum.png         # UI assets
│   └── favicon.png     # Favicon
├── historylog.csv      # Full history log (CSV)
├── historylog2.csv     # Secondary history log
├── monthlog.csv        # Monthly log (root copy)
└── README.md
```

---

## 📚 References

- [Raspberry Pi Foundation](https://www.raspberrypi.com/)
- [DHTXXD Library](https://github.com/nethoncho)

---

## 📄 License

MIT License — see [LICENSE](LICENSE) for details.
