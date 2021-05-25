#include <SoftwareSerial.h>

#define NO_PUMPS 4
#define BUZZER_PIN 8
#define PUMP_DEBIT_MIN 1000 // ml / min

//DO MI SOL DO notes for buzzer
#define NOTE_C6  1047
#define NOTE_E6  1319
#define NOTE_G6  1568
#define NOTE_C7  2093

int pump_pins[NO_PUMPS] = {4,5,6,7}; // pumps are connected to these pins 
int serial_byte_recv[NO_PUMPS]; // numbers received on serial for pumps
int pump_times[NO_PUMPS]; // time powering up pumps

SoftwareSerial mySerial(0, 1); // RX, TX

void setup() 
{
  mySerial.begin(9600); //begin serial transmission with hc-05
  
  for(int i = 0; i < NO_PUMPS; i++){
    pinMode(pump_pins[i], OUTPUT); //pump pins marked as output
  }
  pinMode(BUZZER_PIN, OUTPUT); // buzzer pin marked as output
}

int userInput(){
  char c;
  if(mySerial.available() > 0)  
  {
    c = mySerial.read(); // read incoming char on serial port
    
    if(c == 'c'){
      // char = 'c' -> 'premade' cocktail
      
      while(mySerial.available() <= 0){} //wait untill next char is available
      
      c = mySerial.read(); // read incoming char on serial port
      
      if(c == '1'){
        //cocktail 1 - 50% drink 1 50% drink 3
        serial_byte_recv[0] = 50;
        serial_byte_recv[1] = 0;
        serial_byte_recv[2] = 50;
        serial_byte_recv[3] = 0;
      }
      if(c == '2'){
        //cocktail 2 - 50% drink 2 50% drink 4
        serial_byte_recv[0] = 0;
        serial_byte_recv[1] = 50;
        serial_byte_recv[2] = 0;
        serial_byte_recv[3] = 50;
      }
      if(c == '3'){
        //cocktail 3 - 50% drink 1 50% drink 4
        serial_byte_recv[0] = 50;
        serial_byte_recv[1] = 0;
        serial_byte_recv[2] = 0;
        serial_byte_recv[3] = 50;
      }
      if(c == '4'){
        //cocktail 4 - 50% drink 2 50% drink 3
        serial_byte_recv[0] = 0;
        serial_byte_recv[1] = 50;
        serial_byte_recv[2] = 50;
        serial_byte_recv[3] = 0;
      }
      return 1; // return 1 -> a cocktail needs to be prepared
    }
    if(c == 'C') {
      // char = 'C' -> custom cocktail
      //read NO_PUMPS byte values which represent the number of desired mls from every drink
      for(int i = 0; i < NO_PUMPS; i++){
        while(mySerial.available() <= 0){}
        serial_byte_recv[i] = mySerial.read();
      }
      return 1; // return 1 -> a cocktail needs to be prepared
    }
  }
  return 0; // return 1 -> no cocktail should be prepared
}

void activatePumps(){
  //every pump 'i' will be activated for pump_times[i] milliseconds
  for(int i = 0; i < NO_PUMPS; i++) {
    digitalWrite(pump_pins[i], HIGH);
    delay(pump_times[i]);
    digitalWrite(pump_pins[i], LOW);
  }

}

void activateBuzzer(){
  // play DO MI SOL DO - total time = 1 second
  tone(BUZZER_PIN, NOTE_C6);
  delay(250);
  tone(BUZZER_PIN, NOTE_E6);
  delay(250);
  tone(BUZZER_PIN, NOTE_G6);
  delay(250);
  tone(BUZZER_PIN, NOTE_C7);
  delay(250);
  noTone(BUZZER_PIN);
}

// 6000 msec --- 100 ml
// time       ---  drink ml
//-> time = 6000 * drink / 100
void compute_pump_times(){
  //active pump time must be between 0 and 6000 milliseconds
  //received byte is between 0 and 100 value
  for(int i = 0; i < NO_PUMPS; i++) {
    pump_times[i] = map(serial_byte_recv[i], 0, 100, 0, 6000);
  }
}

void loop()
{
  if (userInput()){
    compute_pump_times();
    activatePumps();
    activateBuzzer();
  }
}
