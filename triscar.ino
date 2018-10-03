#include <AFMotor.h>
#include <NewPing.h>

#define ACCEL 1
#define MAX_SPEED 255
#define TRIGGER_PIN A1
#define ECHO_PIN A0
#define MAX_DISTANCE 200

AF_DCMotor motor[] = {
  AF_DCMotor(1),          // front right
  AF_DCMotor(2),          // front left
  AF_DCMotor(3),          // back left
  AF_DCMotor(4)           // back right
};

uint8_t speed[] = {0, 0, 0, 0};
uint8_t state = RELEASE;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
unsigned long distance;

bool debug = true;


// release motors
void release() {
  uint8_t i;

  if (state != RELEASE) {

    if (debug) Serial.println("Release...");

    state = RELEASE;
    run_state(state);

    delay(1000);
  }
}

void run_state(uint8_t state) {
  uint8_t i;

  for (i=0; i<4; i++) {
    motor[i].run(state);
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

  uint8_t i;

  distance = sonar.ping_cm();
  if (debug) {
    Serial.print(distance);
    Serial.print(" ");
    Serial.print(state);
    Serial.print(" ");
    Serial.println(speed[0]);
  }

  if (state == RELEASE && distance > 5 && distance < 25) {
    state = FORWARD;
    run_state(state);
    delay(1000);
  } else if (state == FORWARD && distance > 0 && distance < 40) {
    state = BRAKE;
    run_state(state);
  }

  for (i=0; i<4; i++) {
    switch (state) {
      case FORWARD || BACKWARD:
        if (speed[i] < MAX_SPEED) {
          speed[i]++;
          delay(10);
        }
        break;
      case BRAKE:
        if (speed[i] >= 2) {
          speed[i] = speed[i] - 2;
          delay(1);
        }
        break;
      default:
        break;
    }

    motor[i].setSpeed(speed[i]);
  }

}
