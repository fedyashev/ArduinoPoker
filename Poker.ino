/*
 Name:		Poker.ino
 Created:	01.02.2017 19:46:55
 Author:	Fedyashev Andrei
*/

///////////////////////////////////////////////////////////////////////////////

#include "LiquidCrystal.h"

///////////////////////////////////////////////////////////////////////////////

// Display settings
const int LCD_RS = 8;
const int LCD_ENABLE = 9;
const int LCD_D0 = 4;
const int LCD_D1 = 5;
const int LCD_D2 = 6;
const int LCD_D3 = 7;
const int LCD_ROWS = 2;
const int LCD_COLS = 16;

// Keypad settings
const int KEYPAD_PIN = 0;
const int BUTTON_NONE = 0;
const int BUTTON_SELECT = 1;
const int BUTTON_LEFT = 2;
const int BUTTON_DOWN = 3;
const int BUTTON_UP = 4;
const int BUTTON_RIGHT = 5;


///////////////////////////////////////////////////////////////////////////////

class Display {
  public:
    Display() : lcd_(LCD_RS, LCD_ENABLE, LCD_D0, LCD_D1, LCD_D2, LCD_D3) {
      lcd_.begin(LCD_COLS, LCD_ROWS);
	  }

	  template<typename T>
	  void print(int row, int col, const T& val) {
      lcd_.setCursor(col, row);
	    lcd_.print(val);
	  }
    
    void clear() {
	    lcd_.clear();
	  }

  private:
    LiquidCrystal lcd_;
} lcd;

///////////////////////////////////////////////////////////////////////////////

class Keypad {
  public:
    Keypad() : pin_(KEYPAD_PIN), action_flag_(0) {pinMode(pin_ + 14, INPUT);}

    int pressedButton() {
      int keypad_state = stateKeypad();
      if (keypad_state < 60) return BUTTON_RIGHT;  // BUTTON_RIGHT
      if (keypad_state < 200) return BUTTON_UP;  // BUTTON_UP
      if (keypad_state < 400) return BUTTON_DOWN;  // BUTTON_DOWN
      if (keypad_state < 600) return BUTTON_LEFT;  // BUTTON_LEFT
      if (keypad_state < 800) return BUTTON_SELECT;  // BUTTON_SELECT
      return BUTTON_NONE;
    }

  private:
    int pin_;
    int action_flag_;

    int stateKeypad() {
      int keypad_state = analogRead(pin_);
      if (keypad_state < 800 && action_flag_ == 0) {
        action_flag_ = 1;
        return keypad_state;
      } else if (keypad_state >= 800 && action_flag_ == 1) {
        action_flag_ = 0;
        return keypad_state;
      }
      return 1000;
    }

} keypad;

///////////////////////////////////////////////////////////////////////////////

class Screen {
  public:
    Screen() : prev_(nullptr), next_(nullptr) {}

    Screen* getPrev() { return prev_; }
    Screen* getNext() { return next_; }
    
    void setPrev(Screen* prev) {
      delete prev_;
      prev_ = prev;
    }

    void setNext(Screen* next) {
      delete next_;
      next_ = next;
    }

  private:
    Screen* prev_;
    Screen* next_;
};

class MainScreen : public Screen {
  public:
  private:
};

///////////////////////////////////////////////////////////////////////////////

void setup() {
  lcd.print(0, 0, "Hello World!!!");
}

///////////////////////////////////////////////////////////////////////////////

void loop() {
  int button = keypad.pressedButton();
  if (button) {
    switch (button) {
    case BUTTON_SELECT:
      lcd.clear();
      lcd.print(0, 0, "Select");
      break;
    case BUTTON_LEFT:
      lcd.clear();
      lcd.print(0, 0, "Left");
      break;
    case BUTTON_RIGHT:
      lcd.clear();
      lcd.print(0, 0, "Right");
      break;
    case BUTTON_UP:
      lcd.clear();
      lcd.print(0, 0, "Up");
      break;
    case BUTTON_DOWN:
      lcd.clear();
      lcd.print(0, 0, "Down");
      break;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////