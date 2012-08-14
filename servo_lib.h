#ifndef SERVO_LIB_H
#define SERVO_LIB_H

void servo_init(void);
void servo_write(int ANGLE);
void servo_timerHandlerFast(void);
void servo_timerHandlerSlow(void);

#endif