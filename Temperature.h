#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>
#include <mutex>

using namespace std;

class Temperature
{
public:
    Temperature(string path, uint8_t);
    ~Temperature();

    float READ_Temperature();
    void GET_Temperature();

private:
    float _temp;
    int _file;
    uint8_t _addr;
    mutex _mtx;
};