#include "mpu9150.h"

#define TIME_CONST 0.3
#define THRESHOLD 0.7
#define I_CONST 0.002

static double acc_sensitivity = ACC_SENSITIVITY / 8.0;   /* default value 2048 LSB/g for full-range 16g */
static double gyro_sensitivity = GYRO_SENSITIVITY / 8.0; /* default value 16.4 LSB per °/s for full range 200°/s */

static uint8_t slave_address = MPU9150_ADDRESS;

/* variables for gyro angle calculation */
static uint8_t gyroinitangle_0 = 0;
static uint8_t gyroinitangle_1 = 0;

static xyz gyro_angles_0;
static double distance;
static xyz velocity;
static double total_velocity;

uint8_t counter;
uint8_t counter_2;

static unsigned int oldtime = 0;
static double DTtime;

static xyz old_1st_gyro_LPF_output;
static xyz old_2nd_gyro_LPF_output;
static xyz old_1st_acc_LPF_output;
static xyz old_2nd_acc_LPF_output;
static xyz old_turn_rate;
static xyz I;
static xyz acc_vect;

/*=====================================*/

/**
 * \fn
 * \brief
 * \param ado (0 or 1) select MPU9150 slave address
 * */

void mpu9150_setup(uint8_t Sensor, mpu9150_bandwidth_dlpf dlpf)
{
	uint8_t slave_address;

	volatile uint8_t buffer[2];
	/* external interrupt enable */

	DDRD &= 0 << PD2; // PD2 as input
	DDRD &= 0 << PD3; // PD3 as input
	//EIMSK= (1 << INT0) | (1 << INT1);         //Enable INT1/0
	// EICRA = 1<<ISC01 | 1 <<ISC00 |  1<<ISC11 | 1 <<ISC10;  //Set interrupt on rising edge

	twi_master_setup(); /* i2c to master mode */

	slave_address = MPU9150_ADDRESS + Sensor;

	/* reset entire device */
	buffer[0] = PWR_MGMT_1;
	buffer[1] = 0x80;
	twi_write_bytes(slave_address, 2, buffer);
	_delay_ms(100);

	/* out of sleep mode to start the sensor and set clock gyro */
	buffer[0] = PWR_MGMT_1;
	buffer[1] = 0x01;
	twi_write_bytes(slave_address, 2, buffer);

	/* configure digital low-pass filter (dlpf) bandwidth */
	buffer[0] = CONFIG;
	buffer[1] = dlpf;
	twi_write_bytes(slave_address, 2, buffer);

	/* enable data ready interrupt for gyroscope */
	buffer[0] = MPU9150_RA_INT_PIN_CFG;
	buffer[1] = 0x10;
	twi_write_bytes(slave_address, 2, buffer);

	buffer[0] = MPU9150_RA_INT_ENABLE;
	buffer[1] = 0x01;
	twi_write_bytes(slave_address, 2, buffer);
}
void mpu9150_acc_setup(mpu9150_acc_measurement_range mr, uint8_t Sensor)
{
	uint8_t slave_address;
	volatile uint8_t buffer[2];
	slave_address = MPU9150_ADDRESS + Sensor;
	/* Accelerometer configuration : full-scale range */
	buffer[0] = ACCEL_CONFIG;
	buffer[1] = (mr << 3);
	twi_write_bytes(slave_address, 2, buffer);

	acc_sensitivity = ACC_SENSITIVITY / (1 << mr); /* LSB/g */
}

/**
 * \fn void mpu9150_gyro_setup (mpu9150_gyro_measurement_range mr)
 * \brief setup gyroscope
 * \param mr measurement range (250, 500, 1000 or 2000°/s)
 * */
void mpu9150_gyro_setup(mpu9150_gyro_measurement_range mr, uint8_t Sensor)
{
	uint8_t slave_address;
	volatile uint8_t buffer[2];
	slave_address = MPU9150_ADDRESS + Sensor;
	/* Gyroscope configuration - full-scale range °/sec */
	buffer[0] = GYRO_CONFIG;
	buffer[1] = (mr << 3);
	twi_write_bytes(slave_address, 2, buffer);

	gyro_sensitivity = GYRO_SENSITIVITY; /// (1 << mr); /* LSB per °/sec */
	gyroinitangle_0 = 0;
	gyroinitangle_1 = 0;
	_delay_ms(1);
}

