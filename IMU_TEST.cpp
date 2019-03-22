#include <iostream>
#include <vector>
#include <stdio.h>
#include <sidekiq_api.h>
#include <cmath>
#include<algorithm>
#include <unistd.h>
using namespace std;
delta time should be 10 ms
#define DELTA_TIME 10
#define GYRO_CONST 0.98
#define ACCEL_CONST 0.02
#define PULL_NUMBER 360000
#define SPLIT_MARKER 5
#define FSR 1000 //FSR defined in gyroscope config
#define GYRO_CONFIG 16

/*
so basically the entire process in int main can run when there are enough received values from
the accel to match the SPLIT_MARKER and to produce a median filter. Whenever a median filter cannot be produced
the "ready" variable is set to 0 so the other functions do not try to run without a median of the accel values
*/
int ready;

float compFilter(float accel_data, float gyro_data)
{
	return GYRO_CONST * gyro_data + ACCEL_CONST * accel_data;
}

int median(vector<double> arr)
{
	if (arr.size() < SPLIT_MARKER)
	{
		ready = 0;
	}
	else
	{
		if (arr.size() >= (SPLIT_MARKER + 1))
		{
			arr.clear();
		}
		else
		{
			if (SPLIT_MARKER % 2 != 0)
			{
				sort(arr.end() - SPLIT_MARKER, arr.end());
				return arr[arr.size() / 2];
				arr.clear();
			}
			else
			{
				sort(arr.end() - SPLIT_MARKER, arr.end());
				return ((arr[arr.size() / 2 - 1]) + arr[arr.size() / 2 - 1]) / 2;
				arr.clear();
			}
		}
	}
}

int read_imu(uint8_t card, uint8_t reg) //reads the raw values
{
	int32_t status = 0;
	uint8_t low_byte = 0, high_byte = 0;
	int result;
	status = skiq_read_accel_reg(card, reg, &high_byte, 1);
	if (status == 0) status = skiq_read_accel_reg(card, reg + 1, &low_byte, 1);
	if (status == 0)
	{
		result = (((int)high_byte) << 8) | low_byte;
		if (result >= 0x8000) result = result - 0x10000;
		return result;
	}
}
int main(int argc, char *argv[])
{
	uint_8t card = 0;
	vector<float> acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z; //raw_values
	float median_ax, median_ay, median_az, median_gx, median_gy, median_gz = 0;
	float angle_ax, angle_ay, angle_az = 0;
	float angle_gx, angle_gy, angle_gz = 0;
	float finalAngle_x, finalAngle_y, finalAngle_z = 0;

	//this is the config code that changes the Gyro Config register to 1000dps
	uint8_t config_byte = 0;
	uint8_t right_side = 0, left_side = 0;
	skiq_read_accel_reg(card, 0x1B, &config_byte, 1);
	left_side = config_byte >> 5;
	left_side = config_byte << 5;
	right_side = config_byte & 7;
	left = left | right;
	left = left | GYRO_CONFIG;
	skiq_write_accel_reg(card,0x1B, &config_byte,1);

	for (int i = 0; i < PULL_NUMBER; i++) // 100HZ of data samples for 1 hr
	{
		acc_x.push_back(read_imu(card, 0x3b));
		acc_y.push_back(read_imu(card, 0x3d));
		acc_z.push_back(read_imu(card, 0x3f));
		gyro_x.push_back(read_imu(card, 0x43)* FSR / (pow(2,15)-1);
		gyro_y.push_back(read_imu(card, 0x45)* FSR / (pow(2,15)-1);
		gyro_z.push_back(read_imu(card, 0x47)* FSR / (pow(2,15)-1);

		//median filter for accel
		median_ax = median(acc_x);
		median_ay = median(acc_y);
		median_az = median(acc_z);

		//arctan A for accel
		if (ready != 0)
		{
			angle_ax += atan2(median_ax, median_az);
			angle_ay += atan2(median_ay, median_az);
			angle_az += atan2(median_az, median_ay);
		}

		//integrate gyro values into angle
		if (ready != 0)
		{
			if (i != 0)
			{
				angle_gx += (gyro_x[i] + finalAngle_x) * DELTA_TIME;
				angle_gy += (gyro_y[i] + finalAngle_y) * DELTA_TIME;
				angle_gz += (gyro_z[i] + finalAngle_z) * DELTA_TIME;
			}
		}


		//complimentary filter
		if (ready != 0)
		{
			finalAngle_x += compFilter(angle_gx, angle_ax);
			finalAngle_y += compFilter(angle_gy, angle_ay);
			finalAngle_z += compFilter(angle_gz, angle_az);
		}

		//output for excel data table -> graph.
		fstream data("output.csv")
		data << "Accel X, Accel Y, Accel Z, Gyro X, Gyro Y, Gyro Z, Delta Theta X, Delta Theta Y, Delta Theta Z, Final Angle X, Final Angle Y, Final Angle Z" << endl;
		for(int i = 0; i < 49; i++)
		{
			data << ("%f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f", acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z, angle_gx, angle_gy, angle_gz, finalAngle_x, finalAngle_y, finalAngle_z) << endl
		}
		usleep(DELTA_TIME);
	}
}
