/////////////////////////////////////////////////////////////////////////////
// StringDriver

#define AMP 180

class StringDriver
{
  int pin1;
  int pin2;
  int pin3;
  int p10 = 0;
  int p20 = 0;
  int p30 = 0;
  int o10 = 0;
  int o20 = 0;
  int o30 = 0;
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

  // Offset : 0 ... 180
  void setDefaultOffset(int _p10, int _p20, int _p30){
    p10 = _p10;
    p20 = _p20;
    p30 = _p30;
  }

  // Offset : 0 ... 180
  void setOffset(int _o10, int _o20, int _o30){
    o10 = _o10;
    o20 = _o20;
    o30 = _o30;
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
    int p1 = constrain(p10 + o10 + dp1, 0, AMP); // limits range of sensor values to between -AMP and AMP
    sendPulse(pin1, p1);
    int p2 = constrain(p20 + o20 + dp2, 0, AMP);
    sendPulse(pin2, p2);
    int p3 = constrain(p30 + o30 + dp3, 0, AMP);
    sendPulse(pin3, p3);
  }

private:
  // the standard pulse width range is from 500 to 2500.
  void sendPulse(int pin, int x){     // x = [0 ... 180]
    x = 180 - x;
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

  stringDriver[0].initialize(2, 3, 4);
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
  // x0 = 0 ... 180
  int x0 = v/6;
  for(int i = 0; i < NDRIVERS; i++)
    stringDriver[i].setDefaultOffset(x0, x0, x0);
}

void setOffsets(int o1, int o2, int o3) {
  for(int i = 0; i < NDRIVERS; i++)
    stringDriver[i].setOffset(o1, o2, o3);
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

int x, y, z;

bool parseString(String data, String &type, int &x, int &y, int &z) {
  int firstSpace = data.indexOf(' ');
  int secondSpace = data.indexOf(' ', firstSpace + 1);
  int thirdSpace = data.indexOf(' ', secondSpace + 1);

  if (firstSpace == -1 || secondSpace == -1 || thirdSpace == -1) {
    return false;
  }
  type = data.substring(0, firstSpace);
  x = data.substring(firstSpace + 1, secondSpace).toInt();
  y = data.substring(secondSpace+ 1, thirdSpace).toInt();
  z = data.substring(thirdSpace + 1).toInt();
  return true;
}

void moveValue(String type, int _value, int _min, int _max, int &x, int &y, int &z) {
    float value = float(_value);
    float min = float(_min);
    float max = float(_max);
    value = (value - (min + max)/2.0) / (max - min);

    float _x = 0;
    float _y = 0;

    if (type == "x") _x = value;
    else if (type == "y") _y = value;
    
    float x0, y0, z0;
    x0 = (1.0 - _y + sqrt(3) * _x) /3.0;
    y0 = (1.0 + 2.0 * _y) /3.0;
    z0 = (1.0 - _y - sqrt(3) * _x) /3.0;
    // x0 + y0 + z0 = 1

    int SUM = 90;
    x = int(min(1, max(0, x0)) * SUM);
    y = int(min(1, max(0, y0)) * SUM);
    z = int(min(1, max(0, z0)) * SUM);
    // x + y + z = SUM
}

void loop() {  // this should be called every 20 ms.
  unsigned long t_end = micros() + 20000;
  setAllOffsets();

  for(int i = 0; i < NDRIVERS; i++)
    stringDriver[i].onTimer();


  while (Serial.available()){
    String input = Serial.readStringUntil('\n');

    String type; // w or o
     if (parseString(input, type, x, y, z)) {
      // Print parsed values to confirm
      // x, y, z : [0 ~ 180]
      // ex) w 0 0 0 for writing value
      if (type == "w") setAll(x, y, z);
      // ex) o 0 0 0 for offset
      else if (type == "o") setOffsets(x, y, z);
      // axis val min max
      // ex) x 50 0 100
      else if (type == "x" || type == "y") {
        int x0, y0, z0;
        moveValue(type, x, y, z, x0, y0, z0);
        setAll(x0, y0, z0);
      }
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

