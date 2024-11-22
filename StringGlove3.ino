/////////////////////////////////////////////////////////////////////////////
// StringDriver

#define AMP 55

class StringDriver
{
  int pin1;
  int pin2;
  int pin3;
  int p10 = 0;
  int p20 = 0;
  int p30 = 0;
  int dp1 = 0;
  int dp2 = 0;
  int dp3 = 0;

public:
  StringDriver(){
  }

  void initialize(int _pin1, int _pin2, int _pin3){
    pin1 = _pin1;
    pinMode(pin1, OUTPUT);
    pin2 = _pin2;
    pinMode(pin2, OUTPUT);
    pin3 = _pin3;
    pinMode(pin3, OUTPUT);
    p10 = 0;
    p20 = 0;
    p30 = 0;
    dp1 = 0;
    dp2 = 0;
    dp3 = 0;
  }

  void setOffset(int x){
    p10 = x;
    p20 = x;
    p30 = x;
  }

  void reset(){
    dp1 = 0;
    dp2 = 0;
    dp3 = 0;
  }

  void set(int _dp1, int _dp2, int _dp3){
    dp1 = _dp1;
    dp2 = _dp2;
    dp3 = _dp3;
  }
  void pull(int d1, int d2, int d3){
    dp1 += d1;
    dp2 += d2;
    dp3 += d3;
  }

  void onTimer(){
    int p1 = constrain(p10 + dp1, -AMP, AMP); // limits range of sensor values to between -AMP and AMP
    sendPulse(pin1, p1 + 92);
    int p2 = constrain(p20 + dp2, -AMP, AMP);
    sendPulse(pin2, p2 + 88);
    int p3 = constrain(p30 + dp3, -AMP, AMP);
    sendPulse(pin3, p3 + 90);
  }

private:
  // the standard pulse width range is from 500 to 2500.
  void sendPulse(int pin, int x){     // x = 0 ... 180
    int pulsewidth = x * 200 / 18 + 500;
    digitalWrite(pin, HIGH);
    delayMicroseconds(pulsewidth);
    digitalWrite(pin, LOW);
  }
};

#define NDRIVERS 1
StringDriver stringDriver[NDRIVERS];

void setup() {
  Serial.begin(115200);

  stringDriver[0].initialize(2, 4, 6);
  // stringDriver[1].initialize(19, 32);
  // stringDriver[2].initialize(23, 4);
  // stringDriver[3].initialize(18, 16);

  for(int i = 0; i < NDRIVERS; i++)
    stringDriver[i].reset();
}

#define X_MIN 30
#define X_MAX 70

void setAllOffsets(){
  int v = analogRead(A0);
  // v = 0 ... 1024
  int x0 = (512 - v) / 6;
  for(int i = 0; i < NDRIVERS; i++)
    stringDriver[i].setOffset(x0);
}

void pullAll(int dx){
  for(int i = 0; i < NDRIVERS; i++)
    stringDriver[i].pull(dx, dx, dx);
}

void setAll(int dp1, int dp2, int dp3){
  for(int i = 0; i < NDRIVERS; i++)
    stringDriver[i].set(dp1, dp2, dp3);
}

int k = 0;
#define D 50
#define H 25
int dp1[] = {+D, +H, -H, -D, -H, +H};
int dp2[] = {-H, +H, +D, +H, -H, -D};
int dp3[] = {-H, -D, -H, +H, +D, +H};
int nsteps = sizeof(dp1) / sizeof(dp1[0]);

#define DT 20

int x, y, z; // x, y : -100 ~ 100 / z: 0 ~ 100

bool parseString(String data, int &x, int &y, int &z) {
  int firstSpace = data.indexOf(' ');
  int secondSpace = data.indexOf(' ', firstSpace + 1);

  if (firstSpace == -1 || secondSpace == -1) {
    return false;
  }
  x = data.substring(0, firstSpace).toInt();
  y = data.substring(firstSpace + 1, secondSpace).toInt();
  z = data.substring(secondSpace+1).toInt();
  return true;
}

void loop() {  // this should be called every 20 ms.
  unsigned long t_end = micros() + 20000;
  setAllOffsets();

  //TODO: the following loop may take 25 ms.
  for(int i = 0; i < NDRIVERS; i++)
    stringDriver[i].onTimer();


  while (Serial.available()){
    String input = Serial.readStringUntil('\n');
     if (parseString(input, x, y, z)) {
      // Print parsed values to confirm

      setAll(x-100, y-100, z-100);
    }
  }

  // k++;
  // if(k == nsteps * DT) k = 0;
  // int kk = k / DT;
  // if(kk * DT == k){
  //   char buffer[50];
  
  // // Format the string using sprintf
  //   sprintf(buffer, "Formatted Values: %d %d %d", dp1[kk], dp2[kk], dp3[kk]);

  //   // Print the formatted string
  //   Serial.println(buffer);
  //   setAll(dp1[kk], dp2[kk], dp3[kk]);
  // }

  while(micros() < t_end);
}

