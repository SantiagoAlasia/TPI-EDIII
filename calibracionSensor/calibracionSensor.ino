int duty_cycle[53] = {0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100, 105, 110, 115, 120, 125, 130, 135, 140, 145, 150, 155, 160, 165, 170, 175, 180, 185, 190, 195, 200, 205, 210, 215, 220, 225, 230, 235, 240,245, 250,255};

void setup() {
  Serial.begin(9600);
  Serial.println("--------------- Sistema de calibracion ---------------");
  
  //pinMode(8, OUTPUT);

  //while(Serial.available() == 0){}
 
  //duty_cycle = Serial.parseInt();
  //Serial.print("Nuevo DC: ");
  //Serial.println(duty_cycle);
  
}

void loop() {
  for(int i = 0; i < 53; i++){
    Serial.print("Nuevo DC: ");
    Serial.println(duty_cycle[i]);
    analogWrite(5, duty_cycle[i]);
    delay(1000);
  }
}
