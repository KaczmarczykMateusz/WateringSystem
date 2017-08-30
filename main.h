// Buttons definitions
#define SET_BTN_DDR DDRC |=  _BV(2)
#define SET_BTN_PIN PINC |=  _BV(2)

// Outputs definitions
#define RELAY_INIT DDRC |= (1<<PC0)
#define RELAY_ON    PORTC &= ~(1<<PC0)
#define RELAY_OFF   PORTC |= (1<<PC0)

#define OUTPUT_PIN_A111_INIT DDRC |= (1<<PC1)
#define OUTPUT_PIN_A111_ON    PORTC &= ~(1<<PC1)
#define OUTPUT_PIN_A111_OFF   PORTC |= (1<<PC1)

double wf;
char printLCDBuffer[50];
double temp;
unsigned char ds18b20_pad[9];
char str[8];


void relOFF(void);
void relON(void);
void outOFF(void);
void outON(void);
