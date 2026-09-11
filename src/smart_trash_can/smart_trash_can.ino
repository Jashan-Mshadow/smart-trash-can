/*
 * Smart Trash Can
 * ---------------
 * An ultrasonic sensor watches for someone approaching. When they get
 * close enough, a servo lifts the lid through a reinforced popsicle
 * stick arm. The lid stays open while you're still there, then closes
 * on its own once you leave.
 *
 * Board: Arduino Uno
 *
 * Jashan Multani
 */

#include <Servo.h>

// ---------------------------------------------------------------- pins
const uint8_t TRIG_PIN  = 9;
const uint8_t ECHO_PIN  = 10;
const uint8_t SERVO_PIN = 6;

// ------------------------------------------------------------- tuning
// OPEN_CM was the single fiddliest number in this project. Too small and
// you have to wave your hand around like you're landing a plane; too big
// and the lid flies open every time somebody walks past the bin.
const int OPEN_CM        = 35;
const int SENSOR_MAX_CM  = 200;

const uint8_t LID_CLOSED = 15;    // servo angle with the lid down
const uint8_t LID_OPEN   = 95;    // servo angle with the lid up

const unsigned long HOLD_MS      = 2500;  // stay open after they step away
const unsigned long SETTLE_MS    = 60;    // between distance samples
const uint8_t       CONFIRM_READS = 3;    // agreeing reads before acting

// Sweep the servo instead of snapping to the angle. Snapping draws a
// current spike the battery pack couldn't cover, and the lid jerked.
const uint8_t SWEEP_STEP_DEG = 2;
const uint8_t SWEEP_STEP_MS  = 8;

Servo lid;
bool isOpen = false;
unsigned long lastSeen = 0;

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  lid.attach(SERVO_PIN);
  lid.write(LID_CLOSED);
  Serial.begin(9600);
  delay(500);
}

int distanceCm() {
  digitalWrite(TRIG_PIN, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  unsigned long echo = pulseIn(ECHO_PIN, HIGH, 25000UL);
  if (echo == 0) return SENSOR_MAX_CM;
  return (int)(echo * 0.0343 / 2.0);
}

// Ultrasonic sensors throw the occasional wild reading. Requiring a few
// agreeing samples in a row stopped the lid twitching at nothing.
bool someoneThere() {
  uint8_t hits = 0;
  for (uint8_t i = 0; i < CONFIRM_READS; i++) {
    if (distanceCm() <= OPEN_CM) hits++;
    delay(SETTLE_MS);
  }
  return hits == CONFIRM_READS;
}

void sweepTo(uint8_t target) {
  uint8_t pos = lid.read();
  while (pos != target) {
    pos = (pos < target) ? min(pos + SWEEP_STEP_DEG, target)
                         : max(pos - SWEEP_STEP_DEG, target);
    lid.write(pos);
    delay(SWEEP_STEP_MS);
  }
}

void loop() {
  if (someoneThere()) {
    lastSeen = millis();
    if (!isOpen) { sweepTo(LID_OPEN); isOpen = true; Serial.println(F("open")); }
  } else if (isOpen && millis() - lastSeen > HOLD_MS) {
    sweepTo(LID_CLOSED); isOpen = false; Serial.println(F("closed"));
  }
  delay(40);
}
