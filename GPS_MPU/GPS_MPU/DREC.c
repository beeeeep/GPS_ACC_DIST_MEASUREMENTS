/*
* DREC.c
*
* Created: 17/4/2017 11:01:22 μμ
*  Author: Anestman
*/
#include "DREC.h"

static double mag_LPF;
static double mag_HPF;
static double mag_old;
static double velocity;
static double distance;
static double gyro_sum;
static uint8_t gyro_sample_no;
static uint8_t stationary;
static double turn_rate;

double DREC_get_LPF_gyro_x(double alpha)
{
	xyz gyro;

	gyro = mpu9150_read_gyro_xyz_0();

	turn_rate = turn_rate + alpha * (gyro.x - turn_rate);

	return turn_rate;
}

double DREC_get_filtered_acc_xyz_0(double mag, double LPF_alpha, double HPF_alpha)
{
	mag_LPF = mag_LPF + LPF_alpha * (mag - mag_LPF); //LPF 1st order

	mag_HPF = HPF_alpha * (mag_HPF + mag_LPF - mag_old); //HPF  1st order

	mag_old = mag_LPF;

	return mag_HPF;
}

double DREC_get_PDR_distance(double DT)
{
	double total_acc;
	double total_acc_f;
	double distance_t;
	double velocity_t;
	xyz acc_vect;

	acc_vect = mpu9150_read_acc_xyz_0();

	total_acc = sqrt(acc_vect.x * acc_vect.x + acc_vect.y * acc_vect.y + acc_vect.z * acc_vect.z);

	total_acc_f = DREC_get_filtered_acc_xyz_0(total_acc, 0.1358, 0.99); //Retrieve gravity filtered acc

	velocity_t = total_acc_f * 9.80665 * DT;
	velocity += velocity_t;

	DREC_detect_stationary();

	if (stationary)
	{
		velocity = 0;
	}
	if (velocity < 0)
	{
		velocity = 0;
	}

	distance_t = velocity * DT * 2;

	distance += distance_t;

	//***********************************************************//
	return distance;
}

void DREC_detect_stationary(void)
{
	double turn_rate;

	turn_rate = DREC_get_LPF_gyro_x(0.1358);

	if (gyro_sample_no <= 5)
	{
		gyro_sum -= turn_rate;
		gyro_sample_no++;
	}
	else
	{
		gyro_sum = abs(gyro_sum);
		gyro_sample_no = 0;

		if (gyro_sum <= 65)
		{
			stationary = 1;
		}
		else
		{
			stationary = 0;
		}
		gyro_sum = 0;
	}
}

void DREC_param_reset(void)
{
	distance = 0;
	velocity = 0;
}