/* =====================================================================
* @author Abraham Medina Carrillo
* @email abraham.medina.carrillo@uabc.edu.mx
*
* @website https://github.com/Medina1402
* @repository ** private **
* ===================================================================== */
#include <avr/io.h>

#define SetBitPort(port, bit) __asm__ ("SBI %0, %1": :"I" (_SFR_IO_ADDR(port)), "I" ((uint8_t) (bit)))
#define ClrBitPort(port, bit) __asm__ ("CBI %0, %1": :"I" (_SFR_IO_ADDR(port)), "I" ((uint8_t) (bit)))
#define TIME_WINDOW 300
#define SEED 37

/* Prototipos externos, ASM */
extern uint8_t myRand(uint8_t seed);
extern void delay(uint16_t mseg);

/* Estados del boton */
enum ButtonStates {
  eBtnUndefined = 0,
  eBtnShortPressed,
  eBtnLongPressed
};

/* Estados del juego */
enum GameStates {
    eGameRestart = 0,
    eWaitForStart,
    eStartCount,
    eEndCount,
    eGameOver,
    eYouWin,
    eYouLoose
};

/* Prototipos internos */
void InitPorts();
uint8_t checkBtn();
void showInDisplayQueues(uint8_t byte, uint8_t reset);
void updateLeds(uint8_t gameState);

/* Variables globales */
uint32_t millis;

int main(void) {
    uint8_t  currentGameState = eGameRestart;
    uint16_t countdown = 0;
    uint16_t countup = 0;
    InitPorts();

    while(1) {
        switch(checkBtn()) {
            case eBtnShortPressed: currentGameState++;
                break;
            case eBtnLongPressed:  currentGameState = eGameRestart;
                break;
        }

        switch(currentGameState) {
            case eGameRestart:
                countdown = (myRand(SEED)+1)*20;
                countup = 0;
                currentGameState++;
                break;
            case eWaitForStart:
                break;
            case eStartCount:
                countdown--;
                if (countdown == 0)
                    currentGameState++;
                break;
            case eEndCount:
                if (countdown != 0)
                    currentGameState++;
                else
                    countup++;
                break;
            case eGameOver:
                if ((countdown+countup)>TIME_WINDOW)
                    currentGameState = eYouLoose;
                else
                    currentGameState = eYouWin;
                break;
        }

        updateLeds(currentGameState);
        delay(1);
        millis++;
    }
}

/*
* Inicializacion de PORTF con PullUp en el 7mo bit (bit mas significativo) y el resto
* como salidas, inicializadas en 0
*/
void InitPorts(void) {
  MCUCR = (1<<JTD); /* Habilitacion de JTAG */
  MCUCR = (1<<JTD); /* Necesario en PORTF para uso de PF7, PF6, PF5, PF4, ya que estos estan multiplexados con ADCx y otros */
  
  DDRF = (uint8_t) ~(1<<PF7); /*Todo el puerto de salida y PF7 de entrada para PullUp*/
  PORTF = 1<<PF7; /*Todas las salidas en 0, y habilitar el PullUp en el pin7*/
}

/*
*
*/
void showInDisplayQueues(uint8_t byte, uint8_t reset) {
    static uint8_t currentBit = 8; /* Desde el MSB */
    if(!(currentBit--) || reset) currentBit = 8;
    uint8_t currentValueBit = (byte & (1 << currentBit));
    DDRF &= 0xF0;

    if( currentBit > 5 ) { /* 6 y 7 */
        DDRF |= (1<<PF3) | (1<<PF2);
        if(currentBit == 7) {
            if(currentValueBit) SetBitPort(PORTF, PF2);
            else ClrBitPort(PORTF, PF2);
            ClrBitPort(PORTF, PF3);
        } else {
            if(currentValueBit) SetBitPort(PORTF, PF3);
            else ClrBitPort(PORTF, PF3);
            ClrBitPort(PORTF, PF2);
        }
    } else if ( currentBit > 3 ) { /* 4 y 5 */
        DDRF |= (1<<PF2) | (1<<PF0);
        if(currentBit == 5) {
            if(currentValueBit) SetBitPort(PORTF, PF2);
            else ClrBitPort(PORTF, PF2);
            ClrBitPort(PORTF, PF0);
        } else {
            if(currentValueBit) SetBitPort(PORTF, PF0);
            else ClrBitPort(PORTF, PF0);
            ClrBitPort(PORTF, PF2);
        }
    } else if ( currentBit > 1 ) { /* 2 y 3 */
        DDRF |= (1<<PF1) | (1<<PF2);
        if(currentBit == 3) {
            if(currentValueBit) SetBitPort(PORTF, PF1);
            else ClrBitPort(PORTF, PF1);
            ClrBitPort(PORTF, PF2);
        } else {
            if(currentValueBit) SetBitPort(PORTF, PF2);
            else ClrBitPort(PORTF, PF2);
            ClrBitPort(PORTF, PF1);
        }
    } else { /* 0 y 1 */
        DDRF |= (1<<PF1) | (1<<PF0);
        if(currentBit == 1) {
            if(currentValueBit) SetBitPort(PORTF, PF0);
            else ClrBitPort(PORTF, PF0);
            ClrBitPort(PORTF, PF1);
        } else {
            if(currentValueBit) SetBitPort(PORTF, PF1);
            else ClrBitPort(PORTF, PF1);
            ClrBitPort(PORTF, PF0);
        }
    }
}

