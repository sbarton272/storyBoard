const int ledPin =  13;      // the number of the LED pin

bool serOn = false;

void setup() {
  // set the digital pin as output:
  pinMode(ledPin, OUTPUT);  
  Serial.begin(9600);
  serOn = (Serial) ? true : false;

  if (serOn) {
  	digitalWrite(ledPin, LOW);
  } else {
  	digitalWrite(ledPin, HIGH);
  }
}

void loop()
{}