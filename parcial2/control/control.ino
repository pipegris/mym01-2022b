//////////////////////////////////////////////////////
// Fixed point operations
#define FADD(a,b) ( (a) + (b) )
#define FSUB(a,b) ( (a) - (b) )
#define FMUL(a,b,q) ( ((a) * (b)) >> (q) )
#define FDIV(a,b,q) ( ((a) << (q)) / (b) )

#define TOFIX(t,d,q) ( (t) ((d) * (double) (1ULL << (q))) )
#define FROMFIX(d,q) ((d) / (double) (1ULL << (q)))

typedef int32_t fix32;
typedef int16_t fix16;

#define QM 7

////////////////////////////////////////////////////////

#define DEFAULT_BAUD_RATE 115200
#define SAMPLING_TIME 10

// Variable init
// From the Matlab Controller auto tuning: a * (1 + b*s) / s 
float output;
float refference = 0.5;
float prevError = 0, error;
float prevControl = 0, control;
unsigned long currentMillis, prevMillis = 0;

// Function signatures
float readFloatADC(int);
float calculateError(float, float);
float calculateControl(float, float, float);

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
    
    control = calculateControl(error, prevError, prevControl);

    // Limit control signal to 0-5V
    control = control > 5 ? 5 : control;
    control = control < 0 ? 0 : control;

    analogWrite(5, control * 255/5);
    
    prevError = error;
    prevControl = control;

    // Streaming results via serial
    Serial.print("Output:");
    Serial.print(output);
    Serial.print(" ");
    Serial.print("Control:");
    Serial.print(control);
    Serial.print(" ");
    Serial.print("SetPoint:");
    Serial.println(refference);

    // Updates refference via serial
    if (Serial.available()) {
        refference = Serial.parseFloat();
    }

    // Wait until 10 ms has passed
    do {
        currentMillis = millis();
    } while (currentMillis - prevMillis <= SAMPLING_TIME);
}

float readFloatADC(int port)
{    
    float read = analogRead(A5);
    return read * 5 / 1023;
}

float calculateError(float refference, float output) {
    fix16 fRefference = TOFIX(fix16, refference, QM);
    fix16 fOutput = TOFIX(fix16, output, QM);
    
    //return refference - output;
    fix16 result = FSUB(fRefference,fOutput);
    return (float) FROMFIX(result, QM);
}

float calculateControl(float error, float prevError, float prevControl) {
    fix16 fError = TOFIX(fix16, error, QM);
    fix16 fErrorConstant = TOFIX(fix16, 27.39, QM);
    fix16 fPrevError = TOFIX(fix16, prevError, QM);
    fix16 fPrevErrorConstant = TOFIX(fix16, 27.17, QM);
    fix16 fPrevControl = TOFIX(fix16, prevControl, QM);

    //return 27.39 * error - 27.17 * prevError + prevControl;
    fix16 result = 
    FADD(
        FSUB(
            FMUL(fErrorConstant,fError,QM),
            FMUL(fPrevErrorConstant,fPrevError,QM)
        ), 
        fPrevControl
    );
    return (float) FROMFIX(result, QM);
}
