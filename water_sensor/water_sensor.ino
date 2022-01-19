void setup () {
  Serial.begin (115200); 
}
 
void loop() {
  //從A0讀取輸入
  int value = analogRead(36);
  Serial.print(value);    
  Serial.print("   ");      
  if (value > 480) {
    Serial.println("High Level");
  }          
  else if ((value > 340) && (value <= 480)) {
    Serial.println("Middle Level");
  }
  else if ((value > 100) && (value <=340)){
    Serial.println("Low Level");
  }     
  else if (value <=100){
    Serial.println("NO Water");     
  }
  delay(500);
}
