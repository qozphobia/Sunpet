# 🌞 SUN PET — Wio Terminal Virtual Pet

A virtual pet game for the **Seeed Studio Wio Terminal** powered entirely by light. No buttons needed — just shine a light on your device to feed and evolve your creature.

---

## 📋 Table of Contents

- [Overview](#overview)
- [Hardware Requirements](#hardware-requirements)
- [How It Works](#how-it-works)
- [Features](#features)
- [Evolution Stages](#evolution-stages)
- [Installation](#installation)
- [Project Structure](#project-structure)
- [Key Techniques](#key-techniques)
- [Possible Improvements](#possible-improvements)
- [License](#license)

---

## Overview

Sun Pet is a real-time interactive game inspired by classic Tamagotchi devices. The creature lives on the Wio Terminal's 320×240 TFT display and gains energy from ambient light detected by the built-in light sensor. Leave it in the dark and it starves — shine a lamp on it and watch it grow.

---

## Hardware Requirements

| Component | Details |
|-----------|---------|
| **Wio Terminal** | Seeed Studio (ATSAMD51, 120 MHz, 192 KB RAM) |
| **Light Sensor** | Built-in LDR on pin `WIO_LIGHT` (0–1023 ADC range) |
| **Display** | Built-in 2.4" TFT LCD, 320×240 px |

No external components required.

---

## How It Works

1. On startup, the device **calibrates** to ambient light levels and sets a dynamic threshold (`dynamicGoal = ambient + 120`).
2. The main loop reads the light sensor every ~100 ms using a **low-pass filter** to smooth out noise.
3. If light exceeds the threshold → the pet **charges** (gains energy + XP).
4. If light is below the threshold → the pet **starves** (loses energy and XP).
5. XP accumulates and drives **level-ups**, triggering visual evolution.

---

## Features

- 🌈 **6 visual evolution stages** based on level
- 💡 **Light sensor as the sole input** — no buttons needed
- 🫁 **Breathing animation** using procedural `sin()` oscillation
- ✨ **Glow/aura effect** when charging
- 📊 **Smooth energy bar** with linear interpolation (easing)
- 🚫 **Zero screen flickering** via double buffering (TFT_eSprite)
- 🎆 **Rainbow evolution animation** every 5 levels

---

## Evolution Stages

| Level | Color | Appearance |
|-------|-------|------------|
| 1–4 | Yellow | Happy default face |
| 5–9 | Orange | Slightly more intense |
| 10–14 | Cyan | Cool-toned |
| 15–19 | Purple | Mysterious |
| 20–49 | Magenta | Vibrant |
| 50+ | Red | Demon horns, aggressive face |

Every **5 levels**, a special rainbow pulse animation plays.

---

## Installation

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) (1.8.x or 2.x)
- [Seeed Arduino Boards](https://wiki.seeedstudio.com/Wio-Terminal-Getting-Started/#software) installed in Board Manager
- [TFT_eSPI library](https://github.com/Bodmer/TFT_eSPI) installed via Library Manager

### Steps

```bash
# 1. Clone the repository
git clone https://github.com/[your-username]/sunpet-wio-terminal.git

# 2. Open in Arduino IDE
#    File → Open → sunpet.ino

# 3. Select board
#    Tools → Board → Seeeduino Wio Terminal

# 4. Select port and upload
#    Tools → Port → (your COM/tty port)
#    Sketch → Upload
```

> **Note:** The TFT_eSPI library must be configured for Wio Terminal. The correct `User_Setup_Select.h` is typically included with the Seeed board package.

---

## Project Structure

```
sunpet-wio-terminal/
├── sunpet.ino          # Main source file (all game logic + rendering)
└── README.md           # This file
```

---

## Key Techniques

### Double Buffering (no flicker)
```cpp
petSpr.fillSprite(TFT_BLACK);   // Clear off-screen buffer
// ... draw everything to petSpr ...
petSpr.pushSprite(75, 15);      // Push complete frame to display
```

### Breathing Animation
```cpp
float breathe = sin(millis() / 400.0) * 3.0;
int radius    = 65 + (int)breathe;  // ±3 px oscillation
```

### Light Sensor Low-Pass Filter
```cpp
filteredLight = (filteredLight * 0.8) + (rawLight * 0.2);
```

### Energy Bar Easing
```cpp
displayEnergy += (energy - displayEnergy) * 0.15;
```

---

## Possible Improvements

- [ ] **Save state to Flash** — persist XP/level between power cycles
- [ ] **Sound effects** — use the built-in buzzer for level-up jingles
- [ ] **Button interactions** — manual feeding, mini-games via the 5 physical buttons
- [ ] **Wi-Fi leaderboard** — submit scores to a server and display global rankings
- [ ] **Additional sensors** — temperature/humidity affecting the pet's mood
- [ ] **More evolution forms** — unique body shapes per evolution stage

---

## License

MIT License — free to use, modify, and distribute.

---
