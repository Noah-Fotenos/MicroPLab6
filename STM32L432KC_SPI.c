// STM32L432KC_SPI.c
// Noah Fotenos
// Nfotenos@g.hmc.edu
// 10/28/25
// difines functions for SPI communication on the STM32L432KC microcontroller

#include <stdint.h>
#include <stm32l432xx.h>
#include "STM32L432KC_TIM.h"
#include "STM32L432KC_RCC.h"
#include "STM32L432KC_GPIO.h"
#include "STM32L432KC_FLASH.h"
#include "STM32L432KC_SPI.h"

//SPI1 -> ABP2

uint8_t spiSendReceive(uint8_t send) {
    
    while(!(SPI1->SR & SPI_SR_TXE)); // Wait until the transmit buffer is empty
    
    *(volatile uint8_t *) (&SPI1->DR) = (uint8_t)(send); // Transmit the character over SPI
    while(!(SPI1->SR & SPI_SR_RXNE)); // Wait until data has been received
    uint8_t rec = (volatile uint8_t) SPI1->DR;
    
    return rec; // Return received character
}


void initSPI(int br, int cpol, int cpha){
// Turn on GPIOA and GPIOB clock domains (GPIOAEN and GPIOBEN bits in AHB1ENR)
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_BR, br); // BR

    SPI1->CR1 &= ~(1 << 1); // clock polarity
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol);

    SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha); // clock phase

    SPI1->CR1 &= ~(SPI_CR1_BIDIMODE); // BIDIMODE -> unidirectional
    SPI1->CR1 &= ~(SPI_CR1_RXONLY); // RXONLY --> fully duplex
    SPI1->CR1 &= ~(SPI_CR1_LSBFIRST); // send and recv with MSB first

    SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSI, 1); // sl*** select management
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_SSM, 1);


    SPI1->CR1 |= _VAL2FLD(SPI_CR1_MSTR, 1); // set MCU as master

    SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 0b0111); // set data size to 8 bits

    SPI1->CR2 &= ~(SPI_CR2_SSOE); // SSOE disable to control from software
    SPI1->CR2 &= ~(SPI_CR2_FRF); // motorola
    SPI1->CR2 &= ~(SPI_CR2_NSSP); // for manual CS pulse generation

    SPI1->CR2 |= _VAL2FLD(SPI_CR2_FRXTH, 1); // set FIFO size to 8 bits


    SPI1->CR1 |= _VAL2FLD(SPI_CR1_SPE, 1); // SPI enable
  
    //Instantiate temperature sensor resolution

    digitalWrite(SPI_CE, 1);
    spiSendReceive((uint8_t)(0b10000000U));
    spiSendReceive((uint8_t)(0b11100000U));
    digitalWrite(SPI_CE, 0);
    
    digitalWrite(SPI_CE, 1);
    spiSendReceive(0);
    int condif = spiSendReceive(0);
    digitalWrite(SPI_CE, 0);

}

