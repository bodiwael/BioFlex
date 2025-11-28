# BioFlex - EMG & GSR Biosignal Monitoring System

A real-time biosignal acquisition and haptic feedback system built on ESP32, featuring EMG (electromyography) and GSR (galvanic skin response) monitoring with cloud integration and vibration motor feedback.

## Features

- **Real-time EMG Signal Processing**: Captures, filters, and processes electromyography signals at 500Hz
- **GSR Monitoring**: Galvanic skin response measurement for stress/arousal detection
- **Haptic Feedback**: Dual vibration motors provide real-time tactile feedback based on muscle activity
- **Cloud Integration**: Automatic data upload to Firebase Realtime Database
- **Signal Processing**: Raw, filtered, and envelope EMG data processing using CheezsEMG library
- **WiFi Connectivity**: Seamless wireless data transmission

## Hardware Requirements

### Core Components
- **ESP32 Development Board** (with WiFi support)
- **EMG Sensor Module** (with detect and signal output)
- **GSR Sensor Module**
- **2x Vibration Motors** (coin/pancake type, 3V-5V)
- **Motor Driver Circuit** (transistors or MOSFETs)
- **Power Supply** (3.3V/5V depending on motors)

### Pin Configuration

| Component | ESP32 Pin | Notes |
|-----------|-----------|-------|
| EMG Signal | GPIO 34 | ADC1_CH6 (analog input) |
| EMG Detect | GPIO 2 | Digital input |
| GSR Sensor | GPIO 36 | ADC1_CH0 (analog input) |
| Vibration Motor 1 | GPIO 25 | PWM capable |
| Vibration Motor 2 | GPIO 26 | PWM capable |

**Important**: All analog pins use ADC1 for WiFi compatibility. Do not use ADC2 pins when WiFi is active.

## Software Requirements

