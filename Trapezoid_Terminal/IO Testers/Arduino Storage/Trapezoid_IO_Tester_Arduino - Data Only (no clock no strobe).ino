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
}
void loop() {

//random data bit set
randNumber = random(1, 10);
Serial.println("DATA");
Serial.println(randNumber);
if (randNumber > 5) {
Serial.println(1);
digitalWrite(D_Data_Input, LOW);  //D=High and H=LOW to make data line low internally
digitalWrite(H_Data_Input, HIGH); //D=LOW and H=HIGH to make data line high internally
}
else {
Serial.println(0);
digitalWrite(D_Data_Input, HIGH);  //D=High and H=LOW to make data line low internally
digitalWrite(H_Data_Input, LOW); //D=LOW and H=HIGH to make data line high internally
}

   delay(4);


}