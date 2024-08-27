int Fpin1=D5; //IN1
int Fpin2=D6; //IN2
int Bpin1=D7; //IN3
int Bpin2=D8;  //IN4

//FYI

// 0010 - Stay Put and Right
// 0101 - Back
// 0110 - Reverse left (No)
// 1000 - Left
// 1001 - Awsome Left
// 1010 - FRONT
// 1011 - Left
// 1110 - Stay Put Right

void setup() {
  pinMode(Bpin1,OUTPUT);
  pinMode(Bpin2,OUTPUT);
  pinMode(Fpin1,OUTPUT);
  pinMode(Fpin2,OUTPUT);
}

void loop() {
  //front
  digitalWrite(Fpin1,HIGH);
  digitalWrite(Fpin2,LOW); 
  digitalWrite(Bpin1,HIGH);
  digitalWrite(Bpin2,LOW); 

  delay(1000);

  //back
  digitalWrite(Fpin1,LOW);
  digitalWrite(Fpin2,HIGH);
  digitalWrite(Bpin1,LOW);
  digitalWrite(Bpin2,HIGH); 

  delay(1000);

  //Right
  digitalWrite(Fpin1,LOW);
  digitalWrite(Fpin2,LOW);
  digitalWrite(Bpin1,HIGH);
  digitalWrite(Bpin2,LOW); 

  delay(1000);

  // Left
  digitalWrite(Fpin1,HIGH);
  digitalWrite(Fpin2,LOW);
  digitalWrite(Bpin1,LOW);
  digitalWrite(Bpin2,HIGH); 

  delay(1000);

}
