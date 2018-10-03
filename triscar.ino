#include <AFMotor.h>
#include <NewPing.h>

#define ACCEL 1
#define MAX_SPEED 255

AF_DCMotor motor[] = {
  AF_DCMotor(1),          // front right
  AF_DCMotor(2),          // front left
  AF_DCMotor(3),          // back left
  AF_DCMotor(4)           // back right
};

#define TRIGGER_PIN A1
#define ECHO_PIN A0
#define MAX_DISTANCE 100

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

uint8_t state = BRAKE;
bool debug = true;

// move forward
void forward(uint8_t accel) {
  uint8_t i, j;

  if (state != FORWARD) {
    if (state == BACKWARD) brake(4 * ACCEL);
    for (i=0; i<4; i++) {
      motor[i].run(FORWARD);
    }

    if (debug) Serial.println("Forward...");

    for (i=0; i<MAX_SPEED-accel; i+=accel) {
      for (j=0; j<4; j++) {
        motor[j].setSpeed(i);
        delay(10);
      }
    }

    state = FORWARD;
  }
}

// move backward
void backward(uint8_t accel) {
  uint8_t i, j;

  if (state != BACKWARD) {
    if (state == FORWARD) brake(4 * ACCEL);
    for (i=0; i<4; i++) {
      motor[i].run(BACKWARD);
    }

    if (debug) Serial.println("Backward...");

    for (i=0; i<MAX_SPEED-accel; i+=accel) {
      for (j=0; j<4; j++) {
        motor[j].setSpeed(i);
        delay(10);
      }
    }

    state = BACKWARD;
  }
}

// brake
void brake(uint8_t accel) {
  uint8_t i, j;

  if (state != BRAKE || state != RELEASE) {

    if (debug) Serial.println("Brake...");

    for (i=MAX_SPEED; i>0+accel; i-=accel) {
      for (j=0; j<4; j++) {
        motor[j].setSpeed(i);
        delay(10);
      }
    }

    state = BRAKE;
  }
}

// release motors
void release() {
  uint8_t i;

  if (state != RELEASE) {

    if (debug) Serial.println("Release...");

    for (i=0; i<4; i++) {
      motor[i].run(state);
    }

    state = RELEASE;
    delay(1000);
  }
}

void setup() {
  uint8_t i;

  Serial.begin(115200);           // set up Serial library at 9600 bps
  Serial.println("Motor test!");

  for (i=0; i<4; i++) {
    motor[i].setSpeed(200);
  }

  release();
}

void loop() {

  //forward(ACCEL);
  //backward(ACCEL);
  delay(50);
  Serial.print("Ping: ");
  Serial.print(sonar.ping_cm());
  Serial.println(" cm");

  //release();
}
