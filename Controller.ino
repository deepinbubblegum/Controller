#include <AccelStepper.h>
#define MOTOR_FL_PWM_GPIO 2
#define MOTOR_FL_DIR_GPIO 3
#define MOTOR_FR_PWM_GPIO 23
#define MOTOR_FR_DIR_GPIO 22
#define MOTOR_BL_PWM_GPIO 4
#define MOTOR_BL_DIR_GPIO 5
#define MOTOR_BR_PWM_GPIO 21
#define MOTOR_BR_DIR_GPIO 20

AccelStepper stepperFL(AccelStepper::DRIVER, MOTOR_FL_PWM_GPIO, MOTOR_FL_DIR_GPIO);
AccelStepper stepperBL(AccelStepper::DRIVER, MOTOR_BL_PWM_GPIO, MOTOR_BL_DIR_GPIO);
AccelStepper stepperFR(AccelStepper::DRIVER, MOTOR_FR_PWM_GPIO, MOTOR_FR_DIR_GPIO);
AccelStepper stepperBR(AccelStepper::DRIVER, MOTOR_BR_PWM_GPIO, MOTOR_BR_DIR_GPIO);
AccelStepper *stepper[] = {
  &stepperFL, &stepperFR,
  &stepperBL, &stepperBR
};
float cmd_speed[] = {0, 0, 0, 0};
long pos[] = {0, 0, 0, 0};
int arrSize = *(&pos + 1) - pos;
IntervalTimer Interval;
IntervalTimer Interval_connect;
String incoming = "";

void setup() {
  Serial.begin(115200);
  Interval.begin(loop_1ms, 2500);//us
  Interval_connect.begin(checker_interval, 5000);//us
  for (int i = 0; i < arrSize; i++) {
    stepper[i]->setMaxSpeed(30000.0);
    stepper[i]->setSpeed(0.0);
  }
}
int counting_checker = 0;
void checker_interval() {
  if (counting_checker > 50) {
    for (int i = 0; i < arrSize; i++) {
      cmd_speed[i] = 0.00;
    }
  }
}

int task_100ms = 0;

void loop_1ms() {
  if (task_100ms++ >= 10) {
    task_100ms = 0;
    for (int i = 0; i < arrSize - 1; i++) {
      Serial.print(pos[i]);
      Serial.print(",");
    }
    Serial.println(pos[arrSize - 1]);
    counting_checker++;
  }
  for (int i = 0; i < arrSize; i++) {
    stepper[i]->setSpeed(cmd_speed[i]);
    pos[i] = stepper[i]->currentPosition();
  }
}

int count_pack = 0;
bool single_data = true;
void loop() {
  for (int i = 0; i < arrSize; i++) {
    stepper[i]->runSpeed();
  }

  if (Serial.available() > 0) {
    char c = Serial.read();
    if (c == 10 || c == 13) {
      cmd_speed[count_pack] = incoming.toFloat();
      incoming = String();
      single_data = true;
      counting_checker = int();
      count_pack = int();
    } else if (c == ',') {
      single_data = false;
      cmd_speed[count_pack] = incoming.toFloat();
      count_pack++;
      incoming = "";
    }
    else incoming += c;
  }
}
