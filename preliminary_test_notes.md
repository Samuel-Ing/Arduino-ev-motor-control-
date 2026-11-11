# Preliminary test notes

## Evidence supplied

- Physical setup photograph: Arduino Uno-based controller, breadboarded input components, motor-driver module, automotive wiper motor, and external motor-supply wiring.
- Serial Monitor export: `ev_serial_monitor_export_Stage_1.csv`.
- Hardware clarification from the project developer: the wiper motor was powered by an external **12 V DC, 5 A** AC-to-DC supply, through a **BTS7960 / IBT-2** motor-driver module.

## Basic inspection of the serial export

The supplied file has 14,673 rows and three columns (`Timestamp`, `Value`, `Type`). It is an exported serial log in which most controller fields are embedded in a text string in the `Value` column; it is not a clean, one-variable-per-column measurement dataset.

A parse of the recurring complete status lines found:

- 14,575 complete status lines.
- Target PWM minimum/maximum: 0 / 169.
- Motor PWM minimum/maximum: 0 / 169.
- Three `ACCELERATION RESET COMPLETE` messages.
- New zero references reported by those messages: 643, 840, and 1012.
- Two complete recurring status lines marked `BRAKE: ACTIVE`.

## Interpretation cautions

1. PWM values are controller commands, not measured motor speed or vehicle speed.
2. The low count of `BRAKE: ACTIVE` recurring status lines should not be interpreted as only two brake activations. The supplied sketch's brake routine blocks the main loop, so periodic status logging is interrupted while that routine is executing.
3. The serial export does not contain a current measurement, motor RPM, torque, braking force, stopping distance, or vehicle-speed measurement.
4. The zero-reference changes show that the calibration routine executed, but do not prove that the accelerator was at its true released position at each calibration event.
5. The log's first and last records may be partial serial lines. Treat the CSV as a raw export and preserve it unchanged.


## Scope statement

These notes describe a preliminary bench-level motor-control test. They do not establish the performance, safety, or roadworthiness of an electric vehicle.