/*
 * \fn uint8_t mpu9150_read_device_id (void)
 * \brief read device id
 * \return device id
 * */
uint8_t mpu9150_read_device_id(uint8_t Sensor)
{
	uint8_t slave_address;
	volatile uint8_t device_id;
	volatile uint8_t reg = WHO_AM_I;
	slave_address = MPU9150_ADDRESS + Sensor;
	twi_read_bytes(slave_address, &reg, 1, &device_id);

	return device_id;
}

/*
 * \fn int16_t mpu9150_read_temperature (void)
 * \return temperature in degrees C = (TEMP_OUT signed / 340) + 35
 * */
int16_t mpu9150_read_temperature(uint8_t Sensor)
{
	uint8_t slave_address;
	volatile uint8_t buffer[2];
	volatile uint8_t reg = TEMP_OUT_H;
	slave_address = MPU9150_ADDRESS + Sensor;

	twi_read_bytes(slave_address, &reg, 2, buffer);

	int16_t temperature;
	temperature = ((buffer[0] << 8) + buffer[1]);
	temperature /= 340;
	temperature += 35;
	return temperature;
}

/**
 * \fn xyz mpu9150_read_acc_xyz (void)
 * \brief read x-, y-, z-axis acceleration
 * \return x, y and z-axis acceleration value in g
 * */
xyz mpu9150_read_acc_xyz_0(void)
{
	volatile uint8_t reg;
	volatile uint8_t buffer[6];
	xyz acc;
	slave_address = MPU9150_ADDRESS;

	reg = ACCEL_XOUT_H;

	twi_read_bytes(slave_address, &reg, 6, buffer);

	acc.x = (double)(((buffer[1] + (buffer[0] << 8)) / acc_sensitivity) - 0.00012);
	acc.y = (double)(((buffer[3] + (buffer[2] << 8)) / acc_sensitivity) - 0.01206);
	acc.z = (double)(((buffer[5] + (buffer[4] << 8)) / acc_sensitivity));

	return acc;
}

/**
 * \fn xyz mpu9150_read_gyro_xyz (void)
 * \brief read x-, y-, z-axis angular velocity
 * \return x, y and z-axis angular velocity value in °/sec */
xyz mpu9150_read_gyro_xyz_0(void)
{
	volatile uint8_t reg = GYRO_XOUT_H;
	volatile uint8_t buffer[4];
	xyz gyro;
	slave_address = MPU9150_ADDRESS;
	twi_read_bytes(slave_address, &reg, 4, buffer);

	gyro.x = (double)((((buffer[0] << 8) + buffer[1]) / gyro_sensitivity) - 0.044611);
	gyro.y = (double)((((buffer[2] << 8) + buffer[3]) / gyro_sensitivity) - 1.47755);
	// gyro.z = (double) ((((buffer[4] << 8) + buffer[5]) / gyro_sensitivity)+1.09071);

	return gyro;
}

xyz mpu9150_get_acc_angles_0(void)
{
	xyz angle;

	acc_vect = mpu9150_get_filtered_acc_xyz_0();

	angle.x = (180 / pi) * (atan2(acc_vect.x, sqrt(acc_vect.y * acc_vect.y + acc_vect.z * acc_vect.z))); //roll

	angle.y = (180 / pi) * (atan2(acc_vect.y, acc_vect.z)); //pitch

	angle.z = (180 / pi) * (atan2(sqrt(acc_vect.x * acc_vect.x + acc_vect.y * acc_vect.y), acc_vect.z)); //yaw

	return angle;
}

