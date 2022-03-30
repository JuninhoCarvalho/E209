#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#define FOSC 16000000U //Clock Speed
#define BAUD 9600
#define MYUBRR ((FOSC / (16 * BAUD)) - 1)

//Prototipos das funcoes
void UART_Init(unsigned int ubrr);
void UART_Transmit(char *dados);
unsigned char UART_ReceiveByte(void);
void UART_ReceiveString(char *buf, uint8_t n);
char msg_tx[20];

int velocidade;

// Valor de THRESHOLD
#define THRES 500
void ADC_init(void)
{
    // Configurando Vref para VCC = 5V
    ADMUX = (1 << REFS0);
    /*
        ADC ativado e preescaler de 128
        16MHz / 128 = 125kHz
        ADEN = ADC Enable, ativa o ADC
        ADPSx = ADC Prescaler Select Bits
        1 1 1 = clock / 128
    */
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}
int ADC_read(u8 ch)
{
    char i;
    int ADC_temp = 0; // ADC temporário, para manipular leitura
    int ADC_read = 0; // ADC_read
    ch &= 0x07;
    // Zerar os 3 primeiros bits e manter o resto
    ADMUX = (ADMUX & 0xF8) | ch;
    // ADSC (ADC Start Conversion)
    ADCSRA |= (1 << ADSC); // Faça uma conversão
    // ADIF (ADC Interrupt Flag) é setada quando o ADC pede interrupção
    // e resetada quando o vetor de interrupção
    // é tratado.
    while (!(ADCSRA & (1 << ADIF)));                   // Aguarde a conversão do sinal
    for (i = 0; i < 8; i++) // Fazendo a conversão 8 vezes para maior precisão
    {
        ADCSRA |= (1 << ADSC); // Faça uma conversão
        while (!(ADCSRA & (1 << ADIF)));                    // Aguarde a conversão do sinal
        ADC_temp = ADCL;         // lê o registro ADCL
        ADC_temp += (ADCH << 8); // lê o registro ADCH
        ADC_read += ADC_temp;    // Acumula o resultado (8 amostras) para média
    }
    ADC_read = ADC_read >> 3; // média das 8 amostras
    return ADC_read;
}

ISR(INT0_vect) {



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

    uint8_t bufSize = 20;
    char buffer[bufSize];
    UART_ReceiveString(buffer, bufSize);
    UART_Transmit(buffer);
    UART_Transmit("/n");
    if (strcmp(buffer, "L") == 0)
    {
        velocidade = 0;
        UART_Transmit("Sistema Ligado");
        UART_Transmit(msg_tx);
        UART_Transmit("\n");

    }
}

void loop() {
		
    u16 adc_result0, adc_result1;
    unsigned long int aux;
    unsigned int tensao;
    DDRB = (1 << PB5); // PB5 como saída
    Serial.begin(9600);
    ADC_init(); // Inicializa ADC
    adc_result0 = ADC_read(ADC0D); // lê o valor do ADC0 = PC0
    _delay_ms(50);                 // Tempo para troca de canal
    adc_result1 = ADC_read(ADC1D); // lê o valor do ADC1 = PC1
    // condição do led
    if (adc_result0 < THRES && adc_result1 < THRES)
        PORTB |= (1 << PB5);
    else
        PORTB &= ~(1 << PB5);
    // Mostra o valor na serial
    //Serial.print("ADC0: ");
    //Serial.println(adc_result0);
    
    OCR0A = adc_result0;
    //Serial.print("ADC1: ");
    //Serial.println(adc_result1);
    //  CÁLCULO TENSÃO PARA VELOCIDADE 280
    velocidade = (long)adc_result0 * 280;
    velocidade/= 1023;
    tensao = (unsigned int)velocidade;
    
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
