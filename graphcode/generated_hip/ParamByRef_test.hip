#include "ParamByRef_test.h"


void test(
  double  &arg1,int  &arg2)
{




  int a = 0;  if (a == 0) {
    arg1 = 1;
    arg2 = 2;


  }  else {
    arg1 = 2;
    arg2 = 3;


  }


}
