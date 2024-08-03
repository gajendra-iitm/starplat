#include"test1.h"

void string_test(string s, boost::mpi::communicator world )
{
 cout<<s<<endl;
}

int main(){
    string s = "Hello World";
    string_test(s);
    return 0;
    
}