#ifndef _TRANSFORM_H
#define _TRANSFORM_H
#include <Arduino.h>

void Translation_Set(float velocity, float dis);
void Rotation_Set(float omega, float ang);
void Translation_Caculate(float velocity, float time);
void Rotation_Caculate(float omega, float time);

#endif