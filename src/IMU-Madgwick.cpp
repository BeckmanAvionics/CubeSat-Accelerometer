#include <iostream>
#include <vector>
#include <stdio.h>
#include "../../sidekiq_core/inc/sidekiq_api.h"
#include <cmath>
#include <algorithm>
#include <unistd.h>
#include <fstream>
#include <Eigen/Dense>
#include "imu_filter.h"
#include "test_helpers.h"
//#include "sidekiq_api.h"
#include "../../arg_parser/inc/arg_parser.h"

using namespace std;
using namespace Eigen;

#define DELTA_TIME 10
#define GYRO_CONST 0.98
#define ACCEL_CONST 0.02
#define PULL_NUMBER 100
#define SPLIT_MARKER 5
#define FSR 1000 //FSR defined in gyroscope config in ICM 20602 datasheet
#define GYRO_CONFIG 16
#define RAD_TO_DEGREES 180/3.141592653589793238463
#define VECTOR_SIZE 5

int ready; // ready = 1 whenever enough accel values are read to run the median function
	   //median filter and arctan for accel values only run whenever ready = 1

#define FILTER_ITERATIONS 10000
template <WorldFrame::WorldFrame FRAME>
void filterStationary(
	double& Ax, double& Ay, double& Az,
	double& Gx, double& Gy, double& Gz,
	double& q0, double& q1, double& q2, double& q3) {
	float dt = 0.1;
	//float Gx = 0.0, Gy = 0.0, Gz = 0.0; // Stationary state => Gyro = (0,0,0)

	ImuFilter filter;
	filter.setDriftBiasGain(0.0);
	filter.setAlgorithmGain(0.1);

	// initialize with some orientation
	filter.setOrientation(q0, q1, q2, q3);
	filter.setWorldFrame(FRAME);

	for (int i = 0; i < FILTER_ITERATIONS; i++) {
		filter.madgwickAHRSupdateIMU(Gx, Gy, Gz, Ax, Ay, Az, dt);
	}

	filter.getOrientation(q0, q1, q2, q3);
}


double compFilter(double accel_data, double gyro_data)
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
		//		if (arr.size() >= (SPLIT_MARKER + 1))
		//		{
		//			arr.clear();
		//		}
			//	else
			//	{
		if (SPLIT_MARKER % 2 != 0)
		{
			ready = 1;
			sort(arr.end() - SPLIT_MARKER, arr.end());
			return arr[arr.size() / 2];
		}
		else
		{
			ready = 1;
			sort(arr.end() - SPLIT_MARKER, arr.end());
			return ((arr[arr.size() / 2 - 1]) + arr[arr.size() / 2 - 1]) / 2;
		}

		//		}
	}
}

