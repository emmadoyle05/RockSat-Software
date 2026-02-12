void setup() {
    // Set up PC output (through Teensy USB; normal serial line)
    Serial.begin(115200);
    while (!Serial);

    // Set up UART output (the one we're doing the data cable with)
    Serial1.begin(115200);
    delay(1000);

    Serial.println("Serial Monitor ready!");
    Serial1.println("Data cable ready!");

    // Heartbeat to make sure Teensy is even on
    // This isn't actually needed for the code; I just wanted to make sure
    // if I was having problems, it wasn't that I forgot to plug in the Teensy
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    // Sending to the serial monitor
    Serial.println("Hello, serial monitor! I'm Teensy! :-D");
    // Sending to the data cable
    Serial1.println("Hello, data cable! I'm Teensy! :-D");

    // Blinking LED so I know Teensy is still connected
    // Once again, this part isn't needed.
    digitalWrite(LED_BUILTIN, HIGH);   
    delay(1000);                       
    digitalWrite(LED_BUILTIN, LOW);    

    // You will probably want some sort of delay on the data transmission though
    // This waits a second
    delay(1000);
}