/*
File: main.c
Author: Noah Fotenos
Email: nfotenos@g.hmc.edu
Date: 11/6/25
*/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "tempsensor.h"

/////////////////////////////////////////////////////////////////
// Provided Constants and Functions
/////////////////////////////////////////////////////////////////

//Defining the web page in two chunks: everything before the current time, and everything after the current time
char* webpageStart = "<!DOCTYPE html><html><head><title>E155 Web Server Demo Webpage</title>\
	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
	</head>\
	<body><h1>E155 Web Server Demo Webpage</h1>";
char* ledStr = "<p>LED Control:</p><form action=\"ledon\"><input type=\"submit\" value=\"Turn the LED on!\"></form>\
	<form action=\"ledoff\"><input type=\"submit\" value=\"Turn the LED off!\"></form>";

char* resconfig = "<p>Resolution Control:</p><form action=\"12bit\"><input type=\"submit\" value=\"12-bit resolution\"></form>\
	<form action=\"11bit\"><input type=\"submit\" value=\"11-bit resolution\"></form>\
        <form action=\"10bit\"><input type=\"submit\" value=\"10-bit resolution\"></form>\
        <form action=\"9bit\"><input type=\"submit\" value=\"9-bit resolution\"></form>\
        <form action=\"8bit\"><input type=\"submit\" value=\"8-bit resolution\"></form>";

char* webpageEnd   = "</body></html>";

//determines whether a given character sequence is in a char array request, returning 1 if present, -1 if not present
int inString(char request[], char des[]) {
	if (strstr(request, des) != NULL) {return 1;}
	return -1;
}

char GetResolution(char request[])
{
    // set resolution config register based on form input
    if (inString(request, "12bit")==1) {
            return(0xE8);
    }
    else if (inString(request, "11bit")==1) {
            return(0xE6); // 0b0110;
    }
    else if (inString(request, "10bit")==1) {
		return(0xE4); // 0b0100;
	}
    else if (inString(request, "9bit")==1) {
		return(0xE2); // 0b0010;
	}
    else if (inString(request, "8bit")==1) {
		return(0xE0); // 0b0000;
	}
  return 0;  // default to 12 bit 
    

}
int updateLEDStatus(char request[])
{
	int led_status = 0;
	// The request has been received. now process to determine whether to turn the LED on or off
	if (inString(request, "ledoff")==1) {
		digitalWrite(LED_PIN, PIO_LOW);
		led_status = 0;
	}
	else if (inString(request, "ledon")==1) {
		digitalWrite(LED_PIN, PIO_HIGH);
		led_status = 1;
	}

	return led_status;
}

/////////////////////////////////////////////////////////////////
// Solution Functions
/////////////////////////////////////////////////////////////////

int main(void) {
  configureFlash();
  configureClock();

  gpioEnable(GPIO_PORT_A);
  gpioEnable(GPIO_PORT_B);
  gpioEnable(GPIO_PORT_C);

  pinMode(LED_PIN, GPIO_OUTPUT);
  
  RCC->APB2ENR |= (RCC_APB2ENR_TIM15EN);
  initTIM(TIM15);

  RCC->APB2ENR |= (1 << 12); // ENABLE SPI1

  configureSPIPins();
  
  USART_TypeDef * USART = initUSART(USART1_ID, 125000);

  initSPI(0b111, 0, 1);  

  initializeTemperatureSensor();

  getTemperatureData();
    
  while(1) {
    /* Wait for ESP8266 to send a request.
    Requests take the form of '/REQ:<tag>\n', with TAG begin <= 10 characters.
    Therefore the request[] array must be able to contain 18 characters.
    */

    // Receive web request from the ESP
    char request[BUFF_LEN] = "                  "; // initialize to known value
    int charIndex = 0;
  
    // Keep going until you get end of line character
    while(inString(request, "\n") == -1) {
      // Wait for a complete request to be transmitted before processing
      while(!(USART->ISR & USART_ISR_RXNE));
      request[charIndex++] = readChar(USART);
    }

    char resolution = GetResolution(request); //get resolution
    if (resolution != 0){
      ConfigRes(resolution);  // change resolution
    }
  
    float temperature = getTemperatureData();
    //float temperature = 0;
    char temperature_string[32];
    sprintf(temperature_string,"Temperature is %0.4f C", temperature);
    
    // Update string with current LED state
  
    int led_status = updateLEDStatus(request);

    char ledStatusStr[20];
    if (led_status == 1)
      sprintf(ledStatusStr,"LED is on!");
    else if (led_status == 0)
      sprintf(ledStatusStr,"LED is off!");

    // finally, transmit the webpage over UART
    sendString(USART, webpageStart); // webpage header code
    sendString(USART, ledStr); // button for controlling LED

    sendString(USART, "<h2>LED Status</h2>");
   

    sendString(USART, "<p>");
    sendString(USART, ledStatusStr);
    sendString(USART, "</p>");

    sendString(USART, resconfig); // buttons for controlling resolution

    sendString(USART, "</p>");
    sendString(USART,  temperature_string);
    sendString(USART, "</p>");

  
    sendString(USART, webpageEnd);
  }
}
