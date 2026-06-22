# Claude-LED 🚦

<div align="center">
  <img width="300" height="236" style="display: block; margin: 0 auto;" alt="aiLED" src="https://github.com/user-attachments/assets/ff76e92f-a932-449c-ae15-31b27944fd02" />
</div>

[中文](README_CN.md)

[![Arduino](https://img.shields.io/badge/Arduino-ESP32C3-00979D)](https://www.arduino.cc/)
[![BLE](https://img.shields.io/badge/BLE-Bluetooth%205.0-blue)](https://www.bluetooth.com/)
[![Python](https://img.shields.io/badge/Python-3.8+-3776AB)](https://www.python.org/)
[![Windows](https://img.shields.io/badge/Platform-Windows-0078d4)](https://www.microsoft.com/windows)
[![License](https://img.shields.io/badge/license-MIT-blue)](LICENSE)
[![GitHub Stars](https://img.shields.io/github/stars/ChenDaqian/claude-signal-light?style=flat)](https://github.com/ChenDaqian/claude-signal-light/stargazers)

ESP32C3 Bluetooth BLE status indicator light for Claude Code workflow notifications.

## Hardware List

| Component | Model / Spec |
|-----------|-------------|
| Board | ESP32C3-Super-Mini |
| LED Board | Tri-color LED (common cathode, Red/Yellow/Green) |
| Buzzer | Active buzzer (active-low trigger) |

## Wiring Definition

| Component | Pin | ESP32C3 GPIO | Board Position |
|-----------|-----|--------------|----------------|
| LED Red | R | GPIO5 | Right side, 1st pin |
| LED Yellow | Y | GPIO6 | Right side, 2nd pin |
| LED Green | G | GPIO7 | Right side, 3rd pin |
| LED Ground | GND | GND | Left side, 2nd pin |
| Buzzer (+) | + | GPIO4 | Left side, 4th pin |
| Buzzer (-) | - | GND | Left side, 2nd pin |

> Note: GPIO8 is the onboard blue LED. GPIO0-GPIO3 are reserved for ADC/SPI — do not use.

## LED Brightness Configuration

The firmware defines maximum brightness constants (0-255) for each color, adjustable as needed:

```cpp
#define BRIGHTNESS_RED    255
#define BRIGHTNESS_YELLOW 255
#define BRIGHTNESS_GREEN  192
```

After modification, re-flash the firmware for global effect.

## LED Effects (20 Modes)

| Mode | Name | Description |
|------|------|-------------|
| 0 | Off | All LEDs off |
| 1 | Both Flash | R/Y/G flash simultaneously, 500ms cycle |
| 2 | Green Flash | Green LED flashes, 500ms cycle |
| 3 | Red Flash | Red LED flashes, 500ms cycle |
| 4 | Yellow Flash | Yellow LED flashes, 500ms cycle |
| 5 | Green On | Green LED steady on |
| 6 | Red On | Red LED steady on |
| 7 | Yellow On | Yellow LED steady on |
| 8 | Both On | R/Y/G all steady on |
| 9 | Police Alt | Red/Green alternating 300ms + Yellow independent flash 150ms |
| 10 | Heartbeat | 80ms on → 100ms off → 80ms on → 600ms off |
| 11 | SOS Morse | Three short, three long, three short |
| 12 | Breathing | Red/Green reverse gradient + Yellow 120° phase offset |
| 13 | Firefly | Three independent sine waves with different periods |
| 14 | ECG | Red ECG waveform, Green QRS spike, Yellow P/T wave |
| 15 | Phase Chase | Green sin, Red cos, Yellow sin+120°, 3s cycle |
| 16 | Strobe Chase | Green → Yellow → Red, 3 flashes each, 80ms on/80ms off |
| 17 | Taichi | Red/Green sin³ inverse growth, Yellow independent 120° offset |
| 18 | HELLO Morse | Continuous HELLO morse code broadcast |
| 19 | Radar | 3s Green sine scan + Yellow echo → 1s Red lock flash → 0.5s all on |

## Bluetooth Communication

- Device Name: `Claude-LED-LUCKEY`
- Service UUID: `12345678-1234-1234-1234-123456789abc`
- Characteristic UUID: `12345678-1234-1234-1234-123456789abd`
- Write Type: Write Without Response
- Data Format: UTF-8 string

### Supported Commands

#### Command Format

```
mode[,buzzer_duration_ms]
```

- `mode`: Mode number (0-19) or name
- `buzzer_duration_ms`: Buzzer duration in milliseconds, `0`=off (default `0`)

Examples:
- `"6,200"` → Mode 6 (Red On) + buzzer 200ms
- `"alarm,20"` → Red on + short beep
- `"alarm,0"` → Red on + no buzzer
- `"alarm"` → Red on + no buzzer
- `"2"` → Mode 2 (Green Flash), buzzer off

#### Numeric Commands (0-19)

Send the mode number directly, e.g. `"0"`, `"17"`.

#### Name Commands

| Command | Equivalent Mode |
|---------|----------------|
| off / close / idle | 0 |
| thinking / think | 2 |
| busy / executing | 7 |
| success / ok / done | 5 |
| error / failed | 3 |
| alarm / permission | 6 |
| green / blink | 2 |
| taichi / running / run | 17 |
| bothoff | 0 |
| bothflash | 1 |
| greenflash | 2 |
| redflash | 3 |
| yellowflash | 4 |
| greenon | 5 |
| redon | 6 |
| yellowon | 7 |
| bothon | 8 |
| police | 9 |
| heartbeat | 10 |
| sos | 11 |
| breathing | 12 |
| firefly | 13 |
| ecg | 14 |
| phasechase / chase | 15 |
| strobe / strobechase | 16 |
| hello / hellomorse | 18 |
| radar | 19 |

## Usage

### 1. Flash Firmware

Open `firmware/claude-led/claude-led.ino` in Arduino IDE, select ESP32C3 board, and upload.

### 2. Test Effects

```bash
# Auto-test all effects (3 seconds each)
python scripts/test-all-effects.py

# Test a single effect manually
python scripts/send-hook.py 17        # Taichi breathing
python scripts/send-hook.py taichi    # Same as above
python scripts/send-hook.py 6 200     # Red on + buzzer 200ms
python scripts/send-hook.py alarm 20  # Red on + short beep
```

### 3. Claude Code Integration

Add hooks in `~/.claude/settings.json`:

```json
{
  "hooks": {
    "SessionStart": [
      {
        "matcher": ".*",
        "hooks": [
          {
            "type": "command",
            "command": "python \"D:\\path\\to\\send-hook.py\" 17",
            "timeout": 5000
          }
        ]
      }
    ],
    "PreToolUse": [
      {
        "matcher": ".*",
        "hooks": [
          {
            "type": "command",
            "command": "python \"D:\\path\\to\\send-hook.py\" 2",
            "timeout": 5000
          }
        ]
      }
    ],
    "PermissionRequest": [
      {
        "matcher": ".*",
        "hooks": [
          {
            "type": "command",
            "command": "python \"D:\\path\\to\\send-hook.py\" 4 200",
            "timeout": 5000
          }
        ]
      }
    ]
  }
}
```

### Claude Code Event to LED Effect Mapping

| Hook Event | Param | LED Effect | Buzzer |
|------------|-------|------------|--------|
| SessionStart | 17 | Taichi breathing | Off |
| UserPromptSubmit | 2 | Green flash | Off |
| PreToolUse | 2 | Green flash | Off |
| PostToolUseFailure | 3 | Red flash | Off |
| PermissionRequest | 4 200 | Yellow flash | 200ms |
| Stop | 5 | Green on | Off |
| SessionEnd | 17 | Taichi breathing | Off |

## Dependencies

- Arduino IDE + ESP32 board support
- Python 3.8+
- bleak (Python BLE library): `pip install bleak`


## Contributors

<a href="https://github.com/Chendaqian/claude-signal-light/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=Chendaqian/claude-signal-light" />
</a>

## Star History

<a href="https://www.star-history.com/?repos=Chendaqian%2Fclaude-signal-light&type=date&legend=top-left">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/chart?repos=Chendaqian/claude-signal-light&type=date&theme=dark&legend=top-left" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/chart?repos=Chendaqian/claude-signal-light&type=date&legend=top-left" />
   <img alt="Star History Chart" src="https://api.star-history.com/chart?repos=Chendaqian/claude-signal-light&type=date&legend=top-left" />
 </picture>
</a>
