#include "transform.h"
#include "motor.h"

float distance = 0;
float angle = 0;
bool flag_rotation_set = false;
bool flag_translation_set = false;


void Translation_Set(float velocity ,float dis){
   
    distance = abs(dis);
    flag_translation_set = true;
    if(dis > 0)
        Motor_Translation_Set(velocity, 0);
    else
        Motor_Translation_Set(-velocity, 0);
}


void Rotation_Set(float omega, float ang){
    angle = abs(ang);
    flag_rotation_set = true;
    if(ang > 0)
        Motor_Translation_Set(0, omega);
    else
        Motor_Translation_Set(0, -omega);
}   

void Rotation_Caculate(float omega, float time){
    if(!flag_rotation_set)
        return;

    // Serial.print("Angle: ");
    // Serial.println(angle);
    // Serial.print("Omega: ");
    // Serial.println(omega);
    // Serial.print("Time: ");
    // Serial.println(time);
    if(angle <= 0){
        Motor_Translation_Set(0, 0);
        angle = 0;
        flag_rotation_set = false;
    }
    angle = angle - abs(omega) * time;
}

void Translation_Caculate(float velocity, float time){
    if(!flag_translation_set)
        return;

    if(distance <= 0){
        Motor_Translation_Set(0, 0);
        distance = 0;
        flag_translation_set = false;
    }
    distance = distance - abs(velocity) * time;
}

