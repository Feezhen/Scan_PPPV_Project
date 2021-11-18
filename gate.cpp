#include "gate.h"


A4gate::A4gate(bool b1){
    _opengate=false;
    wiringPiSetup();
    pinMode(gatepin, OUTPUT);
    digitalWrite(gatepin, LOW);//关门
}

A4gate::~A4gate(){
    unique_lock<mutex> lock(_mtx);
    _opengate = false;
    lock.unlock();
    digitalWrite(gatepin, LOW);
}


void A4gate::Gate_control(){
    while(1){
        if(_opengate){
            digitalWrite(gatepin, HIGH);//open gate
            cout << "open the gate !!!!!" << endl;
            unique_lock<mutex> lock(_mtx);
            _opengate = false;
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(3));
            
        }
        else{
            digitalWrite(gatepin, LOW);
        }
    }
}


void A4gate::Open_gate(){
    if(!_opengate){
        unique_lock<mutex> lock(_mtx);
        _opengate = true;
        lock.unlock();
    }
}

// void A4gate::Close_gate(){
//     digitalWrite(gatepin, LOW);
// }
