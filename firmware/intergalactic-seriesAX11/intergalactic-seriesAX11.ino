#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <StevesAwesomeButton.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "sinewave.h"
#include "square.h"
#include "pulse.h"
#include "sample_hold.h"
#include "triangle.h"
#include "sawtooth.h"

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// GUItool: begin automatically generated code
AudioInputI2S            i2s2;           //xy=279,423
AudioInputUSB            usb2;           //xy=283,449
AudioSynthWaveform       waveform1;      //xy=395,485
AudioMixer4              mixer6;         //xy=408,436
AudioSynthWaveform       waveform2;      //xy=541,522
AudioMixer4              mixer2;         //xy=546,456
AudioEffectMultiply      multiply1;      //xy=688,517
AudioMixer4              mixer5;         //xy=838,482
AudioAnalyzePeak         peak1;          //xy=851,561
AudioFilterBiquad        biquad2;        //xy=1036,467
AudioMixer4              mixer1;         //xy=1278,494
AudioEffectDelay         delay1;         //xy=1321,273
AudioMixer4              mixer3;         //xy=1473,712
AudioFilterBiquad        biquad3;        //xy=1522,228
AudioFilterBiquad        biquad1;        //xy=1681,228
AudioMixer4              mixer4;         //xy=1722,436
AudioOutputUSB           usb1;           //xy=1897,377
AudioOutputI2S           i2s1;           //xy=1898,434
AudioConnection          patchCord1(i2s2, 0, mixer6, 0);
AudioConnection          patchCord2(i2s2, 1, mixer6, 1);
AudioConnection          patchCord3(usb2, 0, mixer6, 2);
AudioConnection          patchCord4(usb2, 1, mixer6, 3);
AudioConnection          patchCord5(waveform1, 0, mixer2, 2);
AudioConnection          patchCord6(mixer6, 0, mixer2, 0);
AudioConnection          patchCord7(mixer6, 0, mixer2, 1);
AudioConnection          patchCord8(waveform2, 0, multiply1, 1);
AudioConnection          patchCord9(mixer2, 0, multiply1, 0);
AudioConnection          patchCord10(mixer2, 0, mixer5, 0);
AudioConnection          patchCord11(mixer2, 0, mixer4, 3);
AudioConnection          patchCord12(multiply1, peak1);
AudioConnection          patchCord13(multiply1, 0, mixer5, 1);
AudioConnection          patchCord14(mixer5, biquad2);
AudioConnection          patchCord15(mixer5, 0, mixer1, 2);
AudioConnection          patchCord16(mixer5, 0, mixer1, 3);
AudioConnection          patchCord17(biquad2, 0, mixer1, 0);
AudioConnection          patchCord18(biquad2, 0, mixer1, 1);
AudioConnection          patchCord19(mixer1, 0, mixer3, 0);
AudioConnection          patchCord20(mixer1, 0, mixer4, 1);
AudioConnection          patchCord21(delay1, 0, biquad3, 0);
AudioConnection          patchCord22(mixer3, delay1);
AudioConnection          patchCord23(biquad3, biquad1);
AudioConnection          patchCord24(biquad1, 0, mixer3, 1);
AudioConnection          patchCord25(biquad1, 0, mixer4, 0);
AudioConnection          patchCord26(biquad1, 0, mixer4, 2);
AudioConnection          patchCord27(mixer4, 0, usb1, 0);
AudioConnection          patchCord28(mixer4, 0, usb1, 1);
AudioConnection          patchCord29(mixer4, 0, i2s1, 0);
AudioConnection          patchCord30(mixer4, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=976,343
// GUItool: end automatically generated code

/* PIN DECLARATIONS
 * Each pin needs their own address for where they live on Teensy
 */
int ledPin = A8;

int mixPin = A11;
int mixVal = 0;

int lpfPin = A12;
int lpf_val = 0;

int delayPin = A13;
int lfoVal = 0;

int wav2Pin = A16;
int ringModcarrier = 0;

int wav1Pin = A17;
int ringModwav1 = 0;

/* BUTTON DECLARATIONS
 * Location and description of every button used
 */
StevesAwesomeButton button1(30, 0, INPUT_PULLUP); // Toggle Low-pass filter
StevesAwesomeButton button2(31, 1, INPUT_PULLUP); // Toggle delay
StevesAwesomeButton button3(32, 2, INPUT_PULLUP); // Toggle bypass
StevesAwesomeButton button4(33, 3, INPUT_PULLUP); // Toggle waveform 2
StevesAwesomeButton button5(34, 4, INPUT_PULLUP); // Toggle waveform 1
StevesAwesomeButton button6(35, 5, INPUT_PULLUP); // Toggle user input


StevesAwesomeButton* buttons[6] = { &button1, &button2, &button3, &button4, &button5, &button6 };

int totalButtons = 6;

int potControl[2] = { wav2Pin, wav1Pin };

bool lpfON = false;
bool userIN = false;
bool delayON = false;
bool changeWAV1 = false;
bool changeWAV2 = false;
bool bypassON = false;
bool ledON = false;

short waveforms1[] = {
  WAVEFORM_SINE,
  WAVEFORM_TRIANGLE,
  WAVEFORM_SAWTOOTH,
  WAVEFORM_SQUARE,
  WAVEFORM_PULSE,
  WAVEFORM_SAMPLE_HOLD
};

const char* waveformNames1[] = {
  "Sine", "Triangle", "Sawtooth", "Square", "Pulse", "Sample & Hold"
};

const int NUM_WAVEFORMS1 = sizeof(waveforms1) / sizeof(waveforms1[0]);
int currentIdx_w1 = 0;

short waveforms2[] = {
  WAVEFORM_SINE,
  WAVEFORM_TRIANGLE,
  WAVEFORM_SAWTOOTH,
  WAVEFORM_SQUARE,
  WAVEFORM_PULSE,
  WAVEFORM_SAMPLE_HOLD
};

const char* waveformNames2[] = {
  "Sine", "Triangle", "Sawtooth", "Square", "Pulse", "Sample & Hold"
};

const int NUM_WAVEFORMS2 = sizeof(waveforms2) / sizeof(waveforms2[0]);
int currentIdx_w2 = 0;

const float BASE_DELAY_MS = 250.0;  // Center delay time (ms)
const float FEEDBACK_GAIN = 0.55;   // Echo repeat decay (0.0 to 0.95)
float smoothedDelayMs = BASE_DELAY_MS;

int tempo = 1000;
unsigned long lastStepTime = 0;

void setup() {
  AudioMemory(1600); // originally at 200
  Serial.begin(9600);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  if (!display2.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { //can't call SCREEN_ADDRESS, just name the address you want to assign
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }  
  Wire1.setClock(1000000);

  for (int i = 0; i < totalButtons; i++) {
    buttons[i]->pressHandler(onButtonPress);
    // buttons[i]->releaseHandler(onButtonRelease);
  }

  

  biquad1.setLowpass(0, 2500, 0.707);
  biquad2.setLowpass(0, 2200, 0.707); // filtering before wave multiplication
  biquad3.setHighpass(0, 180, 0.707); // filtering before wave multiplication

  // use pinMode for Digital Pins

  waveform1.begin(waveforms1[currentIdx_w1]);
  waveform1.frequency(110);
  waveform1.amplitude(0.5);

  waveform2.begin(waveforms2[currentIdx_w2]);
  waveform2.frequency(90);
  waveform2.amplitude(0.5);

  Serial.print("Initial Carrier: ");
  Serial.println(waveformNames2[currentIdx_w2]);


  mixer1.gain(0, 0.0);
  mixer1.gain(1, 0.0);
  mixer1.gain(2, 1.0);
  mixer1.gain(3, 1.0);

  mixer2.gain(0, 0.0);
  mixer2.gain(1, 0.0);
  mixer2.gain(2, 0.0);

  mixer3.gain(0, 1.0);
  mixer3.gain(1, 1.0);

  mixer4.gain(0, 0.0);
  mixer4.gain(1, 1.0);
  mixer4.gain(2, 0.0);
  // mixer4.gain(3, 1.0);

  mixer5.gain(0, 0.3);
  mixer5.gain(1, 0.7);

  mixer6.gain(0, 0.5);
  mixer6.gain(1, 0.5);
  mixer6.gain(2, 0.5);
  mixer6.gain(3, 0.5);



  delay1.delay(0, BASE_DELAY_MS);

  // display2.setRotation(0);
  // display2.clearDisplay();
  // display2.display();
  pinMode(ledPin, OUTPUT);

  updateDisplay();
  updateDisplay_pic();
  sinewave_gif();
  square_gif();
  pulse_gif();
  sampleHold_gif();
  triangle_gif();
  sawtooth_gif();
}

void loop() {

  for (int i = 0; i < totalButtons; i++) {
    buttons[i]->process();
  }

  float envVal = 0.0f;
  if (peak1.available()) {
    envVal = peak1.read();
  }

  processGravityDrift(envVal, lfoVal);
  delay(10);
  // delay1.delay(0, 200.0f);


  lpf_val = analogRead(lpfPin);
  float baseCutoff = map(lpf_val, 0, 1023, 300, 3500);

  float dynamicCutoff = baseCutoff + (envVal * (4500.0 - baseCutoff) * 0.65);
  dynamicCutoff = constrain(dynamicCutoff, 300.0, 4500.0);
  biquad2.setLowpass(0, dynamicCutoff, 0.707);
  delay(10);
  // Serial.println(dynamicCutoff);




  ringModcarrier = analogRead(wav2Pin);
  float freq = map(ringModcarrier, 0, 1023, 2400, 20); // try 20 - 2400 Hz
  // waveform2.frequency(freq);
  waveform2.frequency(freq);
  delay(10);
  //Serial.println(freq);

  // bitcrusher1.sampleRate(16000);
  ringModwav1 = analogRead(wav1Pin);
  float freq1 = map(ringModwav1, 0, 1023, 2400, 20);
  waveform1.frequency(freq1);
  // Serial.println(ringModwav1);
  // bitcrusher1.bits(ringModwav1);
  delay(20);
  
  //Serial.println(freq1);

  lpf_val = analogRead(lpfPin);
  float dw_val = map(lpf_val, 0, 1023, 0, 2);



  // mixer5.gain(0, dw_val);
  // mixer5.gain(1, dw_val);
  // mixer5.gain(2, 1 - dw_val);
  // mixer5.gain(3, 1 - dw_val);
  
  updateDisplay();
  updateDisplay_pic();
}

void processGravityDrift(float peak1, int lfoVal) {
  lfoVal = analogRead(delayPin);

  float lfoRateHz = map(lfoVal, 0, 1023, 1, 15) / 10.0;  // 0.1 Hz to 1.5 Hz
  float modDepthMs = map(lfoVal, 0, 1023, 0, 15) / 10.0;  // 0.0 ms to 1.5 ms

  unsigned long currentMillis = millis();
  float lfoPeriodMs = 1000.0 / lfoRateHz;
  float lfoPhase = fmod(currentMillis, lfoPeriodMs) / lfoPeriodMs;  // Normalize 0.0 to 1.0
  float lfoValue = sin(lfoPhase * 2.0 * M_PI);

  float duckingFactor = 1.0 - constrain(peak1 * 0.3, 0.0, 0.3);
  float activeDepth   = modDepthMs * duckingFactor;

  float targetDelayMs = BASE_DELAY_MS + (lfoValue * activeDepth);

  static float smoothedDelayMs = BASE_DELAY_MS;
  smoothedDelayMs = (smoothedDelayMs * 0.97) + (targetDelayMs * 0.03);

  delay1.delay(0, smoothedDelayMs);



  // Serial.print("Target Delay: ");
  // Serial.print(targetDelayMs);
  // Serial.print(" ms | Smoothed Delay: ");
  // Serial.print(smoothedDelayMs);
  // Serial.print(" ms | LFO Rate: ");
  // Serial.print(lfoRateHz);
  // Serial.println(" Hz");

  delay(10); 
}

void onButtonPress(int _buttonNum) {

  if (_buttonNum == 0) {
    lpfON = !lpfON;
    if (lpfON == true) {
      Serial.println("LPF on");
      mixer1.gain(0, 1.0);  
      mixer1.gain(1, 1.0);  
      mixer1.gain(2, 0.0);  // turn dry channel off
      mixer1.gain(3, 0.0);  // turn dry channel off

    } else if (lpfON == false) {
      Serial.println("LPF off");
      mixer1.gain(0, 0.0);  
      mixer1.gain(1, 0.0);  
      mixer1.gain(2, 1.0);  // turn dry channel on
      mixer1.gain(3, 1.0);  // turn dry channel on
    }
  }

  if (_buttonNum == 5) {
    userIN = !userIN;
    ledON = !ledON;
    if (userIN == true) {
      Serial.println("user input on");
      mixer2.gain(0, 0.5);  // turn user input channel on
      mixer2.gain(1, 0.5);  // turn user input channel on
      mixer2.gain(2, 0.0);  // turn osclliator channel off

    } else if (userIN == false) {
      Serial.println("user input off");
      mixer2.gain(0, 0.0);  // turn user input channel off
      mixer2.gain(1, 0.0);  // turn user input channel off
      mixer2.gain(2, 0.7);  // turn osclliator channel on
    }
    if(ledON == true) {
      Serial.println("led on");
      digitalWrite(ledPin, HIGH);
    } else {
      Serial.println("led off");
      digitalWrite(ledPin, LOW);
    }
  }

  if (_buttonNum == 4) {
    changeWAV1 = !changeWAV1;
    if (changeWAV1 == true) {
      currentIdx_w1 = (currentIdx_w1 + 1) % NUM_WAVEFORMS1;  // Cycle index
      Serial.print("Switched Modulating Waveform1 to: ");
      Serial.println(waveformNames1[currentIdx_w1]);
      waveform1.begin(waveforms1[currentIdx_w1]);

    } else if (changeWAV1 == false) {
      currentIdx_w1 = (currentIdx_w1 + 1) % NUM_WAVEFORMS1;  // Cycle index
      Serial.print("Switched Modulating Waveform1 to: ");
      Serial.println(waveformNames1[currentIdx_w1]);
      waveform1.begin(waveforms1[currentIdx_w1]);
    }
    updateDisplay();
  }

  if (_buttonNum == 3) {
    changeWAV2 = !changeWAV2;



    if (changeWAV2 == true) {
      currentIdx_w2 = (currentIdx_w2 + 1) % NUM_WAVEFORMS2;
      Serial.print("Switched Modulating Waveform2 to: ");
      Serial.println(waveformNames2[currentIdx_w2]);
        // Cycle index
      waveform2.begin(waveforms2[currentIdx_w2]);

    } else if (changeWAV2 == false) {
      currentIdx_w2 = (currentIdx_w2 + 1) % NUM_WAVEFORMS2;
      Serial.print("Switched Modulating Waveform2 to: ");
      Serial.println(waveformNames2[currentIdx_w2]);
        // Cycle index
      waveform2.begin(waveforms2[currentIdx_w2]);
    }
    updateDisplay_pic();


  }

  if (_buttonNum == 1) {
    delayON = !delayON;
    if (delayON == true) {
      Serial.println("delay on");
      mixer3.gain(0, 0.8);
      mixer3.gain(1, 0.6);
      mixer4.gain(0, FEEDBACK_GAIN);
      mixer4.gain(1, 0);
      mixer4.gain(2, FEEDBACK_GAIN);  // keep dry channel on
      // mixer4.gain(3, 0.0);  // turn feedback channel on

    } else if (delayON == false) {
      Serial.println("delay off");
      mixer3.gain(0, 0.0);
      mixer3.gain(1, 0);
      mixer4.gain(0, 0);
      mixer4.gain(1, 1.0);
      mixer4.gain(2, 0);
      // mixer4.gain(2, 1.0);  // keep dry channel on
      // mixer4.gain(3, 1.0);  // turn feedback channel on
    }
  }

  if (_buttonNum == 2) {
    bypassON = !bypassON;
    if (bypassON == true) {
      Serial.println("bypass on");
      mixer4.gain(0, 0);
      mixer4.gain(1, 0);
      mixer4.gain(2, 0);
      mixer4.gain(3, 1);
    } else if (bypassON == false) {
      Serial.println("bypass off");
      mixer4.gain(0, 0);
      mixer4.gain(1, 1.0);
      mixer4.gain(2, 0);
      mixer4.gain(3, 0);      
    }
  }
}

// void mixer5_update(){

// }

void updateDisplay(){
    display.clearDisplay();  // Clear display buffer
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(25, 10);
    // display.println(currentIdx_w2);
    if(currentIdx_w1 == 0) {
      display.println("SINE");
    } 
    if(currentIdx_w1 == 1) {
      display.println("TRIANGLE");
    }
    if(currentIdx_w1 == 2) {
      display.println("SAWTOOTH");
    }
    if(currentIdx_w1 == 3) {
      display.println("SQUARE");
    }
    if(currentIdx_w1 == 4) {
      display.println("PULSE");
    } 
    if(currentIdx_w1 == 5) {
      display.println("SAMPLE HOLD");
    }               
    // display.fillRect(0, 15, 60, 40, WHITE);
    display.display();
}

void updateDisplay_pic(){
    display2.setRotation(0);
    display2.clearDisplay();  // Clear display buffer
    display2.setTextSize(3);
    display2.setTextColor(WHITE);
    display2.setCursor(0, 0);
    // display.println(currentIdx_w2);
    if(currentIdx_w2 == 0) {
      // display2.println("FML");
      sinewave_gif();
    } 
    if(currentIdx_w2 == 1) {
      triangle_gif();
    }
    if(currentIdx_w2 == 2) {
      sawtooth_gif();
    }
    if(currentIdx_w2 == 3) {
      square_gif();
    }
    if(currentIdx_w2 == 4) {
      pulse_gif();
    } 
    if(currentIdx_w2 == 5) {
      sampleHold_gif();
    }               
    // display2.fillRect(0, 15, 60, 40, WHITE);
    display2.display();
}

void sinewave_gif() {
  static uint8_t frameIdx = 0;
  static unsigned long lastMs = 0;
  const uint8_t* frames[] = {frame0, frame1, frame2, frame3, frame4, frame5, frame6, frame7};
  const uint8_t numFrames = 8;
  if (millis() - lastMs >= 83) {
    lastMs = millis();
    display2.clearDisplay();
    display2.drawBitmap(0, 0, frames[frameIdx], 128, 64, SSD1306_WHITE);
    display2.display();
    frameIdx = (frameIdx + 1) % numFrames;
  }
}

void square_gif() {
  static uint8_t frameIdx_s = 0;
  static unsigned long lastMs_s = 0;
  const uint8_t* frames_s[] = {frame0_s, frame1_s, frame2_s, frame3_s, frame4_s, frame5_s, frame6_s, frame7_s, frame8_s, frame9_s};
  const uint8_t numFrames_s = 10;
  if (millis() - lastMs_s >= 100) {
    lastMs_s = millis();
    display2.clearDisplay();
    display2.drawBitmap(0, 0, frames_s[frameIdx_s], 128, 64, SSD1306_WHITE);
    display2.display();
    frameIdx_s = (frameIdx_s + 1) % numFrames_s;
  }
}

void pulse_gif() {
  static uint8_t frameIdx_p = 0;
  static unsigned long lastMs_p = 0;
  const uint8_t* frames_p[] = {frame0_p, frame1_p, frame2_p, frame3_p, frame4_p, frame5_p, frame6_p, frame7_p, frame8_p, frame9_p};
  const uint8_t numFrames_p = 10;
  if (millis() - lastMs_p >= 83) {
    lastMs_p = millis();
    display2.clearDisplay();
    display2.drawBitmap(0, 0, frames_p[frameIdx_p], 128, 64, SSD1306_WHITE);
    display2.display();
    frameIdx_p = (frameIdx_p + 1) % numFrames_p;
  }
}

void sampleHold_gif() {
  static uint8_t frameIdx_sh = 0;
  static unsigned long lastMs_sh = 0;
  const uint8_t* frames_sh[] = {frame0_sh, frame1_sh, frame2_sh, frame3_sh, frame4_sh, frame5_sh, frame6_sh, frame7_sh, frame8_sh, frame9_sh, frame10_sh, frame11_sh, frame12_sh, frame13_sh, frame14_sh, frame15_sh, frame16_sh, frame17_sh, frame18_sh, frame19_sh};
  const uint8_t numFrames_sh = 20;
  if (millis() - lastMs_sh >= 50) {
    lastMs_sh = millis();
    display2.clearDisplay();
    display2.drawBitmap(0, 0, frames_sh[frameIdx_sh], 128, 64, SSD1306_WHITE);
    display2.display();
    frameIdx_sh = (frameIdx_sh + 1) % numFrames_sh;
  }
}

void triangle_gif() {
  static uint8_t frameIdx_tri = 0;
  static unsigned long lastMs_tri = 0;
  const uint8_t* frames_tri[] = {frame0_tri, frame1_tri, frame2_tri, frame3_tri, frame4_tri, frame5_tri, frame6_tri, frame7_tri, frame8_tri, frame9_tri, frame10_tri, frame11_tri, frame12_tri, frame13_tri, frame14_tri, frame15_tri, frame16_tri, frame17_tri, frame18_tri, frame19_tri};
  const uint8_t numFrames_tri = 20;
  if (millis() - lastMs_tri >= 100) {
    lastMs_tri = millis();
    display2.clearDisplay();
    display2.drawBitmap(0, 0, frames_tri[frameIdx_tri], 128, 64, SSD1306_WHITE);
    display2.display();
    frameIdx_tri = (frameIdx_tri + 1) % numFrames_tri;
  }
}

void sawtooth_gif() {
  static uint8_t frameIdx_saw = 0;
  static unsigned long lastMs_saw = 0;
  const uint8_t* frames_saw[] = {frame0_saw, frame1_saw, frame2_saw, frame3_saw, frame4_saw, frame5_saw, frame6_saw, frame7_saw, frame8_saw, frame9_saw, frame10_saw, frame11_saw, frame12_saw, frame13_saw, frame14_saw, frame15_saw};
  const uint8_t numFrames_saw = 16;
  if (millis() - lastMs_saw >= 100) {
    lastMs_saw = millis();
    display2.clearDisplay();
    display2.drawBitmap(0, 0, frames_saw[frameIdx_saw], 128, 64, SSD1306_WHITE);
    display2.display();
    frameIdx_saw = (frameIdx_saw + 1) % numFrames_saw;
  }
}
