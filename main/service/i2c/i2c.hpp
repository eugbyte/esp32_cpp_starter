//
// Created by eugen on 8/6/2026.
//

#ifndef CPP_STARTER_I2C_HPP
#define CPP_STARTER_I2C_HPP

#include <driver/i2c_master.h>

#define I2C_MASTER_SCL_IO           22
#define I2C_MASTER_SDA_IO           21
#define I2C_MASTER_NUM              I2C_NUM_0
#define I2C_MASTER_FREQ_HZ          100000

namespace svc::i2c {
class I2CService {
public:
	I2CService();
	~I2CService();
};
}

#endif // CPP_STARTER_I2C_HPP
