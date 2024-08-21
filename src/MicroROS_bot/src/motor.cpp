#include "pinout.h"
#include "motor.h"
#include "pid.h"
#include "imu.h"

#define GEAR 234
#define L    230
#define r    36

const int freq = 1000;
const int motor_r_pwmChannel = 0;
const int motor_l_pwmChannel = 1;
const int resolution = 8;

volatile uint16_t encoder_freq_r = 0; 
volatile uint16_t encoder_freq_l = 0;
uint16_t motor_r_speed = 0;
uint16_t motor_l_speed = 0;

volatile unsigned long lastTime_r, lastTime_l = 0;

bool flag_motor_r = false;
bool flag_motor_l = false;
int r_motor_direction = 0, l_motor_direction = 0;

pid motor_r_pid(0.3, 0.1, 0.01);
pid motor_l_pid(0.3, 0.1, 0.01);


void Motor_ResetEncoder(void) {
  if(flag_motor_r == false) {
    encoder_freq_r = 0;
  }
  if(flag_motor_l == false){
    encoder_freq_l = 0;
  }
  flag_motor_r = false;
  flag_motor_l = false;
}

void IRAM_ATTR Encoder_R() {
    flag_motor_r = true;
    unsigned long currentTime = micros();
    uint16_t encoder_cycle_r = currentTime - lastTime_r;

    if(encoder_cycle_r > 0)
        encoder_freq_r = 1000000 / encoder_cycle_r;     // Encoder frequency
    
    lastTime_r = currentTime;
}

void IRAM_ATTR Encoder_L() {
    flag_motor_l = true;
    unsigned long currentTime = micros();
    uint16_t encoder_cycle_l = currentTime - lastTime_l;

    if(encoder_cycle_l > 0)
        encoder_freq_l = 1000000 / encoder_cycle_l;      // Encoder frequency

    lastTime_l = currentTime;
}

void Motor_Init(void){
    Serial.println("Motor Init!");
    /*
        Motor Right Configuration
    */
    pinMode(R_MOTOR_EN, OUTPUT);
    pinMode(R_MOTOR_INT_A, OUTPUT);
    pinMode(R_MOTOR_INT_B, OUTPUT);    
    ledcSetup(motor_r_pwmChannel, freq, resolution);
    ledcAttachPin(R_MOTOR_EN, motor_r_pwmChannel);
    ledcWrite(motor_r_pwmChannel, 0);

    /*
        Motor Left Configuration
    */
    pinMode(L_MOTOR_EN, OUTPUT);
    pinMode(L_MOTOR_INT_A, OUTPUT);
    pinMode(L_MOTOR_INT_B, OUTPUT);    
    ledcSetup(motor_l_pwmChannel, freq, resolution);
    ledcAttachPin(L_MOTOR_EN, motor_l_pwmChannel);
    ledcWrite(motor_l_pwmChannel, 0);

    pinMode(R_MOTOR_PWM_B, INPUT_PULLUP);
    pinMode(L_MOTOR_PWM_B, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(R_MOTOR_PWM_B), Encoder_R, FALLING);
    attachInterrupt(digitalPinToInterrupt(L_MOTOR_PWM_B), Encoder_L, FALLING);
}

void Motor_Forward(uint8_t motor, uint8_t speed){
    if(motor == R_MOTOR){
        // Serial.println("Motor R Forward");
        digitalWrite(R_MOTOR_INT_A, HIGH);
        digitalWrite(R_MOTOR_INT_B, LOW);
        ledcWrite(motor_r_pwmChannel, speed);
    }
    else if(motor == L_MOTOR){
        // Serial.println("Motor L Forward");
        digitalWrite(L_MOTOR_INT_A, LOW);
        digitalWrite(L_MOTOR_INT_B, HIGH);
        ledcWrite(motor_l_pwmChannel, speed);
    }
}

void Motor_Backward(uint8_t motor, uint8_t speed){
    if(motor == R_MOTOR){
        // Serial.println("Motor R Backward");
        digitalWrite(R_MOTOR_INT_A, LOW);
        digitalWrite(R_MOTOR_INT_B, HIGH);
        ledcWrite(motor_r_pwmChannel, speed);
    }

    else if(motor == L_MOTOR){
        // Serial.println("Motor L Backward");
        digitalWrite(L_MOTOR_INT_A, HIGH);
        digitalWrite(L_MOTOR_INT_B, LOW);
        ledcWrite(motor_l_pwmChannel, speed);
    }
}

void Motor_Translation_Controller(void){

    float pwm_r = motor_r_pid.calculate(motor_r_speed, encoder_freq_r);
    float pwm_l = motor_l_pid.calculate(motor_l_speed, encoder_freq_l);
    
    if(motor_l_speed < 10)
        pwm_l = 0;

    if(motor_r_speed < 10)
        pwm_r = 0;
    

    if(r_motor_direction > 0){
        Motor_Forward(R_MOTOR, pwm_r);
    }
    else if(r_motor_direction < 0){
        Motor_Backward(R_MOTOR, pwm_r);
    }

    if (l_motor_direction > 0)
    {
        Motor_Forward(L_MOTOR, pwm_l);
    }
    else if (l_motor_direction < 0)
    {
        Motor_Backward(L_MOTOR, pwm_l);
    }
    
}

void Motor_Translation_Set(float velocity, float omega){
    float v_r = velocity + omega * L;
    float v_l = velocity - omega * L;

    if(v_r >= 0){
        r_motor_direction = 1;
    }
    else{
        r_motor_direction = -1;
    }

    if (v_l >= 0)
    {
        l_motor_direction = 1;
    }
    else
    {
        l_motor_direction = -1;
    }
    
    motor_r_speed = (uint16_t)abs( (v_r / (2 * 3.14 * r)) * GEAR);
    motor_l_speed = (uint16_t)abs( (v_l / (2 * 3.14 * r)) * GEAR);

    motor_r_pid.reset();
    motor_l_pid.reset();
}


float Motor_Velocity_Get(void){
    float v_r = (float) ( r_motor_direction * r * (2 * 3.14 * encoder_freq_r / 234));
    float v_l = (float) ( l_motor_direction * r * (2 * 3.14 * encoder_freq_l / 234));
    return (float) (v_l + v_r) / 2;
}

float Motor_Omega_Get(void){
    float v_r = (float) ( r_motor_direction * r * (2 * 3.14 * encoder_freq_r / 234));
    float v_l = (float) ( l_motor_direction * r * (2 * 3.14 * encoder_freq_l / 234));
    return (float) (v_r - v_l) / (2 * 230);
}


void controlMotors(float linear_vel, float angular_vel) {
    // Sử dụng hàm Motor_Translation_Set để thiết lập vận tốc và omega cho động cơ
    Motor_Translation_Set(linear_vel, angular_vel);
    // Sau đó sử dụng hàm Motor_Translation_Controller để điều khiển động cơ theo các giá trị đã đặt
    Motor_Translation_Controller();
}