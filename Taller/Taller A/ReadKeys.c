/*
* Created: 21/05/2021
* Author: Abraham Medina Carrillo
* Repository: https://github.com/medina1402 <private>
* Email: abraham.medina.carrillo@uabc.edu.mx
*/

/**/
void Init_ReadKeys() {
	/*
	* ADLAR (ADC Left Adjust Result)
	* > ADLAR = 0: ADCH = ADC9-ADC8
	* > ADLAR = 1: ADCH = ADC9-ADC2
	*
	* Reference Selection Bits
	* > REFS1 = 0: REFS0 = 1 (AVCC capacitor externo en el pin AREF)
	*
	* De manera implicita MUX4:0 son 0, por lo que sera el canal 0
	*/
	ADMUX = (1<<REFS0) | (ADLAR < 1);

	/* Nos aseguramos que MUX5 sea 0 para que el canal de ADC sea 0 */
	ADCSRB = 0; // ADCSRB &= ~(1 << MUX5);

	/*
	* Habilitamos ADC
	* > ADEN = 1
	*
	* Preescalador para XTAL: En TJuino es de 16MHz con un divisor de 16
	* solo necesitamos un Byte para el Clock
	* > ADPS2:0 = 1 0 0
	*/
	ADCSRA = (1 << ADEN) | (1 << ADPS2);
}

/* Retornar 0 = Tecla no presionada */
uint8_t ReadKeys() {
	uint8_t position = 0; /* Si no esta en ningun rango, regresa cero */
	/*
	* Debido a que en algunos casos la diferencia entre una posicion y otra es minima
	* se disminuira el margen o eliminar (en este caso se disminuira)
	*/
	uint8_t margin = 7;

	/* Ponemos en 0 MUX5:0 para asegurar canal 0 */
	ADMUX &= ~(0x1F) // ADMUX &= ~(1<<MUX4 | 1<<MUX3 | 1<<MUX2 | 1<<MUX1 | 1<<MUX0);
	ADCSRB = 0; // ADCSRB &= ~(1 << MUX5);

	/* Iniciamos la conversion > ADSC = 1 */
	ADCSRA |= (1 << ADSC);

	/* Fin de la conversion > ADSC = 0 */
	while(ADCSRA & (1 << ADSC));

	/* Determinamos la posicion */
	if( ADCH == 0 ) position = 0; /* No esta un boton persionado */
	else if( ADCH < (0x33 + margin) ) position = 4; /* 3.3V / 5 */
	else if( ADCH < (0x3F + margin) ) position = 3; /* 3.3V / 4 */
	else if( ADCH < (0x55 + margin) ) position = 2; /* 3.3V / 3 */
	else if( ADCH < (0x7F + margin) ) position = 1; /* 3.3V / 2 */

	return position;
}