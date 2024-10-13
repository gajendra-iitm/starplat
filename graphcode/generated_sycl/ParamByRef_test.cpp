#ifndef GENCPP_PARAMBYREF_TEST_H
#define GENCPP_PARAMBYREF_TEST_H
#include <CL/sycl.hpp>
#include <iostream>
#include <fstream>
#include "../graph.hpp"
using namespace sycl;

void test(double & arg1,int & arg2)
{
  queue Q(default_selector_v);
  std::cout << "Selected device: " << Q.get_device().get_info<info::device::name>() << std::endl;

  //DECLAR DEVICE AND HOST vars in params

  //BEGIN DSL PARSING 
  int a = 0; // asst in main

  if (a == 0){ // if filter begin 
    arg1 = 1.000000;
    arg2 = 2;

  } // if filter end
  else
  arg1 = 2.000000;
  arg2 = 3;


  //TIMER STOP
  std::chrono::steady_clock::time_point toc = std::chrono::steady_clock::now();
  std::cout<<"Time required: "<<std::chrono::duration_cast<std::chrono::microseconds>(toc - tic).count()<<"[µs]"<<std::endl;

} //end FUN

#endif
