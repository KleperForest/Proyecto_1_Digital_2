//******************************************************************************
// Universidad Del Valle De Guatemala
// IE2023: Electrónica digital 2
// Autor: Samuel Tortola - 22094, Alan Gomez - 22115
// Proyecto: Proyecto 1: Red de sensores
// Hardware: Atmega328p
// Creado: 4/08/2024
// Última modificación: 24/08/2024
//******************************************************************************
  //CODIGO DEL ESCLAVO 1
  //Este esclavo controla el brazo robótico y la banda transportadora



#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <util/twi.h>


#include "I2C/I2C.h"
#include "PWM1/PWM1.h"
#include "PWM2/PWM2.h"
#include "UART/UART.h"
#include "Ultrasonico/Ultrasonico.h"


int dato_a_enviarI2C = 0, dato_a_recibirI2C = 0;   //Variables que almacenan los datos a enviar  y recibir por I2C
int codo = 180, garra = 50, brazo = 80, rota = 143;  //Datos de 0 a 255 para colocar angulos de 0 a 180 a cada servo
uint8_t sistema_activado = 0, cajas = 0, activacaja = 0;



char buffer[16];

uint16_t distance = 0;
 
void setup(void);
void setup(void){
	cli();  //Apagar interrupciones
	
	DDRB |= (1 << PORTB2) | (1 << PORTB1) | (1 << PORTB3) | (1 << PORTB4); //PB1, PB2, PB3 como salida de servos 1,2,3, PB4 como salida de banda transportadora
	DDRD |= (1 << DDD3) ;   //PD3 como salida de servo 4

	 
	//channelA, conectado a elevar y bajar garra
	//channelB, conetado a elevar y bajar brazo completo
	//channel2A, conectado a la garra
	//channel2B, conectado a rotar a garra
	
	initFastPWM1(8);  //Iniciar funcion de FASTPWM en timer1
	channel(channelB, modo);
	channel(channelA, modo);
	topValue(29999);   //Valor alto de desbordamiento para 1 a 2 ms que se necesita de servo

	initFastPWM2();  //Iniciar funcion de FASTPWM en timer2
	
	I2C_Config_SLAVE(0x03);   //Iniciar el I2C como esclavo, enviarle su dirección
	initUART9600();  //Iniciar UART
	initUltrasonic(); //Iniciar sensor Ultrasónico
	
	PORTB |= ~(1 << PORTB4);  //Iniciar con la banda transportadora apagada


	sei();   //Activar interrupciones
}