/*
*
*/
void updateLeds(uint8_t gameState) {
    static uint16_t internalCounter = 0;
    static uint16_t bit_8_LSR = 0xFDFF;
    static uint8_t byteValuePort = 0;
    uint8_t flagUpdate = 0; /* En 1 => Regresa el 1 a MSB de los LEDs */

    switch(gameState) {
        case eWaitForStart:
            if(internalCounter%100 == 0) {
                bit_8_LSR = bit_8_LSR >> 1;
                if(!(bit_8_LSR & 0x0001)) bit_8_LSR |= 0x0080;
                byteValuePort = (uint8_t) bit_8_LSR;
                if(!(bit_8_LSR & 0x0001)) bit_8_LSR = 0xFDFF;
                internalCounter = 0;
                flagUpdate = 1;
            } else flagUpdate = 0;
            break;
        case eStartCount:
            if(internalCounter%100 == 0) {
                internalCounter = 0;
                byteValuePort = myRand(SEED);
                flagUpdate = 1;
            } else flagUpdate = 0;
            break;
        case eEndCount:
            internalCounter = 0;
            byteValuePort = 0x00;
            flagUpdate = 0;
            break;
        case eYouLoose:
            if(internalCounter%1000 == 0) {
                internalCounter = 0;
                byteValuePort = 0xF0;
                flagUpdate = 1;
            } else if(internalCounter%500 == 0) {
                flagUpdate = 1;
                byteValuePort = 0x0F;
            } else flagUpdate = 0;
            break;
        case eYouWin:
            if(internalCounter%500 == 0) {
                internalCounter = 0;
                byteValuePort = 0x00;
                flagUpdate = 1;
            } else if(internalCounter%250 == 0) {
                flagUpdate = 1;
                byteValuePort = 0xFF;
            } else flagUpdate = 0;
            break;
    }
    internalCounter++;
    showInDisplayQueues(byteValuePort, flagUpdate);
}

/*
* Verifica si se presiono el push button, cuenta desde ese momento
* al dejar de presionar retorna el estado de acorde al tiempo presionado
* mientras se mantenga presionado, o no este presionado el valor sera indefinido
*/
uint8_t checkBtn(void) {
    static uint32_t time_push_btn = 0;
    static uint8_t current_state = eBtnUndefined;
    uint16_t timer_temp = 0;

    /* PF7 estara en 1 siempre que no reciba valor de PF6, el cual sera 0 */
    if( !(PINF & (1<<PF7)) ) {
        if(!time_push_btn) {
            time_push_btn = millis;
            current_state = eBtnUndefined;
        } else {
            timer_temp = millis - time_push_btn;
            if(timer_temp > 1000) current_state = eBtnLongPressed;
            else if(timer_temp > 50) current_state = eBtnShortPressed;
        }
        /* Siempre que este presionado el estado sera indefinido */
        return eBtnUndefined;
    }

    /*
    * Se dejo de presionar el boton, el time_push_btn aun conserva el valor de millis
    * el valor de estado se retorna
    */
    if(time_push_btn) {
        time_push_btn = 0; /* En 0 para un nuevo conteo */
        return current_state;
    }

    /* No se ha presionado el boton o ya fue presionado y obtuvimos su valor */
    return eBtnUndefined;
}