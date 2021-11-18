#include "Temperature.h"
#include <ctime>

static inline __s32 i2c_smbus_access(int file, char read_write, __u8 command,
                                     int size, union i2c_smbus_data *data)
{
        struct i2c_smbus_ioctl_data args;

        args.read_write = read_write;
        args.command = command;
        args.size = size;
        args.data = data;
        return ioctl(file,I2C_SMBUS,&args);
}

static inline __s32 i2c_smbus_read_byte_data(int file, __u8 command)
{
        union i2c_smbus_data data;
        if (i2c_smbus_access(file,I2C_SMBUS_READ,command,
                             I2C_SMBUS_WORD_DATA,&data))
                return -1;
        else
                return 0x0FFFF & data.word;
}


Temperature::Temperature(string path, uint8_t addr)
{
    _addr = addr;
    // 初始化smbus
    _file = open(path.c_str(), O_RDWR);
    if (_file < 0)
        err(errno, "Tried to open '%s'", path);
    int rc = ioctl(_file, I2C_SLAVE, _addr);
    if (rc < 0)
        err(errno, "Tried to set device address '0x%02x'", addr);
   // cout<<"111"<<endl;
}
Temperature::~Temperature()
{
    ;
}

float Temperature::READ_Temperature()
{
    unique_lock<mutex> lock(_mtx);
    return _temp;
    
}

void Temperature::GET_Temperature()
{
    while(1)
    {
        uint16_t temp = i2c_smbus_read_byte_data(_file, 0x07);
        float temp2 = temp*0.02-273.15;
        // cout << "读取到温度temp : " << temp << endl;
        // temp2 = rand()%100/100;
        unique_lock<mutex> lock(_mtx);
        _temp = temp2;
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(1));

        
    }

}
