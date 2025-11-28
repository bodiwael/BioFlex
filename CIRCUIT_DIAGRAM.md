# BioFlex - Complete Circuit Diagrams

This document provides detailed circuit diagrams and wiring specifications for the BioFlex biosignal monitoring system.

## Table of Contents
- [System Overview](#system-overview)
- [Complete Schematic](#complete-schematic)
- [Component Specifications](#component-specifications)
- [Motor Driver Circuit](#motor-driver-circuit)
- [Power Supply](#power-supply)
- [PCB Layout Recommendations](#pcb-layout-recommendations)
- [Bill of Materials](#bill-of-materials)

---

## System Overview

```
┌──────────────────────────────────────────────────────────────────┐
│                         BioFlex System                           │
│                                                                  │
│  ┌────────────┐        ┌──────────────┐        ┌─────────────┐ │
│  │ EMG Sensor │───────▶│              │        │  Vibration  │ │
│  │  Module    │        │              │◀──────▶│  Motor 1    │ │
│  └────────────┘        │              │        └─────────────┘ │
│                        │              │                         │
│  ┌────────────┐        │   ESP32      │        ┌─────────────┐ │
│  │ GSR Sensor │───────▶│   Dev Board  │◀──────▶│  Vibration  │ │
│  │  Module    │        │              │        │  Motor 2    │ │
│  └────────────┘        │              │        └─────────────┘ │
│                        │              │                         │
│                        │              │        ┌─────────────┐ │
│                        │              │◀──────▶│   WiFi      │ │
│                        └──────────────┘        │  Firebase   │ │
│                                                └─────────────┘ │
└──────────────────────────────────────────────────────────────────┘
```

---

## Complete Schematic

### Full System Wiring Diagram

```
                                    ┌─────────────────────────────────────┐
                                    │         ESP32 DevKit V1             │
                                    │                                     │
                ┌───────────────────┤ 3V3                            VIN  │◀─── 5V Power
                │                   │                                     │
                │   ┌───────────────┤ GND                            GND  │◀─── GND
                │   │               │                                     │
                │   │               │ GPIO 34 (ADC1_6)              GPIO 25│─────┐
                │   │               │                                     │     │
EMG Sensor VCC  │   │               │ GPIO 2                        GPIO 26│───┐ │
      │         │   │               │                                     │   │ │
      └─────────┘   │   ┌───────────┤ GPIO 36 (ADC1_0)              GPIO  │   │ │
                    │   │           │                                     │   │ │
GSR Sensor VCC      │   │           └─────────────────────────────────────┘   │ │
      │             │   │                                                     │ │
      └─────────────┘   │                                                     │ │
                        │                                                     │ │
                        │                                                     │ │
┌──────────────────┐    │            ┌──────────────────┐                    │ │
│  EMG Sensor      │    │            │  GSR Sensor      │                    │ │
│  Module          │    │            │  Module          │                    │ │
│                  │    │            │                  │                    │ │
│  VCC ────────────┼────┘            │  VCC ────────────┼────────────────────┘ │
│  GND ────────────┼─────────────────┤  GND             │                      │
│  Signal ─────────┼──── GPIO 34     │  Analog ─────────┼──── GPIO 36          │
│  Detect ─────────┼──── GPIO 2      └──────────────────┘                      │
│                  │                                                           │
└──────────────────┘                                                           │
                                                                               │
                                                                               │
┌──────────────────────────────────────────────────────────────────────────────┼───┐
│                         MOTOR DRIVER CIRCUITS                                │   │
│                                                                              │   │
│  ┌──────────────────────────────────────────────┐                           │   │
│  │          Motor 1 Driver Circuit              │                           │   │
│  │                                              │                           │   │
│  │    GPIO 25 ────[1kΩ]────┬─── GND            │                           │   │
│  │                          │                   │                           │   │
│  │                        ┌─┴─┐ 2N2222          │                           │   │
│  │                        │   │ NPN             │                           │   │
│  │                        └─┬─┘                 │                           │   │
│  │                          │                   │                           │   │
│  │          VCC+ ───[Motor]─┴───[1N4001]─── GND│                           │   │
│  │            │      M1            │            │                           │   │
│  │            │                    ▼            │                           │   │
│  └────────────┼────────────────────────────────┼┘                           │   │
│               │                                                             │   │
│  ┌────────────┼────────────────────────────────────┐                        │   │
│  │            │     Motor 2 Driver Circuit         │                        │   │
│  │            │                                    │◀───────────────────────┘   │
│  │    GPIO 26 ────[1kΩ]────┬─── GND              │                            │
│  │                          │                     │                            │
│  │                        ┌─┴─┐ 2N2222            │                            │
│  │                        │   │ NPN               │                            │
│  │                        └─┬─┘                   │                            │
│  │                          │                     │                            │
│  │          VCC+ ───[Motor]─┴───[1N4001]─── GND  │                            │
│  │            │      M2            │              │                            │
│  │            │                    ▼              │                            │
│  └────────────┼────────────────────────────────────┘                            │
│               │                                                                 │
│               └───────────────────── 5V Power Supply (or 3.3V)                  │
│                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────┘

Power Notes:
- VCC+ for motors can be 3.3V or 5V depending on motor specification
- ESP32 VIN accepts 5V from USB or external power supply
- All components must share common ground
```

---

## Component Specifications

### ESP32 Development Board

```
┌──────────────────────────────────────┐
│        ESP32 DevKit V1 Pinout        │
├──────────────────────────────────────┤
│                                      │
│  Left Side:           Right Side:    │
│  ──────────           ──────────     │
│  3V3                  GND            │
│  EN                   GPIO 23        │
│  GPIO 36 (VP) ◀───GSR GPIO 22        │
│  GPIO 39 (VN)         GPIO 1         │
│  GPIO 34 ◀──────EMG   GPIO 3         │
│  GPIO 35              GPIO 21        │
│  GPIO 32              GPIO 19        │
│  GPIO 33              GPIO 18        │
│  GPIO 25 ───▶ MOTOR1  GPIO 5         │
│  GPIO 26 ───▶ MOTOR2  GPIO 17        │
│  GPIO 27              GPIO 16        │
│  GPIO 14              GPIO 4         │
│  GPIO 12              GPIO 0         │
│  GPIO 13              GPIO 2  ◀──DET │
│  GND                  GPIO 15        │
│  VIN                  GND            │
│                                      │
└──────────────────────────────────────┘

Key Features:
- Dual-core 32-bit processor
- 4MB Flash memory
- Built-in WiFi and Bluetooth
- 18x 12-bit ADC channels (ADC1: safe with WiFi)
- 16x PWM channels
- Operating voltage: 3.3V
- Input voltage: 5V via USB/VIN
```

### EMG Sensor Module

**Typical EMG Module (e.g., Muscle Sensor v3, MyoWare)**

```
┌─────────────────────────────┐
│     EMG Sensor Module       │
├─────────────────────────────┤
│                             │
│  Pins:                      │
│  • VCC    → 3.3V or 5V      │
│  • GND    → Ground          │
│  • SIG    → Analog Output   │
│  • DET    → Digital Detect  │
│                             │
│  Features:                  │
│  • Gain: ~1000x             │
│  • Bandwidth: 25-150 Hz     │
│  • Output: 0-3.3V analog    │
│  • Rectified & smoothed     │
│                             │
│  Electrode Connection:      │
│  • MID (Reference)          │
│  • END (Muscle belly 1)     │
│  • END (Muscle belly 2)     │
│                             │
└─────────────────────────────┘

Electrode Placement:
    Reference (GND)
         │
         ▼
    ┌────────┐
    │ Muscle │
    │        │
    └────────┘
    ▲        ▲
    │        │
  EMG 1    EMG 2
  (both ends of muscle)
```

### GSR Sensor Module

**Grove GSR or similar**

```
┌─────────────────────────────┐
│      GSR Sensor Module      │
├─────────────────────────────┤
│                             │
│  Pins:                      │
│  • VCC → 3.3V or 5V         │
│  • GND → Ground             │
│  • OUT → Analog (0-3.3V)    │
│                             │
│  Finger Electrodes:         │
│  • Two metal pads           │
│  • Worn on two fingers      │
│                             │
│  Measurement:               │
│  • Skin conductance         │
│  • Stress/arousal indicator │
│  • 0-3.3V output range      │
│                             │
└─────────────────────────────┘

Electrode Placement:
  Index    Middle
  Finger   Finger
    │        │
    ▼        ▼
  [===]    [===]
   GSR      GSR
  Pad 1    Pad 2
```

---

## Motor Driver Circuit

### Detailed Per-Motor Driver

```
                    ESP32 GPIO Pin (25 or 26)
                              │
                              │ PWM Signal (0-3.3V, 1kHz)
                              │
                          [1kΩ R1]  ← Base resistor
                              │
                              ├──────┐
                              │      │
                            (GND)  (B) Base
                                     │
                                  ┌──┴──┐
                                  │2N222│  NPN Transistor
                                  │ or  │  hFE > 100
                                  │BC547│
                                  └──┬──┘
                                   (C) │ Collector
                                     │
                          VCC+ ──────┴────┬──── (M+)
                              Vibration   │     Vibration
                                Motor     │      Motor
                              ┌─────────┐ │
                              │    M    │ │
                              └─────────┘ │
                                   │      │
                                 (M-)   (│▌) Flyback Diode
                                   │    1N4001
                                   │      │
                                   ├──────┘
                                   │
                                  GND

Component Values:
- R1: 1kΩ (base current limiting)
- Transistor: 2N2222, BC547, or similar NPN
  - Ic(max) > motor current (typically 100mA)
  - hFE > 100
- Diode: 1N4001 or 1N4007
  - Reverse voltage > supply voltage
  - Forward current > motor current
- Motor: Vibration motor, 3.3V or 5V
  - Typical current: 50-100mA
  - Resistance: ~30-100Ω
```

### Alternative: MOSFET Driver (Higher Current)

For motors requiring > 100mA:

```
                    ESP32 GPIO Pin
                              │
                              │ PWM Signal
                              │
                          [10kΩ R1]  ← Gate pull-down
                              │
                              ├──────┐
                              │      │
                            (GND)  (G) Gate
                                     │
                                  ┌──┴──┐
                                  │IRLZ │  N-Channel MOSFET
                                  │44N  │  Logic-level
                                  │     │  VGS(th) < 2V
                                  └──┬──┘
                                   (D) │ Drain
                                     │
                          VCC+ ──────┴────┬──── Motor+
                                          │
                              ┌─────────┐ │
                              │  Motor  │ │
                              └─────────┘ │
                                   │    (│▌)
                                   │    Diode
                                   ├──────┘
                                   │
                                  GND (Source)

MOSFET Advantages:
- Higher current capacity (2-10A+)
- Lower on-resistance (< 0.1Ω)
- No base current needed
- More efficient (less heat)

Recommended MOSFETs:
- IRLZ44N (47A, 55V, VGS=2.5V)
- 2N7000 (200mA, 60V, VGS=2V) - for small motors
- IRF520 (9.2A, 100V, VGS=2-4V)
```

---

## Power Supply

### Power Distribution

```
┌────────────────────────────────────────────────────────┐
│                   Power Supply Options                 │
├────────────────────────────────────────────────────────┤
│                                                        │
│  OPTION 1: USB Powered (5V)                            │
│  ─────────────────────────                             │
│                                                        │
│    USB 5V ──┬──▶ ESP32 VIN (5V → 3.3V regulator)      │
│             │                                          │
│             └──▶ Motor Driver VCC (5V motors)          │
│                                                        │
│    Pros: Simple, no external supply                    │
│    Cons: Limited to 500mA (USB 2.0) or 900mA (USB 3.0) │
│                                                        │
├────────────────────────────────────────────────────────┤
│                                                        │
│  OPTION 2: External 5V Supply                          │
│  ──────────────────────────                            │
│                                                        │
│    5V PSU ──┬──▶ ESP32 VIN                             │
│             │                                          │
│             ├──▶ Motor Driver VCC                      │
│             │                                          │
│             └──▶ Sensors VCC                           │
│                                                        │
│    Pros: More current capacity (1-2A+)                 │
│    Cons: Requires external adapter                     │
│                                                        │
├────────────────────────────────────────────────────────┤
│                                                        │
│  OPTION 3: Battery Powered (Portable)                  │
│  ───────────────────────────────────                   │
│                                                        │
│    Li-Ion ──▶ [Boost/Buck] ──┬──▶ ESP32 (5V or 3.3V)  │
│    3.7V       Converter       │                        │
│                               └──▶ Motors (3.3V)       │
│                                                        │
│    Recommended: TP4056 + Boost converter               │
│    Battery: 18650 Li-Ion (2000-3500mAh)                │
│    Runtime: 4-8 hours typical                          │
│                                                        │
└────────────────────────────────────────────────────────┘

Current Budget:
┌──────────────────────────────────────┐
│ Component         │ Current (mA)     │
├───────────────────┼──────────────────┤
│ ESP32 (WiFi on)   │ 160-260         │
│ EMG Sensor        │ 5-10            │
│ GSR Sensor        │ 5-10            │
│ Motor 1 (max)     │ 50-100          │
│ Motor 2 (max)     │ 50-100          │
├───────────────────┼──────────────────┤
│ TOTAL (typical)   │ 270-480 mA      │
│ TOTAL (peak)      │ 370-580 mA      │
└──────────────────────────────────────┘

Recommendation: Use 1A+ power supply
```

### Decoupling Capacitors (Important!)

```
┌─────────────────────────────────────────┐
│     Recommended Decoupling Network      │
├─────────────────────────────────────────┤
│                                         │
│  Near ESP32:                            │
│    VCC ──┬──[100µF electrolytic]── GND │
│          │                             │
│          └──[100nF ceramic]──────── GND│
│                                         │
│  Near Each Motor Driver:                │
│    VCC ──[100nF ceramic]────────── GND │
│                                         │
│  Purpose:                               │
│  • Reduces noise from motor switching   │
│  • Stabilizes ESP32 power during WiFi   │
│  • Prevents brown-out resets            │
│                                         │
└─────────────────────────────────────────┘
```

---

## PCB Layout Recommendations

### Component Placement

```
┌───────────────────────────────────────────────────────────┐
│                    Top View - PCB Layout                  │
│                                                           │
│  ┌─────────────┐                          ┌────────────┐ │
│  │   EMG       │                          │   Motor    │ │
│  │  Connector  │                          │  Driver 1  │ │
│  └─────────────┘                          └────────────┘ │
│                                                           │
│  ┌─────────────┐    ┌──────────────────┐  ┌────────────┐│
│  │   GSR       │    │                  │  │   Motor    ││
│  │  Connector  │    │     ESP32        │  │  Driver 2  ││
│  └─────────────┘    │    DevKit V1     │  └────────────┘│
│                     │                  │                 │
│  ┌─────────────┐    │                  │  ┌────────────┐│
│  │   Power     │    └──────────────────┘  │ Decoupling ││
│  │  Connector  │                          │  Caps      ││
│  │   5V/GND    │                          └────────────┘│
│  └─────────────┘                                         │
│                                                          │
└───────────────────────────────────────────────────────────┘

Layout Guidelines:
1. Keep motor drivers away from analog inputs
2. Place decoupling caps close to ESP32 power pins
3. Use wide traces for motor power (>20 mil)
4. Star ground topology (all grounds to single point)
5. Separate analog and digital ground planes if possible
```

### Trace Width Recommendations

```
┌────────────────────────────────────────────┐
│ Signal Type       │ Min Width │ Typical   │
├───────────────────┼───────────┼───────────┤
│ Signal (digital)  │ 8 mil     │ 10-12 mil │
│ Analog inputs     │ 10 mil    │ 15 mil    │
│ Power (< 500mA)   │ 15 mil    │ 20 mil    │
│ Motor power       │ 25 mil    │ 30-40 mil │
│ Ground plane      │ Fill      │ Fill      │
└────────────────────────────────────────────┘

mil = 1/1000 inch = 0.0254mm
```

---

## Bill of Materials (BOM)

### Complete Parts List

| Qty | Component | Specification | Example Part # | Est. Cost |
|-----|-----------|---------------|----------------|-----------|
| 1 | ESP32 Dev Board | DevKit V1 or similar | ESP32-DEVKITV1 | $5-10 |
| 1 | EMG Sensor | Muscle sensor module | MyoWare / Muscle Sensor v3 | $30-40 |
| 1 | GSR Sensor | Grove GSR or similar | SEN-12012 | $15-25 |
| 2 | Vibration Motor | 3V-5V coin/pancake | ROB-08449 | $2-3 each |
| 2 | NPN Transistor | 2N2222 or BC547 | 2N2222A | $0.10 each |
| 2 | Diode | 1N4001 or 1N4007 | 1N4001 | $0.05 each |
| 2 | Resistor 1kΩ | 1/4W, 5% | CFR-25JB-1K0 | $0.01 each |
| 2 | Capacitor 100nF | Ceramic, 25V+ | CL21B104KBCNNNC | $0.10 each |
| 1 | Capacitor 100µF | Electrolytic, 16V+ | UWX1C101MCL1GB | $0.20 |
| 1 | Breadboard | 830 tie-points | Optional | $5 |
| 1 | Jumper Wires | Male-Male, Female-Female | Optional | $5 set |
| 1 | USB Cable | Micro-USB or USB-C | Depends on ESP32 | $3 |
| 1 | Power Supply | 5V 1A+ USB adapter | Any phone charger | $5 |
| | | | **TOTAL** | **~$70-95** |

### Optional Components

| Qty | Component | Purpose | Est. Cost |
|-----|-----------|---------|-----------|
| 1 | PCB | Custom board vs breadboard | $5-15 |
| 1 | Enclosure | Project box | $5-10 |
| 1 | Li-Ion Battery | 18650 3.7V | $5-8 |
| 1 | Battery Holder | 18650 holder with wires | $1-2 |
| 1 | TP4056 Module | Li-Ion charging circuit | $1-2 |
| 1 | Boost Converter | 3.7V → 5V | $1-2 |
| 1 | Switch | Power on/off | $0.50 |

---

## Testing & Verification Checklist

### Pre-Power Checks
- [ ] Verify all connections match schematic
- [ ] Check for short circuits with multimeter
- [ ] Confirm correct polarity on all polarized components
- [ ] Verify diode orientation (cathode stripe toward VCC)
- [ ] Double-check ADC1 pins are used (not ADC2)

### Power-On Tests
- [ ] Measure 3.3V on ESP32 3V3 pin (should be 3.2-3.4V)
- [ ] Measure 5V on VIN if using external supply
- [ ] Verify no excessive heat from any component
- [ ] Check current draw (should be < 300mA without motors)

### Functional Tests
- [ ] ESP32 connects to WiFi (check Serial Monitor)
- [ ] EMG sensor produces changing values with muscle flexing
- [ ] GSR sensor responds to finger pressure
- [ ] Motors vibrate when EMG threshold exceeded
- [ ] Data uploads successfully to Firebase
- [ ] Serial output shows all four data streams

### Signal Quality Checks
- [ ] EMG baseline noise < 50 (relative to sensor gain)
- [ ] EMG peak values reach 300-1000+ with strong flex
- [ ] GSR values stable and change with arousal
- [ ] No dropouts or NaN values in Serial output

---

## Safety Notes

### Electrical Safety
1. **Never connect body sensors while connected to AC power**
   - Use battery power or isolated USB supply for human testing
   - Risk of electric shock if ground fault occurs

2. **Medical Device Disclaimer**
   - This is NOT a medical device
   - Not approved for clinical or diagnostic use
   - For research and educational purposes only

3. **Current Limits**
   - Keep electrode current below 10µA DC
   - EMG/GSR sensors should be fully isolated
   - Use commercial biomedical-grade sensors

### Component Protection
1. **ESD Precautions**
   - ESP32 is sensitive to static discharge
   - Use ESD wrist strap when handling
   - Touch grounded metal before handling

2. **Reverse Polarity Protection**
   - Double-check power connections
   - Consider adding reverse polarity diode

3. **Overcurrent Protection**
   - Use fuse or PTC resettable fuse on VCC line
   - Recommended: 500mA-1A fuse

---

## Troubleshooting Circuit Issues

### No Power / ESP32 Won't Start
- Check USB cable and power supply
- Verify VIN voltage is 4.5-5.5V
- Check for shorts between VCC and GND
- Press EN button to reset

### Motors Don't Vibrate
- Check transistor connections (BCE pins)
- Verify motor power supply voltage
- Test motor directly with power supply
- Measure GPIO output with multimeter (should pulse 0-3.3V)
- Check diode orientation

### Erratic ADC Readings
- Add/improve decoupling capacitors
- Separate motor and sensor grounds
- Use ADC1 pins only (not ADC2)
- Check for loose connections
- Shield analog wires from motor noise

### WiFi Connection Fails
- Never use ADC2 pins with WiFi active
- Check 2.4GHz WiFi (not 5GHz)
- Verify SSID and password
- Reduce WiFi transmit power if unstable

---

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2025-11-28 | Initial circuit design with vibration feedback |

---

**For additional support, see main README.md or open an issue on GitHub**
