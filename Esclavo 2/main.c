///////////////////////////////////////////////////////////////////////
// Universidad Del Valle De Guatemala
// IE2023: Electronica digital 2
// Autor: Samuel Tortola - 22094, Alan Gomez - 22115
// Proyecto: Proyecto 1: Red de sensores
// Hardware: Atmega328p
// Creado: 10/08/2024
///////////////////////////////////////////////////////////////////////
//CODIGO DEL ESCLAVO 2
//Este esclavo controla motores .

// PINES DE MOTORES

//PD7 A Verde
//PB0 N1 - Azul
//PB1 N2 + Naranja

//PD6 B Amarillo 
//PB2 N3 - Naranja
//PB3 N4 + Azul

// Pin de sensor de corriente A0

//PD4 sensor de vibraci�n.

// ESCLAVO2 0x02  Direccion del esclavo 2

////////////////////////////////////////////
// F_CPU ATmega
////////////////////////////////////////////

#define F_CPU 16000000

////////////////////////////////////////////
//Librerias Generales
////////////////////////////////////////////

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <util/twi.h>

////////////////////////////////////////////
// Librerias Secundarias
////////////////////////////////////////////
#include "I2C/I2C.h"   //Incluir libreria de I2C
#include "UART/UART.h" //Incluir libreria de UART

////////////////////////////////////////////
//Variables
////////////////////////////////////////////

#define ESCLAVO2 0x02


uint8_t dato=0, Uart_Digital_PD4=0;



volatile char receivedChar = 0;    //Variable que almacena el valor del UART
bool valorDigitalPD4 = false; // Variable para almacenar el valor del pin PD4

////////////////////////////////////////////
//Sub_rutinas
////////////////////////////////////////////

void setup(void);
void PINES(void);
//void TT_UART(void);

void setup(void) {
	cli();  // Apagar interrupciones
	PINES();  // Configuraci�n de pines
	initUART9600();  // Iniciar UART
	I2C_Config_SLAVE(ESCLAVO2);
	sei();   // Activar interrupciones
}

void PINES(void){
	//Pines para el L298N
	// Configurar PB0, PB1, PB2 y PB3 como salidas. N1, N2, N3 y N4.
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB3);
	// Configurar PD6 y PD7 como salidas. A y B.
	DDRD |= (1 << PD6) | (1 << PD7);
	
	// Configuraci�n de pines de RGB.
}

void Motor_L298N(void){
	cli();  //Apagar interrupciones
	DDRC =0;  //Puerto C como entrada
	initUART9600();  //Iniciar UART
	
	I2C_Config_SLAVE(ESCLAVO2);
	
	sei();   //Activar interrupciones
}

/*
// TT_UART solo es para las prueba de software sin I2C. CONTROL. L298N.
void TT_UART(void);
void TT_UART(void){
	// Visualizacion de ADC y Motores.	
	writeTextUART("Estate_Motor:");
	UART_PrintNum(dato);
	writeTextUART("\n");
	
	writeTextUART("Estate_Vi:");
	UART_PrintNum(Uart_Digital_PD4);
	writeTextUART("\n");
	
	//Mapeo para control de motores mediante UART.
	
	if (receivedChar == '1') {dato = 1;}
	else if (receivedChar == '2') {dato = 2;}
	else if (receivedChar == '3') {dato = 3;}
	else if (receivedChar == '4') {dato = 4;}
	else {dato = 0;}
}*/

////////////////////////////////////////////
//Programa Principal
////////////////////////////////////////////