### Arduino IDE Setup
1. Install [Arduino IDE](https://www.arduino.cc/en/software) (v1.8.x or v2.x)
2. Add ESP32 board support:
   - Go to **File → Preferences**
   - Add to "Additional Board Manager URLs":
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Go to **Tools → Board → Board Manager**
   - Search for "esp32" and install "ESP32 by Espressif Systems"

### Required Libraries
Install via Arduino Library Manager (**Sketch → Include Library → Manage Libraries**):

1. **Firebase ESP32 Client** by Mobizt (v4.x.x)
2. **CheezsEMG** - Custom EMG processing library (see below)

#### CheezsEMG Library Installation
If not available in Library Manager, obtain the library files and place in your Arduino libraries folder:
- Windows: `Documents\Arduino\libraries\CheezsEMG\`
- macOS: `~/Documents/Arduino/libraries/CheezsEMG/`
- Linux: `~/Arduino/libraries/CheezsEMG/`

## Configuration

### WiFi Settings
Edit in `Bioflex.ino`:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### Firebase Settings
Edit in `Bioflex.ino`:
```cpp
#define FIREBASE_HOST "your-project.firebaseio.com"
#define FIREBASE_AUTH "your-firebase-auth-token"
```

### EMG Feedback Thresholds
Adjust vibration sensitivity in `Bioflex.ino` (lines 39-41):
```cpp
#define EMG_THRESHOLD_LOW 100.0f    // Low activity threshold
#define EMG_THRESHOLD_MED 300.0f    // Medium activity threshold
#define EMG_THRESHOLD_HIGH 500.0f   // High activity threshold
```

**Calibration Tips**:
- Monitor Serial output to see typical EMG envelope values
- Start with default values and adjust based on sensor sensitivity
- Lower values = more sensitive, higher values = less sensitive

## Circuit Diagram

### Complete System Wiring

```
ESP32                                    EMG Sensor Module
┌─────────────────┐                    ┌──────────────────┐
│                 │                    │                  │
│ GPIO 34 (ADC1_6)├────────────────────┤ Signal Out       │
│ GPIO 2          ├────────────────────┤ Detect Out       │
│ GND             ├────────────────────┤ GND              │
│ 3.3V            ├────────────────────┤ VCC              │
└─────────────────┘                    └──────────────────┘


ESP32                                    GSR Sensor Module
┌─────────────────┐                    ┌──────────────────┐
│                 │                    │                  │
│ GPIO 36 (ADC1_0)├────────────────────┤ Analog Out       │
│ GND             ├────────────────────┤ GND              │
│ 3.3V            ├────────────────────┤ VCC              │
└─────────────────┘                    └──────────────────┘


ESP32                Motor Driver (per motor - use NPN transistor or MOSFET)
┌─────────────────┐
│                 │                    ┌─────────────┐
│ GPIO 25 (Motor1)├────[1kΩ]──────────┤ Base/Gate   │
│                 │                    │   (2N2222   │
│ GPIO 26 (Motor2)├────[1kΩ]──────────┤   or        │
│                 │                    │   2N7000)   │
│ GND             ├────────────────────┤ Emitter/Src │
│                 │                    └─────────────┘
└─────────────────┘                          │
                                             │
    ┌────────────────────────────────────────┘
    │
    │  Vibration Motor 1          Vibration Motor 2
    │  ┌──────────┐               ┌──────────┐
    └──┤ -    +   │           ┌───┤ -    +   │
       └──────────┘           │   └──────────┘
            │                 │        │
            │                 │        │
       [Flyback Diode]        │   [Flyback Diode]
         1N4001               │      1N4001
            │                 │        │
            └─────────────────┴────────┴──── VCC (3.3V or 5V)
```

### Motor Driver Circuit (Detailed - Per Motor)

```
         ESP32 GPIO (25 or 26)
                 │
                 │
              [1kΩ R]
                 │
                 ├──────┐
                 │      │ NPN Transistor (2N2222/BC547)
                 │    B ┤ or MOSFET (2N7000/IRLZ44N)
                 │      │
                GND   C/D├───────┬─────────[ M ]──── VCC+
                         │       │      Vibration
                         │     [Diode]   Motor
                         │     1N4001
                         │       │
                         └───────┴──────────────────── GND

Notes:
- Flyback diode (1N4001) protects against back-EMF
- 1kΩ base/gate resistor limits current
- For higher current motors, use MOSFET (IRLZ44N)
- VCC can be 3.3V or 5V depending on motor specs
```

### Important Circuit Notes

1. **ADC Protection**: All analog inputs should stay within 0-3.3V range
2. **Motor Driver**: ESP32 GPIO pins provide max 40mA - always use transistor/MOSFET
3. **Flyback Diodes**: Essential to protect ESP32 from motor voltage spikes
4. **Common Ground**: Ensure all components share common ground with ESP32
5. **Power Supply**: If motors draw significant current, use separate power supply with common ground

## Installation & Upload

1. **Clone or Download** this repository
2. **Open** `ESP32 Code/Bioflex.ino` in Arduino IDE
3. **Configure** WiFi and Firebase credentials
4. **Select Board**: Tools → Board → ESP32 Dev Module (or your specific board)
5. **Select Port**: Tools → Port → (your ESP32 COM port)
6. **Upload**: Click Upload button or Ctrl+U
7. **Monitor**: Open Serial Monitor (115200 baud) to view real-time data

## Data Format

### Firebase Structure
Data is uploaded in rotating slots (0-4) to prevent excessive database size:
```json
{
  "signal_data": {
    "slot_0": {
      "timestamp": 12345,
      "emg_raw": [val1, val2, ..., val100],
      "emg_filtered": [val1, val2, ..., val100],
      "emg_envelope": [val1, val2, ..., val100],
      "gsr_voltage": [val1, val2, ..., val100]
    },
    "slot_1": { ... },
    ...
  }
}
```

### Serial Output Format
CSV format at 500Hz sample rate:
```
raw_emg, filtered_emg, envelope_emg, gsr_voltage
1234.56, 1156.78, 234.12, 1.45
1235.12, 1157.23, 235.67, 1.46
...
```

## Vibration Feedback Behavior

The system provides progressive haptic feedback based on muscle activity:

| EMG Level | Motor 1 | Motor 2 | Sensation |
|-----------|---------|---------|-----------|
| < 100 | OFF | OFF | No feedback |
| 100-300 | Gentle (50-120 PWM) | OFF | Light pulse on one motor |
| 300-500 | Moderate (120-200 PWM) | Moderate (80-150 PWM) | Both motors, medium intensity |
| > 500 | Strong (255 PWM) | Strong (220 PWM) | Maximum feedback |

PWM values range from 0-255 (8-bit resolution at 1kHz frequency)

## Troubleshooting

### WiFi Won't Connect
- Verify SSID and password
- Ensure 2.4GHz WiFi (ESP32 doesn't support 5GHz)
- Check router security (WPA2 recommended)

### Firebase Upload Fails
- Verify Firebase URL format (exclude `https://` and trailing slash)
- Check Firebase auth token is valid
- Ensure Firebase Realtime Database is enabled

### EMG Readings Erratic
- Check electrode connection and skin contact
- Ensure proper grounding
- Verify ADC1 pins are used (GPIO 34, 36)
- Check sensor power supply voltage

### Motors Not Vibrating
- Verify motor driver circuit and connections
- Check motor power supply (3.3V or 5V)
- Test motors directly with power supply
- Ensure flyback diodes are correctly oriented
- Monitor Serial output to confirm EMG values exceed thresholds

### ADC Errors with WiFi Active
- Only use ADC1 pins (GPIO 32-39)
- Never use ADC2 pins (GPIO 0, 2, 4, 12-15, 25-27) - conflict with WiFi

## System Specifications

- **Sample Rate**: 500 Hz
- **Block Size**: 100 samples per upload
- **Upload Frequency**: ~5 times per second
- **ADC Resolution**: 12-bit (0-4095)
- **ADC Attenuation**: 11dB (0-3.3V range)
- **PWM Frequency**: 1000 Hz
- **PWM Resolution**: 8-bit (0-255)
- **Baud Rate**: 115200

## Project Structure

```
BioFlex/
├── ESP32 Code/
│   └── Bioflex.ino          # Main ESP32 Arduino sketch
├── Flutter Code/            # Mobile app (if applicable)
├── CIRCUIT_DIAGRAM.md       # Detailed circuit diagrams
└── README.md               # This file
```

## Future Enhancements

- [ ] Adaptive threshold calibration
- [ ] Custom vibration patterns (pulse, ramp, etc.)
- [ ] Battery monitoring and low-power modes
- [ ] SD card data logging for offline operation
- [ ] Multi-channel EMG support
- [ ] Real-time visualization mobile app
- [ ] Machine learning gesture recognition

## Credits & References

- **CheezsEMG Library**: EMG signal processing
- **Firebase ESP32 Client**: by Mobizt
- **ESP32 Arduino Core**: by Espressif Systems

## License

This project is open source. Check individual library licenses for dependencies.

## Support

For issues, questions, or contributions:
- Open an issue on GitHub
- Check Serial Monitor output for debugging
- Verify hardware connections match circuit diagram

---

**Created for biometric feedback research and development**

**Last Updated**: November 2025
