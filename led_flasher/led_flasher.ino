#define EACH_100MS (20)
#define EACH_10MS (2)

#define  DUREE_LED_ON  2	   // multiple de 10 ms
#define  DUREE_LED_OFF 50      // multiple de 10 ms


// ------------------------------------------------------------
// Nombre de LED
 #define NB_LED (5)

// DEFINITIONS DES PINs En FONCTION DE LA CIBLE 
  #define PIN_EMBEDED_LED  (13)

// Structure d'etat d'une LED
typedef struct {   
                   int pinLED;        	// pin à la quelle est affectée la led
				   bool etat;        	// etat de la led (off, On)
                   int tempo_etat_off;      //!  temps ou la led est off
				   int tempo_etat_on;       //!  temps ou la led est on
                 } ST_ETAT_LED;

// Initialisation du tableau contenant les structure de chaque led.
  ST_ETAT_LED tabLed[NB_LED] = {{8,false,  DUREE_LED_OFF,DUREE_LED_ON},
								{9,false,  DUREE_LED_OFF*2,DUREE_LED_ON}, 
								{10,false, DUREE_LED_OFF*3,DUREE_LED_ON},
								{11,false, DUREE_LED_OFF*4,DUREE_LED_ON},
								{12,false, DUREE_LED_OFF*5,DUREE_LED_ON} }; 

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
		digitalWrite(tabLed[i].pinLED,tabLed[i].etat);
		
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
	if (tabLed[channel].etat)	//La LED est ALLUMEE, on vérifie que son temp d'allumage n'est pas atteind
	{
		if(tabLed[channel].tempo_etat_on--  <= 0 ) {
			tabLed[channel].etat = false; // Sa durée a ALLUMEE est passé, son état devient ETEIND
			tabLed[channel].tempo_etat_on = DUREE_LED_ON; // On replace la valeur pour la prochaine fois
		}
	} else {
		if(tabLed[channel].tempo_etat_off--  <= 0 ) {
			tabLed[channel].etat = true; // Sa durée a ETEIND est passé, son état devient ALLUMEE
			tabLed[channel].tempo_etat_off = DUREE_LED_OFF; // On replace la valeur pour la prochaine fois
		}
	}
}


