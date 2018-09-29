#include <AFMotor.h>

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

  if (state = BACKWARD) brake(2);
  else if (state != FORWARD) {
    for (i=0; i<4; i++) {
      motor[i].run(state);
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

  if (state = FORWARD) brake(2);
  else if (state != BACKWARD) {
    for (i=0; i<4; i++) {
      motor[i].run(state);
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

  state = BRAKE;

  for (i=255; i!=0; i-=speed) {
    for (i=0; i<4; i++) {
      motor[i].setSpeed(i);
      delay(10);
    }
  }

  if (debug) Serial.println("brake...");
}

// release motors
void release() {
  uint8_t i;

  state = RELEASE;

  for (i=0; i<4; i++) {
    motor[i].run(state);
  }

  delay(1000);
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
  uint8_t i;

  forward(1);
  backward(1);

  release();
}