xyz mpu9150_get_gyro_angles_0()
{
	xyz newangle;
	xyz acc_angle;

	//Set the angle once
	if (gyroinitangle_0 == 0)
	{
		for (uint8_t i = 0; i < 100; i++)
		{
			acc_angle = mpu9150_get_acc_angles_0();
			gyro_angles_0.x += acc_angle.y;
			gyro_angles_0.y += acc_angle.x;
			gyro_angles_0.z += acc_angle.z;
		}

		gyroinitangle_0 = 1;

		gyro_angles_0.x /= -100;
		gyro_angles_0.y /= -100;
		gyro_angles_0.z /= -100;
	}
	else
	{
		newangle = mpu9150_get_filtered_gyro_xyz_0();

		//newtime=millis_get();
		// DTtime=(newtime-oldtime)*0.001;
		//oldtime=newtime;

		gyro_angles_0.x += (newangle.x * DTtime);
		gyro_angles_0.y += (newangle.y * DTtime);
		gyro_angles_0.z += (newangle.z * DTtime);
	}

	return gyro_angles_0;
}

uint8_t mpu9150_read_int_status(uint8_t Sensor)
{
	slave_address = MPU9150_ADDRESS + Sensor;
	volatile uint8_t status;
	volatile uint8_t reg = MPU9150_RA_INT_PIN_CFG;
	twi_read_bytes(slave_address, &reg, 1, &status);
	return status;
}

xyz mpu9150_get_complement_angles_0(double filter_gain)
{
	xyz comp_angles;

	gyro_angles_0 = mpu9150_get_gyro_angles_0();

	xyz acc_angles = mpu9150_get_acc_angles_0();

	comp_angles.x = filter_gain * gyro_angles_0.y + ((1 - filter_gain) * acc_angles.x * -1);
	comp_angles.y = filter_gain * gyro_angles_0.x + ((1 - filter_gain) * acc_angles.y * -1);
	return comp_angles;
}

xyz mpu9150_get_filtered_acc_xyz_0(void)
{
	/* LPF filter */
	xyz acc;
	acc = mpu9150_read_acc_xyz_0();

	acc.x = acc.x * TIME_CONST + (old_1st_acc_LPF_output.x + old_2nd_acc_LPF_output.x) * (1 - TIME_CONST);
	acc.y = acc.y * TIME_CONST + (old_1st_acc_LPF_output.y + old_2nd_acc_LPF_output.y) * (1 - TIME_CONST);
	acc.z = acc.z * TIME_CONST + (old_1st_acc_LPF_output.z + old_2nd_acc_LPF_output.z) * (1 - TIME_CONST);
	old_1st_acc_LPF_output.x = acc.x;
	old_1st_acc_LPF_output.y = acc.y;
	old_1st_acc_LPF_output.z = acc.z;
	old_2nd_acc_LPF_output.x = old_2nd_acc_LPF_output.x;
	old_2nd_acc_LPF_output.y = old_2nd_acc_LPF_output.y;
	old_2nd_acc_LPF_output.z = old_2nd_acc_LPF_output.z;

	return acc;
}