int main(void)
{
	setup();

	while(1)
	{
		
		
		
		if (dato_a_recibirI2C != 0)  //Cuando se recibe algo por I2C
		{
			if (dato_a_recibirI2C == 1)        //Verificar el número de servo y aumentar o disminuir su ángulo
			{
				codo = codo + 5;
				if (codo >= 254)
				{
					codo = 255;
				}
			
			}
			
			if (dato_a_recibirI2C == 2)
			{
			
				codo = codo - 5;
				if (codo <= 1)
				{
					codo = 0;
				}
				
			}
			
			if (dato_a_recibirI2C == 4)
			{
				brazo = brazo + 5;
				if (brazo >= 254)
				{
					brazo = 255;
				}
				
			}
			
			if (dato_a_recibirI2C == 3)
			{
				
				brazo = brazo - 5;
				if (brazo <= 1)
				{
					brazo = 0;
				}
				
				
			}
				
			if (dato_a_recibirI2C == 6)
			{
				rota = rota + 2;
				if (rota >= 254)
				{
					rota = 255;
				}
				
			}
			
			if (dato_a_recibirI2C == 5)
			{
				
				rota = rota - 2;
				if (rota <= 1)
				{
					rota = 0;
				}
				
			}
			
			if (dato_a_recibirI2C == 7)
			{
				garra = garra + 7;
				if (garra >= 254)
				{
					garra = 255;
				}
				
			}
			
			if (dato_a_recibirI2C == 8)
			{
				
				garra = garra - 7;
				if (garra <= 1)
				{
					garra = 0;
				}
			
			}
			
			if (dato_a_recibirI2C == 10)
			{
				
				sistema_activado = 1;
				
			}
			
			if (dato_a_recibirI2C == 11)
			{
				sistema_activado = 0;
				dato_a_enviarI2C = 0;
				
			}
			
			if (dato_a_recibirI2C == 20)   //Si se detecto gas
			{
				dato_a_enviarI2C = 1;
				sistema_activado = 0;
				PORTB &= ~(1 << PORTB4);  // Apaga PB4
			}
			
			if (dato_a_recibirI2C == 12)
			{
				if (sistema_activado==1)
				{
					PORTB |= (1 << PORTB4); 
				}
			}
			
			if (dato_a_recibirI2C == 75)    //Reinicio del sistema
			{
				dato_a_enviarI2C = 0;
			}
			
			
			if (dato_a_recibirI2C == 13)  //Cuando la caja llegue al final de la banda
			{
				if (sistema_activado==1)
				{
					PORTB &= ~(1 << PORTB4);  // Apaga PB4
					
					if (cajas == 2)
					{
						convertServo(80, channelB);
						_delay_ms(200);
						convertServo2(143, channel2B);
						_delay_ms(5);
						convertServo2(50, channel2A);
						_delay_ms(5);
						convertServo(180,channelA);

						
						_delay_ms(50);
						for (garra = garra; garra<=255; garra++)
						{
							convertServo2(garra, channel2A);
							_delay_ms(15);
							
						}
						_delay_ms(20);
						
						for (brazo = brazo; brazo<=120; brazo++)
						{
							convertServo(brazo, channelB);
							_delay_ms(15);
							
						}
						_delay_ms(20);

						for (codo = codo; codo<=250; codo++)
						{
							convertServo(codo,channelA);
							_delay_ms(15);
							
						}
						
						_delay_ms(20);
						for (garra = garra; garra>=190; garra--)
						{
							convertServo2(garra, channel2A);
							_delay_ms(15);
							
						}
						
						_delay_ms(40);
						
						for (codo = codo; codo>=200; codo--)
						{
							convertServo(codo,channelA);
							_delay_ms(15);
							
						}
						
						_delay_ms(20);
						
						for (rota = rota; rota<=182; rota++)
						{
							convertServo2(rota, channel2B);
							_delay_ms(15);
							
						}
						
						_delay_ms(1000);
						
						for (brazo = brazo; brazo<=160; brazo++)
						{
							convertServo(brazo, channelB);
							_delay_ms(15);
							
						}
						
						_delay_ms(1000);
						for (garra = garra; garra<=255; garra++)
						{
							convertServo2(garra, channel2A);
							_delay_ms(15);
							
						}
						
						_delay_ms(1000);
						
							
							for (brazo = brazo; brazo>=80; brazo--)
							{
								convertServo(brazo, channelB);
								_delay_ms(15);
								
							}
							_delay_ms(20);
						
						for (rota = rota; rota>=143; rota--)
						{
							convertServo2(rota, channel2B);
							_delay_ms(15);
							
						}
					
						
						_delay_ms(20);
						for (garra = garra; garra>=50; garra--)
						{
							convertServo2(garra, channel2A);
							_delay_ms(15);
							
						}
						
						_delay_ms(20);

						for (codo = codo; codo>=180; codo--)
						{
							convertServo(codo,channelA);
							_delay_ms(15);
							
						}

					}
					
					 if (cajas == 1)
					{
						
						//Si se colocó caja grande
						    convertServo(80, channelB);
						    _delay_ms(200);
							convertServo2(143, channel2B);
							_delay_ms(5);
							convertServo2(50, channel2A);
							_delay_ms(5);
							convertServo(180,channelA);
					
							for (brazo = brazo; brazo>=70; brazo--)
							{
								convertServo(brazo, channelB);
								_delay_ms(15);
								
							}
							
							_delay_ms(20);
							for (garra = garra; garra<=255; garra++)
							{
								convertServo2(garra, channel2A);
								_delay_ms(15);
								
							}
							_delay_ms(20);

							for (codo = codo; codo<=250; codo++)
							{
								convertServo(codo,channelA);
								_delay_ms(15);
								
							}
							
							_delay_ms(20);
							for (garra = garra; garra>=210; garra--)
							{
								convertServo2(garra, channel2A);
								_delay_ms(15);
								
							}
							
							_delay_ms(40);
							
							for (codo = codo; codo>=200; codo--)
							{
								convertServo(codo,channelA);
								_delay_ms(15);
								
							}
							
							_delay_ms(20);
							
							for (rota = rota; rota>=100; rota--)
							{
								convertServo2(rota, channel2B);
								_delay_ms(15);
								
							}
							
							_delay_ms(1000);
							
							for (brazo = brazo; brazo<=150; brazo++)
							{
								convertServo(brazo, channelB);
								_delay_ms(15);
								
							}
							
							_delay_ms(1000);
							for (garra = garra; garra<=255; garra++)
							{
								convertServo2(garra, channel2A);
								_delay_ms(15);
								
							}
							
							_delay_ms(1000);
							
								
								for (brazo = brazo; brazo>=80; brazo--)
								{
									convertServo(brazo, channelB);
									_delay_ms(15);
									
								}
								_delay_ms(20);
							
							for (rota = rota; rota<=143; rota++)
							{
								convertServo2(rota, channel2B);
								_delay_ms(15);
								
							}
						
							
							_delay_ms(20);
							for (garra = garra; garra>=50; garra--)
							{
								convertServo2(garra, channel2A);
								_delay_ms(15);
								
							}
							
							_delay_ms(20);

							for (codo = codo; codo>=180; codo--)
							{
								convertServo(codo,channelA);
								_delay_ms(15);
								
							}
					}
					cajas = 9;
					activacaja = 0;
					
					
				}		
			} 

			
			
			dato_a_recibirI2C = 0;    //Evitar sumas indebidas
		}
		
		
		if (sistema_activado == 1)   //Si el sistema se activo con el RTC
		{
			 distance = measureDistance();
			 dato_a_enviarI2C = distance;
			
			 
			 if (distance <= 3 && activacaja == 0)
			 {
				 cajas = 1; //Se reconoció caja grande
				 activacaja = 1;
			 }
			 
			 if (distance <= 6 && distance >=4 && activacaja == 0)
			 {
				 cajas = 2; //Se reconoció caja pequeña
			
			 }
			 UART_PrintNum(cajas);
	
		}
		
		
		
		convertServo2(garra, channel2A);       //Enviar cada dato a los timers para controlar los servos
		convertServo(brazo, channelB);
		convertServo(codo,channelA);
		convertServo2(rota, channel2B);
		_delay_ms(15);  //Pequeño retardo para evitar malos funcionamientos
		
		
	}
}


