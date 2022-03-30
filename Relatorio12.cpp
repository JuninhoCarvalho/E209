#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define FOSC 16000000U // Clock Speed
#define BAUD 9600
#define MYUBRR ((FOSC / (16 * BAUD)) - 1)

//Prototipos das funcoes
void UART_Init(unsigned int ubrr);
void UART_Transmit(char *dados);
unsigned char UART_ReceiveByte(void);
void UART_ReceiveString(char *buf, uint8_t n);
char msg_tx[20];

int potencia_enchimento = 0;
bool SC = false, SV = true;

ISR(INT0_vect){
	potencia_enchimento += 25;// + 10% DC
	
	if(SV = true){
		if(potencia_enchimento >= 255){
			UART_Transmit("Potência máxima");
	        UART_Transmit(msg_tx);
	        UART_Transmit("\n");
			potencia_enchimento = 0;
		}
	}
	
	else if(SC == true){
		UART_Transmit("Sistema Desligado");
		UART_Transmit(msg_tx);
		UART_Transmit("\n");
	}
}

void setup()
{
    UART_Init(MYUBRR);
    
    //Configurando o PWM
	TCCR0A |= (1 << WGM01) | (1 << WGM00); // Modo fast PWM
	
	//TCNT0 = 0 -> PD6(OC0A) = 1
	//TCNT0 = OCR0A -> PD6(OC0A) = 0
	TCCR0A |= (1 << COM0A1); //TCNT0 = 0, PD6(OC0A) = 1
	TCCR0B |= (1 << CS00); // preescaler 1
    
    //Configuração da interrupção externa
    EICRA |= (1<<ISC01) | (1 << ISC00);
    EIMSK |= (1 << INT0); // Habilita o INT0
    sei();
    
    //Configurando pd6 saida
	DDRD |= (1 << PD6);
	//Pull UP em PD2
    PORTD |= (1 << PD2);
    
    // Super-loop
    while (1)
    {
        uint8_t bufSize = 20;
        char buffer[bufSize];
        UART_ReceiveString(buffer, bufSize);
        UART_Transmit(buffer);
        UART_Transmit("/n");
        if (strcmp(buffer, "L") == 0)
        {
        	potencia_enchimento = 0;
        	SV = true;
        	SC = false;
            UART_Transmit("Sistema Ligado");
            UART_Transmit(msg_tx);
            UART_Transmit("\n");
        }
        else if(strcmp(buffer, "D") == 0){
			UART_Transmit("Sistema Desligado");
			UART_Transmit(msg_tx);
			UART_Transmit("\n");
		}
		else if(strcmp(buffer, "V") == 0){
			UART_Transmit("Potencia de enchimento: ");
			itoa(potencia_enchimento, msg_tx, 10);
			UART_Transmit(msg_tx);
			UART_Transmit("\n");
		}
    }
}

void loop(){
	OCR0A = potencia_enchimento;
}

unsigned char UART_ReceiveByte(void)
{
    // wait for data
    while (!(UCSR0A & (1 << RXC0)));
    // return data
    return UDR0;
}
void UART_ReceiveString(char *buf, uint8_t n)
{
    uint8_t bufIdx = 0;
    char c;
    // while received character is not carriage return
    // and end of buffer has not been reached
    do
    {
        // receive character
        c = UART_ReceiveByte();
        // store character in buffer
        buf[bufIdx++] = c;
    } while ((bufIdx < n) && (c != '*'));
    // ensure buffer is null terminated
    buf[--bufIdx] = 0;
}
void UART_Transmit(char *dados)
{
    // Envia todos os caracteres do buffer dados ate chegar um final de linha
    while (*dados != 0)
    {
        while (!(UCSR0A & (1 << UDRE0))); // Aguarda a transmissão acabar
        // Escreve o caractere no registro de tranmissão
        UDR0 = *dados;
        // Passa para o próximo caractere do buffer dados
        dados++;
    }
}
void UART_Init(uint16_t ubrr)
{
    // Configura a baud rate */
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    // Habilita a recepcao e tranmissao
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    // Configura o formato da mensagem: 8 bits de dados e 1 bits de stop */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
