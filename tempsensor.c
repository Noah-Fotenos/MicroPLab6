// STM32L432KC_SPI.h
/*
Author: Noah Fotenos
Email: nfotenos@g.hmc.edu
Date: 11/6/25
*/
// header file for spi funcions

#include <stdint.h>
#include <stm32l432xx.h>
#include "STM32L432KC_TIM.h"
#include "STM32L432KC_RCC.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_FLASH.h"
#include "STM32L432KC_SPI.h"
#include "tempsensor.h"
#include "main.h"

void configureSPIPins()
{
    pinMode(SPI_SCK, GPIO_ALT); // SPI1_SCK
    pinMode(SPI_MISO, GPIO_ALT); // SPI1_MISO
    pinMode(SPI_MOSI, GPIO_ALT); // SPI1_MOSI
    pinMode(SPI_CE, GPIO_OUTPUT); //  Manual CS
    digitalWrite(SPI_CE, 0); // set CS to low

    //// Set output speed type to high for SCK
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED3);
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED3);
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED4);
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED4);
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED5);
    GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED5);

    //// Set to AF05 for SPI alternate functions
    GPIOB->AFR[0] &= ~(_VAL2FLD(GPIO_AFRL_AFSEL3, 5));
    GPIOB->AFR[0] &= ~(_VAL2FLD(GPIO_AFRL_AFSEL4, 5));
    GPIOB->AFR[0] &= ~(_VAL2FLD(GPIO_AFRL_AFSEL5, 5));
    GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL3, 5);
    GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL4, 5);
    GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 5);
}

void initializeTemperatureSensor() {
    digitalWrite(SPI_CE, 1); // CS high
    spiSendReceive(0x80); // tell temp sensor we are writing to it for setup
    spiSendReceive(0xE8); // continuous calculation with 12-bit resolution (0xE8)
    digitalWrite(SPI_CE, 0); // CS low
}

void ConfigRes(char resolution) {
 digitalWrite(SPI_CE, 1);
 spiSendReceive(0x80); // tell temp sensor we are writing to it for setup
 spiSendReceive(resolution); // setup resolution
 delay_millis(TIM15, 900); //wait max conversion time
 digitalWrite(SPI_CE, 0); // CS low
}

float getTemperatureData() {

    digitalWrite(SPI_CE, 1);
    spiSendReceive(0x02); // tell sensor we want to read MSB
    uint8_t msb = spiSendReceive(0x00); // get MSB out with 8 clk pulses
    digitalWrite(SPI_CE, 0);


    digitalWrite(SPI_CE, 1);
    spiSendReceive(0x01); // tell sensor we want to read LSB
    uint8_t lsb = spiSendReceive(0x00); // get LSB out with another 8 clk pulses
    digitalWrite(SPI_CE, 0); // set CS to high
    
    // calculate temperature
    int16_t sensor_bits = ((uint16_t) msb << 8) | (uint16_t) lsb;
    float temperature = (float) sensor_bits / 256.0;

    return temperature;
}

