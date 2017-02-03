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
const int LCD_RS = 8;  // 4
const int LCD_ENABLE = 9;  // 5
const int LCD_D0 = 4;  // 10
const int LCD_D1 = 5;  // 11
const int LCD_D2 = 6;  // 12
const int LCD_D3 = 7;  // 13
const int LCD_ROWS = 2;
const int LCD_COLS = 16;

// Keypad settings
const int KEYPAD_PIN = 0;

// Chars
byte card_backside[8] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
};

byte heart[8] = {
  B01010,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000,
};

byte diamond[8] = {
  B00100,
  B01110,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000,
};

byte spade[8] = {
  B00100,
  B01110,
  B11111,
  B11111,
  B00100,
  B01110,
  B00000,
  B00000,
};

byte club[8] = {
  B01110,
  B01110,
  B11111,
  B11111,
  B00100,
  B01110,
  B00000,
  B00000,
};

class Display {
  public:
    Display() : lcd_(LCD_RS, LCD_ENABLE, LCD_D0, LCD_D1, LCD_D2, LCD_D3) {
      lcd_.begin(LCD_COLS, LCD_ROWS);
      lcd_.createChar(0, card_backside);
      lcd_.createChar(1, heart);
      lcd_.createChar(2, diamond);
      lcd_.createChar(3, spade);
      lcd_.createChar(4, club);
    }

    LiquidCrystal getLCD() { return lcd_; }

    template<typename T>
    void print(int row, int col, const T& val) {
      lcd_.setCursor(col, row);
      lcd_.print(val);
    }

    void setCursor(uint8_t row, uint8_t col) { lcd_.setCursor(col, row); }

    void write(uint8_t glyph) { lcd_.write(glyph); }

    void printGlyph(uint8_t row, uint8_t col, uint8_t glyph) {
      lcd_.setCursor(col, row);
      lcd_.write(glyph);
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

class Poker {

  char card_rank[13] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};

  struct Card {
    char suit;
    char rank;
  };

  struct Player {
    Card hand[5] = { { '0', '0' },{ '0', '0' },{ '0', '0' },{ '0', '0' },{ '0', '0' } };
    int coins = 0;
  };

  public:
    Poker() : player_(), arduino_(), bank_coins_(0), start_game_(false) {}

    void startGame() {
      player_.coins = 500;
      arduino_.coins = 500;
      bank_coins_ = 0;
      start_game_ = true;
    }

    void startTurn() {}
    void dealCards() {}
    void printPlayerHand() {
      if (start_game_) {
        for (uint8_t i = 0; i < 5; ++i) {
          
        }
      } else {
        hw::lcd.printGlyph(0, 0, 0);
        hw::lcd.printGlyph(0, 1, 0);
        hw::lcd.printGlyph(0, 2, 0);
        hw::lcd.printGlyph(0, 3, 0);
        hw::lcd.printGlyph(0, 4, 0);
        hw::lcd.printGlyph(1, 0, 0);
        hw::lcd.printGlyph(1, 1, 0);
        hw::lcd.printGlyph(1, 2, 0);
        hw::lcd.printGlyph(1, 3, 0);
        hw::lcd.printGlyph(1, 4, 0);
      }
    }
    
    void printArduinoCloseHand() {
      hw::lcd.printGlyph(0, 11, 0);
      hw::lcd.printGlyph(0, 12, 0);
      hw::lcd.printGlyph(0, 13, 0);
      hw::lcd.printGlyph(0, 14, 0);
      hw::lcd.printGlyph(0, 15, 0);
      hw::lcd.printGlyph(1, 11, 0);
      hw::lcd.printGlyph(1, 12, 0);
      hw::lcd.printGlyph(1, 13, 0);
      hw::lcd.printGlyph(1, 14, 0);
      hw::lcd.printGlyph(1, 15, 0);
    }

    void printArduinoOpenHand() {}
    void playerAdd() {}
    void playerPass() {}
    void playerOpen() {}
    void arduinoAdd() {}
    void arduinoPass() {}
    void arduinoOpen() {}

  private:
    Player player_;
    Player arduino_;
    int bank_coins_;
    bool start_game_;
};

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
    TableScreen(Poker& poker) : Screen(), poker_(poker) {}
    
    void show() override {
      hw::lcd.clear();
      poker_.printPlayerHand();
      poker_.printArduinoCloseHand();
      hw::lcd.print(0, 6, "BANK");
      hw::lcd.print(1, 6, "0$");
    }
    
    void action(int button) {}
    
  private:
    Poker& poker_;
};

class MoneyScreen : public Screen {
  public:
    MoneyScreen() : Screen() {}
    
    void show() override {
      hw::lcd.clear();
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
      hw::lcd.clear();
      hw::lcd.print(0, 0, "S-open L-pass");
      hw::lcd.print(1, 0, "R-add U/D-scroll");
    }
    
    void action(int button) override {}
};

class AuthorScreen : public Screen {
  public:
    AuthorScreen() : Screen() {}
    
    void show() override {
      hw::lcd.clear();
      hw::lcd.print(0, 0, "Fedyashev");
      hw::lcd.print(1, 0, "Andrei");
      hw::lcd.print(1, 12, "2017");
    }
    
    void action(int button) override {}
};

class Controller {
  public:
    Controller() : current_(0), poker_() {createScreenStructure();}
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
        case hw::Keypad::BUTTON_SELECT:
        case hw::Keypad::BUTTON_LEFT:
        case hw::Keypad::BUTTON_RIGHT:
          current_->action(button);
          break;
      }
    }
    
    void showScreen() {current_->show();}
    
  private:
    void createScreenStructure() {
      Screen* main_screen = new MainScreen();
      Screen* tabel_screen = new TableScreen(poker_);
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
    Poker poker_;
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
