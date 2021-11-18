#ifndef GATE_H
#define GATE_H


#include <sys/stat.h>
#include <sys/types.h>

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <future>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>
#include <cmath>
#include <iomanip>

#include <unistd.h>
#include <stdlib.h>
#include <wiringPi.h>

#define gatepin 26

using namespace std;

class A4gate
{
public:
    A4gate(bool b1);
    ~A4gate();

    void Gate_control();
    void Open_gate();
    // void Close_gate();

private:
    bool _opengate;
    mutex _mtx;
};

#endif