xyz mpu9150_get_filtered_gyro_xyz_0(void)
{
	xyz turn_rate;
	xyz I_gain;
	xyz abs_turnrate;
	xyz abs_old_turnrate;

	turn_rate = mpu9150_read_gyro_xyz_0();

	turn_rate.x = turn_rate.x * TIME_CONST + (old_1st_gyro_LPF_output.x + old_2nd_gyro_LPF_output.x) * (1 - TIME_CONST);
	turn_rate.y = turn_rate.y * TIME_CONST + (old_1st_gyro_LPF_output.y + old_2nd_gyro_LPF_output.y) * (1 - TIME_CONST);
	old_1st_gyro_LPF_output.x = turn_rate.x;
	old_1st_gyro_LPF_output.y = turn_rate.y;
	old_2nd_gyro_LPF_output.x = old_2nd_gyro_LPF_output.x;
	old_2nd_gyro_LPF_output.y = old_2nd_gyro_LPF_output.y;

	if (turn_rate.x < 0)
	{
		abs_turnrate.x = turn_rate.x * -1;
	}
	else
	{
		abs_turnrate.x = turn_rate.x;
	}

	if (turn_rate.y < 0)
	{
		abs_turnrate.y = turn_rate.y * -1;
	}
	else
	{
		abs_turnrate.y = turn_rate.y;
	}
	if (old_turn_rate.x < 0)
	{
		abs_old_turnrate.x = old_turn_rate.x * -1;
	}
	else
	{
		abs_old_turnrate.x = old_turn_rate.x;
	}

	if (old_turn_rate.y < 0)
	{
		abs_old_turnrate.y = old_turn_rate.y * -1;
	}
	else
	{
		abs_old_turnrate.y = old_turn_rate.y;
	}

	//*************I_GAIN*******************************************//

	if (abs_old_turnrate.x <= THRESHOLD) // 1deg/s is the filter threshold, anything bellow that is considered noise
	{
		I_gain.x = 1 - (abs_old_turnrate.x / THRESHOLD);
	}
	if (abs_old_turnrate.x > THRESHOLD)
	{
		I_gain.x = 0;
	}

	if (abs_old_turnrate.y <= THRESHOLD) // 1deg/s is the filter threshold, anything bellow that is considered noise
	{
		I_gain.y = 1 - (abs_old_turnrate.y / THRESHOLD);
	}
	if (abs_old_turnrate.y > THRESHOLD)
	{
		I_gain.y = 0;
	}

	//*******************************************************************//

	//***************************I controller***************************//
	if (old_turn_rate.x > 0)
	{
		I.x -= I_gain.x * I_CONST;
	}
	if (old_turn_rate.x < 0)
	{
		I.x += I_gain.x * I_CONST;
	}

	if (old_turn_rate.y > 0)
	{
		I.y -= I_gain.y * I_CONST;
	}
	if (old_turn_rate.y < 0)
	{
		I.y += I_gain.y * I_CONST;
	}

	//*************************OUTPUT**************************************//

	turn_rate.x += I.x;
	turn_rate.y += I.y;
	old_turn_rate = turn_rate;

	return turn_rate;
}

xyz mpu9150_get_compesated_acc_vectors(void)
{
	double scale = 0.001;
	double sin_acc_y;
	double sin_acc_x;
	mpu9150_get_gyro_angles_0();
	acc_vect = mpu9150_get_filtered_acc_xyz_0();

	sin_acc_x = sin((pi / 180) * gyro_angles_0.y);
	sin_acc_y = sin((pi / 180) * gyro_angles_0.x) - 0.056;

	acc_vect.x += sin_acc_x;
	acc_vect.y -= sin_acc_y;
	acc_vect.z -= cos((pi / 180) * gyro_angles_0.x) * cos((pi / 180) * gyro_angles_0.y);
	acc_vect.z += 0.04;

	acc_vect.x = (int)(acc_vect.x / scale) * scale;
	acc_vect.y = (int)(acc_vect.y / scale) * scale;
	acc_vect.z = (int)(acc_vect.z / scale) * scale;

	return acc_vect;
}

double mpu9150_get_distance(double timeframe)
{
	DTtime = timeframe;

	xyz acc_vect = mpu9150_get_compesated_acc_vectors();

	if (acc_vect.x > 0.015 || acc_vect.x < -0.015)
	{
		velocity.x += acc_vect.x * 9.80665 * DTtime;
	}

	if (acc_vect.y > 0.015 || acc_vect.y < -0.015)
	{
		velocity.y += acc_vect.y * 9.80665 * DTtime;
	}

	if (acc_vect.z > 0.015 || acc_vect.z < -0.015)
	{
		velocity.z += acc_vect.z * 9.80665 * DTtime;
	}
	serialWrite_double(velocity.x);
	serialWrite(" ");
	serialWrite_double(velocity.y);
	serialWrite(" ");
	serialWrite_double(velocity.z);
	serialWrite_newline();

	total_velocity = sqrt(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);

	distance += total_velocity * DTtime;

	return distance;
}

void mpu9150_param_reset(void)
{
	DTtime = 0;
	distance = 0;
	velocity.x = 0;
	velocity.y = 0;
	velocity.z = 0;
	total_velocity = 0;
	oldtime = 0;
	gyro_angles_0.x = 0;
	gyro_angles_0.y = 0;
	millis_reset();
}