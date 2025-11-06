// STM32L432KC_SPI.h
/*
Author: Noah Fotenos
Email: nfotenos@g.hmc.edu
Date: 11/6/25
*/
// header file for spi funcions

#ifndef Temp_sensor
#define Temp_sensor

void configureSPIPins();

float getTemperatureData();

void ConfigRes(char resolution);

void initializeTemperatureSensor();

#endif
