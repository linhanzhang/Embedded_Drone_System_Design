#ifndef CONTROL_H_
#define CONTROL_H_

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define BOUND(X, Y, Z) (MAX((X), MIN((Y), (Z))))



#define MAX_MOTOR_VALUE 700

#define MOTOR_OFFSET 185

#include <inttypes.h>
#include <stdbool.h>

extern uint16_t motor[4];
//extern int16_t ae[4];
extern bool wireless_mode;
extern struct kf kf;

typedef struct Pid
{
	int16_t setValue;
	int16_t actualValue;
	int16_t actualValueCascaded;
	int16_t err;
    int16_t err_cascaded;
	int16_t Kp, Kp_cascaded, Kd;
	int16_t err_last;
}Pid;

void update_motors(void);
void motors_enable(void);
void motors_disable(void);
void motors_calc_moments(int16_t moments[3], int16_t s_phi, int16_t s_theta, int16_t s_psi);
void motors_set_momentum(int16_t roll, int16_t pitch, int16_t yaw, uint16_t lift);
uint8_t fast_sqrt_c(uint16_t n);
void pid_init();
int16_t pd_control(int16_t setValue, int16_t actualValue, struct Pid pid);
int16_t cascaded_p_control(int16_t setValue, int16_t actual_s_speed, int16_t actual_s_angle, struct Pid pid);


extern int32_t fil_p;
extern int32_t fil_q;
extern int32_t fil_phi;
extern int32_t fil_theta;

#define WINDOWS 3 // window size of butterworth filter, second-order = 3
#define NUM_PARAMETER 6 // window size of butterworth filter, second-order = 3
struct kf{
    float P2PHI;
    int32_t PHI2P;            
    int32_t C1;
    int32_t C2;
    int32_t p;
    int32_t bias_p;
    int32_t phi;
    int32_t e_p;
    int32_t q;
    int32_t bias_q;
    int32_t theta;
    int32_t e_q;
};

struct bw{
    int32_t x_butter[NUM_PARAMETER][WINDOWS];            
    int32_t y_butter[NUM_PARAMETER][WINDOWS];
    int32_t a[WINDOWS];
    int32_t b[WINDOWS];
};

void KF_init(int32_t sp_offset, int32_t sq_offset);
void KF_realize(int32_t sp,int32_t say,int32_t sq,int32_t sax);
void Butterworth_init();
int32_t Butterworth_realize(int16_t param_index, int32_t sensor_value);

int32_t Float2Fix(float x);
int32_t Fixpoint_multiply(int32_t x, int32_t y);
double Fix2Float(int32_t x);

#endif /* CONTROL_H_ */
