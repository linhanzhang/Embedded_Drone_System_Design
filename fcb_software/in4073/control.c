/*------------------------------------------------------------------
 *  control.c -- here you can implement your control algorithm
 *		 and any motor clipping or whatever else
 *		 remember! motor input =  0-1000 : 125-250 us (OneShot125)
 *
 *  Koen Wosten
 *  I. Protonotarios
 *  Embedded Software Lab
 *
 *  July 2016
 *------------------------------------------------------------------
 */
#include "control.h"

#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdbool.h>
#include "state/state.h"

//#define TESTING



static inline uint16_t range(int min, uint8_t val, int max);
extern struct state state;

uint16_t motor[4];
bool wireless_mode;
bool motors_enabled = false;


#define MASS 1

#define B_CONST 1
#define D_CONST 1
#define YAW_CONST 4

#define HOVER_VALUE 440
#define AE_ROOT (((HOVER_VALUE - MOTOR_OFFSET)*255)/(MAX_MOTOR_VALUE-MOTOR_OFFSET))
#define MG_Z (AE_ROOT*AE_ROOT * 2 * B_CONST)
#define MG_Z_SCALED (0.00006593406*MG_Z) 
//#define MG_Z_SCALED ((0.012*MG_Z)/182)


/**
 * @brief Update motor values.
 * 
 * Updates the motor values, if they are enabled, and bounds them to MAX_MOTOR_VALUE
 * 
 * @author Koen Wosten
 */
void update_motors(void)
{
	if(motors_enabled){
		motor[0] = BOUND(0,  state.ae[0], MAX_MOTOR_VALUE);
		motor[1] = BOUND(0,  state.ae[1], MAX_MOTOR_VALUE);
		motor[2] = BOUND(0,  state.ae[2], MAX_MOTOR_VALUE);
		motor[3] = BOUND(0,  state.ae[3], MAX_MOTOR_VALUE);
	} else {
		motor[0] = 0;
		motor[1] = 0;	
		motor[2] = 0;
		motor[3] = 0;
	}
}

/**
 * @brief Ranges the the given value linearly between min and max.
 * 
 * val = 0 -> min
 * val = 255 -> max
 * 
 * @author Koen Wosten
 * 
 * @param min minimum value
 * @param val value
 * @param max maximum value
 * @return uint16_t 
 */
static uint16_t range(int min, uint8_t val, int max){
	return (val * (max-min)/255 + min);
}


/**
 * @brief disables motors, and sets them to 0.
 * 
 * @author Koen Wosten
 */
void motors_disable()
{
	motors_enabled = false;

	for (int i = 0; i < 4; i++)
	{
		state.ae[i] = 0;
		motor[i] = 0;
	}
}

/**
 * @brief enable motors, afterwards they can be updated
 * 
 * @author Koen Wosten
 */
void motors_enable()
{
	motors_enabled = true;
}

/**
 * @brief Calculates the required momements, to set the drone at specified angles.
 * 
 * 
 *  Only valid for angles up to about +/i 20 degress for phi and theta, psi doesn't really matter
 * 
 * @author Koen Wosten
 * 
 * @param moments 
 * @param s_phi 
 * @param s_theta 
 * @param s_psi 
 * @return * void 
 */
void motors_calc_moments(int16_t moments[3], int16_t s_phi, int16_t s_theta, int16_t s_psi){
	moments[0] = BOUND(-32767, s_phi * MG_Z_SCALED, 32767);
	moments[1] = BOUND(-32767, s_theta * MG_Z_SCALED, 32767);
	moments[2] = BOUND(-32767, s_psi * YAW_CONST, 32767); 
}

/**
 * @brief Sets the given signal to the right values, for the given roll, pitch, lift and yaw.
 * 
 * @author Koen Wosten
 * 
 * @param roll roll
 * @param pitch pitch	
 * @param yaw yaw
 * @param lift lift
 */
