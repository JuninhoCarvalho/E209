#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define FOSC 16000000U // Clock Speed
#define BAUD 9600
#define MYUBRR ((FOSC / (16 * BAUD)) - 1)
#define botao (1 << PD2)
//Prototipos das funcoes
void UART_Init(unsigned int ubrr);
void UART_Transmit(char *dados);
unsigned char UART_ReceiveByte(void);
void UART_ReceiveString(char *buf, uint8_t n);
char msg_tx[20];
int x = 0;

int main(void)
{
    UART_Init(MYUBRR);
    
    //Configurando PB3 E PB2 como saida
	DDRB |= (1<<PB3) | (1<<PB2);
    
    // Super-loop
    while (1)
    {
        uint8_t bufSize = 20;
        char buffer[bufSize];
        UART_ReceiveString(buffer, bufSize);
        UART_Transmit(buffer);
        UART_Transmit("/n");
        if (strcmp(buffer, "0") == 0){
            PORTB |= (1<<PB3);
            PORTB &= ~(1<<PB2);
        }
		else if(strcmp(buffer, "1") == 0){
			PORTB |= (1<<PB2);
            PORTB &= ~(1<<PB3);
		}
    }
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
