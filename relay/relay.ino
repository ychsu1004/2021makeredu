//Output Pin
const int lampPin = 22;
 
void setup()
{
  Serial.begin(115200);
  //Pin22設定為DO
  pinMode(lampPin, OUTPUT);
  //預設關燈
  digitalWrite(lampPin, LOW); 
}
    
void loop()
{
  //開燈
  delay(10000);
  digitalWrite(lampPin, HIGH);  
  //關燈
  delay(10000);
  digitalWrite(lampPin, LOW); 
}
