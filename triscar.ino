#include <AFMotor.h>
#include <NewPing.h>

#define STOP 0
//FORWARD 1
//BACKWARD 2
//BRAKE 3
//RELEASE 4
#define AVOID 5

#define NORMAL_ACCEL 1
#define BRAKE_ACCEL 4
#define AVOID_ACCEL 4
#define MAX_SPEED 255

// Ultrassom
#define TRIGGER_PIN A1
#define ECHO_PIN A0
#define MAX_DISTANCE 100
#define SONAR_MIN_TIME 200

AF_DCMotor motor[] = {
  AF_DCMotor(1),          // front right
  AF_DCMotor(2),          // front left
  AF_DCMotor(3),          // back left
  AF_DCMotor(4)           // back right
};

uint8_t motor_speed[4];
uint8_t state;
bool obstacle = false;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
unsigned long distance;
unsigned long time;
unsigned long last_time;

bool debug = true;

void avoid_obstacle() {
  Serial.println("Avoiding");
  uint8_t i, j;

  motor[0].run(FORWARD);
  motor[1].run(BACKWARD);
  motor[2].run(BACKWARD);
  motor[3].run(FORWARD);

  for (j=0; j<MAX_SPEED - AVOID_ACCEL; j+=AVOID_ACCEL) {
    for (i=0; i<4; i++) {
      motor_speed[i] = j;
      motor[i].setSpeed(motor_speed[i]);
      delay(10);
    }
  }

  for (j=MAX_SPEED; j>AVOID_ACCEL; j-=AVOID_ACCEL) {
    for (i=0; i<4; i++) {
      motor[i].setSpeed(j);
      delay(10);
    }
  }

  for (i=0; i<4; i++) {
    motor_speed[i] = 0;
  }

  state = STOP;
}

void print_info() {
  uint8_t i;

  Serial.print("state: ");
  Serial.print(state);
  Serial.print(" obstacle? ");
  Serial.print(obstacle);
  Serial.print(" distance: ");
  Serial.print(distance);
  Serial.print(" speed: ");
  for (i=0; i<4; i++) {
    Serial.print(motor_speed[i]);
    Serial.print(" ");
  }
  Serial.println();
}

// SETUP

void setup() {
  uint8_t i;

  last_time = millis();
  state = RELEASE;

  for (i=0; i<4; i++) {
    motor_speed[i] = 0;
    motor[i].setSpeed(motor_speed[i]);
    motor[i].run(state);
  }

  Serial.begin(115200);
  delay(1000);
}

// LOOP

void loop() {
  uint8_t i, j;
  unsigned sum;

  time = millis();

  if (time - last_time > SONAR_MIN_TIME) {
    last_time = time;
    distance = sonar.ping_cm();
    obstacle = (distance > 0 && distance < 40) ? true : false;
  }

  if (debug) {
    print_info();
  }

  if (obstacle) {
    switch (state) {
      case RELEASE:
        if (distance > 0 && distance < 10) {
          delay(1000);
          state = FORWARD;

          for (i=0; i<4; i++) {
            motor[i].run(state);
          }
        }
        break;
      case FORWARD:
        state = BRAKE;
        break;
      case STOP:
        state = AVOID;
        break;
    }
  }

  switch (state) {
    case FORWARD:
      for (i=0; i<4; i++) {
        if (motor_speed[i] < MAX_SPEED-NORMAL_ACCEL) {
          motor_speed[i] += NORMAL_ACCEL;
        } else {
          motor_speed[i] = MAX_SPEED;
        }
        motor[i].setSpeed(motor_speed[i]);
      }
      break;
    case BRAKE:
      if (obstacle) {
        sum = 0;

        for (i=0; i<4; i++) {
          if (motor_speed[i] > BRAKE_ACCEL) {
            motor_speed[i] -= BRAKE_ACCEL;
          } else {
            motor_speed[i] = 0;
          }
          motor[i].setSpeed(motor_speed[i]);
          sum += motor_speed[i];
        }
        if (sum == 0) state = STOP;
      } else {
        state = FORWARD;
        for (i=0; i<4; i++) {
          motor[i].run(state);
        }
      }
      break;
    case STOP:
      state = FORWARD;
      for (i=0; i<4; i++) {
        motor[i].run(state);
      }
      break;
    case AVOID:
      avoid_obstacle();
      break;
  }

  delay(10);

}
