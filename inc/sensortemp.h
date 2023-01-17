#ifndef SENSORTEMP_H_
#define SENSORTEMP_H_

#include "bme280.h"

void user_delay_us(uint32_t period, void *intf_ptr);
void print_sensor_data(struct bme280_data *comp_data);
int8_t user_i2c_read(uint8_t reg_addr, uint8_t *data, uint32_t len, void *intf_ptr);
int8_t user_i2c_write(uint8_t reg_addr, const uint8_t *data, uint32_t len, void *intf_ptr);
int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev);

#endif /* SENSORTEMP_H_ */