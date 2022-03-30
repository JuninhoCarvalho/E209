int main(){
  
  //PD 7 e 5 estão como saída
  //PD =   76543210
  DDRD = 0b10100000;  //1 = saida
  					 //0 = entrada	
  
  //PB 5 e 3 estão como saída
  //PB =   76543210
  DDRB = 0b00101000;  //1 = saida
  					 //0 = entrada	
  
  for(;;){
    /*
  	//1ª operaçao
    //PD5 = 1 = VCC
    //PD7 = 1 = VCC
    //PD      76543210
  	PORTD = 0b10100000;
    //PB5 = 0 = GND
    //PB3 = 0 = GND
    //PB      76543210
  	PORTB = 0b00101000;
    _delay_ms(1500);
    //2ª operacao
  	//PD5 = 0 = GND
    //PD7 = 0 = GND
    //PD      76543210
  	PORTD = 0b00000000;
    //PB5 = 1 = VCC
    //PB3 = 1 = VCC
    //PB      76543210
  	PORTB = 0b00101000;
    _delay_ms(1500);
    */
  	PORTD = 0b00000000;
  	PORTB = 0b00100000;
    _delay_ms(100);
  	PORTB = 0b00001000;
    _delay_ms(100);
  	PORTB = 0b00000000;
  	PORTD = 0b10000000;
    _delay_ms(100);
  	PORTD = 0b00100000;
    _delay_ms(100);
  }	  
}
