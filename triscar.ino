#include <AFMotor.h>

#define SPEED 1

AF_DCMotor motor[] = {
  AF_DCMotor(1),
  AF_DCMotor(2),
  AF_DCMotor(3),
  AF_DCMotor(4)
};

uint8_t state = BRAKE;
bool debug = true;

// move forward
void forward(uint8_t speed) {
  uint8_t i;

  if (state != FORWARD) {
    if (state == BACKWARD) brake(2 * SPEED);
    for (i=0; i<4; i++) {
      motor[i].run(FORWARD);
    }
    for (i=0; i<255; i+=speed) {
      for (i=0; i<4; i++) {
        motor[i].setSpeed(i);
        delay(10);
      }
    }

    state = FORWARD;
  }

  if (debug) Serial.println("Forward...");
}

// move backward
void backward(uint8_t speed) {
  uint8_t i;

  if (state != BACKWARD) {
    if (state == FORWARD) brake(2 * SPEED);
    for (i=0; i<4; i++) {
      motor[i].run(BACKWARD);
    }
    for (i=0; i<255; i+=speed) {
      for (i=0; i<4; i++) {
        motor[i].setSpeed(i);
        delay(10);
      }
    }

    state = BACKWARD;
  }

  if (debug) Serial.println("Backward...");
}

// brake
void brake(uint8_t speed) {
  uint8_t i;

  if (state != BRAKE || state != RELEASE) {
    for (i=255; i!=0; i-=speed) {
      for (i=0; i<4; i++) {
        motor[i].setSpeed(i);
        delay(10);
      }
    }

    state = BRAKE;
  }

  if (debug) Serial.println("Brake...");
}

// release motors
void release() {
  uint8_t i;

  if (state != RELEASE) {
    for (i=0; i<4; i++) {
      motor[i].run(state);
    }

    state = RELEASE;
    delay(1000);
  }

  if (debug) Serial.println("Release...");
}

void setup() {
  uint8_t i;

  Serial.begin(9600);           // set up Serial library at 9600 bps
  Serial.println("Motor test!");

  for (i=0; i<4; i++) {
    motor[i].setSpeed(200);
  }

  release();
}

void loop() {

  forward(SPEED);
  backward(SPEED);
  brake(2 * SPEED);

  release();
}
