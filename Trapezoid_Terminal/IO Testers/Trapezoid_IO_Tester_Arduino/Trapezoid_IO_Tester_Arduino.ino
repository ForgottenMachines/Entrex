// J/P Clock Input 
// D/H Data Input 
// N/R Strobe Input

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
//  Serial.begin(115200);
//  Serial.println("Hello World!");
  pinMode(J_Clock_Input, OUTPUT);  
  pinMode(P_Clock_Input, OUTPUT);  
  pinMode(D_Data_Input, OUTPUT);  
  pinMode(H_Data_Input, OUTPUT);  
  pinMode(N_Strobe_Input, OUTPUT);  
  pinMode(R_Strobe_Input, OUTPUT);  
  pinMode(Onboard_LED, OUTPUT);  


}
void loop() {

digitalWrite(J_Clock_Input, LOW); 
digitalWrite(P_Clock_Input, LOW);

digitalWrite(D_Data_Input, LOW);  //D=High and H=LOW to make data line low internally
digitalWrite(H_Data_Input, HIGH); //D=LOW and H=HIGH to make data line high internally

digitalWrite(N_Strobe_Input, LOW);  
digitalWrite(R_Strobe_Input, HIGH); 

//Pulse the clock
digitalWrite(P_Clock_Input, LOW);
digitalWrite(J_Clock_Input, HIGH); 
  delay(1);
digitalWrite(P_Clock_Input, HIGH);  
digitalWrite(J_Clock_Input, LOW); 
  delay(1);


digitalWrite(D_Data_Input, HIGH);  
digitalWrite(H_Data_Input, LOW); 

digitalWrite(N_Strobe_Input, LOW);  
digitalWrite(R_Strobe_Input, HIGH); 


//Pulse the clock
digitalWrite(P_Clock_Input, LOW);
digitalWrite(J_Clock_Input, HIGH); 
digitalWrite(Onboard_LED, HIGH); 
  delay(1000);

digitalWrite(P_Clock_Input, HIGH);  
digitalWrite(J_Clock_Input, LOW); 
digitalWrite(Onboard_LED, LOW); 
  delay(1000);


}