void motors_set_momentum(int16_t L, int16_t M, int16_t N, uint16_t Z){

	if (Z > 0) {

		// ORIGINAL
		// int a = yaw  / (D_CONST * 4);
		// int b = pitch / (B_CONST * 2);
		// int c = -lift / (B_CONST * 4); //since lift is pointing down, 
		// int d = roll / (B_CONST * 2);


		// Makes max use of possible range
		int16_t a = -Z / (B_CONST * 2); //since lift is pointing down, 
		
		int16_t b = BOUND(a/2,L / (B_CONST), -a/2);
		int16_t c = BOUND(a/2,M / (B_CONST ), -a/2);
		int16_t d = BOUND(a/2,N  / (D_CONST * 2),  -a/2);


		uint16_t ae1_sq = BOUND(0,  -d +  c - a, 65535); //max uint16 (asuming D_CONST, B_CONST > 1) //  - N / 4d + M / 2B +  Z / c4
		uint16_t ae2_sq = BOUND(0,   d -  b - a, 65535);
		uint16_t ae3_sq = BOUND(0,  -d -  c - a, 65535);
		uint16_t ae4_sq = BOUND(0,   d +  b - a, 65535); // max -2c = lift/(B_CONST) =  65535/2 = 32767 or 32768. 

		// sqrt(65535) = 255 is max ae value

		uint8_t ae1 = fast_sqrt_c(ae1_sq);
		uint8_t ae2 = fast_sqrt_c(ae2_sq);
		uint8_t ae3 = fast_sqrt_c(ae3_sq);
		uint8_t ae4 = fast_sqrt_c(ae4_sq);

		//range back to MAX_MOTOR_VALUE
		state.ae[0] = range(MOTOR_OFFSET, ae1, MAX_MOTOR_VALUE);
		state.ae[1] = range(MOTOR_OFFSET, ae2, MAX_MOTOR_VALUE);
		state.ae[2] = range(MOTOR_OFFSET, ae3, MAX_MOTOR_VALUE);
		state.ae[3] = range(MOTOR_OFFSET, ae4, MAX_MOTOR_VALUE);

	} else {
		state.ae[0] = 0;
		state.ae[1] = 0;
		state.ae[2] = 0;
		state.ae[3] = 0;
	}

}
/**
 * @brief fast square root
 * 
 * Implementation of the "Fast Integer Square Root" algorithm (described by Ross M. Fossler in Microchip's TB040)
 * 
 * @author CPallini
 * All credit for this function to CPallini
 * https://www.codeproject.com/Tips/5290784/Fast-Integer-Square-Root-For-8051
 * 
 * Protected under: The Code Project Open License (CPOL) 1.02
 * 
 */
uint8_t fast_sqrt_c(uint16_t n)
{
	uint8_t g = 0x80; // initial guess
	uint8_t b = 0x80; // 'additional' bit is in position 7
	for (;;)
	{
		uint16_t g2 = g * g;
		if (g2 == n)
			break; // an exact match is found
		if (g2 > n)
			g ^= b; // here g*g > n, remove the added bit
		b >>= 1;	// shift right the 'addional' bit
		if (b == 0)
			break; // the 'additional' bit was shifted out, the iteration is complete
		g |= b;	   // add the current 'additional bit'
	}
	return g;
}


/*------------------------------------------------------------------
 *  cascaded P control and PD control
 *
 *  Tianrui Mao
 *
 *  16 May 2022
 *------------------------------------------------------------------
 */
void pid_init(struct Pid pid)
{
	//printf("pidD init begin \n");
	pid.err = 0;
	pid.setValue = 0;
	pid.actualValue = 0;
	pid.err_last = 0;
	//Kp_cascaded>=4Kp
	pid.Kp = 2;
	pid.Kp_cascaded = 8;
	pid.Kd = 1;
	//printf("pidD init end \n");
}




int16_t pd_control(int16_t setValue, int16_t actualValue, struct Pid pid)
{
	pid.actualValue = actualValue;
	pid.setValue = setValue;
	pid.err = pid.setValue - pid.actualValue;
	//no sure if the data type transform will work or not
	pid.actualValue = (int16_t)(pid.Kp * pid.err + pid.Kd * (pid.err - pid.err_last));
	pid.err_last = pid.err;
	return pid.actualValue;
	//pid.actualValue is value to be used e.g. new_yaw
}


/**
 * @brief Sets the given signal to the right values, for the given roll, pitch, lift and yaw.
 * 
 * @param setValue 	target value 
 * @param actual_s_speed  sensor value of speed: sp,sq
 * @param actual_s_angle  sensor value of angle: sphi, stheta
 */

// NOT sure if the controller is correct or not, need furthur check
int16_t cascaded_p_control(int16_t setValue, int16_t actual_s_speed, int16_t actual_s_angle, struct Pid pid)
{
	int16_t intermediate_setValue = 0;
	int16_t last_setValue = 0;
    pid.setValue = setValue;
	pid.actualValue = actual_s_angle;
	pid.actualValueCascaded = actual_s_speed;
	pid.err = pid.setValue - pid.actualValue;
	intermediate_setValue = (int16_t)((pid.Kp * pid.err)/32); //32 is a scalar for the P values
	pid.err_cascaded = intermediate_setValue - pid.actualValueCascaded;
	last_setValue = (int16_t)((pid.Kp_cascaded*pid.err_cascaded)/32); //not sure, may need to multiply by map_engine
	return last_setValue;
	//pid.actualValue is value to be used e.g. new_yaw
}

/*------------------------------------------------------------------
 *  Float2Fix arithmetic
 *
 *  Tianrui Mao
 *
 *  28 May 2022
 *------------------------------------------------------------------
 */
#define FIXED_POINT_FRACTIONAL_BITS 14
int32_t Float2Fix(float x) {
    int32_t y = x * (1 << FIXED_POINT_FRACTIONAL_BITS);
    return y;
}

int32_t Fixpoint_multiply(int32_t x, int32_t y) {
    int32_t multiplication = (x * (int64_t) y) >> 14; 
    return multiplication;
}

double Fix2Float(int32_t x)
{
	double y = ((double) x) / (1 << FIXED_POINT_FRACTIONAL_BITS);
	return y;
}


struct kf kf;
struct bw bw;

