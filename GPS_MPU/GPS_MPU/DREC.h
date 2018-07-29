/*
 * DREC.h
 *
 * Created: 17/4/2017 11:00:08 μμ
 *  Author: Anestman
 */

#ifndef DREC_H_
#define DREC_H_

#include "mpu9150.h"
#include "imu.h"
#include "txSerial.h"

double DREC_get_filtered_acc_xyz_0(double mag, double LPF_alpha, double HPF_alpha);

double DREC_get_PDR_distance(double DT);

void DREC_param_reset(void);

void DREC_detect_stationary(void);

#endif /* DREC_H_ */