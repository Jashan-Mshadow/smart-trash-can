# Wiring

| Arduino | Connects to |
|---|---|
| D9  | HC-SR04 TRIG |
| D10 | HC-SR04 ECHO |
| D6  | Servo signal (orange/yellow) |
| 5V  | HC-SR04 VCC |
| GND | Common ground — shared with the servo supply |

## Power the servo separately

This is the whole lesson of this project. A servo lifting a real lid draws far more current than the
Arduino's regulator can supply. Run it from its own battery pack and tie the grounds together. If the lid
moves weakly, or the Arduino resets when the lid lifts, this is why.

## Sensor placement

Mount the HC-SR04 on the front face, pointing slightly upward. Aimed level it reads the floor as an
obstacle and the lid never closes.
