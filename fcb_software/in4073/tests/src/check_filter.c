/**
 * @file check_filter.c
 * @author Koen Wosten
 * @brief checks the butter worth filter
 * @version 0.1
 * @date 2022-06-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "check_check.h"

#include <stdlib.h>

#include <inttypes.h>
#include <check.h>
#include <fftw3.h>
#include "control.h"

#define N 1024
#define PI 3.141592653589793
#define Fs 200

// Fs = 1000Hz, FFT size, N = 1024. 1000/1024 = 0.9765625Hz per bin.
// 50/0.78125 ~ 51. So from bin 51 on I expect close to 0 values in magnitude.
START_TEST(test_filter_float2fix) {

	int res = Float2Fix(1.5f);

	int res_should = 8192 + pow(2, 14);

	ck_assert_int_eq(res, res_should);

} END_TEST

START_TEST(test_filter_float2fix_negative) {

	int res = Float2Fix(-1.5f);

	int res_should = -8192 - pow(2, 14);

	ck_assert_int_eq(res, res_should);

} END_TEST

START_TEST(test_filter_fix2float) {


	int integer = 8192 + pow(2, 14);

	float res = Fix2Float(integer);


	ck_assert_float_eq(res, 1.5f);

} END_TEST

START_TEST(test_filter_fix2float_negative) {


	int integer = -8192 - pow(2, 14);

	float res = Fix2Float(integer);


	ck_assert_float_eq(res, -1.5f);

} END_TEST

START_TEST(test_filter_cutoff) {
	Butterworth_init();

    float frequency = powf(((_i+1)),2);

	printf("\n frequency: %f\n", frequency);

	
    float xf[N];
	float yf[N];

    int x[N];
    int y[N];


	fftw_complex *in, *out, *original;
	fftw_plan p;

	in  = fftw_malloc(N * sizeof(fftw_complex));
	out = fftw_malloc(N * sizeof(fftw_complex));
	original = fftw_malloc(N * sizeof(fftw_complex));


    //generate sinusoid
    for(int i = 0; i < N; i++){
        xf[i] = sinf(2*PI*frequency/Fs * i); //200Hz sampling
		xf[i] += sinf(2*PI*3/Fs * i);
		xf[i] += sinf(2*PI*5/Fs * i);
		xf[i] += sinf(2*PI*17/Fs * i);
        yf[i] = 0;
    }

	//to fixed point
	for(int i = 0; i < N; i++){
        x[i] = Float2Fix(xf[i]); 
		y[i] = 0;
    }

	// printf("x: \n");
	// for(int i = 0; i < N; i++){
	// 	printf(" %3f", xf[i]);
	// }	
	// printf("\n ---- \n");

	//put in complex fftw stuff
	for(int i = 0; i < N; i++){
        in[i][0] = (double) xf[i];
		in[i][1] = 0;

    }

	p = fftw_plan_dft_1d(N, in, original, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);

	Butterworth_init();
    for(int i = 0; i < N; i++){
		y[i] = Butterworth_realize(0,x[i]);
	}

	// for(int i = 0; i < N; i++){
    //     y[i] = x[i] ; 
    // }
	
	//to float
	for(int i = 0; i < N; i++){
        yf[i] = Fix2Float(y[i]);
    }


	
	//put in complex fftw stuff
	for(int i = 0; i < N; i++){
        in[i][0] = (double) yf[i];
		in[i][1] = 0;

    }

	p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);



	// printf("in: \n");
	// for(int i = 0; i < N; i++){
	// 	//printf(" %3f", sqrt(pow(in[i][0],2) + pow(in[i][1],2)));
	// 	printf(" %3f", in[i][0]);

	// }
	// // printf("out: \n");
	// // for(int i = 0; i < N; i++){
	// // 	printf(" %3f", sqrt(pow(out[i][0],2) + pow(out[i][1],2)));
	// // }
	// // printf("\n ----- \n ");


	for(int i = 450; i < N/2; i++){ //after about 90Hz should be close to zero
		ck_assert((sqrt(pow(out[i][0],2) + pow(out[i][1],2)) / sqrt(pow(original[i][0],2) + pow(original[i][1],2))) < 0.5 || sqrt(pow(out[i][0],2) + pow(out[i][1],2)) < 5);
	}

	fftw_free(in);
	fftw_free(out);
	fftw_free(original);

	fftw_cleanup();

} END_TEST


Suite *make_suite(void) {
	Suite *s;
	TCase *tc_core;

	s = suite_create("Filter");
	tc_core = tcase_create("Filter");

	tcase_add_test(tc_core, test_filter_float2fix);
	tcase_add_test(tc_core, test_filter_float2fix_negative);
	tcase_add_test(tc_core, test_filter_fix2float);
	tcase_add_test(tc_core, test_filter_fix2float_negative);
	tcase_add_loop_test(tc_core, test_filter_cutoff, 0, 10);

	suite_add_tcase(s, tc_core);

	return s;
}
