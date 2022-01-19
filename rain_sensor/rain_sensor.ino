int get_D3 = 39;
int get_A0 = 34;

void setup(){
  pinMode(get_D3, INPUT);
  Serial.begin(115200);  
}
void loop(){

  int value = analogRead(A0);    //從A0讀取感測值

  Serial.print(value);    
  Serial.print("   ");  

  if(digitalRead(get_D3) == LOW){
    Serial.println("Digital value : wet");   
  }
  else{
    Serial.println("Digital value : dry");
  }
  delay(1000);
}
