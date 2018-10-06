#include <AFMotor.h>
#include <NewPing.h>

/* Constants */

// State
#define STOP 0
//FORWARD 1
//BACKWARD 2
//BRAKE 3
//RELEASE 4
#define AVOID 5

// Motor
#define NORMAL_ACCEL 1
#define BRAKE_ACCEL 4
#define AVOID_ACCEL 4
#define MAX_SPEED 255

// Ultrassom
#define TRIGGER_PIN A1
#define ECHO_PIN A0
#define MAX_DISTANCE 100
#define SONAR_MIN_TIME 200

/* Objects and variables */

AF_DCMotor motor[] = {
  AF_DCMotor(1),          // front right
  AF_DCMotor(2),          // front left
  AF_DCMotor(3),          // back left
  AF_DCMotor(4)           // back right
};

uint8_t motor_speed[4];
uint8_t state;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);
unsigned long time;
unsigned long last_time;

bool obstacle = false;
bool debug = true;

/* Functions */

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

  state = FORWARD;
  run_state();
}

void print_info(unsigned long distance) {
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

uint8_t brake() {
  uint8_t i;
  unsigned sum = 0;

  for (i=0; i<4; i++) {
    if (motor_speed[i] > BRAKE_ACCEL) {
      motor_speed[i] -= BRAKE_ACCEL;
    } else {
      motor_speed[i] = 0;
    }
    motor[i].setSpeed(motor_speed[i]);
    sum += motor_speed[i];
  }

  return sum / 4;
}

uint8_t accel() {
  uint8_t i;
  unsigned sum = 0;

  for (i=0; i<4; i++) {
    if (motor_speed[i] < MAX_SPEED-NORMAL_ACCEL) {
      motor_speed[i] += NORMAL_ACCEL;
    } else {
      motor_speed[i] = MAX_SPEED;
    }
    motor[i].setSpeed(motor_speed[i]);
    sum += motor_speed[i];
  }

  return sum / 4;
}

void run_state() {
  uint8_t i;

  for (i=0; i<4; i++) {
    motor[i].run(state);
  }
}

/* SETUP */

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

/* LOOP */

void loop() {
  uint8_t i, j, speed;
  unsigned long distance;

  time = millis();

  // Efeituar leitura do sonar
  if (time - last_time > SONAR_MIN_TIME) {
    last_time = time;
    distance = sonar.ping_cm();
    obstacle = (distance > 0 && distance < 40) ? true : false;
  }

  if (debug) {
    print_info(distance);
  }

  // Ações quando há obstáculo na frente
  if (obstacle) {
    switch (state) {
      case RELEASE:
        // Gesto de ativação
        if (distance > 0 && distance < 10) {
          delay(1000);
          state = FORWARD;
          run_state();
        }
        break;
      case FORWARD:
        // Acionar o freado
        speed = brake();
        if (speed == 0) avoid_obstacle();

        break;
    }
  } else if (state == FORWARD) {
    // Acelerar até MAX_SPEED
    speed = accel();
  }

  delay(10);

}
