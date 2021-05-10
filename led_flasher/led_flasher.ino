#define EACH_100MS (20)
#define EACH_10MS (2)

#define  DUREE_FLASH_1  	2	   // multiple de 10 ms
#define  DUREE_FLASH_2  	3	   // multiple de 10 ms
#define  DUREE_INTER_FLASH  10	   // multiple de 10 ms


// ------------------------------------------------------------
// Nombre de LED
 #define NB_LED (5)

// DEFINITIONS DES PINs En FONCTION DE LA CIBLE 
  #define PIN_EMBEDED_LED  (13)
  
typedef enum { FLASH_1,  INTER_FLASH,  FLASH_2 ,  PAUSE } EN_ETAT_LED;

// Structure d'etat d'une LED
typedef struct {   
                   int pinLED;        	// pin à la quelle est affectée la led
				   bool etatLed;        	// etat de la led (off, On)
				   EN_ETAT_LED etat;  
				   int tempo;        	//!  varaible pour gerer les différents temps d'attente entre chaque état
				   int interval;       	//!  temps ou la led est off (ne varie pas, référence)
                 } ST_PILOT_LED;

// Initialisation du tableau contenant les structure de chaque led.
  ST_PILOT_LED tabLed[NB_LED] = {{8,true, FLASH_1, DUREE_FLASH_1 ,100-19}, //100 c'est 100x10ms soit une seconde, et le -19 c'est le tempsdu pulse.
								{9, true, FLASH_1, DUREE_FLASH_1 ,200-19 }, 
								{10,true, FLASH_1, DUREE_FLASH_1 ,300-19 },
								{11,true, FLASH_1, DUREE_FLASH_1 ,400-19 },
								{12,true, FLASH_1, DUREE_FLASH_1 ,500-19	 } }; 

// Protoypage  
void calcbright(const int channel);
void action(void);
  
/* ************************************************************ */
// PGM SETUP, Lance une seul fois au demarage
/* ************************************************************ */
void setup() {
  int i=0; 

// Initialisation des pin en sortie
	pinMode(PIN_EMBEDED_LED, OUTPUT);
	for ( i=0 ; i<NB_LED ; i++){
		pinMode(tabLed[i].pinLED, OUTPUT);
	}
// ================================================================================
//  Configuration du Timer  (TIMER 1 16BIT)
  cli();
  TCCR1A = 0;
  TCCR1B = ( 1<<WGM12) | (0<<CS12) | (1<<CS11) | (1<<CS10) ; // Prescaler / 64 ==> des PAS de 5ms
  TCCR1C = 0; // not forcing output compare
  TCNT1 = 0; // set timer counter initial value (16 bit value)
  OCR1A = 1250; // Closer to one second than value above, 1250 pas de 4Âµs ca fait
  TIMSK1 = 1 << OCIE1A; // enable timer compare match 1A interrupt
  sei(); // enable interrupts 
  
}

/* ************************************************************************ */
/* ON RENTRE DANS SE TIMER TOUTES LES 5ms
/* ************************************************************************ */
ISR(TIMER1_COMPA_vect) // 16 bit timer 1 compare 1A match
{
  // toutes les 5ms on rentre dans cette fonction !
  
  static int tempo = EACH_10MS;
  static uint16_t iLed = 0;
 
  if (tempo-- == 0 ) {
    action();
    tempo = EACH_10MS ;
  }
}

/* *********************************************************************** */
/* PGM PRINCCIPAL                                                          */
/* ilest executé en permanance                                             */
/* *********************************************************************** */
void action(void) 
{	
	static int val=5;
	static int onoff=0;
	if (--val == 0){
		onoff = 1-onoff;
		val = 5;
	}
	digitalWrite(13,onoff); // pour faire clignoter la Led de l'arduino.
	

	for ( int i=0 ; i<NB_LED ; i++){
		// digitalWrite(tabLed[i].pinLED,onoff);
		
		calcbright(i);
		digitalWrite(tabLed[i].pinLED,tabLed[i].etatLed);
		
	}
}


/* *********************************************************************** */
/* PGM PRINCCIPAL                                                          */
/* Rien a faire                                                            */
/* *********************************************************************** */
void loop() {
  
}



/* *********************************************************************** */
/* Calcul l'etat de chaque LED dans le tableau                             */
/* *********************************************************************** */
void calcbright(const int channel)
{
	
	switch(tabLed[channel].etat)
	{
		case FLASH_1: 	  
			if(tabLed[channel].tempo--  <= 0 ) {
				tabLed[channel].etat = INTER_FLASH; // La prochaine étape sera d'eteindre la led entre les deux flashs
				tabLed[channel].etatLed = false; 
				tabLed[channel].tempo = DUREE_INTER_FLASH; // On place la prochaine durée
			}
			break;
		case INTER_FLASH: 
			if(tabLed[channel].tempo--  <= 0 ) {
				tabLed[channel].etat = FLASH_2; // La prochaine étape sera d'allumée la led pour le deuxime falsh
				tabLed[channel].etatLed = true; 
				tabLed[channel].tempo = DUREE_FLASH_2; // On place la prochaine durée
			}
			break;
		case FLASH_2: 	  
			if(tabLed[channel].tempo--  <= 0 ) {
				tabLed[channel].etat = PAUSE; // La prochaine étape ser d'eteindre la led pendant qqs seconde
				tabLed[channel].etatLed = false; 
				tabLed[channel].tempo = tabLed[channel].interval; // On place la prochaine durée propre à chaque led
			}break;  
		default: //PAUSE 
			if(tabLed[channel].tempo--  <= 0 ) {
				tabLed[channel].etat = FLASH_1; // La prochaine étape sera d'allumer la led pendant qqs seconde
				tabLed[channel].etatLed = true; 
				tabLed[channel].tempo = DUREE_FLASH_1 ; // On place la prochaine durée
			}break;
		
	}
}


