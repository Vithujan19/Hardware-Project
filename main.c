// Measuring distance using ultrasonic distance sensor
// Detect methane gas using MQ5 sensor


//PINA0=MQ5 data out
//PINA1=Ultra trigger
//PIND2=Interrupt/Ultra echo
//PINB0=Outputs
//SCL=C0
//SDA=C1
//PINB7=DHT DO



#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "temperature.h"
#include "i2cmaster.h"
#include "i2c_lcd.h"

static volatile int pulse = 0;
static volatile int i = 0;



//When echo receives reflected signal by trigger (interrupt code)

ISR(INT0_vect)//INT0 is PIND2 which interrupt pin
{
	if(i == 1)
	{
		TCCR1B = 0;
		pulse = TCNT1;
		TCNT1 = 0;
		i = 0;
	}

	if(i==0)
	{
		TCCR1B |= 1<<CS10;
		i = 1;
	}
}



int main(void)
{
	char data[5];
	int count_a = 0;
	lcd_init(LCD_BACKLIGHT_ON);
	lcd_clear();
	lcd_goto_xy(0,0);
	
	DDRB =  (1<<PINB0);
	PORTB =0;
	DDRA |= (1<<PINA1);
	DDRA &= ~(1<<PINA0);
	
	
	
	GICR |= 1<<INT0;
	MCUCR |= 1<<ISC00;
	
	char dist[5];
				
	


	 
	sei();
lcd_init(LCD_BACKLIGHT_ON);
lcd_clear();
lcd_goto_xy(0,0);
lcd_puts("   ...Welcome...");
lcd_goto_xy(0,1);
lcd_puts("  Smart compost");
lcd_goto_xy(0,2);
lcd_puts("    Machine  ");
	_delay_ms(2000);
	while(1)
	{
		
		
		//MQ5 code------------------------------------------------------
		
		if(PINA & (1<<PINA0))
		{
			
			PORTB &= ~(1<<PINB0);
			
		}
		
		else
		{
			
			PORTB|= (1<<PINB0);
			
		}
		
		
		//---------------------------------------------
		lcd_init(LCD_BACKLIGHT_ON);
		lcd_clear();
		lcd_goto_xy(0,0);
		lcd_puts(" Humidity = ");
		lcd_goto_xy(0,1);
		lcd_puts("Temp = ");
		
		lcd_goto_xy(0,2);
		lcd_puts("Space left= ");

		//DHT11------------------------------------------
		DH11_Data x = dht11_get_data();
		
		if (!dht11_validate(x))
		{
			lcd_clear();
			lcd_goto_xy(0,0);
			lcd_puts("Error");
		}
		
		else
		{
			itoa(dht11_get_humidity(x),data,10);
			lcd_goto_xy(11,0);
			lcd_puts(data);
			lcd_puts("%");
			lcd_goto_xy(7,1);
			
			itoa(dht11_get_temperature(x), data,10);
			lcd_puts(data);
			lcd_puts(" C deg ");
			
			//dc water pump motor
			if(data<85){
				PORTB=0b00000100;
			}else{
				PORTB=0x00;
			}
		}
		
		//--------------------------------------------------------
		
		
		//Ultrasonic----------------------------------------------
		
		PORTA |= 1<<PINA1;
		_delay_us(10);

		PORTA &= ~(1<<PINA1);
		count_a = pulse/467;
		itoa(abs(count_a),dist,10);
		lcd_goto_xy(0,2);
		if(abs(count_a) >= 20)
		{
					
			lcd_puts("Have space.");
			lcd_goto_xy(0,3);
			lcd_puts(dist);
			lcd_puts(" cm");
			_delay_ms(30);
		}
		
		
		else
		{
			
			lcd_puts("No space!  ");
			lcd_goto_xy(0,3);
			lcd_puts(dist);
			lcd_puts(" cm");
			_delay_ms(30);
		}
		
		_delay_ms(1000);
		lcd_clear();
		_delay_ms(110);
		lcd_goto_xy(0,0);
		
		
		//---------------------------------------------------------
		
		
		
		
	
	}
}


