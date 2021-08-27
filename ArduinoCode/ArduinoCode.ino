#include <Adafruit_NeoPixel.h>
#include "Adafruit_VL53L0X.h"
#define BUTTON 2
#define LED_R 6
#define LED_G 9
#define LED_B 5
#define WS_PIN 7
#define SPK_PIN 10
#define VIB_PIN 11
#define FAN_PWM 3
#define FAN_SEN 4
#define BTN_PIN 2
#define FAN_INPUT A1
#define COLOR_INPUT A3
#define NUM_LEDS 8

#define GUN_COLOR 170
#define SWORD_COLOR 290


// WS2812
Adafruit_NeoPixel strip(NUM_LEDS, WS_PIN, NEO_GRB + NEO_KHZ800);
uint8_t Brightness = 25;

// 创建激光测距对象
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// 用于存储电位器数值
float fan_read = 0;
float color_read = 0;

// 中断标志位
bool is_interrupted = false;

// 工作标志位
int work_sta = 0;

void setup() {
  // 初始化GPIO
  pinConfigeration();
  pinInit();
  // 初始化中断
  pinMode(BUTTON, INPUT);
  attachInterrupt(digitalPinToInterrupt(BUTTON), interruptHandle, FALLING);
  
  // 初始化串口
  Serial.begin(9600);
  
  // 初始化激光测距模块
  if (!lox.begin()) {
  Serial.println(F("Failed to boot VL53L0X"));
  while(1);
  }

  // 初始化WS2812
  strip.begin();
  // 迅速熄灭WS2812
  strip.show();

  // 渐入跑马灯动画
  for(int i=0; i < Brightness; i++){
    strip.setBrightness(i); // Set BRIGHTNESS
    FillLEDsFromPaletteColors(0);
    strip.show();
    delay(20);
  }
}

void loop() {
  // 读取电位器
  pointemeterRead();
  if(color_read < 45){
    // 关闭
    if(work_sta == 2){
      dimmingOut(GUN_COLOR, 0, 10);
    }
    work_sta = 1;
    
  }
  else if(color_read < 90){
    if(work_sta == 1 || work_sta ==0){
      dimmingIn(GUN_COLOR, 1, 10);
    }
    // 剑模式
    work_sta = 2;
    
    // 接近变色
    int distance = measureDistance();
    if(distance == 0){
      distance = 10000;
    }
    
    if(distance < 200){
      setLedColorHSV(0, 1, 1);
      viberate(800);
    }
    setLedColorHSV(GUN_COLOR, 1, 1);
  }

  // 判断按钮是否按
  
  if(is_interrupted && work_sta == 1){
    
    // 红色亮起
    float initV = 0.1;
    int HSV_yellow = 30;
    int HSV_blue = GUN_COLOR;
    dimmingIn(0, initV, 80);

    // 红转黄
    for(int i = 0; i < HSV_yellow; i++){
      setLedColorHSV(i, 1, initV);
      delay(20);
    }

    // 黄转白
    for(float i = 1; i > 0; i-=0.01){
      setLedColorHSV(HSV_yellow, i, initV);
      delay(2);
    }

    // 白转蓝
    for(float i = 0; i < 1; i+=0.01){
      setLedColorHSV(HSV_blue, i, initV);
      delay(5);
    }

    while(digitalRead(BUTTON) == LOW){
      viberate(50);
    }
    
    // 迅速亮起
    for(float j = initV; j < 1; j+=0.01){
      setLedColorHSV(HSV_blue, 1, j);
      delay(1);
    }
    delay(500);

    // 熄灭
    dimmingOut(HSV_blue, 0, 20);
    
    is_interrupted = false;
    
  }

  // 跑马灯更新
  
  static long startIndex = 0;
  FillLEDsFromPaletteColors(startIndex);
  startIndex = startIndex + 512;
  strip.show();
  delay(10);

  // 控制风扇转速
  analogWrite(FAN_PWM, fan_read);
  
}
