// STM32L432KC_SPI.h
// Noah Fotenos
// Nfotenos@g.hmc.edu
// 10/28/25
// header file for spi funcions

#ifndef Temp_sensor
#define Temp_sensor

void configureSPIPins();

float getTemperatureData();

void initializeTemperatureSensor();

#endif
