// STM32L432KC_SPI.c
/*
Author: Noah Fotenos
Email: nfotenos@g.hmc.edu
Date: 11/6/25
*/
// difines functions for SPI communication on the STM32L432KC microcontroller

#include <stdint.h>
#include <stm32l432xx.h>
#include "STM32L432KC_TIM.h"
#include "STM32L432KC_RCC.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_FLASH.h"
#include "main.h"

void initSPI(int br, int cpol, int cpha) {
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_BR, br); // BR

    SPI1->CR1 &= ~(1 << 1); // clock polarity
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol);

    SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha); // clock phase

    SPI1->CR1 &= ~(SPI_CR1_BIDIMODE | SPI_CR1_RXONLY | SPI_CR1_LSBFIRST); // BIDIMODE -> unidirectional // RXONLY --> fully duplex // send and recv with MSB first

    SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSI, 1); // sl*** select management
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSM, 1);

    SPI1->CR1 |= _VAL2FLD(SPI_CR1_MSTR, 1); // set MCU as master

    SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 0b0111); // set data size to 8 bits

    SPI1->CR2 &= ~(SPI_CR2_SSOE | SPI_CR2_FRF | SPI_CR2_NSSP); // SSOE disable to control from software // motorola // for manual CS pulse generation

    SPI1->CR2 |= _VAL2FLD(SPI_CR2_FRXTH, 1); // set FIFO size to 8 bits

    SPI1->CR1 |= _VAL2FLD(SPI_CR1_SPE, 1); // SPI enable

    RCC->APB2ENR |= (1 << 12); // ENABLE SPI1
}


uint8_t spiSendReceive(uint8_t send) {
    while(!(SPI1->SR & SPI_SR_TXE));                        // wait until transmit buffer is empty
    *(volatile uint8_t *) (&SPI1->DR) = send;               // set the data register
    while(!(SPI1->SR & SPI_SR_RXNE));                       // wait until receive buffer not empty
    volatile uint8_t data = (volatile uint8_t) SPI1->DR;    // read data
    return(data);
}