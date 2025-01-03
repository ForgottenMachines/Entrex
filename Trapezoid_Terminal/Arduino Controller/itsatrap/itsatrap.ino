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


  send_asci_decimal(164); //reset
  send_asci_decimal(164);
  terminal_print(TERMINAL_ID, " "
                              " Entrex 'Trapezoid' Data/Terminal Test  ");
  send_asci_decimal(192);  // cursor to to top row
  send_asci_decimal(169);  // cursor to leftmost cursor position
delay(1000);
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
int t;
unsigned long s;
int f = 0;
uint8_t word_0 = 0;
uint8_t word_1 = 0;
char readbuff[FIFO_SIZE];
uint8_t c;
uint8_t id;
uint8_t i;
uint8_t r;
uint8_t blen;


  if(Serial.available()){
      switch(Serial.read()){ 

        case 'k' : //keyboard keypress 'clock the data out'
//terminal_attention(terminal_id, S_CHAR_TO_TERM); vs another flag, its probably going to be 0x80, 0x20 are the other flags
//i dont remember the schematic enough 
//then you would need to "send" 0x00 to the terminal to clock the data out
//https://discord.com/channels/700194611091472415/805549399475617833/1248404236862226604

//probably the easiest way to explain is that you need to implement "terminal_getc()" function that is almost exactly like terminal_putc(), the terminal attention function will need a different flag argument  of the 
//ones he highlighted, i dont remember which is which (i can look this weekend). then you would change 
//"term_write_lowlevel(c);" to something like "data-out=term_write_lowlevel(0x00);"  because the protocol is 
//SPI like, data needs to be clocked and as you "send" the 0x00 over the tx wires, the return value should be 
//clocked out simultaneously. which is also how the arduinos SPI.transfer() is written

//oh and the the fifo stuff they didnt recognize in the code was a driver to create a unix socket to provide 
//service to multiple terminals so they can be unix terminals

          Serial.println("Case k: keyboard keypress 'clock the data out'");

          break;

        case 'j': // steps cursor from bottom up to top, and back a few spaces, and cursor stays
          Serial.println("Case j");  //how do we get a final backspace out of this, and keep the cursor like we do?  
          Serial.println();
for (i = 206; i > 188; i--) {
          send_asci_decimal(i);
          send_asci_decimal(i);
};
          break;

        case 'S':  //steps cursor from top to bottom, but cursor doesn't stay
          Serial.println("Case S");
          Serial.println();
for (i = 188; i < 206; i++) {
          send_asci_decimal(i);
          send_asci_decimal(i);
};
          break;

          case '@':
          Serial.println("Case @");
i=192;
          send_asci_decimal(i);
          send_asci_decimal(64); 
          //but this puts a full line of amprosands above line 1....or is it line 1?
          break;

          case '2':
          Serial.println("Case 2");
for (i = 192; i < 208; i++) {
          send_asci_decimal(i);
          send_asci_decimal(64); 
};
          //but this puts a full SCREEN of amprosands above line 1....or is it line 1?
          break;


//first byte is row, second byte is column?  
//rows top to bottom:  192-203 (202 when last line won't display)
//columns: 169-191 stays, 192-208 won't stay
        case 't':
          Serial.println("Case t for Cursor on Top Line position 24, but it doesn't stay");
          send_asci_decimal(192);  // cursor to to top row
          send_asci_decimal(160);  // cursor to leftmost cursor position
          break;

        case '3':
          Serial.println("Case 3 ");
i=191;
          send_asci_decimal(i);
          send_asci_decimal(169); //anything past 191 "wont' stay"
          break;


        case 'b':
          Serial.println("Case b for Cursor on Bottom Line position 34, but it doesn't stay");
i=202;
          send_asci_decimal(i);
          send_asci_decimal(169);
          break;

        case 'B':
          Serial.println("Case B for bb for Cursor below Bottom");
i=203;
          send_asci_decimal(i);
          send_asci_decimal(169);
          break;


        case 'm':
          Serial.println("Case m for middle");
i=197;
          send_asci_decimal(i);
          send_asci_decimal(169);
          break;



        case 'l': //LISTEN for the Keyboard (or try to)
          Serial.println("LISTEN for the Keyboard (or try to)");
          send_asci_decimal(182);
          break;

        case 'e':
          Serial.println("Case e for ≠");
          send_asci_decimal(182);
          send_asci_decimal(28);
                    break;

        case 'c':
          Serial.println("Case c for ¢");
          send_asci_decimal(184);
          send_asci_decimal(30);
          break;

        case 'd':
          Serial.println("Case d for DEL");
          send_asci_decimal(127);
          break;

          case 'w':  //it locks the cursor but we don't know what it's trying to do
          send_asci_decimal(162);
//          send_asci_decimal(162);
          break;


        case 'r':  //once works, unless the terminal is locked and then we have to do it twice, so might as well do it twice all the time
          Serial.println("RESET!");
          send_asci_decimal(164);
          send_asci_decimal(164);
//          terminal_print(TERMINAL_ID,  "Screen cleared/reset with 164 twice ");
          break;

        case 'x':
          Serial.println("XMISSION ERROR");
          send_asci_decimal(163);
          send_asci_decimal(163);
          break;

          case 'M':
          Serial.println("RESET!");
          send_asci_decimal(164);
          send_asci_decimal(164);
          send_asci_decimal(192);  // cursor to to top row
          send_asci_decimal(129);  // cursor to leftmost cursor position
          for (i = 1; i < 255; i++) {
            send_asci_decimal(i);
            delay(6);
          };
          break;

        case 'K':
          send_asci_decimal(164); //reset
          send_asci_decimal(164);
          terminal_print(TERMINAL_ID, "     Annual 'Last' CHICAGO CoCoFEST!     "
                                       "         May 4th and 5th 2024           "
                                       "      MAY THE FOURTH BE WITH YOU!       ");
          break;

        case 'J':
        Serial.println("     I am the Entrex Trapezoid...       ");
          send_asci_decimal(164); //reset
          send_asci_decimal(164);
          send_asci_decimal(192);  // cursor to to top row
          send_asci_decimal(169);  // cursor to leftmost cursor position on the 2nd row
          terminal_print(TERMINAL_ID,  " "
                                       "     I am the Entrex Trapezoid...       ");
          break;

        case 'L':
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
         break;

        case 'P':
        Serial.println("Case P");
for (i = 0; i < 128; i++) {
          send_asci_decimal(164); //reset
          send_asci_decimal(164);
          send_asci_decimal(192);  // cursor to to top row
          send_asci_decimal(169);  // cursor to leftmost cursor position
          itoa(i,numberArray,10);
          if (i < 10) { terminal_print(TERMINAL_ID, "00"); }
          else if (i < 100) { terminal_print(TERMINAL_ID, "0"); }
          terminal_print(TERMINAL_ID, numberArray);
          terminal_print(TERMINAL_ID, " - ");
          send_asci_decimal(i); 
          delay(300);
};
         break;

         case 's':   //SCROLL
          send_asci_decimal(160);  //SCROLL
         break;


//first byte is row, second byte is column?  
//rows top to bottom:  192-203 
//columns: 129-168 BUT 160, 163 and 164 error out and 128 goes to the last column of the previous line
//previous thought on columns: 169-191 stays, 192-208 won't stay

        case 'q':
          send_asci_decimal(164); //reset
          send_asci_decimal(164);
          send_asci_decimal(192);  // cursor to to top row
          send_asci_decimal(129);  // cursor to leftmost cursor position
          terminal_print(TERMINAL_ID,  "1234567890123456789012345678901234567890"
                                       "2        1         2         3         4"
                                       "3                                       "
                                       "4                                       "
                                       "5                                       "
                                       "6                                       "
                                       "7                                       "
                                       "8                                       "
                                       "9                                       "
                                       "10       1         2         3         4"
                                       "11  567890123456789012345678901234567890"
                                       "12                                      ");
          send_asci_decimal(195);  //row
          send_asci_decimal(133);  //column
          terminal_print(TERMINAL_ID, "r=");
for (r = 192; r < 204; r++) { // row
          itoa(r,numberArray,10);
          send_asci_decimal(195);  //row
          send_asci_decimal(135);  //column
          terminal_print(TERMINAL_ID, numberArray);
          send_asci_decimal(196);  //row
          send_asci_decimal(133);  //column
          terminal_print(TERMINAL_ID, "c=");
for (i = 128; i < 169; i++) { // column but skip 160, 163 and 164 cuz they do something else
          send_asci_decimal(196);  //row
          send_asci_decimal(135);  //column
          itoa(i,numberArray,10);
          if (i < 10) { terminal_print(TERMINAL_ID, "00"); }
          else if (i < 100) { terminal_print(TERMINAL_ID, "0"); }
          terminal_print(TERMINAL_ID, numberArray);
          
if (i == 160) {
  terminal_print(TERMINAL_ID, " - Skipped / INVALID");
}
else if (i == 162) {
  terminal_print(TERMINAL_ID, " - Skipped / INVALID");
}
else if (i == 163) {
  terminal_print(TERMINAL_ID, " - Skipped / INVALID");
}
else if (i == 164) {
  terminal_print(TERMINAL_ID, " - Skipped / INVALID");
}
else {
  if (i == 161) { terminal_print(TERMINAL_ID, "                    ");}
  if (i == 165) { terminal_print(TERMINAL_ID, "                    ");}
          send_asci_decimal(r);
          send_asci_decimal(i); 

};
          delay(350);
};
};
         break;


        case 'n':
        randNumber = random(10, 20);
          Serial.println(randNumber);
          delay(50);
        break;


case 'i':
          Serial.println(Serial.read());
break;



        }
      }

  service_update(); 
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