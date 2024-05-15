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
#define X_Video_Pixels 12  //GREEN 1


#define Onboard_LED 13

int ACount;
int BCount;
int DCount;
int ClockDuration;
char Data;
char Strobe;
char Clock;
char SendString;

int Data_Transmit = 0;
int Other_Transmit = 0;
int Video_Pixels = 0;


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
  pinMode(B_Data_Transmit, INPUT);
  pinMode(F_Data_Transmit, INPUT); 
  pinMode(A_Other_Transmit, INPUT);
  pinMode(C_Other_Transmit, INPUT);
  pinMode(X_Video_Pixels, INPUT);



randomSeed(1);

ClockDuration = 9;

    Serial.begin(250000); 
    delay(2000);  
    Serial.println("Arduino Ready!");

}
char DataRecord[1000];
bool Recording = true;
int MaxDCount = 267;


char DataRecord2[1000] = {
0, 1, 8, 9, 2, 4, 8, 8, 1, 3, 2, 2, 9, 3, 5, 6, 3,  
3, 6, 1, 2, 4, 6, 9, 3, 9, 3, 1, 7, 2, 6, 9, 8,  
6, 1, 4, 2, 7, 6, 9, 8, 3, 9, 5, 1, 1, 1, 3, 2,  
7, 8, 2, 5, 1, 7, 4, 3, 5, 9, 5, 6, 8, 9, 5, 5,  
5, 8, 1, 1, 3, 7, 6, 3, 2, 2, 5, 5, 3, 6, 4, 1,  
2, 8, 8, 5, 3, 8, 2, 6, 5, 1, 6, 7, 6, 5, 7, 9,  
6, 8, 8, 3, 1, 7, 5, 7, 2, 4, 9, 7, 2, 5, 6, 8,  
8, 4, 6, 2, 3, 1, 9, 7, 6, 6, 3, 2, 3, 6, 2, 9,  
9, 2, 4, 3, 2, 3, 4, 4, 1, 3, 9, 8, 5, 4, 9, 7,  
8, 5, 4, 7, 9, 8, 5, 9, 2, 7, 3, 1, 7, 8, 6, 2,  
1, 6, 7, 6, 5, 2, 9, 4, 5, 5, 6, 2, 8, 7, 6, 7,  
6, 2, 9, 8, 3, 8, 1, 5, 6, 8, 6, 6, 4, 7, 8, 1,  
6, 5, 6, 2, 1, 4, 5, 7, 2, 8, 2, 1, 3, 2, 3, 4,  
6, 7, 9, 8, 4, 7, 5, 3, 3, 2, 5, 4, 1, 4, 9, 2,  
3, 3, 9, 4, 1, 3, 3, 4, 3, 1, 3, 4, 8, 2, 6, 4,  
4, 5, 4, 7, 7, 7, 6, 3, 6, 9, 7, 9, 8, 0, 0, 0,  
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

char DataRecord3[1000] = {
  6, 1, 2, 4, 6, 9, 3, 9, 3, 1, 7, 2, 6, 9, 8,  
6, 1, 4, 2, 7, 6, 9, 8, 3, 9, 5, 1, 1, 1, 3, 2,  
7, 8, 2, 5, 1, 7, 4, 3, 5, 9, 5, 6, 8, 9, 5, 5,  
5, 8, 1, 1, 3, 7, 6, 3, 2, 2, 5, 5, 3, 6, 4, 1,  
2, 8, 8, 5, 3, 8, 2, 6, 5, 1, 6, 7, 6, 5, 7, 9,  
6, 8, 8, 3, 1, 7, 5, 7, 2, 4, 9, 7, 2, 5, 6, 8,  
8, 4, 6, 2, 3, 1, 9, 7, 6, 6, 3, 2, 3, 6, 2, 9,  
9, 2, 4, 3, 2, 3, 4, 4, 1, 3, 9, 8, 5, 4, 9, 7,  
8, 5, 4, 7, 9, 8, 5, 9, 2, 7, 3, 1, 7, 8, 6, 2,  
1, 6, 7, 6, 5, 2, 9, 4, 5, 5, 6, 2, 8, 7, 6, 7,  
6, 2, 9, 8, 3, 8, 1, 5, 6, 8, 6, 6, 4, 7, 8, 1,  
6, 5, 6, 2, 1, 4, 5, 7, 2, 8, 2, 1, 3, 2, 3, 4,  
6, 7, 9, 8, 4, 7, 5, 3, 3, 2, 5, 4, 1, 4, 9, 2,  
3, 3, 9, 4, 1, 3, 3, 4, 3, 1, 3, 4, 8, 2, 6, 4,  
4, 5, 4, 7, 7, 7, 6, 3, 6, 9, 7, 9, 8, 0, 0, 0,  
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

void loop()
{

  Data_Transmit = digitalRead(B_Data_Transmit);
  Other_Transmit = digitalRead(A_Other_Transmit);
  Video_Pixels = digitalRead(X_Video_Pixels);

  if (Data_Transmit == 1)
  {
    Serial.println("----------DATA OUTPUT----------");
  }
  if (Other_Transmit == 0)
  {
    Serial.println("----------Serial Handshake CTS---------");
  }
  if (Video_Pixels == 1)
  {
    Serial.println("----------VIDEO OUTPUT----------");
    Recording = false; // now play what we recorded.
  }

  if (DCount == 0 && !Recording)
  {
    Serial.println("About to play recording.");
    Serial.println("char DataRecord[1000] = {");
    for(int i = 0; i < 268; i++)
    {
      char buf[5];
      sprintf(buf, "%d", DataRecord[i]);
      Serial.print(buf);
      Serial.print(", ");
      if (i > 1 && i%16 == 0)
        Serial.println(" ");
    }
    Serial.println("}");
    delay(2000);
  }
  
      DCount++;
      if (Recording)
      {
        randNumber = random(1, 10);
        DataRecord[DCount] = randNumber;
      }
      else
      {
        randNumber = DataRecord3[DCount];
      }
      if (randNumber > 5)
      {
        digitalWrite(D_Data_Input, LOW);  // D=High and H=LOW to make data line low internally
        digitalWrite(H_Data_Input, HIGH); // D=LOW and H=HIGH to make data line high internally
        Data = '1';
      }
      else
      {
        digitalWrite(D_Data_Input, HIGH); // D=High and H=LOW to make data line low internally
        digitalWrite(H_Data_Input, LOW);  // D=LOW and H=HIGH to make data line high internally
        Data = '0';
      }
    

    if (ACount < ClockDuration)
    {
      ACount++;
      // Clock Not Random
      digitalWrite(J_Clock_Input, LOW);
      digitalWrite(P_Clock_Input, HIGH);
      Clock = '1';
      Serial.print("C");
      Serial.print(Clock);
      Serial.print("S");
      Serial.print(Strobe);
      Serial.print("D");
      Serial.println(Data);
    }
    else
    {
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

    delay(2);

    if (BCount < ClockDuration)
    {
      BCount++;
    }
    else
    {
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

    // Clock Not Random
    digitalWrite(J_Clock_Input, HIGH);
    digitalWrite(P_Clock_Input, LOW);
    Clock = '0';
    Serial.print("C");
    Serial.print(Clock);
    Serial.print("S");
    Serial.print(Strobe);
    Serial.print("D");
    Serial.println(Data);
  
    delay(2);
  
}