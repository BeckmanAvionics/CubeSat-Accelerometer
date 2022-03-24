#include<stdio.h>
#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;
int main(void)

{

Eigen::Quaternionf q(2, 0, 1, -3); 

  std::cout << "This quaternion consists of a scalar " << q.w() << " and a vector " << std::endl << q.vec() << std::endl;


  q.normalize();

  std::cout << "To represent rotation, we need to normalize it such that its length is " << q.norm() << std::endl;


 //Dot Product and Cross Product
Vector3d v(1, 2, 3);

Vector3d w(0, 1, 2);


Vector3d vCrossw = v.cross(w); // cross product of two vectors
}
// to compile -> g++ -I /path/to/eigen program.cpp -o program
//make sure it is in the same directory as 'eigen'