int32_t fil_p;
int32_t fil_q;
int32_t fil_phi;
int32_t fil_theta;
/*------------------------------------------------------------------
 *  Kalman filter and Butterworth filter
 *
 *  Tianrui Mao
 *
 *  3 June 2022
 *------------------------------------------------------------------
 */
/**
 * @brief Initialized the Kalman Filter
 * 
 * @param sp_offset sp_offset, is in integer representation
 * @param sq_offset sq_offset, is in integer representation
 */
void KF_init(int32_t sp_offset, int32_t sq_offset){

    kf.P2PHI = 0.01;//not sure how to define this value now  // can be removed later?
	kf.PHI2P = 100;
    //C2 should be much bigger than C1
    kf.C1 = 60;              //60;
    kf.C2 = 800000;        //800000;
    //init bias from calibration???????to be discussed
	//currently initialization from 0, to be determined in the future
    kf.bias_p = sp_offset << 14;
    kf.bias_q = sq_offset << 14;
    kf.p = 0;
    kf. phi = 0 ;
    kf.e_p = 0;
    kf.q = 0;
    kf. theta = 0 ;
    kf.e_q = 0;

}

/** 
 * @param sp input velocity
 * @param sax input acceleration
 * so do sq and stheta
 */
void KF_realize(int32_t sp,int32_t say,int32_t sq,int32_t sax){
    kf.p = sp - kf.bias_p;
    kf.phi = kf.phi + Fixpoint_multiply(Float2Fix(kf.P2PHI),kf.p);
    kf.e_p = kf.phi - say;
    kf.phi = kf.phi - kf.e_p/kf.C1;
    kf.bias_p = kf.bias_p + (kf.e_p*kf.PHI2P)/kf.C2;

    kf.q = sq - kf.bias_q;
    kf.theta = kf.theta + Fixpoint_multiply(Float2Fix(kf.P2PHI),kf.q);
    kf.e_q = kf.theta - sax;
    kf.theta = kf.theta - kf.e_q/kf.C1;
    kf.bias_q = kf.bias_q + (kf.e_p*kf.PHI2P)/kf.C2;
    // printf("This is filtered value:%d",kf.theta);
    // printf("\n");
    //This is the filtered value used for motor
    fil_p = kf.p;
    fil_q = kf.q;
    fil_phi = kf.phi;
    fil_theta = kf.theta;
}

/**
 * @brief Second-order butterworth filter initialization
 * @brief sr-sp-sq-sax-say = 0-1-2-3-4  
 */
void Butterworth_init() {
    //assuming our fs = 100, fc = 15
    bw.a[0] = Float2Fix(0.0674f);                    //Float2Fix(0.1311);
    bw.a[1] = Float2Fix(0.1349f);
	bw.a[2] = bw.a[0];
    bw.b[0] = Float2Fix(1.0f);
	bw.b[1] = Float2Fix(-1.1430f);
	bw.b[2] = Float2Fix(0.4128f);
	// bw.a[0] = Float2Fix(1.0);
    // bw.a[1] = Float2Fix(0.0);
	// bw.a[2] = Float2Fix(0.1715728);
    // bw.b[0] = Float2Fix(0.2928932);
	// bw.b[1] = Float2Fix(0.5857864);
	// bw.b[2] = Float2Fix(0.2928932);
	for(int i = 0; i < NUM_PARAMETER; i++)
	{
    for(int j = 0; j < WINDOWS; j++) 
	{
        bw.x_butter[i][j] = 0;
        bw.y_butter[i][j] = 0;
    }
	}
}


/**
 * @brief Butterworth filter realization
 * @param y_butter[param_index][0] means output value at current state
 * x0 = current state x1 = previous state
 * @brief sr-sp-sq-sax-say-saz = 0-1-2-3-4-5 
 */
 int32_t Butterworth_realize(int16_t param_index, int32_t sensor_value) {
    // read sensor(the sensor reading is regarded as sampling)
    bw.x_butter[param_index][0] = sensor_value;
    //printf("This is sensor value:%d",bw.x_butter[param_index][0]);
    //printf("\n");
    // apply butterworth
    bw.y_butter[param_index][0] = Fixpoint_multiply(bw.x_butter[param_index][0],bw.a[0]) + Fixpoint_multiply(bw.x_butter[param_index][1],bw.a[1]) + 
	Fixpoint_multiply(bw.x_butter[param_index][2],bw.a[2]) - Fixpoint_multiply(bw.y_butter[param_index][1],bw.b[1]) - 
	Fixpoint_multiply(bw.y_butter[param_index][2],bw.b[2]);
    //printf("This is filtered value:%d",bw.y_butter[param_index][0]);
    //printf("\n");
    // store the value in window
    for (int i = WINDOWS - 1; i > 0; i--) {
        bw.x_butter[param_index][i] = bw.x_butter[param_index][i-1];
        bw.y_butter[param_index][i] = bw.y_butter[param_index][i-1];
	}
    
    int32_t fil_sensor_value =  bw.y_butter[param_index][0];
     //printf("This is final value:%d",fil_sensor_value);
    //printf("\n");
    return fil_sensor_value;
}