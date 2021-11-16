const int enable = 4;
const int input1 = 5;
const int input2 = 16;

void setup() {
 pinMode(enable, OUTPUT);
 pinMode(input1, OUTPUT);
 pinMode(input2, OUTPUT);
}

void loop() {
 digitalWrite(enable, HIGH);
 digitalWrite(input1, LOW);
 digitalWrite(input2, HIGH);

 delay(2000);
}
