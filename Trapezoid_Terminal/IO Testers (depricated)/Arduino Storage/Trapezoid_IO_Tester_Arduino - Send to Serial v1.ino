// J/P Clock Input 
// D/H Data Input 
// N/R Strobe Input

long randNumber;

#define J_Clock_Input 2   //GREEN 2
#define P_Clock_Input 3   //WHT/ORG 1
#define D_Data_Input 4    //WHT/BLUE 2
#define H_Data_Input 5   //ORANGE
#define N_Strobe_Input 6  //WHT/BLUE 1
#define R_Strobe_Input 7  //BLUE
#define B_Data_Transmit 8  //WHT/BROWN 1
#define F_Data_Transmit 9  //BLUE 2
#define A_Other_Transmit 10  //WHT/GREEN 1
#define C_Other_Transmit 11  //GREEN 1

#define Onboard_LED 13

int ACount;
int BCount;
int DCount;
int ClockDuration;
char Data;
char Strobe;
char Clock;
char SendString;


void setup() {
  Serial.begin(115200);
  Serial.println("Entrex Trapezoid I/O Experimenter");
  pinMode(J_Clock_Input, OUTPUT);  
  pinMode(P_Clock_Input, OUTPUT);  
  pinMode(D_Data_Input, OUTPUT);  
  pinMode(H_Data_Input, OUTPUT);  
  pinMode(N_Strobe_Input, OUTPUT);  
  pinMode(R_Strobe_Input, OUTPUT);  
  pinMode(Onboard_LED, OUTPUT);  

randomSeed(1);

ClockDuration = 32;

    Serial.begin(115200); 
    delay(2000);  
    Serial.println("Arduino Ready!");

}
void loop() {


if (DCount < 253) {
DCount++;
randNumber = random(1, 10);
if (randNumber > 5) {
digitalWrite(D_Data_Input, LOW);  //D=High and H=LOW to make data line low internally
digitalWrite(H_Data_Input, HIGH); //D=LOW and H=HIGH to make data line high internally
Data = '1';
}
else {
digitalWrite(D_Data_Input, HIGH);  //D=High and H=LOW to make data line low internally
digitalWrite(H_Data_Input, LOW); //D=LOW and H=HIGH to make data line high internally
Data = '0';
}
} else {
if (DCount < 256) {
DCount++;
//0
digitalWrite(D_Data_Input, HIGH);  //D=High and H=LOW to make data line low internally
digitalWrite(H_Data_Input, LOW); //D=LOW and H=HIGH to make data line high internally
Data = '0';
} else {
if (DCount < 262) { //6 ONES is all we need, so previous value +6
DCount++;
//1
digitalWrite(D_Data_Input, LOW);  //D=High and H=LOW to make data line low internally
digitalWrite(H_Data_Input, HIGH); //D=LOW and H=HIGH to make data line high internally
Data = '1';
} else {
if (DCount < 266) { //6 ONES is all we need, so previous value +6
DCount++;
//0
digitalWrite(D_Data_Input, HIGH);  //D=High and H=LOW to make data line low internally
digitalWrite(H_Data_Input, LOW); //D=LOW and H=HIGH to make data line high internally
Data = '0';
} else {
DCount = 0;
}
}
}
}



if (ACount < ClockDuration) {
ACount++;
//Clock Not Random
digitalWrite(J_Clock_Input, LOW); 
digitalWrite(P_Clock_Input, HIGH);  
 Clock = '0';
 Serial.print("C0S");
 Serial.print(Strobe);
 Serial.print("D");
 Serial.println(Data);
} else {
digitalWrite(N_Strobe_Input, HIGH);  
digitalWrite(R_Strobe_Input, LOW); 
Strobe = '0';
 Serial.print("C");
 Serial.print(Clock);
 Serial.print("S");
 Serial.print(Strobe);
 Serial.print("D");
 Serial.println(Data);
ACount = 0;
}


   delay(10);
 


if (BCount < ClockDuration) {
BCount++;
} else {
digitalWrite(N_Strobe_Input, LOW);  
digitalWrite(R_Strobe_Input, HIGH); 
Strobe = '1';
 Serial.print("C");
 Serial.print(Clock);
 Serial.print("S");
 Serial.print(Strobe);
 Serial.print("D");
 Serial.println(Data);
BCount = 0;
}


 //Clock Not Random
 digitalWrite(J_Clock_Input, HIGH); 
 digitalWrite(P_Clock_Input, LOW);
 Clock = '1';
 Serial.print("C");
 Serial.print(Clock);
 Serial.print("S");
 Serial.print(Strobe);
 Serial.print("D");
 Serial.println(Data);


   delay(10);
 

}