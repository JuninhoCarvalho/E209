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
    int ADC_temp = 0; // ADC tempor�rio, para manipular leitura
    int ADC_read = 0; // ADC_read
    ch &= 0x07;
    // Zerar os 3 primeiros bits e manter o resto
    ADMUX = (ADMUX & 0xF8) | ch;
    // ADSC (ADC Start Conversion)
    ADCSRA |= (1 << ADSC); // Fa�a uma convers�o
    // ADIF (ADC Interrupt Flag) � setada quando o ADC pede interrup��o
    // e resetada quando o vetor de interrup��o
    // � tratado.
    while (!(ADCSRA & (1 << ADIF)))
        ;                   // Aguarde a convers�o do sinal
    for (i = 0; i < 8; i++) // Fazendo a convers�o 8 vezes para maior precis�o
    {
        ADCSRA |= (1 << ADSC); // Fa�a uma convers�o
        while (!(ADCSRA & (1 << ADIF)))
            ;                    // Aguarde a convers�o do sinal
        ADC_temp = ADCL;         // l� o registro ADCL
        ADC_temp += (ADCH << 8); // l� o registro ADCH
        ADC_read += ADC_temp;    // Acumula o resultado (8 amostras) para m�dia
    }
    ADC_read = ADC_read >> 3; // m�dia das 8 amostras
    return ADC_read;
}
void setup()
{
	//Configurando o PWM
	TCCR0A |= (1 << WGM01) | (1 << WGM00); // Modo fast PWM	
	//TCNT0 = 0 -> PD6(OC0A) = 1
	//TCNT0 = OCR0A -> PD6(OC0A) = 0
	TCCR0A |= (1 << COM0A1); //TCNT0 = 0, PD6(OC0A) = 1
	TCCR0B |= (1 << CS00); // preescaler 1	
	//Configurando INT0
	EICRA |= (1 << ISC01);
	EIMSK |= (1 << INT0);	
	//Configurando pd6 saida
	DDRD |= (1 << PD6);
	sei();
	
}

void loop(){
	u16 adc_result0, adc_result1;
    unsigned long int aux;
    unsigned int tensao;
    DDRB = (1 << PB5); // PB5 como sa�da
    Serial.begin(9600);
    ADC_init(); // Inicializa ADC
	adc_result0 = ADC_read(ADC0D); // l� o valor do ADC0 = PC0
        _delay_ms(50);                 // Tempo para troca de canal
        adc_result1 = ADC_read(ADC1D); // l� o valor do ADC1 = PC1
        // condi��o do led
        if (adc_result0 < THRES && adc_result1 < THRES)
            PORTB |= (1 << PB5);
        else
            PORTB &= ~(1 << PB5);
        // Mostra o valor na serial
        Serial.print("ADC0: ");
        Serial.println(adc_result0);
        OCR0A = adc_result0;
        //Serial.print("ADC1: ");
        //Serial.println(adc_result1);
        /*
            C�LCULO TENS�O
            aux = (long)adc_result0 * 5000;
            aux /= 1023;
            tensao = (unsigned int)aux;
            Serial.print("TENSAO0: ");
            Serial.println(tensao);
        */
        _delay_ms(1000);
}
