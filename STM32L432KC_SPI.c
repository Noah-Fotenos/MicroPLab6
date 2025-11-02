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

#define SPI_SCK PB3
#define SPI_MOSI PB5
#define SPI_MISO PB4
#define SPI_CE PB1

//SPI1 -> ABP2

char spiSendReceive(uint8_t send) {
    
    while(!(SPI1->SR & SPI_SR_TXE)); // Wait until the transmit buffer is empty
    digitalWrite(SPI_CE, 1);
    *(volatile uint8_t *) (&SPI1->DR) = (uint8_t)(send); // Transmit the character over SPI
    while(!(SPI1->SR & 1U)); // Wait until data has been received
    char rec = (volatile char) SPI1->DR;
    digitalWrite(SPI_CE, 0);
    return rec; // Return received character
}


void initSPI(int br, int cpol, int cpha){
    //SET_BIT(RCC->APB2ENR, 12); 

          //SET_BIT(GPIO->AFRL, 16); //AFRL for PA5 (SKC)
          //SET_BIT(GPIO->AFRL, 18); //AFRL for PA5 (SKC)
          //pinMode(5, GPIO_ALT); //set PA6 as output 

          //SET_BIT(GPIO->AFRH, 12); //AFRL for PA11 (MISO)
          //SET_BIT(GPIO->AFRH, 14); //AFRL for PA11 (MISO)
          //pinMode(11, GPIO_ALT); //set PA6 as output 

          //SET_BIT(GPIO->AFRH, 16); //AFRL for PA12 (MOSI)
          //SET_BIT(GPIO->AFRH, 18); //AFRL for PA12 (MOSI)
          //pinMode(12, GPIO_ALT); //set PA6 as output 

                ////RCC->AHB2ENR |= 1<<0; //enable GPIOA clock
    //SET_BIT(RCC->AHB2ENR, 0); //enable GPIOA clock
// Turn on GPIOA and GPIOB clock domains (GPIOAEN and GPIOBEN bits in AHB1ENR)
    RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN);
    
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Turn on SPI1 clock domain (SPI1EN bit in APB2ENR)

                //// Initially assigning SPI pins
                pinMode(SPI_SCK, GPIO_ALT); // SPI1_SCK
                pinMode(SPI_MISO, GPIO_ALT); // SPI1_MISO
                pinMode(SPI_MOSI, GPIO_ALT); // SPI1_MOSI
                pinMode(SPI_CE, GPIO_OUTPUT); //  Manual CS

                //// Set output speed type to high for SCK
                //GPIOB->OSPEEDR |= (GPIO_OSPEEDR_OSPEED3);

                //// Set to AF05 for SPI alternate functions
                GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL3, 5);
                GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL4, 5);
                GPIOB->AFR[0] |= _VAL2FLD(GPIO_AFRL_AFSEL5, 5);
    
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_BR, br); // Set baud rate divider

    SPI1->CR1 |= (SPI_CR1_MSTR);
    SPI1->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA | SPI_CR1_LSBFIRST | SPI_CR1_SSM);
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPHA, cpha);
    SPI1->CR1 |= _VAL2FLD(SPI_CR1_CPOL, cpol);
    SPI1->CR2 |= _VAL2FLD(SPI_CR2_DS, 0b0111);
    SPI1->CR2 |= (SPI_CR2_FRXTH | SPI_CR2_SSOE);

    SPI1->CR1 |= (SPI_CR1_SPE); // Enable SPI
  
    //Instantiate temperature sensor resolution
    //digitalWrite(SPI_CE, 1);
    
     //while(!(SPI1->SR & SPI_SR_TXE)); // Wait until the transmit buffer is empty
     //*(volatile uint8_t *) (&SPI1->DR) = (uint8_t)(0b10000000U); // Transmit the character over SPI
     //while(!(SPI1->SR & 1U)); // Wait until data has been received
     //while(!(SPI1->SR & SPI_SR_TXE)); // Wait until the transmit buffer is empty
     //*(volatile uint8_t *) (&SPI1->DR) = (uint8_t)(0b11100000U); // Transmit the character over SPI
     //while(!(SPI1->SR & SPI_SR_TXE)); // Wait until the transmit buffer is empty
     //while(!(SPI1->SR & 1U)); // Wait until data has been received
     //digitalWrite(SPI_CE, 0);

     //spiWriteByte((uint8_t)(0b10000000U), (uint8_t)(0b11100000U));

    //while(!(SPI1->SR & SPI_SR_TXE)); // Wait until the transmit buffer is empty
    //digitalWrite(SPI_CE, 1);
    //*(volatile uint8_t *) (&SPI1->DR) = (uint8_t)(0b10000000U); // Transmit the character over SPI
    //while(!(SPI1->SR & 1U)); // Wait until data has been received
    //char temp1 = (volatile char) SPI1->DR;
    
    //while(!(SPI1->SR & SPI_SR_TXE)); // Wait until the transmit buffer is empty
    //*(volatile uint8_t *) (&SPI1->DR) = (uint8_t)('a'); // Transmit the character over SPI
    //while(!(SPI1->SR & 1U)); // Wait until data has been received
    //char temp2 = (volatile char) SPI1->DR;
    //digitalWrite(SPI_CE, 0);
    
    
    spiSendReceive(0);

}

