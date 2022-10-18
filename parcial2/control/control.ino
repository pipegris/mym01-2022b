#define DEFAULT_BAUD_RATE 115200

// Variable init
// From the Matlab Controller auto tuning: a * (1 + b*s) / s 
// float a = 2.2738*100000; // 2.2734e+05
// float b = 12/100000; // 12e-05
// float T = 10/1000; // 10 ms
float aT_ab = 2246.5144; // a*T - a*b;
float ab = 27.2856; // a*b;
float output;
float refference = 1.5;
float prevError = 0, error;
float prevControl = 0, control;
unsigned long currentMillis, prevMillis = 0;

// Function signatures
float readFloatADC(int);
float calculateError(float, float);

void setup()
{
    Serial.begin(DEFAULT_BAUD_RATE);
}

void loop()
{
    // Take first time stamp 
    prevMillis = millis();

    output = readFloatADC(A5);
    error = calculateError(refference, output);
    
    control = aT_ab * prevError + ab * error + prevControl;

    // Limit control signal to 0-5V
    control = control > 5 ? 5 : control;
    control = control < 0 ? 0 : control;

    analogWrite(5, control * 255 / 5);
    
    prevError = error;
    prevControl = control;

    // Streaming results via serial
    Serial.print("Output:");
    Serial.println(output);
    Serial.print("Control:");
    Serial.println(control);
    Serial.print("SetPoint:");
    Serial.println(refference);
    // Wait until 10 ms have passed
    do {
        currentMillis = millis();
    } while (currentMillis - prevMillis <= 10);
}

float readFloatADC(int port)
{    
    float read = analogRead(A5);
    return read * 5 / 1023;
}

float calculateError(float refference, float output) {
    return refference - output;
}
