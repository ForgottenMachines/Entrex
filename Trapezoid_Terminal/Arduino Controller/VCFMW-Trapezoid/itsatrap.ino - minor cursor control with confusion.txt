/*

case F works - https://discord.com/channels/700194611091472415/805549399475617833/1120986033346064427
https://discord.com/channels/700194611091472415/805549399475617833/1120496376174891068

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
#define  MISO_PIN  5 
#define  SCK_PIN  4
#define  BDIR_PIN  6
#define  BSYNC_PIN  7
#define  CLOCK_PIN  4
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

struct terminal_t TERMINAL_FIFOS[MAX_TERMINALS];

#define STATE_FLAG_0 0x80
#define S_CHAR_TO_TERM 0x40 
#define STATE_FLAG_2 0x20   //targets UF7A


#define TERMINAL_ID 0x1a
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

  terminal_print(TERMINAL_ID, " "
                              "Entrex 'Trapezoid' Data/Terminal Test   ");
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
uint8_t blen;

  if(Serial.available()){
      switch(Serial.read()){ 
          case 'X':
            Serial.write("Case x\n");
            Serial.write(0x41);
            delay(5);
            Serial.write("   Was that an 'A'? \n");
            Serial.write(0x1b);
          break;

          case 'T': //define termid
            while (!Serial.available());
            id = Serial.read();
            c = new_terminal(id);
            if(c == -1){
              Serial.write(0);  //fail
            }else{
              Serial.write(1);  //success
            }
            break;

          case 's': //send (pc send to terminal)
            while (!Serial.available());
            id = get_idx_from_termid(Serial.read());
              while (!Serial.available());
              blen = Serial.read();
              if (blen <= sizeof(readbuff)){
                if (id != -1 && blen <= (FIFO_SIZE - TERMINAL_FIFOS[id].fifo_o.fifohead + TERMINAL_FIFOS[id].fifo_o.fifotail) % FIFO_SIZE){ 
                  Serial.write(0x01); //continue
                  Serial.readBytes(readbuff, blen);                
                  Serial.write(appendBufferToFifo((fifo_t *)&TERMINAL_FIFOS[id].fifo_o,(char *) readbuff, blen));
                }else{
                  Serial.write(0); //error
                }
              }
            break;

          case 'R': //recv (pc recv from terminal)
            while (!Serial.available());
            id = get_idx_from_termid(Serial.read());
            readBufferFromFifo((fifo_t *)&TERMINAL_FIFOS[id].fifo_i,(char *) readbuff, (TERMINAL_FIFOS[i].fifo_i.fifotail - TERMINAL_FIFOS[i].fifo_i.fifohead + FIFO_SIZE) % FIFO_SIZE);
            Serial.write(readbuff, (TERMINAL_FIFOS[i].fifo_i.fifotail - TERMINAL_FIFOS[i].fifo_i.fifohead + FIFO_SIZE) % FIFO_SIZE); //length
            break;

          case 'p': //poll
            Serial.write(MAX_TERMINALS);
            for(i=0;i<MAX_TERMINALS;i++){
              Serial.write(TERMINAL_FIFOS[i].inuse);
              Serial.write(TERMINAL_FIFOS[i].termid);
              Serial.write((FIFO_SIZE - TERMINAL_FIFOS[i].fifo_o.fifohead + TERMINAL_FIFOS[i].fifo_o.fifotail) % FIFO_SIZE); //remaining
              Serial.write((TERMINAL_FIFOS[i].fifo_i.fifotail - TERMINAL_FIFOS[i].fifo_i.fifohead + FIFO_SIZE) % FIFO_SIZE); //available
            }
            break;
//----------------------------------------------------

          case 'F':
            Serial.write("Case F");
            Serial.println();
            terminal_print(TERMINAL_ID, "hello world!\n");

              delay(100);
              for(int i=0;i<0xff;i++){ 
                if(isprint(i)){
                  terminal_attention(TERMINAL_ID, S_CHAR_TO_TERM);
                    delay(2);
                    term_write_lowlevel(i);
                    term_bsync();
                    delay(4); 
                }
              }  
              terminal_print(TERMINAL_ID,"\n");            
              delay(100);
              for(int i=0;i<0xff;i++){ 
                if(isprint(i)){
                    terminal_attention(TERMINAL_ID, S_CHAR_TO_TERM);
                    delay(1);
                    term_write_lowlevel(i);
                    term_bsync();
                    delay(5); 
                }
            }  
            break;
            
        case 'G':
          terminal_print(TERMINAL_ID, "hello world!\r\n\0");
          terminal_print(TERMINAL_ID, "hello world!\n\0");
          // for(int i=0;i<10;i++){
          //   terminal_print(TERMINAL_ID, "\xa7");
          //   delay(500);
          // }
          break;
          
        case 'H':
          terminal_print(TERMINAL_ID, "hello world!\r\n");
          terminal_print(TERMINAL_ID, "hello world!\xA8");
          terminal_print(TERMINAL_ID, "hello world!\n");
          
          for(int i=0;i<10;i++){
            terminal_print(TERMINAL_ID, "\xa9");
            delay(500);
          }
          break;        
        

        case 'j': // steps cursor from bottom up to top, and back a few spaces, and cursor stays
          Serial.println("Case j");  //how do we get a final backspace out of this, and keep the cursor like we do?  
          Serial.println();
//for (i = 206; i > 188; i--) {
for (i = 206; i > 201; i--) {
          send_asci_decimal(i);
          send_asci_decimal(i);
          send_asci_decimal(i);
          send_asci_decimal(i);
          send_asci_decimal(i);
          send_asci_decimal(i);
};
          break;

        case 'k':  //steps cursor from top to bottom, but cursor doesn't stay
          Serial.println("Case k");
          Serial.println();
//for (i = 188; i < 206; i++) {
for (i = 192; i < 193; i++) {
          send_asci_decimal(i);
          send_asci_decimal(i);
          send_asci_decimal(i);
          send_asci_decimal(i);
          send_asci_decimal(i);
          send_asci_decimal(i);
};
          break;


        case 't':
          Serial.println("Case t for Cursor on Top Line position 24, but it doesn't stay");
i=192;
          send_asci_decimal(i);
          send_asci_decimal(i);
          send_asci_decimal(i);
          break;

        case 'b':
          Serial.println("Case b for Cursor on Bottom Line position 34, but it doesn't stay");
i=202;
          send_asci_decimal(i);
          send_asci_decimal(i);
          send_asci_decimal(i);
          break;


        case 'l': 
          Serial.println("Casel l  - relocates cursor to posotion 21 on the current line, and it stays there!");
i=189;
          send_asci_decimal(i);
          send_asci_decimal(i);
          break;

        case 'e':
          Serial.println("Casel e for ≠");
          send_asci_decimal(182);
          break;

        case 'c':
          Serial.println("Casel c for ¢");
          send_asci_decimal(184);
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

          case 'm':
          for (i = 0; i < 255; i++) {
            Serial.println(i);
            delay(6);
          };
          break;

        case 'K':
          terminal_print(TERMINAL_ID, "     Annual 'Last' CHICAGO CoCoFEST!     "
                                       "         May 4th and 5th 2024           "
                                       "      MAY THE FOURTH BE WITH YOU!       ");
          break;
        case 'J':
        Serial.println("     I am the Entrex Trapezoid...       ");
          terminal_print(TERMINAL_ID,  " "
                                       "     I am the Entrex Trapezoid...       ");
          break;

        case 'L':
          send_asci_decimal(164);
          send_asci_decimal(164);
        Serial.println("     I am the Entrex Trapezoid...       ");
          terminal_print(TERMINAL_ID,  ""
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
          send_asci_decimal(163);
          send_asci_decimal(163);
         break;

        case 'n':
        randNumber = random(10, 20);
          Serial.println(randNumber);
          delay(50);
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
  pinMode(BDIR_PIN, INPUT_PULLUP);

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