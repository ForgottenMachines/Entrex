/*

BSYNC:  pin 7
BDIR:    pin 6

MOSI: pin 51
MISO: pin 50
SCK:  pin 52


(differential pairs may need to be swapped, these are unconfirmed), we can tune 
params through SPI modes as well

                    /----------------------------------------->D 
__________         |         ___________                         TX                          
      MOSI|-o>-----+----->o-|1  ('04)  2|-o>------------------>H 
          |                 |           |      
M        7|-o>----+------>o-|3         4|-o>------------------>R 
E         |-      |         |           |                       BSYNC    TERMINAL
G         |-       \------------------------------------------>N         
A         |-                |           |
2      SCK|-o>----+------>o-|5         7|-o>------------------>P
5         |-      |          -----------                        CLK
6         |-       \------------------------------------------>J
0         |-                          
         6|-o<------------------------------------------------<C BDIR (positive logic pin only)              
      MISO|-o<------------------------------------------------<B RX  (positive logic pin only)          
 */

#include <SPI.h>

uint8_t term_write_lowlevel(uint8_t word_0);

void terminal_print(uint8_t terminal_id, char * str);
void sync_bitcounter();


#define  MOSI_PIN  2
#define  Port_Select_Switch  4
#define  BDIR_PIN  6
#define  BDIR_PIN_inverse  5 
#define  BSYNC_PIN  7
#define  CLOCK_PIN  4

#define  Data_Transmit 50
#define  Data_Transmit_inverse 49



#define  BIT_SPEED  2000




#define MAGIC_CHARACTER  0xA5
#define CRLF_CHARACTER  0xA8

#define BEEP_CHARACTER  0xA9  //out 0x04 to port 50
#define LED_CHARACTER  0xA7  //out 0x01 to port 50
// #define CRLF_CHARACTER  0xA2
// #define CRLF_CHARACTER  0xA0
// #define CRLF_CHARACTER  0xA3
// #define CRLF_CHARACTER  0xA4  //call mask display (clear screen?)

#define WHITESPACE2_CHARACTER  0xB6
#define WHITESPACE3_CHARACTER  0xB8
#define VERTICAL_TAB_CHARACTER 0x0B
#define BACKSPACE_CHARACTER    0x08
#define LINEFEED_CHARACTER    0x0a


#define MAX_TERMINALS 4
#define FIFO_SIZE 10

typedef struct fifo_t{
  uint8_t  fifo[FIFO_SIZE];
  uint16_t fifohead;
  uint16_t fifotail;
}fifo_t;

typedef struct terminal_t{
  uint8_t   inuse;
  uint8_t   termid;
  fifo_t    fifo_o;
  fifo_t    fifo_i;
}terminal_t;

long randNumber;

char numberArray[20];

struct terminal_t TERMINAL_FIFOS[MAX_TERMINALS];

#define STATE_FLAG_0 0x80
#define S_CHAR_TO_TERM 0x40 
#define STATE_FLAG_2 0x20   //targets UF7A

/////////////SET TERMINAL ADDRESS HERE///////////////////////
//#define TERMINAL_ID 0x1a
#define TERMINAL_ID 0x5e
/////////////SET TERMINAL ADDRESS HERE///////////////////////
//Address alternatives
//1E = normal/same
//9E = 'winks' all text
//DE = 'winks' all text
//FE = 'winks' all text
//7E = can get "PRINTER NOT READY"
//BE = 'winks' all text
//3E = can get "PRINTER NOT READY"
//5E = normal/same

#define COMMAND_MODE  0
#define ECHO_MODE     1

int MODE = COMMAND_MODE;

int8_t new_terminal(uint8_t terminal_id){
uint8_t newidx = 0,i;

  for(i=0;i<MAX_TERMINALS;i++){               //find an unused terminal slot
    if(TERMINAL_FIFOS[i].inuse == false){
      TERMINAL_FIFOS[i].inuse = true;
      TERMINAL_FIFOS[i].termid = terminal_id;
      return i;
    }
  }

  for(i=0;i<MAX_TERMINALS;i++){             //if there isnt an unused terminal slot, find a disused one
    if((TERMINAL_FIFOS[i].fifo_i.fifotail == TERMINAL_FIFOS[i].fifo_i.fifohead) &&
    (TERMINAL_FIFOS[i].fifo_o.fifotail == TERMINAL_FIFOS[i].fifo_o.fifohead)){
      TERMINAL_FIFOS[i].inuse = true;
      TERMINAL_FIFOS[i].termid = terminal_id;
      return i;
    }
  }
  return -1;
}

int8_t get_idx_from_termid(uint8_t terminal_id){
uint8_t i;


 for ( i = 0; i < MAX_TERMINALS; i++) {
   if((TERMINAL_FIFOS[i].termid == terminal_id)){
     return i;
   }
 }
  return -1; //fixme
}  



