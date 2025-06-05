#ifndef UTIL_H
#define UTIL_H

#include <Wire.h>

// Model SIM800L_IP5306_VERSION_20200811

#define MODEM_RST             5
#define MODEM_PWRKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             17
#define MODEM_RX             16

#define MODEM_DTR            32
#define MODEM_RI             33

#define I2C_SDA              21
#define I2C_SCL              22
#define LED_GPIO             13
#define LED_ON               HIGH
#define LED_OFF              LOW

#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00


#endif