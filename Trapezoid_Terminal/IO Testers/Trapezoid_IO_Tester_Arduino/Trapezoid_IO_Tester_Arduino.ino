// J/P Clock Input 
// D/H Data Input 
// N/R Strobe Input

constexpr int J_Clock_Input = 0;
constexpr int N_Clock_Input = 1;
constexpr int D_Data_Input = 2;
constexpr int H_Data_Input = 3;
constexpr int N_Strobe_Input = 4;
constexpr int R_Strobe_Input = 5;

void setup() {
  Serial.begin(115200);
  Serial.println("Hello World!");
  pinMode(J_Clock_Input, OUTPUT);  
  pinMode(N_Clock_Input, OUTPUT);  
  pinMode(D_Data_Input, OUTPUT);  
  pinMode(H_Data_Input, OUTPUT);  
  pinMode(N_Strobe_Input, OUTPUT);  
  pinMode(R_Strobe_Input, OUTPUT);  

}
void loop() {

digitalWrite(J_Clock_Input, LOW);  //keep one side of the pair low all the time
digitalWrite(N_Clock_Input, LOW);

digitalWrite(D_Data_Input, LOW);  
digitalWrite(H_Data_Input, LOW);  

digitalWrite(N_Strobe_Input, LOW);  
digitalWrite(R_Strobe_Input, LOW); 

//Pulse the clock
digitalWrite(N_Clock_Input, LOW);
  delay(10);
digitalWrite(N_Clock_Input, HIGH);  
  delay(10);


}