int main(void)
{
	setup();// Sub_Rutinas
	
	while(1)
	{
		
		//TT_UART(); // Visualizar ADC y control de estado de motoers por UART.
		
		
		
		
		//CONTROL DE MOTORES
		//dato Instruccion de motor.
		// dato = 1 (Adelante), 2(Atras), 3(Derecha), 4(Izquierda). PORTB &= ~(1 << PB0);
		switch(dato) {
			case 1:
			//Adelante
			PORTD |= (1 << PD7);//A
			PORTB &= ~(1 << PB0);//N1
			PORTB |= (1 << PB1);//N2
			
			
			PORTD |= (1 << PD6);
			PORTB &= ~(1 << PB2);//N3
			PORTB |= (1 << PB3);//N4
			break;
			
			case 2:
			//Atras
			PORTD |= (1 << PD7);//A
			PORTB |= (1 << PB0);//N1
			PORTB &= ~(1 << PB1);//N2
			
			
			PORTD |= (1 << PD6);//B
			PORTB |= (1 << PB2);//N3
			PORTB &= ~(1 << PB3);//N4
		
			break;
			
			case 3:
			//Derecha
			PORTD |= (1 << PD7);//A
			PORTB &= ~(1 << PB0);//N1
			PORTB |= (1 << PB1);//N2
			
			PORTD |= (1 << PD6);//B
			PORTB |= (1 << PB2);//N3
			PORTB &= ~(1 << PB3);//N4
			
			break;
			
			case 4:
			//Izquierda
			PORTD |= (1 << PD7);//A
			PORTB |= (1 << PB0);//N1
			PORTB &= ~(1 << PB1);//N2
			
			PORTD |= (1 << PD6);//B
			PORTB &= ~(1 << PB2);//N3
			PORTB |= (1 << PB3);//N4
			
			break;
			
			case 5:
			UART_PrintNum(1);  //Sistema esta apagado
			break;
			
			case 6:
			UART_PrintNum(5);   //Se detecto caja grande
			break;
			
			case 7:
			UART_PrintNum(2);    //Se detecto caja peque�a
			break;
			
			case 8:
			UART_PrintNum(7);    //Se detecto gas
			break;
			
			case 9:
			UART_PrintNum(6);    // No Se detecto gas
			break;
			
			case 10:
			UART_PrintNum(8);    //Se inicio el sistema
			break;
			
			
			default:
			// Motores apagados default
			PORTD &= ~(1 << PD7);//A
			PORTD &= ~(1 << PD6);//B
			break;
		}
		dato=0;
		
		_delay_ms(40);   //Retardo para evitar malos procesamientos del Atmega328P
		
		
	}
}

////////////////////////////////////////////
//Vects
////////////////////////////////////////////

ISR(USART_RX_vect)
{
	receivedChar = UDR0; // Almacena el caracter recibido
	
	while(!(UCSR0A & (1<<UDRE0)));    //Mientras haya caracteres
	
}

////////////////////////////////////////////
//I2C
////////////////////////////////////////////

ISR(TWI_vect){           //Vector de interrupcion de I2C
	uint8_t estado;
	
	estado = TWSR & 0xFC;  //Lee el estado de la interfaz
	
	switch(estado){
		case 0x60:
		case 0x70:              //Direccionado con su direccion de esclavo
		TWCR |= (1 << TWINT); //
		break;
		
		case 0x80:
		case 0x90:
		dato = TWDR;  //Recibi? el dato, llamada general
		TWCR |= 1 << TWINT; //Borra la bandera TWINT
		break;
		
		case 0xA8: // SLA+R recibido, maestro solicita lectura
		case 0xB8: // Dato transmitido y ACK recibido
		//TWDR = ; // Cargar el dato en el registro de datos*****************
		TWCR |= (1 << TWINT) | (1 << TWEN) | (1 << TWIE)| (1 << TWEA); // Listo para la prixima operacion
		
		case 0xC0: // Dato transmitido y NACK recibido
		case 0xC8: // ?ltimo dato transmitido y ACK recibido
		TWCR |= (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Listo para la prixima operacion
		break;
		
		default:    //Libera el BUS de cualquier errror
		TWCR |= (1 << TWINT) | (1 << TWSTO);
		
	}
	
}



