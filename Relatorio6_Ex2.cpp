ISR(INT0_vect){
	PORTD |= (1 << PD6);
	_delay_ms(200);
	PORTD &= ~(1 << PD6);
}

ISR(INT1_vect){
	EIMSK ^= (1 << INT0);
}

int main(){
	
	//configuraçoes
	DDRD |= (1<<PD6) | (1<<PD7);	//pd6 e pd7 como saida
	
	PORTD |= (1 << PD2); //Pull Up pd2
	
	//config interrupcao INT0
	EICRA |= (1 << ISC01) | (1 << ISC00);
	
	//config interrupcao INT1 ISC11 e ISC01
	EICRA |= (1<<ISC11);
	
	EIMSK |= (1 << INT0) | (1 << INT1); // Habilita o INT0
	
	sei();
	
	for(;;){
		PORTD ^= (1<<PD7); //^ muda se estiver 1 para para 0 e vice versa
		_delay_ms(2000);
	}
	
}