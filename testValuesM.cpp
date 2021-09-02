#include <iostream>
#include <stdio.h>
#include <cmath>
#include <algorithm>
#include <string.h>
#include <string>
#include <fstream>


using namespace std;

#define DELTA_TIME 0.1
#define KGYRO 0.5
#define KACCEL 0.5
#define RAD_TO_DEGREES 180/3.141592653589793238463
#define FILTER_ITERATIONS 10000
template <WorldFrame::WorldFrame FRAME>
void filterStationary(
	float Ax, float Ay, float Az,
	float Gx, float Gy, float Gz,
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
	return KGYRO * gyro_data + KACCEL * accel_data;
}

int main()
{
	double gyro_y;  //raw_value
	double median_ay, median_az;
	double angle_ay = 0;
	double angle_gy = 0;
	double finalAngle_y = 0;
	double dTheta = 0;

	string fgyro_y, faccel_y, faccel_z;
	ifstream inFile;
	inFile.open("D:\\CubeSat\\IMUv2.csv");
	if (inFile.fail())
	{
		printf("failure\n");
		cin.get();
	}
	else
		printf("success\n\n");

	fstream data("D:\\CubeSat\\IMUoutput.csv");
	data << "Accel Y, Accel Z, Gyro Y, Delta Theta Y, Accel Angle Y, Final Angle Y" << endl;

	for (int i = 0; i < 49; i++) // 49 samples from ashwins data
	{
		getline(inFile, fgyro_y, ','); //reads gyro y data
		getline(inFile, faccel_y, ','); //reads accel y data
		getline(inFile, faccel_z, '\n'); //reads accel z data
		printf("\nIteration %d\n", (i + 1));
		gyro_y = atof(fgyro_y.c_str()); //converts string to double
		printf("gyro y from csv is %.9f\n", gyro_y);
		median_ay = atof(faccel_y.c_str()); //converts string to double
		printf("accel y from csv is %.9f\n", median_ay);
		median_az = atof(faccel_z.c_str()); //converts string to double
		printf("accel z from csv is %.9f\n", median_az);

		//arctan A for accel
		angle_ay = (atan2(median_ay, median_az) * RAD_TO_DEGREES); 
		printf("accel angle y is %.9f\n", angle_ay); //accel angle

		//integrate gyro values into angle
		angle_gy = ((gyro_y * DELTA_TIME) + finalAngle_y); // added up delta thetas
		dTheta = (gyro_y * DELTA_TIME);
		printf("gyro angle y is %.9f\n", angle_gy); //gyro angle
		printf("delta theta is %.9f\n", dTheta); //delta theta

		//complementary filter
		finalAngle_y = compFilter(angle_gy, angle_ay); //needed to help calculate delta theta 
		printf("final angle y is %.9f\n", finalAngle_y); //final angle
		printf("------------------");

		data << ("%.9f", median_ay);
		data << ",";
		data << ("%.9f", median_az);
		data << ",";
		data << ("%.9f", gyro_y);
		data << ",";
		data << ("%.9f", angle_gy);
		data << ",";
		data << ("%.9f", angle_ay);
		data << ",";
		data << ("%.9f", finalAngle_y) << endl;
	}
	data.close();
	cin.get();
}
/*
success


Iteration 1
gyro y from csv is 21.136758190
accel y from csv is 0.032051578
accel z from csv is 0.999486216
accel angle y is 1.836734719
gyro angle y is 2.113675819
delta theta is 2.113675819
final angle y is 1.975205269
------------------
Iteration 2
gyro y from csv is 21.821271990
accel y from csv is 0.064070220
accel z from csv is 0.997945393
accel angle y is 3.673469388
gyro angle y is 4.157332468
delta theta is 2.182127199
final angle y is 3.915400928
------------------
Iteration 3
gyro y from csv is 21.306067300
accel y from csv is 0.096023026
accel z from csv is 0.995379113
accel angle y is 5.510204087
gyro angle y is 6.046007658
delta theta is 2.130606730
final angle y is 5.778105872
------------------
Iteration 4
gyro y from csv is 19.644386220
accel y from csv is 0.127877162
accel z from csv is 0.991790014
accel angle y is 7.346938792
gyro angle y is 7.742544494
delta theta is 1.964438622
final angle y is 7.544741643
------------------
Iteration 5
gyro y from csv is 21.757542150
accel y from csv is 0.159599895
accel z from csv is 0.987181783
accel angle y is 9.183673471
gyro angle y is 9.720495858
delta theta is 2.175754215
final angle y is 9.452084665
------------------
Iteration 6
gyro y from csv is 19.224847390
accel y from csv is 0.191158629
accel z from csv is 0.981559157
accel angle y is 11.020408180
gyro angle y is 11.374569404
delta theta is 1.922484739
final angle y is 11.197488792
------------------
Iteration 7
gyro y from csv is 21.656250220
accel y from csv is 0.222520934
accel z from csv is 0.974927912
accel angle y is 12.857142862
gyro angle y is 13.363113814
delta theta is 2.165625022
final angle y is 13.110128338
------------------
Iteration 8
gyro y from csv is 18.861874780
accel y from csv is 0.253654584
accel z from csv is 0.967294863
accel angle y is 14.693877557
gyro angle y is 14.996315816
delta theta is 1.886187478
final angle y is 14.845096686
------------------
Iteration 9
gyro y from csv is 21.820514260
accel y from csv is 0.284527587
accel z from csv is 0.958667853
accel angle y is 16.530612266
gyro angle y is 17.027148112
delta theta is 2.182051426
final angle y is 16.778880189
------------------
Iteration 10
gyro y from csv is 19.888642830
accel y from csv is 0.315108218
accel z from csv is 0.949055747
accel angle y is 18.367346938
gyro angle y is 18.767744472
delta theta is 1.988864283
final angle y is 18.567545705
------------------
Iteration 11
gyro y from csv is 19.975405530
accel y from csv is 0.345365054
accel z from csv is 0.938468422
accel angle y is 20.204081611
gyro angle y is 20.565086258
delta theta is 1.997540553
final angle y is 20.384583934
------------------
Iteration 12
gyro y from csv is 21.503934580
accel y from csv is 0.375267005
accel z from csv is 0.926916757
accel angle y is 22.040816340
gyro angle y is 22.534977392
delta theta is 2.150393458
final angle y is 22.287896866
------------------
Iteration 13
gyro y from csv is 20.015181550
accel y from csv is 0.404783343
accel z from csv is 0.914412623
accel angle y is 23.877551014
gyro angle y is 24.289415021
delta theta is 2.001518155
final angle y is 24.083483018
------------------
Iteration 14
gyro y from csv is 20.382419950
accel y from csv is 0.433883739
accel z from csv is 0.900968868
accel angle y is 25.714285706
gyro angle y is 26.121725013
delta theta is 2.038241995
final angle y is 25.918005359
------------------
Iteration 15
gyro y from csv is 20.600023860
accel y from csv is 0.462538290
accel z from csv is 0.886599306
accel angle y is 27.551020406
gyro angle y is 27.978007745
delta theta is 2.060002386
final angle y is 27.764514076
------------------
Iteration 16
gyro y from csv is 20.540489000
accel y from csv is 0.490717552
accel z from csv is 0.871318704
accel angle y is 29.387755105
gyro angle y is 29.818562976
delta theta is 2.054048900
final angle y is 29.603159040
------------------
Iteration 17
gyro y from csv is 18.563051070
accel y from csv is 0.518392568
accel z from csv is 0.855142763
accel angle y is 31.224489781
gyro angle y is 31.459464147
delta theta is 1.856305107
final angle y is 31.341976964
------------------
Iteration 18
gyro y from csv is 20.619041730
accel y from csv is 0.545534901
accel z from csv is 0.838088105
accel angle y is 33.061224476
gyro angle y is 33.403881137
delta theta is 2.061904173
final angle y is 33.232552807
------------------
Iteration 19
gyro y from csv is 18.390254940
accel y from csv is 0.572116660
accel z from csv is 0.820172255
accel angle y is 34.897959165
gyro angle y is 35.071578301
delta theta is 1.839025494
final angle y is 34.984768733
------------------
Iteration 20
gyro y from csv is 19.637750870
accel y from csv is 0.598110530
accel z from csv is 0.801413622
accel angle y is 36.734693850
gyro angle y is 36.948543820
delta theta is 1.963775087
final angle y is 36.841618835
------------------
Iteration 21
gyro y from csv is 19.584377740
accel y from csv is 0.623489802
accel z from csv is 0.781831482
accel angle y is 38.571428594
gyro angle y is 38.800056609
delta theta is 1.958437774
final angle y is 38.685742602
------------------
Iteration 22
gyro y from csv is 18.580652250
accel y from csv is 0.648228395
accel z from csv is 0.761445958
accel angle y is 40.408163266
gyro angle y is 40.543807827
delta theta is 1.858065225
final angle y is 40.475985546
------------------
Iteration 23
gyro y from csv is 20.062862190
accel y from csv is 0.672300890
accel z from csv is 0.740277997
accel angle y is 42.244897951
gyro angle y is 42.482271765
delta theta is 2.006286219
final angle y is 42.363584858
------------------
Iteration 24
gyro y from csv is 20.753581770
accel y from csv is 0.695682551
accel z from csv is 0.718349350
accel angle y is 44.081632673
gyro angle y is 44.438943035
delta theta is 2.075358177
final angle y is 44.260287854
------------------
Iteration 25
gyro y from csv is 21.725761700
accel y from csv is 0.718349350
accel z from csv is 0.695682551
accel angle y is 45.918367327
gyro angle y is 46.432864024
delta theta is 2.172576170
final angle y is 46.175615675
------------------
Iteration 26
gyro y from csv is 21.639081390
accel y from csv is 0.740277997
accel z from csv is 0.672300890
accel angle y is 47.755102049
gyro angle y is 48.339523814
delta theta is 2.163908139
final angle y is 48.047312932
------------------
Iteration 27
gyro y from csv is 22.196663790
accel y from csv is 0.761445958
accel z from csv is 0.648228395
accel angle y is 49.591836734
gyro angle y is 50.266979311
delta theta is 2.219666379
final angle y is 49.929408023
------------------
Iteration 28
gyro y from csv is 19.151534960
accel y from csv is 0.781831482
accel z from csv is 0.623489802
accel angle y is 51.428571406
gyro angle y is 51.844561519
delta theta is 1.915153496
final angle y is 51.636566462
------------------
Iteration 29
gyro y from csv is 19.365906320
accel y from csv is 0.801413622
accel z from csv is 0.598110530
accel angle y is 53.265306150
gyro angle y is 53.573157094
delta theta is 1.936590632
final angle y is 53.419231622
------------------
Iteration 30
gyro y from csv is 18.432439450
accel y from csv is 0.820172255
accel z from csv is 0.572116660
accel angle y is 55.102040835
gyro angle y is 55.262475567
delta theta is 1.843243945
final angle y is 55.182258201
------------------
Iteration 31
gyro y from csv is 19.371573480
accel y from csv is 0.838088105
accel z from csv is 0.545534901
accel angle y is 56.938775524
gyro angle y is 57.119415549
delta theta is 1.937157348
final angle y is 57.029095536
------------------
Iteration 32
gyro y from csv is 20.840598880
accel y from csv is 0.855142763
accel z from csv is 0.518392568
accel angle y is 58.775510219
gyro angle y is 59.113155424
delta theta is 2.084059888
final angle y is 58.944332822
------------------
Iteration 33
gyro y from csv is 19.387308630
accel y from csv is 0.871318704
accel z from csv is 0.490717552
accel angle y is 60.612244895
gyro angle y is 60.883063685
delta theta is 1.938730863
final angle y is 60.747654290
------------------
Iteration 34
gyro y from csv is 18.367797270
accel y from csv is 0.886599306
accel z from csv is 0.462538290
accel angle y is 62.448979594
gyro angle y is 62.584434017
delta theta is 1.836779727
final angle y is 62.516706805
------------------
Iteration 35
gyro y from csv is 21.276816710
accel y from csv is 0.900968868
accel z from csv is 0.433883739
accel angle y is 64.285714294
gyro angle y is 64.644388476
delta theta is 2.127681671
final angle y is 64.465051385
------------------
Iteration 36
gyro y from csv is 22.006963170
accel y from csv is 0.914412623
accel z from csv is 0.404783343
accel angle y is 66.122448986
gyro angle y is 66.665747702
delta theta is 2.200696317
final angle y is 66.394098344
------------------
Iteration 37
gyro y from csv is 21.960174810
accel y from csv is 0.926916757
accel z from csv is 0.375267005
accel angle y is 67.959183660
gyro angle y is 68.590115825
delta theta is 2.196017481
final angle y is 68.274649742
------------------
Iteration 38
gyro y from csv is 19.228018900
accel y from csv is 0.938468422
accel z from csv is 0.345365054
accel angle y is 69.795918389
gyro angle y is 70.197451632
delta theta is 1.922801890
final angle y is 69.996685011
------------------
Iteration 39
gyro y from csv is 19.710647420
accel y from csv is 0.949055747
accel z from csv is 0.315108218
accel angle y is 71.632653062
gyro angle y is 71.967749753
delta theta is 1.971064742
final angle y is 71.800201407
------------------
Iteration 40
gyro y from csv is 21.732937960
accel y from csv is 0.958667853
accel z from csv is 0.284527587
accel angle y is 73.469387734
gyro angle y is 73.973495203
delta theta is 2.173293796
final angle y is 73.721441469
------------------
Iteration 41
gyro y from csv is 18.571750030
accel y from csv is 0.967294863
accel z from csv is 0.253654584
accel angle y is 75.306122443
gyro angle y is 75.578616472
delta theta is 1.857175003
final angle y is 75.442369458
------------------
Iteration 42
gyro y from csv is 21.550038690
accel y from csv is 0.974927912
accel z from csv is 0.222520934
accel angle y is 77.142857138
gyro angle y is 77.597373327
delta theta is 2.155003869
final angle y is 77.370115232
------------------
Iteration 43
gyro y from csv is 20.506285990
accel y from csv is 0.981559157
accel z from csv is 0.191158629
accel angle y is 78.979591820
gyro angle y is 79.420743831
delta theta is 2.050628599
final angle y is 79.200167826
------------------
Iteration 44
gyro y from csv is 19.614309470
accel y from csv is 0.987181783
accel z from csv is 0.159599895
accel angle y is 80.816326529
gyro angle y is 81.161598773
delta theta is 1.961430947
final angle y is 80.988962651
------------------
Iteration 45
gyro y from csv is 20.984702790
accel y from csv is 0.991790014
accel z from csv is 0.127877162
accel angle y is 82.653061208
gyro angle y is 83.087432930
delta theta is 2.098470279
final angle y is 82.870247069
------------------
Iteration 46
gyro y from csv is 20.411648890
accel y from csv is 0.995379113
accel z from csv is 0.096023026
accel angle y is 84.489795913
gyro angle y is 84.911411958
delta theta is 2.041164889
final angle y is 84.700603936
------------------
Iteration 47
gyro y from csv is 22.233524900
accel y from csv is 0.997945393
accel z from csv is 0.064070220
accel angle y is 86.326530612
gyro angle y is 86.923956426
delta theta is 2.223352490
final angle y is 86.625243519
------------------
Iteration 48
gyro y from csv is 21.787936610
accel y from csv is 0.999486216
accel z from csv is 0.032051578
accel angle y is 88.163265281
gyro angle y is 88.804037180
delta theta is 2.178793661
final angle y is 88.483651231
------------------
Iteration 49
gyro y from csv is 21.737995050
accel y from csv is 1.000000000
accel z from csv is 0.000000000
accel angle y is 90.000000000
gyro angle y is 90.657450736
delta theta is 2.173799505
final angle y is 90.328725368
------------------
*/

