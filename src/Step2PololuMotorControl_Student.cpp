/*
 * disclaimer:
 * The implementation choices in this file are made to improve the focus of the given lecture session.
 * They are not universally best design or implementation choices for all projects or production code.
 * Please consider this as a pedagogical scaffold, not a reference for production grade implementation
 */

#include <Arduino.h>
#include "Step2PololuMotorControl.h"

// TODO 1: Assign the correct pin for Pololu PWMA
const int POLOLU_PWMA = 38;
// TODO 2: Assign the correct pin for Pololu AIN1
const int POLOLU_AIN1 = 39;
// TODO 3: Assign the correct pin for Pololu AIN2
const int POLOLU_AIN2 = 40;
// TODO 4: Assign the correct pin for Pololu STBY
const int POLOLU_STBY = 16;

// TODO 5: Choose a PWM channel (0-15)
const int MOTOR_PWM_CHANNEL = 1;
// TODO 6: Experiment with suitable frequency for motor control
const int MOTOR_PWM_FREQ_HZ = 1000;
// TODO 7: Experiment with suitable resolution for motor control
const int MOTOR_PWM_RES = 8;
TaskHandle_t motorTaskHandle = NULL;

// Direction enum
enum RotationDirection_e { DIR_CW = 0, DIR_CCW = 1 };


// Static function prototypes
// Configure PWM and set all Pololu control pins as outputs
static void PololuDriver_HwInit(void);
// Set motor direction 
static void PololuDriver_SetDirection(RotationDirection_e direction);
// short brake
static void PololuDriver_ShortBrake(void);
// Enable the motor driver 
static void PololuDriver_Enable(void);
// Disable the motor driver 
static void PololuDriver_Disable(void);
// Set the PWM duty cycle for motor speed control
static void PololuDriver_UpdateDutyCycle(uint8_t duty);

// Main task loop:
static void TaskLoop(void* args);

// TODO 8: Implement PololuDriver_HwInit to configure PWM and IO pins
void PololuDriver_HwInit() 
{
  pinMode(POLOLU_AIN1, OUTPUT);
  pinMode(POLOLU_AIN2, OUTPUT);
  pinMode(POLOLU_STBY, OUTPUT);

  ledcSetup(MOTOR_PWM_CHANNEL, MOTOR_PWM_FREQ_HZ, MOTOR_PWM_RES);
  ledcAttachPin(POLOLU_PWMA, MOTOR_PWM_CHANNEL);

  digitalWrite(POLOLU_AIN1, LOW);
  digitalWrite(POLOLU_AIN2, LOW);
  digitalWrite(POLOLU_STBY, LOW);
  ledcWrite(MOTOR_PWM_CHANNEL, 0);
}

// TODO 9: Implement PololuDriver_SetDirection to set AIN1/AIN2 for direction
void PololuDriver_SetDirection(RotationDirection_e direction) 
{
  if(direction == DIR_CW)
  {
    digitalWrite(POLOLU_AIN1, HIGH);
    digitalWrite(POLOLU_AIN2, LOW);
  }
  else
  {
    digitalWrite(POLOLU_AIN1, LOW);
    digitalWrite(POLOLU_AIN2, HIGH);
  }
}

// TODO 10: Implement PololuDriver_Enable to set STBY HIGH
void PololuDriver_Enable()
{ 
  digitalWrite(POLOLU_STBY, HIGH);
}
// TODO 11: Implement PololuDriver_Disable to set STBY LOW
void PololuDriver_Disable()
{ 
  digitalWrite(POLOLU_STBY, LOW);
}

// TODO 12: Implement PololuDriver_UpdateDutyCycle to set PWM duty cycle
void PololuDriver_UpdateDutyCycle(uint8_t duty) 
{
  ledcWrite(MOTOR_PWM_CHANNEL, duty);
}

// TODO 13: Implement PololuDriver_ShortBrake 
void PololuDriver_ShortBrake() 
{
  digitalWrite(POLOLU_AIN1, HIGH);
  digitalWrite(POLOLU_AIN2, HIGH);
  ledcWrite(MOTOR_PWM_CHANNEL, 0);
}

// TODO 14: Implement TaskLoop to ramp motor up/down and reverse direction
void TaskLoop(void* args) 
{
  // TODO 14a: Initialize required HW resources
  PololuDriver_HwInit();
  // TODO 14b: Make sure pololu driver is enabled
  PololuDriver_Enable();
  // TODO 14c: Set an initial movement direction
  RotationDirection_e direction = DIR_CW;
  PololuDriver_SetDirection(direction);

  const uint8_t targetDuty = 100;
  const uint8_t stepDuty = 5;
  const TickType_t rampDelay = pdMS_TO_TICKS(30);
  const TickType_t holdRunDelay = pdMS_TO_TICKS(2500);
  const TickType_t holdStillDelay = pdMS_TO_TICKS(200);
 
  for(;;)
  {
    // TODO 15: Ramp up duty cycle from 0 to a target you choose
    for(uint8_t duty = 0; duty <= targetDuty; duty = (uint8_t)(duty + stepDuty))
    {
      PololuDriver_UpdateDutyCycle(duty);
      vTaskDelay(rampDelay);
      if(duty >= (uint8_t)(targetDuty - stepDuty))
      {
        break;
      }
    }

    // TODO 16: Keep at target speed for a while
    PololuDriver_UpdateDutyCycle(targetDuty);
    vTaskDelay(holdRunDelay);

    // TODO 17: Ramp down duty cycle from target to 0
    for(int duty = targetDuty; duty >= 0; duty -= stepDuty)
    {
      PololuDriver_UpdateDutyCycle((uint8_t)duty);
      vTaskDelay(rampDelay);
    }

    // TODO 18: Brake to stop
    PololuDriver_ShortBrake();
    vTaskDelay(pdMS_TO_TICKS(100));
    PololuDriver_UpdateDutyCycle(0);

    // TODO 19: Keep at still for a while
    vTaskDelay(holdStillDelay);

    // TODO 20: Reverse direction for next cycle
    direction = (direction == DIR_CW) ? DIR_CCW : DIR_CW;
    PololuDriver_SetDirection(direction);

  }
}

// TODO 0: Implement Step2PololuMotor_CreateTask to create the FreeRTOS task for TaskLoop
bool Step2PololuMotor_CreateTask(void)
{
  bool result = false;
  BaseType_t taskCreateResult = xTaskCreate(
    TaskLoop,
    "PololuMotorTask",
    2048,
    NULL,
    1,
    &motorTaskHandle
  );
  result = (taskCreateResult == pdPASS);
  return result;
}
