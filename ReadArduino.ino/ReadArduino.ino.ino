#define led 13
void setup()
{
  Serial.begin(9600);
  pinMode(led,OUTPUT);
  Serial.setTimeout(30);
}
int c=0;
void loop() 
{ 
   while (Serial.available())c = Serial.read();
   digitalWrite(led,1);
   delay(c);
   digitalWrite(led,0);
   delay(c);
} 
