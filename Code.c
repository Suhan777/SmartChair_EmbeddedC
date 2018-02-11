#define F_CPU 8000000UL //Make sure to set the system clock to 8MHZ in fuse settings
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) - 1)

//Declaration of our functions
void USART_init(void);
unsigned char USART_receive(void);
void USART_send( unsigned char data);
void USART_putstring(char* StringPtr);
uint16_t adc_read(uint8_t adcx);
void adc_init(void);

char temp0[10]="";
char temp1[10]="";
char temp2[10]="";
char temp3[10]="";
char temp4[10]="";
char temp5[10]="";
char temp6[10]="";
char temp7[10]="";
// only 6 ADC pins

uint16_t adc_value0 = 0;
uint16_t adc_value1 = 0;
uint16_t adc_value2 = 0;
uint16_t adc_value3 = 0;
uint16_t adc_value4 = 0;
uint16_t adc_value5 = 0;
uint16_t adc_value6 = 0;
uint16_t adc_value7 = 0;
char post[200];

int main(void){
	
	USART_init();        //Call the USART initialization code
	adc_init(); //ADC Init
	
//	while(1){        //Infinite loop

	
		
		while(1){

		USART_putstring("sk=net.createConnection(net.TCP, 0)");
		USART_send(13);
		USART_send(10);
		_delay_ms(500);
			
		USART_putstring("sk:on(\"receive\", function(sck, c) print(c) end )");
		USART_send(13);
		USART_send(10);
		_delay_ms(500);
		USART_putstring("sk:connect(8099,\"13.55.201.70\")");
		USART_send(13);
		USART_send(10);
		_delay_ms(500);	
		
		adc_value0 = adc_read(0);
		adc_value1 = adc_read(1);
		adc_value2 = adc_read(2);
		adc_value3 = adc_read(3);
		adc_value4 = adc_read(4);
		adc_value5 = adc_read(5);
		adc_value6 = adc_read(6);
		adc_value7 = adc_read(7);

		itoa(adc_value0, temp0, 10); //converts decimal to string and stores in temp0
		itoa(adc_value1, temp1, 10);
		itoa(adc_value2, temp2, 10);
		itoa(adc_value3, temp3, 10);
		itoa(adc_value4, temp4, 10);
		itoa(adc_value5, temp5, 10);
		itoa(adc_value6, temp6, 10);
		itoa(adc_value7, temp7, 10);

		
		strcpy(post,"sk:send(\"POST /sensorReadings/");
		strcat(post,temp0);
		strcat(post,"/");
		strcat(post,temp1);
		strcat(post,"/");
		strcat(post,temp4); 
		strcat(post,"/");
		strcat(post,temp3);
		strcat(post,"/");
		strcat(post,temp5);
		strcat(post,"/");
		strcat(post,temp2);
		strcat(post,"/");
		strcat(post,temp6);
		strcat(post,"/");
		strcat(post,temp7);
		strcat(post,"/");
		strcat(post,"1000");
		strcat(post," HTTP/1.1\\r\\nHost: 13.55.201.70:8099\\r\\nConnection: keep-alive\\r\\nAccept: */*\\r\\n\\r\\n\")");

		//USART_putstring("sk:send(\"GET /sensorReadings HTTP/1.1\\r\\nHost: 13.55.201.70:8099\\r\\nConnection: keep-alive\\r\\nAccept: */*\\r\\n\\r\\n\")");
		//USART_putstring("sk:send(\"POST /sensorReadings/" + temp0 + "/" + temp1 + "/" + temp2 + "/" + temp3 + "/" + temp4 + "/" + temp5 + "/" + temp6 + "/" + temp7 + "/1000" + "HTTP/1.1\\r\\nHost: 13.55.201.70:8099\\r\\nConnection: keep-alive\\r\\nAccept: */*\\r\\n\\r\\n\")");	
				//USART_send(13);
				//USART_send(10);
				//_delay_ms(1000);
		
		USART_putstring(post);
		strcpy(post,"");


		//
		USART_send(13);
		USART_send(10);
		_delay_ms(500);

		USART_putstring("sk:close()");
		USART_send(13);
		USART_send(10);
		_delay_ms(500);
				
		
		}
		
		
//	}
	
	return 0;
}

uint16_t adc_read(uint8_t adcx) {
	/* adcx is the analog pin we want to use.  ADMUX's first few bits are
	 * the binary representations of the numbers of the pins so we can
	 * just 'OR' the pin's number with ADMUX to select that pin.
	 * We first zero the four bits by setting ADMUX equal to its higher
	 * four bits. */
	ADMUX	&=	0xf0;
	ADMUX	|=	adcx;

	/* This starts the conversion. */
	ADCSRA |= _BV(ADSC);

	/* This is an idle loop that just wait around until the conversion
	 * is finished.  It constantly checks ADCSRA's ADSC bit, which we just
	 * set above, to see if it is still set.  This bit is automatically
	 * reset (zeroed) when the conversion is ready so if we do this in
	 * a loop the loop will just go until the conversion is ready. */
	while ( (ADCSRA & _BV(ADSC)) );

	/* Finally, we return the converted value to the calling function. */
	return ADC;
}

void adc_init(void){
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1);    
	ADMUX |= (1<<REFS0);                //Voltage reference from Avcc (5v)
	ADCSRA |= (1<<ADEN);                //Turn on ADC
	ADCSRA |= (1<<ADSC);                //Do an initial conversion because this one is the slowest and to ensure that everything is up and running
}

void USART_init(void){
	
	UBRRH = (uint8_t)(BAUD_PRESCALLER>>8);
	UBRRL = (uint8_t)(BAUD_PRESCALLER);
	UCSRB = (1<<RXEN)|(1<<TXEN);
	UCSRC = (1<<URSEL) | (1<<USBS) | (3<<UCSZ0);
}

unsigned char USART_receive(void){
	
	while(!(UCSRA & (1<<RXC))){
		
	}
	return UDR;
	
}

void USART_send( unsigned char data){
	
	while(!(UCSRA & (1<<UDRE)));
	UDR = data;
	
}

void USART_putstring(char* StringPtr){
	
	while(*StringPtr != 0x00){
		USART_send(*StringPtr);
	StringPtr++;}
	
}