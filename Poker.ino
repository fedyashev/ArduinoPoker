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
const int LCD_RS = 4;      // 8  // 4
const int LCD_ENABLE = 5;  // 9  // 5
const int LCD_D0 = 10;     // 4  //  10
const int LCD_D1 = 11;     // 5  //  11
const int LCD_D2 = 12;     // 6  //  12
const int LCD_D3 = 13;     // 7  //  13
const int LCD_ROWS = 2;
const int LCD_COLS = 16;

// Keypad settings
const int KEYPAD_PIN = 0;

// Glyphs
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

// Card suits
const int SUIT_HEART = 0;
const int SUIT_DIAMOND = 1;
const int SUIT_SPADE = 2;
const int SUIT_CLUB = 3;
const int CARD_BACKSIDE = 4;

class Display {
  public:
    Display() : lcd_(LCD_RS, LCD_ENABLE, LCD_D0, LCD_D1, LCD_D2, LCD_D3) {
      lcd_.begin(LCD_COLS, LCD_ROWS);
      lcd_.createChar(SUIT_HEART, heart);
      lcd_.createChar(SUIT_DIAMOND, diamond);
      lcd_.createChar(SUIT_SPADE, spade);
      lcd_.createChar(SUIT_CLUB, club);
      lcd_.createChar(CARD_BACKSIDE, card_backside);
    }

    LiquidCrystal& getLCD() { return lcd_; }

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

char card_rank[13] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};

struct Card {
  uint8_t suit = 0;
  uint8_t rank = 0;
};

struct Player {
  Card hand[5] = { {0, 8}, {1, 9}, {2, 10}, {3, 11}, {0, 12} };
  int hand_rank = 0;
  int coins = 0;
};

class PokerSingleton {
  public:
    static PokerSingleton& getInstance() {
      if (!instance_) instance_ = new PokerSingleton();
      return *instance_;
    }
  
    Player& getPlayer() {return player_;}
    const Player& getPlayer() const {return player_;}
    
    Player& getArduino() {return arduino_;}
    const Player& getArduino() const {return arduino_;}
    
    int getBankCoins() const {return bank_coins_;}

    void startGame() {
      player_.coins = 500;
      arduino_.coins = 500;
      bank_coins_ = 0;
      start_game_ = true;
    }

    void startTurn() {}
    
    void dealCards() {
      Card hands[10];
      uint8_t i = 0;
      while (i < 13) {
        hands[i] = {random(4), random(13)};
        if (!i) {
          ++i;
          continue;
        };
        bool is_card_unique = true;
        for (uint8_t j = 0; j < i; ++j) {
          if (hand[i].suit == hand[i].suit && hand[j].rank == hand[j].rank) {
            is_card_unique = false;
            break;
          }
        }
        if (is_card_unique) ++i;
      }
      
    }
    
    void printClosedCard(uint8_t col) {
      hw::lcd.printGlyph(0, col, hw::CARD_BACKSIDE);      
      hw::lcd.printGlyph(1, col, hw::CARD_BACKSIDE);
    }
    
    void printOpenedCard(uint8_t col, const Card& card) {
      if (card.suit < 0 || card.suit > 3 || card.rank < 0 || card.rank > 12) printClosedCard(col);
      hw::lcd.print(0, col, card_rank[card.rank]);
      hw::lcd.printGlyph(1, col, card.suit);      
    }
    
    void printPlayerHand() {
      for (uint8_t i = 0; i < 5; ++i) {
        if (start_game_) {
          printOpenedCard(i, player_.hand[i]);
        } else {
          printClosedCard(i);
        }
      }
    }
    
    void printArduinoCloseHand() {
      for (uint8_t i = 0; i < 5; ++i) {
        printClosedCard(11 + i);
      }
    }

    void printArduinoOpenHand() {}
    void playerAdd() {}
    void playerPass() {}
    void playerOpen() {}
    void arduinoAdd() {}
    void arduinoPass() {}
    void arduinoOpen() {}

  private:
    PokerSingleton() : player_(), arduino_(), bank_coins_(0), start_game_(false) {}
    PokerSingleton(const PokerSingleton& poker) {}
  
    static PokerSingleton* instance_;
  
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
    const Screen* getPrev() const {return prev_;}
    void setPrev(Screen* prev) { prev_ = prev; }
    
    Screen* getNext() { return next_; }
    const Screen* getNext() const {return next_;}
    void setNext(Screen* next) { next_ = next; }
    
    virtual Screen* Select() {return 0;}
    virtual Screen* Left() {return 0;}
    virtual Screen* Right() {return 0;}
    virtual Screen* Up() {return (prev_ ? prev_ : 0);}
    virtual Screen* Down() {return (next_ ? next_ : 0);}
    
    virtual void show() = 0;
    
    Screen* action(int button) {
      switch (button) {
        case hw::Keypad::BUTTON_SELECT : return Select();
        case hw::Keypad::BUTTON_LEFT : return Left();
        case hw::Keypad::BUTTON_RIGHT : return Right();
        case hw::Keypad::BUTTON_UP : return Up();
        case hw::Keypad::BUTTON_DOWN : return Down();
        default : return 0;
      }
    };
    
  private:
    Screen* prev_;
    Screen* next_;
};

class MainScreen : public Screen {
  public:
    Screen* Select() {
      PokerSingleton::getInstance().startGame();
      return getNext();
    }
    
    void show() override {
      hw::lcd.clear();
      hw::lcd.print(0, 6, "POKER");
      hw::lcd.print(1, 0, "-START NEW GAME-");
    }
};

class TableScreen : public Screen {
  public:
    Screen* Select() {
      return 0;
    }
    
    Screen* Left() {
      return 0;
    }
    
    Screen* Right() {
      return 0;
    }
  
    void show() override {
      hw::lcd.clear();
      PokerSingleton::getInstance().printPlayerHand();
      PokerSingleton::getInstance().printArduinoCloseHand();
      hw::lcd.print(0, 6, "BANK");
      hw::lcd.print(1, 6, "0$");
    }
};

class MoneyScreen : public Screen {
  public:
    void show() override {
      hw::lcd.clear();
      hw::lcd.print(0, 0, "Player");
      hw::lcd.print(1, 0, PokerSingleton::getInstance().getPlayer().coins + "$");
      hw::lcd.print(0, 9, "Arduino");
      hw::lcd.print(1, 14, PokerSingleton::getInstance().getArduino().coins + "$");
    }
};

class HelpScreen : public Screen {
  public:
    void show() override {
      hw::lcd.clear();
      hw::lcd.print(0, 0, "S-open L-pass");
      hw::lcd.print(1, 0, "R-add U/D-scroll");
    }
};

class AuthorScreen : public Screen {
  public:
    void show() override {
      hw::lcd.clear();
      hw::lcd.print(0, 0, "Fedyashev");
      hw::lcd.print(1, 0, "Andrei");
      hw::lcd.print(1, 12, "2017");
    }
};

class Controller {
  public:
    Controller() : current_(0) {createScreenStructure();}
    ~Controller() {deleteScreenStructure();}
    
    void run() {
      Screen* screen = current_ -> action(hw::keypad.pressedButton());
      if (screen) {
        current_ = screen;
        current_ -> show();
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

Game::PokerSingleton* Game::PokerSingleton::instance_ = 0;
Game::Controller controller;

void setup() {
  randomSeed(analogRead(0));
  controller.showScreen();
}

void loop() {
  controller.run();
}

///////////////////////////////////////////////////////////////////////////////
