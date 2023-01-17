#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#include "bme280.h"

int i2c_filestream;
struct bme280_dev bmeconeccted;

int8_t userI2cRead(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len) {
  write(i2c_filestream, &reg_addr, 1);
  read(i2c_filestream, data, len);

  return 0;
}

void userDelayMs(uint32_t period) { usleep(period * 1000); }

int8_t userI2cWrite(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len) {
  int8_t *buf;

  buf = malloc(len + 1);
  buf[0] = reg_addr;
  memcpy(buf + 1, data, len);
  if (write(i2c_filestream, buf, len + 1) < len) {
    return BME280_E_COMM_FAIL;
  }

  free(buf);

  return BME280_OK;
}

float getCurrentTemperature() {
  uint8_t settings_sel = 0;
  uint32_t req_delay;
  struct bme280_data comp_data;

  bmeconeccted.settings.osr_h = BME280_OVERSAMPLING_1X;
  bmeconeccted.settings.osr_p = BME280_OVERSAMPLING_16X;
  bmeconeccted.settings.osr_t = BME280_OVERSAMPLING_2X;
  bmeconeccted.settings.filter = BME280_FILTER_COEFF_16;

  settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL |
                 BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

  bme280_set_sensor_settings(settings_sel, &bmeconeccted);

  req_delay = bme280_cal_meas_delay(&bmeconeccted.settings);
  bme280_set_sensor_mode(BME280_FORCED_MODE, &bmeconeccted);

  bmeconeccted.delay_ms(req_delay);
  bme280_get_sensor_data(BME280_ALL, &comp_data, &bmeconeccted);
  return comp_data.temperature;
}

struct bme280_dev connectBme() {
 

  int8_t rslt = BME280_OK;
  char i2c_file[] = "/dev/i2c-1";

  bmeconeccted.dev_id = BME280_I2C_ADDR_PRIM;
  bmeconeccted.intf = BME280_I2C_INTF;
  bmeconeccted.read = userI2cRead;
  bmeconeccted.write = userI2cWrite;
  bmeconeccted.delay_ms = userDelayMs;

  if ((i2c_filestream = open(i2c_file, O_RDWR)) < 0) {
    exit(1);
  }

  if (ioctl(i2c_filestream, I2C_SLAVE, bmeconeccted.dev_id) < 0) {
    fprintf(stderr, "Falha ao conectar o dispositivo\n");
    exit(1);
  }

  /* Initialize the bme280 */
  rslt = bme280_init(&bmeconeccted);
  if (rslt != BME280_OK) {
    fprintf(stderr, "Falha ao inicializar o dispositivo (Codigo %+d).\n", rslt);
    exit(1);
  }
  return bmeconeccted;
}