void setup() {
  randomSeed(analogRead(0)); //for random number testing

  Serial.begin(115200);

  memset(&TERMINAL_FIFOS,0,sizeof(TERMINAL_FIFOS));

  term_init();

  Serial.println("boot");

  
}


void service_update(){
int i;
static int last_termal_poll_time = 0;

  for ( i = 0; i < MAX_TERMINALS; i++) {
    // terminal_t *terminal = &TERMINAL_FIFOS[i];

    if (TERMINAL_FIFOS[i].inuse && TERMINAL_FIFOS[i].fifo_o.fifotail != TERMINAL_FIFOS[i].fifo_o.fifohead) {
      uint8_t cb = TERMINAL_FIFOS[i].fifo_o.fifo[TERMINAL_FIFOS[i].fifo_o.fifotail];
      terminal_putc(TERMINAL_FIFOS[i].termid, cb);
      TERMINAL_FIFOS[i].fifo_o.fifotail = (TERMINAL_FIFOS[i].fifo_o.fifotail + 1) % FIFO_SIZE;
    }
  }
  
  if(last_termal_poll_time < (millis() + 200)){
     for ( i = 0; i < MAX_TERMINALS; i++) {
       int c;
        //fixmepoll the terminal for data
        // if(poll_terminal){
          // appendBufferToFifo((fifo_t *)&TERMINAL_FIFOS[i].fifo_i,(char *) &c, 1);
        // }
     }
     last_termal_poll_time = millis();
  }
}

void loop() {

  int TheDelay = 2000;

  send_asci_decimal(164); //reset
  send_asci_decimal(164);
  terminal_print(TERMINAL_ID, " "
                              " Entrex 'Trapezoid' Data/Terminal Test  ");
  send_asci_decimal(192);  // cursor to to top row
  send_asci_decimal(169);  // cursor to leftmost cursor position
delay(TheDelay);
          send_asci_decimal(164); //reset
          send_asci_decimal(164);
          Serial.println("  GRID of numbers...       ");
          send_asci_decimal(192);  // cursor to to top row
          send_asci_decimal(129);  // cursor to leftmost cursor position
          terminal_print(TERMINAL_ID,  "01 - 67890123456789012345678901234567890"
                                       "02       1         2         3         4"
                                       "03 - 67890123456789012345678901234567890"
                                       "04       1         2         3         4"
                                       "05 - 67890123456789012345678901234567890"
                                       "06       1         2         3         4"
                                       "07 - 67890123456789012345678901234567890"
                                       "08       1         2         3         4"
                                       "09 - 67890123456789012345678901234567890"
                                       "10       1         2         3         4"
                                       "11 - 67890123456789012345678901234567890"
                                       "12 -   I am the Entrex Trapezoid...     ");
          send_asci_decimal(192);  // cursor to to top row
          send_asci_decimal(129);  // cursor to leftmost cursor position
delay(TheDelay);
          send_asci_decimal(164); //reset
          send_asci_decimal(164);
          Serial.println("  VCFMW 20   #1    ");
          send_asci_decimal(192);  // cursor to to top row
          send_asci_decimal(129);  // cursor to leftmost cursor position
          terminal_print(TERMINAL_ID,  " #     #  #####  ####### #     # #     #" 
                                       " #     # #     # #       ##   ## #  #  #" 
                                       " #     # #       #####   # # # # #  #  #" 
                                       "  #   #  #       #       #  #  # #  #  #" 
                                       "   # #   #     # #       #     # #  #  #" 
                                       "    #     #####  #       #     #  ## ##"  
                                       " "
                                       "              #####    ###"               
                                       "                   #  #   #"              
                                       "              #####  #     #"             
                                       "             #        #   #"             
                                       "             #######   ###");
          send_asci_decimal(192);  // cursor to to top row
          send_asci_decimal(129);  // cursor to leftmost cursor position
delay(TheDelay);
          send_asci_decimal(164); //reset
          send_asci_decimal(164);
          Serial.println("  VCFMW 20  #2     ");
          send_asci_decimal(192);  // cursor to to top row
          send_asci_decimal(129);  // cursor to leftmost cursor position
          terminal_print(TERMINAL_ID,  "  V     V   CCC  FFFF  M   M  W     W"
                                       "  V     V  C     F     MM MM  W     W"
                                       "   V   V   C     FFF   M M M  W  W  W"
                                       "    V V    C     F     M   M   W W W"
                                       "     V      CCC  F     M   M    W W"
                                       " "
                                       " "
                                       "                 22   000"
                                       "                2  2 0  00"
                                       "                  2  0 0 0"
                                       "                 2   00  0"
                                       "                2222  000");
          send_asci_decimal(192);  // cursor to to top row
          send_asci_decimal(129);  // cursor to leftmost cursor position
delay(TheDelay);

}




