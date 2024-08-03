#include"test1.h"

void string_test(string s)
{
    string text = "This is a text";
    cout<<text<<endl;
    cout<< s <<" is the string\n";
}

int main(){
    string s;
    cin>> s;
    string_test(s);
    return 0;
}