#include <EverydayCalendar_lights.h>
#include <EverydayCalendar_touch.h>

typedef struct {
   int8_t    x;
   int8_t    y;
} Point;

EverydayCalendar_touch cal_touch;
EverydayCalendar_lights cal_lights;
int16_t brightness = 128;

void setup() {
  Serial.begin(9600);
  Serial.println("Sketch setup started");
  
  // Initialize LED functionality
  cal_lights.configure();
  cal_lights.setBrightness(200);
  cal_lights.begin();

  // Perform startup animation
  hohmanDesign();

  // Fade out
  for(int b = 200; b >= 0; b--){
    cal_lights.setBrightness(b);
    delay(4);
  }

  // Initialize touch functionality
  cal_touch.configure();
  cal_touch.begin();
  cal_lights.loadLedStatesFromMemory();
  delay(1500);

  // Fade in
  for(int b = 0; b <= brightness; b++){
    cal_lights.setBrightness(b);
    delay(4);
  }
  
  Serial.println("Sketch setup complete");
}

void loop() {
  static Point previouslyHeldButton = {0xFF, 0xFF}; // 0xFF and 0xFF if no button is held
  static uint16_t touchCount = 1;
  static const uint8_t debounceCount = 3;
  static const uint16_t clearCalendarCount = 1300; // ~40 seconds.  This is in units of touch sampling interval ~= 30ms.  
  Point buttonPressed = {0xFF, 0xFF};
  bool touch = cal_touch.scanForTouch();
  // Handle a button press
  if(touch)
  {
    // Brightness Buttons
    if(cal_touch.y == 31){
      if(cal_touch.x == 4){
        brightness -= 3;
      }else if(cal_touch.x == 6){
        brightness += 2;
      }
      brightness = constrain(brightness, 0, 200);
      Serial.print("Brightness: ");
      Serial.println(brightness);
      cal_lights.setBrightness((uint8_t)brightness);
    }
    // If all buttons aren't touched, reset debounce touch counter
    if(previouslyHeldButton.x == -1){
      touchCount = 0;
    }

    // If this button is been held, or it's just starting to be pressed and is the only button being touched
    if(((previouslyHeldButton.x == cal_touch.x) && (previouslyHeldButton.y == cal_touch.y))
    || (debounceCount == 0))
    {
      // The button has been held for a certain number of consecutive checks 
      // This is called debouncing
      if (touchCount == debounceCount){
        // Button is activated
        cal_lights.toggleLED((uint8_t)cal_touch.x, (uint8_t)cal_touch.y);
        cal_lights.saveLedStatesToMemory();
        Serial.print("x: ");
        Serial.print(cal_touch.x);
        Serial.print("\ty: ");
        Serial.println(cal_touch.y);
      }

      // Check if the special "Reset" January 1 button is being held
      if((cal_touch.x == 0) && (cal_touch.y == 0) && (touchCount == clearCalendarCount)){
        Serial.println("Resetting all LED states");
        clearAnimation();
      }
      
      if(touchCount < 65535){
        touchCount++;
        Serial.println(touchCount);
      }
    }
  }

  previouslyHeldButton.x = cal_touch.x;
  previouslyHeldButton.y = cal_touch.y;
}

void honeyDrip(){
  uint16_t interval_ms = 25;
  static const uint8_t monthDayOffset[12] = {0, 3, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0};
  // Turn on all LEDs one by one in the span of a few second
  for(int day = 0; day < 31; day++){
    for(int month = 0; month < 12; month++){
      int8_t adjustedDay = day - monthDayOffset[month];
      if(adjustedDay >= 0 ){
        cal_lights.setLED(month, adjustedDay, true);
      }
    }
    delay(interval_ms);
    interval_ms = interval_ms + 2;
  }
}


void hohmanDesign(){
  uint16_t interval_ms = 25;
  static const uint8_t monthDayTotal[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  static const uint8_t hohmanArray[12][31] = 
  { // Values here can be anything, when using check-value rather than serach
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //Jan
    {0,0,3,4,5,0,0,8,9,10,11,0,13,14,15,16,0,18,0,20,21,22,23,0,25,0,0,28,0,0,0},
    {0,0,3,0,0,6,0,8,0,0,0,0,0,0,0,16,0,18,0,20,0,0,23,0,25,0,0,28,0,0,0},
    {0,0,3,0,0,6,0,8,9,10,0,0,0,14,15,0,0,18,0,20,0,22,23,0,25,0,27,28,0,0,0},
    {0,0,3,0,0,6,0,8,0,0,0,0,13,0,0,0,0,18,0,20,0,0,0,0,25,26,0,28,0,0,0},
    {0,0,3,4,5,0,0,8,9,10,11,0,13,14,15,16,0,18,0,20,21,22,23,0,25,0,0,28,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, //July
    {0,2,0,0,5,0,7,8,9,10,0,12,0,0,15,0,17,0,0,20,0,22,0,0,25,0,27,0,0,30,0},
    {0,2,0,0,5,0,7,0,0,10,0,12,0,0,15,0,17,0,0,20,0,22,0,0,25,0,27,0,0,30,0},
    {0,2,3,4,5,0,7,0,0,10,0,12,13,14,15,0,17,0,20,0,22,23,24,25,0,27,0,29,30,0},
    {0,2,0,0,5,0,7,0,0,10,0,12,0,0,15,0,17,18,19,20,0,22,0,0,25,0,27,28,0,30,0},
    {0,2,0,0,5,0,7,8,9,10,0,12,0,0,15,0,17,0,0,20,0,22,23,24,25,0,27,0,0,30,0}
   }
  
  // Turn on rows of LEDs, sequentially leaving on a Hohman Design LOGO above after a scan line
  for(int day = 0; day < 31; day++){
    for(int month = 0; month < 12; month++) {
      if (day >= 1){ // Skip first day (scan line)
        logoPixel = hohmanArray[month][day-1]; // previous day
        if (logoPixel == 0) {
            cal_lights.setLED(month, day-1, false); // turn off non-logo pixels
          }
        }
      if (day == monthDayTotal[month]) {
          continue; // Skip Month (no more days)
        }
      cal_lights.setLED(month, day, true); // Set row to ON (scan line)
    }
    delay(interval_ms);
    interval_ms = interval_ms + 2;
  }
  
  delay(5000); // 5 Second Display of Logo
}

void clearAnimation(){
  uint16_t interval_ms = 25;
  static const uint8_t monthMaxDay[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  for(int month = 11; month >= 0; month--){
    for(int day = monthMaxDay[month]-1; day >=0; day--){
       cal_lights.setLED(month, day, false);
       delay(interval_ms);
    }
  }
  cal_lights.saveLedStatesToMemory();
}