ISR(TWI_vect){           //Vector de interrupción de I2C
	uint8_t estado;
	
	estado = TWSR & 0xFC;  //Lee el estado de la interfaz
	
	switch(estado){
		case 0x60:
		case 0x70:              //Direccionado con su direccion de esclavo
		TWCR |= (1 << TWINT); //
		break;
		
		case 0x80:
		case 0x90:
		dato_a_recibirI2C = TWDR;  //Recibió el dato, llamada general
		TWCR |= 1 << TWINT; //Borra la bandera TWINT
		if (dato_a_recibirI2C == 20)   //Si se detecto gas
		{
			dato_a_enviarI2C = 1;
			sistema_activado = 0;
			PORTB &= ~(1 << PORTB4);  // Apaga PB4
		}
		break;
		
		case 0xA8: // SLA+R recibido, maestro solicita lectura
		case 0xB8: // Dato transmitido y ACK recibido
		TWDR = dato_a_enviarI2C; // Cargar el dato en el registro de datos*****************
		TWCR |= (1 << TWINT) | (1 << TWEN) | (1 << TWIE)| (1 << TWEA); // Listo para la próxima operación
		
		case 0xC0: // Dato transmitido y NACK recibido
		case 0xC8: // Último dato transmitido y ACK recibido
		TWCR |= (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Listo para la próxima operación
		break;
		
		default:    //Libera el BUS de cualquier errror
		TWCR |= (1 << TWINT) | (1 << TWSTO);
		
	}
	
}









		