void term_bsync(){
    digitalWrite(BSYNC_PIN, LOW); //logic positive logic verified.. idle state of the line is high
    delayMicroseconds(5);
    digitalWrite(BSYNC_PIN, HIGH);
}


void sync_bitcounter(){
    SPI.transfer(0xff);
    SPI.transfer(0xff);
}


void terminal_println(uint8_t terminal_id, char * instr){
  terminal_print(terminal_id, instr);
  terminal_print(terminal_id, "\n");
}

void terminal_print(uint8_t terminal_id, char * instr){
int e;
  terminal_send(terminal_id, instr, strlen(instr));
}

void terminal_send(uint8_t terminal_id, char * buff, uint16_t bufflen){
int e;
  for(e=0;e<bufflen;e++){
    terminal_putc(terminal_id, buff[e]);
  }
}

void terminal_putc(uint8_t terminal_id, uint8_t c){
    // Serial.print("putc(");
    // Serial.print(c,HEX);
    // Serial.println(")");
    
    // terminal_attention(terminal_id, S_CHAR_TO_TERM);
    // delay(2);
    // term_write_lowlevel(c);
    // term_bsync();
    // delay(2);    //might be able to yank this out   

    term_write_lowlevel(c);
    delay(2);
    digitalWrite(BSYNC_PIN, HIGH);
    delay(1);
    terminal_attention(terminal_id, S_CHAR_TO_TERM);
    delay(1);
    digitalWrite(BSYNC_PIN, LOW);
    delay(2);
}

bool terminal_attention(uint8_t terminal_id, uint8_t command){

   term_write_lowlevel(terminal_id|command);

//fixme, everuthing after tthis is wacky, and im not sure how ot treat recv yet

  if(command & STATE_FLAG_0){ 

  }

  if(command & S_CHAR_TO_TERM){

    return true;
  }
  
  if(command & STATE_FLAG_2){
 
  }


}



uint8_t appendBufferToFifo(fifo_t *fifo, uint8_t *buffer, uint8_t length) {
uint8_t i;

  for (i = 0; i < length; i++) {
    if (((fifo->fifohead + 1) % FIFO_SIZE) != fifo->fifotail) {
      fifo->fifo[fifo->fifohead] = buffer[i];
      fifo->fifohead = (fifo->fifohead + 1) % FIFO_SIZE;
    } else {
      // printf("FIFO is full. Cannot append buffer.\n");
        return length;
      break;
    }
  }
  return length;
}

void readBufferFromFifo(fifo_t *fifo, uint8_t *buffer, uint8_t * length) {
  for (uint16_t i = 0; i < *length; i++) {
    if (fifo->fifotail != fifo->fifohead) {
      buffer[i] = fifo->fifo[fifo->fifotail];
      fifo->fifotail = (fifo->fifotail + 1) % FIFO_SIZE;
    } else {
      length = i;
      return;
    }
  }
}

void term_init(){
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV128); //slow things down if needed
  SPI.setBitOrder(LSBFIRST);
  SPI.setDataMode(SPI_MODE2);
  pinMode(BSYNC_PIN,OUTPUT);
  pinMode(BDIR_PIN_inverse, INPUT_PULLUP);
  pinMode(BDIR_PIN, INPUT);  //lives "low" so don't pull up.  Adding pull-down resistor
  pinMode(Port_Select_Switch, INPUT_PULLUP);
  pinMode(Data_Transmit, INPUT_PULLUP);
  pinMode(Data_Transmit_inverse, INPUT_PULLUP);


  sync_bitcounter();
  //term_bsync();

  digitalWrite(BSYNC_PIN, HIGH);
  delay(1);
  terminal_attention(TERMINAL_ID, S_CHAR_TO_TERM);
  delay(1);
  digitalWrite(BSYNC_PIN, LOW);
  delay(50);
}


inline void term_beep(uint8_t terminal_id){
uint8_t arg = BEEP_CHARACTER;

  terminal_send(terminal_id, &arg, 1);
}

void send_asci_decimal(uint8_t asciiDecimal){
          Serial.println(asciiDecimal);
          term_write_lowlevel(asciiDecimal);
          delay(50);
          digitalWrite(BSYNC_PIN, HIGH);
          delay(1);
          terminal_attention(TERMINAL_ID, S_CHAR_TO_TERM);
          delay(1);
          digitalWrite(BSYNC_PIN, LOW);
          delay(50);
}

void term_magic(uint8_t terminal_id, uint8_t command, uint8_t arg){
  terminal_send(terminal_id, &command,1);
  terminal_send(terminal_id, &arg, 1);
}


uint8_t term_write_lowlevel(uint8_t word_0){
  // was DDDDDDDD01111111
  // 1DDDDDDDD0111111
  return  SPI.transfer16( (word_0 << 7) | 0x803f); //9 bit transfer hack seems to work ith the cards state machine without consequence
  delay(4);  //will need tightening
}


uint16_t term_read_lowlevel(uint8_t termid, uint8_t cmd){
}