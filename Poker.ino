/*
 Name:		Poker.ino
 Created:	01.02.2017 19:46:55
 Author:	Fedyashev Andrei
*/

///////////////////////////////////////////////////////////////////////////////

#include "LiquidCrystal.h"

///////////////////////////////////////////////////////////////////////////////

namespace hw {

// Display settings
const int LCD_RS = 4;  // 8
const int LCD_ENABLE = 5;  // 9
const int LCD_D0 = 10;  // 4
const int LCD_D1 = 11;  // 5
const int LCD_D2 = 12;  // 6
const int LCD_D3 = 13;  // 7
const int LCD_ROWS = 2;
const int LCD_COLS = 16;

// Keypad settings
const int KEYPAD_PIN = 0;

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
    
    static const int BUTTON_NONE = 0;
    static const int BUTTON_SELECT = 1;
    static const int BUTTON_LEFT = 2;
    static const int BUTTON_DOWN = 3;
    static const int BUTTON_UP = 4;
    static const int BUTTON_RIGHT = 5;    

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

};  // namespace hw

///////////////////////////////////////////////////////////////////////////////

namespace Game {

class Screen {
  public:
    Screen() : prev_(0), next_(0) {}
    virtual ~Screen() {}

    Screen* getPrev() { return prev_; }
    Screen* getNext() { return next_; }
    void setPrev(Screen* prev) { prev_ = prev; }
    void setNext(Screen* next) { next_ = next; }
    
    virtual void show() = 0;
    virtual void action(int button) = 0;

  private:
    Screen* prev_;
    Screen* next_;
};

class ISelectable {
  public:
    virtual ~ISelectable() {}
    virtual void select() = 0;
};

class ILeftable {
  public:
    virtual ~ILeftable() {}
    virtual void left() = 0;
};

class IRightable {
  public:
    virtual ~IRightable() {}
    virtual void right() = 0;
};

class MainScreen : public Screen {
  public:
    MainScreen() : Screen() {}
    
    void show() override {
      hw::lcd.clear();
      hw::lcd.print(0, 6, "POKER");
      hw::lcd.print(1, 0, "-START NEW GAME-");
    }
    
    void action(int button) override {}
    
  private:
  
};

class TableScreen : public Screen {
  public:
    TableScreen() : Screen() {}
    
    void show() override {
      hw::lcd.clear();
      hw::lcd.print(0, 0, "* * *");
      hw::lcd.print(1, 0, " * * ");
      hw::lcd.print(0, 11, "* * *");
      hw::lcd.print(1, 11, " * * ");
      hw::lcd.print(0, 6, "BANK");
      hw::lcd.print(1, 6, "0$");
    }
    
    void action(int button) {}
    
  private:
  
};

class MoneyScreen : public Screen {
  public:
    MoneyScreen() : Screen() {}
    
    void show() override {
      hw::lcd.print(0, 0, "Player");
      hw::lcd.print(1, 0, "0$");
      hw::lcd.print(0, 9, "Arduino");
      hw::lcd.print(1, 14, "0$");
    }
    
    void action(int button) override {}
    
  private:
  
};

class HelpScreen : public Screen {
  public:
    HelpScreen() : Screen() {}
    
    void show() override {
      hw::lcd.print(0, 0, "S-open L-pass");
      hw::lcd.print(1, 0, "R-add U/D-scroll");
    }
    
    void action(int button) override {}
};

class AuthorScreen : public Screen {
  public:
    AuthorScreen() : Screen() {}
    
    void show() override {
      hw::lcd.print(0, 0, "Fedyashev");
      hw::lcd.print(1, 0, "Andrei");
      hw::lcd.print(1, 10, "@2017");
    }
    
    void action(int button) override {}
};

class Controller {
  public:
    Controller() : current_(0) {createScreenStructure();}
    ~Controller() {deleteScreenStructure();}
    
    void run() {
      int button = hw::keypad.pressedButton();
      switch (button) {
        case hw::Keypad::BUTTON_UP :
          current_ = current_->getPrev();
          current_->show();
          break;
        case hw::Keypad::BUTTON_DOWN :
          current_ = current_->getNext();
          current_->show();
          break;
      }
    }
    
    void showScreen() {current_->show();}
    
  private:
    void createScreenStructure() {
      Screen* main_screen = new MainScreen();
      Screen* tabel_screen = new TableScreen();
      Screen* money_screen = new MoneyScreen();
      Screen* help_screen = new HelpScreen();
      Screen* author_screen = new AuthorScreen();
      
      main_screen->setPrev(author_screen);
      main_screen->setNext(tabel_screen);
      
      tabel_screen->setPrev(main_screen);
      tabel_screen->setNext(money_screen);
      
      money_screen->setPrev(tabel_screen);
      money_screen->setNext(help_screen);
      
      help_screen->setPrev(money_screen);
      help_screen->setNext(author_screen);
      
      author_screen->setPrev(money_screen);
      author_screen->setNext(main_screen);
      
      current_ = main_screen;
    }
    
    void deleteScreenStructure() {
      current_->getPrev()->setNext(0);
      Screen* next = 0;
      while (next = current_->getNext()) {
        delete current_;
        current_ = next;
      }
      delete current_;
    }
  
    Screen* current_;
};

};  // namespace Game
///////////////////////////////////////////////////////////////////////////////

Game::Controller controller;

void setup() {
  controller.showScreen();
}

void loop() {
  controller.run();
}

///////////////////////////////////////////////////////////////////////////////
