int brilho = 0;

ISR(INT0_vect){
	brilho += 25;// + 10% DC
	
	if(brilho >= 255){
		brilho = 0;
	}
}

void setup(){
	
	//Configs
	
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
	
	//Pull UP em PD2
	PORTD |= (1 << PD2);
		
	sei();	
}

void loop(){
	OCR0A = brilho;
}


