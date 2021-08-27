// 色温转RGB
void colorTemperatureToRGB(int kelvin){
  float ctemp = kelvin / 100;
  float red, green, blue;
  if(ctemp <= 66){
    red = 255;
    green = ctemp;
    green = 99.47 * log(green) - 161.12;
    if(ctemp <= 19){
      blue = 0;
    }else{
      blue = ctemp - 10;
      blue = 138.52 * log(blue) - 305.04;
    }
  }
  else{
    red = ctemp - 60;
    red = 329.70 * pow(red, -0.13);
    green = ctemp - 60;
    green = 288.12 * pow(green, -0.08);
    blue = 255;
  }

  
  analogWrite(LED_R, clamp(int(red), 0, 255));
  analogWrite(LED_G, clamp(int(green), 0, 255));
  analogWrite(LED_B, clamp(int(blue), 0, 255));
}

int clamp(int x, int min, int max){
  if(x < min){return min;}
  if(x > max){return max;}
  return x;
}


// 射灯渐出
void dimmingOut(int h, float max_val, int delay_time){
  for(float i = 1; i > max_val; i -= 0.01){
      setLedColorHSV(h, 1, i);
      delay(delay_time);
      }
}

// 射灯渐入
void dimmingIn(int h, float max_val, int delay_time){
  for(float i = 0; i < max_val; i += 0.01){
      setLedColorHSV(h, 1, i);
      delay(delay_time);
      }
}

// 电位器读取H值转换为RGB
void setLedColorHSV(int h, double s, double v) {
  double r=0; 
  double g=0; 
  double b=0;

  double hf=h/60.0;

  int i=(int)floor(h/60.0);
  double f = h/60.0 - i;
  double pv = v * (1 - s);
  double qv = v * (1 - s*f);
  double tv = v * (1 - s * (1 - f));

  switch (i)
  {
  case 0: //rojo dominante
    r = v;
    g = tv;
    b = pv;
    break;
  case 1: //verde
    r = qv;
    g = v;
    b = pv;
    break;
  case 2: 
    r = pv;
    g = v;
    b = tv;
    break;
  case 3: //azul
    r = pv;
    g = qv;
    b = v;
    break;
  case 4:
    r = tv;
    g = pv;
    b = v;
    break;
  case 5: //rojo
    r = v;
    g = pv;
    b = qv;
    break;
  }

  //set each component to a integer value between 0 and 255
  int red=constrain((int)255*r,0,255);
  int green=constrain((int)255*g,0,255);
  int blue=constrain((int)255*b,0,255);

  analogWrite(LED_R, red);
  analogWrite(LED_G, green);
  analogWrite(LED_B, blue);
}


// 跑马灯更新
void FillLEDsFromPaletteColors(long ColorIndex)
{
  for( int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(ColorIndex)));
//    ColorIndex += 65536L / 8;
    ColorIndex += 65536L / 32;
    }
}

// 中断处理
void interruptHandle(){
  if(digitalRead(BUTTON) == LOW){
    is_interrupted = true;
  }
}

// GPIO 状态初始化
void pinInit(){
  digitalWrite(LED_R, LOW);
  digitalWrite(LED_G, LOW);
  digitalWrite(LED_B, LOW);
  digitalWrite(SPK_PIN, LOW);
  digitalWrite(VIB_PIN, LOW);
  analogWrite(FAN_PWM, 0);
}
// GPIO初始化
void pinConfigeration(){
  pinMode(FAN_INPUT, INPUT);
  pinMode(COLOR_INPUT, INPUT);

  pinMode(FAN_PWM, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  pinMode(SPK_PIN, OUTPUT);
  pinMode(VIB_PIN, OUTPUT);
}

// 扬声器控制
void speaker(int delayTime){
  // 扬声器循环一个周期
//  for(int i = 0; i < (delayTime / 2); i++){
//    digitalWrite(SPK_PIN, HIGH);
//    delayMicroseconds(100);
//    digitalWrite(SPK_PIN, LOW);
//    delayMicroseconds(100);
//  }
//  TCCR2B = TCCR2B & B11111000 | B00000001; // for PWM frequency of 31372.55 Hz
  analogWrite(SPK_PIN, 128);
  delay(delayTime);
  analogWrite(SPK_PIN, 0);
  
}

// 线性马达控制
void viberate(int delayTime){
  // 控制线性马达振动一个周期
  for(int i = 0; i < (delayTime / 50); i++){
  digitalWrite(VIB_PIN, HIGH);
  delay(25);
  digitalWrite(VIB_PIN, LOW);
  delay(25);
  }
  
}

// 读取电位器的数值
void pointemeterRead(){
  fan_read = map(analogRead(FAN_INPUT), 0, 1023, 0, 255);
  color_read = map(analogRead(COLOR_INPUT), 0, 1023, 0, 89);
}

// 激光测距
uint16_t measureDistance(){
  // 创建数据返回结构
  VL53L0X_RangingMeasurementData_t measure;
  
  // 开始测距
  lox.rangingTest(&measure, false); 

  // 返回数据
  if (measure.RangeStatus != 4) {
    return measure.RangeMilliMeter;
  } else {
    // 超出量程返回10000
    return 10000;
  }
  delay(5);
}
