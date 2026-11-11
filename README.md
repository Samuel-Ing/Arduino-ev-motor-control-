# Arduino-Based EV Motor Control — Preliminary Physical Prototype

> **Project stage:** Preliminary physical motor-control test  
> **Platform used in this stage:** Arduino Uno R3  
> **Planned next platform:** ESP32

## Overview

This repository documents an early physical test of a motor-control concept for an electric-vehicle (EV) development project undertaken during an engineering attachment at the CSIR–Institute of Industrial Research (CSIR-IIR).

The preliminary test used an Arduino Uno R3 to investigate basic accelerator input, PWM motor-command generation, gradual changes in the commanded PWM value, and brake-input override. A 12 V DC automotive windshield-wiper motor was used as the physical load and was powered by an external 12 V DC, 5 A AC-to-DC power supply through a BTS7960/IBT-2 motor-driver module.

The Arduino Serial Monitor was used to observe controller variables during testing. This Arduino Uno implementation served as an initial hardware test before adapting the control concept for the planned ESP32-based stage.

**This is not a completed EV, road-going propulsion system, or validated vehicle braking system.**

## Objectives of the preliminary test

- Read accelerator position using a potentiometer.
- Convert the analogue input into a PWM motor command.
- Apply a gradual ramp to changes in the commanded PWM value.
- Give the brake input priority over accelerator commands.
- Test START activation and continued system operation after a braking event.
- Observe input values, target PWM, motor PWM, and system/brake status through serial output.

## Hardware used

| Component | Specification / role |
|---|---|
| Microcontroller | Arduino Uno R3 |
| Motor driver | BTS7960 / IBT-2 module |
| Motor/load | Automotive DC windshield-wiper motor |
| Motor supply | External 12 V DC, 5 A AC-to-DC supply |
| Accelerator input | 10 kΩ potentiometer |
| User inputs | START and BRAKE pushbuttons |
| Monitoring | Arduino Serial Monitor |

The 12 V, 5 A supply has a nominal maximum output rating of 60 W. The motor's actual current draw, including startup or stall current, was not measured as part of the supplied test record. Therefore, no motor-power, torque, speed, or efficiency claims are made.

## Control approach implemented

The supplied Arduino sketch uses the following basic approach:

1. **START activation:** a debounced START-button press enables the control system when the brake input is released.
2. **Accelerator input:** the potentiometer is read through analogue input A0. The reading is offset against a software zero reference and mapped to a PWM command.
3. **PWM ramping:** the commanded PWM value is adjusted in steps rather than jumping immediately to the target value.
4. **Brake input:** while the brake button is held, the target motor command is set to zero and the PWM command is ramped down. Accelerator processing is suspended during the brake routine.
5. **Five-second brake hold:** the sketch detects a brake hold lasting at least five seconds and then updates the software accelerator zero reference.
6. **Serial monitoring:** the sketch prints accelerator reading, zero reference, target PWM, motor PWM, system state, and brake state.

The sketch's PWM output is a motor-drive command, **not a measurement of motor RPM, vehicle speed, torque, or braking force**.

## Test evidence

- `hardware/preliminary_physical_setup.jpg` — photograph of the physical test arrangement.
- `testing/ev_serial_monitor_export_Stage_1.csv` — Serial Monitor export supplied for this preliminary test.
- `documentation/preliminary_test_notes.md` — notes on the supplied serial log and interpretation limits.

The supplied serial export contains 14,673 records, of which 14,575 contain the complete recurring status-line format. In those complete status lines, the recorded target PWM ranged from 0 to 169 and the recorded motor PWM ranged from 0 to 169. Three `ACCELERATION RESET COMPLETE` messages are present, with reported new zero references of 643, 840, and 1012.

These are observations from the exported log only. They do not independently establish motor speed, physical acceleration, braking distance, or vehicle performance. The export contains only two complete recurring status lines marked `BRAKE: ACTIVE`; because the sketch uses a blocking brake routine, the normal periodic status output does not continuously sample the braking sequence. The log should therefore not be used to claim measured braking performance.

## Limitations and safety scope

- No wheel-speed or motor-speed sensor was used in this test.
- No current, torque, temperature, or mechanical output measurements are included in the supplied evidence.
- PWM reduction is not equivalent to mechanical braking or regenerative braking.
- No battery pack, battery-management system, traction system, or vehicle-level safety architecture was validated here.
- The five-second zero-reference recalibration needs further review. If the accelerator is not at its true released position when the reference is captured, subsequent accelerator readings may be interpreted incorrectly.
- The motor's startup/stall current and the power supply's response under load were not measured.
- This bench prototype is not intended for road use.


## Next development stage

The next planned stage is to adapt and validate the control logic on an ESP32 platform. Further work may include safer accelerator calibration, non-blocking state-based control, motor-speed feedback, fault handling, and appropriately engineered braking functions.

## Repository contents

```text
.
├── README.md
├── code/
│   └── README.md
├── hardware/
│   └── preliminary_physical_setup.jpg
├── testing/
│   └── ev_serial_monitor_export_Stage_1.csv
└── documentation/
    └── preliminary_test_notes.md
```

