#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <sstream>
#include <fstream>

using namespace std;

// template <class T>
// void func1( T vec)
// {
//     cout<< vec << endl;
// };


int main (){

//     int arr1[] = {10, 99, 20, 30, 40};
//
//     func1(arr1[0]);
    ofstream onf;
    ostringstream filename;
    int i = 0;

    filename << "test.dat" << i << "wegewg";
    onf.open(filename.str().c_str());
    onf << "1" << endl;
    onf.close();

    // char *a;
    // for (int i =0; i<8; i++){
    //     sprintf(a, "/sys/devices/%d/weg", i);
    //     printf("%s", a);
    // }

    return 0;
}