vector<double> editVector(vector<double> arr) {
	for (int i = 0; i < arr.size(); i++) {
		if (i != arr.size() - 1) {
			arr.at(i) = arr.at(i + 1);
		}
		return arr;
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


int main()
{
	cout << "compiled" << endl;
	uint8_t card = 0;
	vector<double> acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z; //raw_values
	double median_ax, median_ay, median_az = 0;
	double angle_ax, angle_ay, angle_az = 0;
	double angle_gx, angle_gy, angle_gz = 0;
	double finalAngle_x, finalAngle_y, finalAngle_z = 0;

	fstream data;
	data.open("imu_data.csv");
	data << "Median Accel X, Median Accel Y, Median Accel Z, Raw Gyro X, Raw Gyro Y, Raw Gyro Z, Delta Theta X, Delta Theta Y, Delta Theta Z, filtered theta X, filtered theta Y, filtered theta Z" << endl;

	/*
		fstream data("Path_To_File.csv");
		data << "Median Accel X, Median Accel Y, Median Accel Z, Raw Gyro X, Raw Gyro Y, Raw Gyro Z, Delta Theta X, Delta Theta Y, Delta Theta Z, filtered theta X, filtered theta Y, filtered theta Z" << endl;
	*/

	//this is the config code that changes the Gyro Config register to 1000dps
	uint8_t config_byte = 0;
	uint8_t right_side = 0, left_side = 0;
	skiq_read_accel_reg(card, 0x1B, &config_byte, 1);
	left_side = config_byte >> 5;
	left_side = config_byte << 5;
	right_side = config_byte & 7;
	left_side = left_side | right_side;
	left_side = left_side | GYRO_CONFIG;

	//intialize the sidekiq
	skiq_init(skiq_xport_type_auto, skiq_xport_init_level_basic, &card, 1);

	//configure the above in the sidekiq
	skiq_write_accel_reg(card, 0x1B, &config_byte, 1);

	for (int i = 0; i < PULL_NUMBER; i++) // 100HZ of data samples for 1 hr
	{
		acc_x.push_back(read_imu(card, 0x3b));
		acc_y.push_back(read_imu(card, 0x3d));
		acc_z.push_back(read_imu(card, 0x3f));
		gyro_x.push_back(read_imu(card, 0x43) * FSR / (pow(2, 15) - 1));
		gyro_y.push_back(read_imu(card, 0x45) * FSR / (pow(2, 15) - 1));
		gyro_z.push_back(read_imu(card, 0x47) * FSR / (pow(2, 15) - 1));

		//median filter for accel
		median_ax = median(acc_x);
		median_ay = median(acc_y);
		median_az = median(acc_z);

		//rolling median filter
		editVector(acc_x);
		editVector(acc_y);
		editVector(acc_z);

		//arctan A for accel to convert raw values to angles
		if (ready == 1)
		{
			angle_ax = (atan2(median_ax, median_az) * RAD_TO_DEGREES);
			angle_ay = (atan2(median_ay, median_az) * RAD_TO_DEGREES);
			angle_az = (atan2(median_az, median_ay) * RAD_TO_DEGREES);
		}

		//integrate gyro values into angle
		if (i == 0)
		{
			angle_gx = (gyro_x[i] + finalAngle_x) * DELTA_TIME;
			angle_gy = (gyro_y[i] + finalAngle_y) * DELTA_TIME;
			angle_gz = (gyro_z[i] + finalAngle_z) * DELTA_TIME;
		}

		//complimentary filter
		/*finalAngle_x = compFilter(angle_gx, angle_ax);
		finalAngle_y = compFilter(angle_gy, angle_ay);
		finalAngle_z = compFilter(angle_gz, angle_az);*/

		Quaternionf q;
		
		// Assuming roll is X, pitch is Y, and yaw is Z.
		double roll = angle_gx, pitch = angle_gy, yaw = angle_gz;

		q = AngleAxisf(roll, Vector3f::UnitX())
			* AngleAxisf(pitch, Vector3f::UnitY())
			* AngleAxisf(yaw, Vector3f::UnitZ());

		//double q0 = 0.5, q1 = 0.5, q2 = 0.5, q3 = 0.5;

double q0 = q.w();
double q1 = q.x();
double q2 = q.y();
double q3 = q.z();

		filterStationary<WorldFrame::ENU>(angle_ax, angle_ay, angle_az, angle_gx, angle_gy, angle_gz, q0, q1, q2, q3);

		//output into a .csv file
		data << ("%.9f", median_ax);
		data << ",";
		data << ("%.9f", median_ay);
		data << ",";
		data << ("%.9f", median_az);
		data << ",";
		data << ("%.9f", gyro_x[i]);
		data << ",";
		data << ("%.9f", gyro_y[i]);
		data << ",";
		data << ("%.9f", gyro_z[i]);
		data << ",";
		data << ("%.9f", angle_gx);
		data << ",";
		data << ("%.9f", angle_gy);
		data << ",";
		data << ("%.9f", angle_gz);
		data << ",";
		data << ("%.9f", finalAngle_x);
		data << ",";
		data << ("%.9f", finalAngle_y);
		data << ",";
		data << ("%.9f", finalAngle_z) << endl;

		usleep(DELTA_TIME);

	}
	data.close();
	skiq_exit();
}
