#define BUTTON_ON !(PIND & 0b00010000)
#define LED_RED_ON PORTD = PORTD | 0b10000000
#define LED_RED_OFF PORTD = PORTD & ~(0b10000000)
#define LED_YELLOW_ON PORTD = PORTD | 0b01000000
#define LED_YELLOW_OFF PORTD = PORTD & ~(0b01000000)
#define LED_GREEN_ON PORTD = PORTD | 0b00100000
#define LED_GREEN_OFF PORTD = PORTD & ~(0b00100000)
#define DELAY _delay_ms(500)
#define VERDE   0
#define AMALELO 1
#define VERMELHO 2
int estado_atual = VERDE;

int main(void)
{
    DDRD = DDRD | 0b10100000;   // Configurando pino 5 e 7 como saída
    PORTD = PORTD | 0b00010000; // Habilita resistor de PULL-UP   
    {
        for(;;) {
            switch (estado_atual)
            {
            case VERDE:
                LED_RED_OFF;   //desliga led do pino 7
                LED_YELLOW_OFF; //desliga led do pino 6
				LED_GREEN_ON; //desliga led do pino 5
				DELAY(12000);
				estado_atual = AMARELO;
                break;
            case AMARELO:
                LED_RED_ON;    //desliga led do pino 7
                LED_YELLOW_ON; //liga led pino 6
                LED_GREEN_OFF; //desliga led do pino 5
                DELAY(3000);
                estado_atual = VERMELHO;
                break;
            case VERMELHO:
                LED_RED_ON  //liga led do pino 7
                LED_YELLOW_OFF //desliga led do pino 6
                LED_GREEN_ON; //desliga led do pino 5
                DELAY(15000);
                estado_atual = VERDE;
                break;
            }
        }
    }
}
