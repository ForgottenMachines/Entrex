String input;
String Clock;
String Strobe;
String Data;

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
#define X_Video_Pixels 12  //GREEN 1

#define Onboard_LED 13


void setup() {
  pinMode(J_Clock_Input, OUTPUT);  
  pinMode(P_Clock_Input, OUTPUT);  
  pinMode(D_Data_Input, OUTPUT);  
  pinMode(H_Data_Input, OUTPUT);  
  pinMode(N_Strobe_Input, OUTPUT);  
  pinMode(R_Strobe_Input, OUTPUT);  
  pinMode(Onboard_LED, OUTPUT);  
  pinMode(B_Data_Transmit, INPUT);
  pinMode(F_Data_Transmit, INPUT); 
  pinMode(A_Other_Transmit, INPUT);
  pinMode(C_Other_Transmit, INPUT);
  pinMode(X_Video_Pixels, INPUT);

    Serial.begin(250000); 
    Serial.setTimeout(1);

}
 
void loop() {
        Serial.println("xyz123");
        input = "";
        input = Serial.readString();
        if (input != "") {
        Serial.println(input);
        delay(1);
        Clock = (input.substring(1,2));
        Strobe = (input.substring(3,4));
        Data = (input.substring(5,6));
if (Data == "1") {
digitalWrite(D_Data_Input, LOW);  //D=High and H=LOW to make data line low internally
digitalWrite(H_Data_Input, HIGH); //D=LOW and H=HIGH to make data line high internally
}
else {
digitalWrite(D_Data_Input, HIGH);  //D=High and H=LOW to make data line low internally
digitalWrite(H_Data_Input, LOW); //D=LOW and H=HIGH to make data line high internally
}


if (Strobe == "1") {
digitalWrite(N_Strobe_Input, LOW);  
digitalWrite(R_Strobe_Input, HIGH); 
}
else {
digitalWrite(N_Strobe_Input, HIGH);  
digitalWrite(R_Strobe_Input, LOW); 
}


if (Clock == "1") {
digitalWrite(J_Clock_Input, LOW); 
digitalWrite(P_Clock_Input, HIGH);  

}
else {
 digitalWrite(J_Clock_Input, HIGH); 
 digitalWrite(P_Clock_Input, LOW);

}

   delay(1);
        
        }

    }

