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
const int LCD_RS = 8;      // 8  // 4
const int LCD_ENABLE = 9;  // 9  // 5
const int LCD_D0 = 4;     // 4  //  10
const int LCD_D1 = 5;     // 5  //  11
const int LCD_D2 = 6;     // 6  //  12
const int LCD_D3 = 7;     // 7  //  13
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
  int suit = 0;
  int rank = 0;
};

struct Player {
  Card hand[5];// = { {0, 8}, {1, 9}, {2, 10}, {3, 11}, {0, 12} };
  double hand_rank = 0;
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
    bool isGameStarted() const {return start_game_;}

    void printMessage(const String& line1, const String& line2) {
      hw::lcd.print(0, 5, line1);
      hw::lcd.print(1, 5, line2);
    }

    void printBank() {
      hw::lcd.print(0, 6, "BANK");
      hw::lcd.print(1, 6, bank_coins_);
    }

    void startGame() {
      player_.coins = 100;
      arduino_.coins = 100;
      bank_coins_ = 0;
      start_game_ = true;
      startTurn();
    }
    
    void endGame() {
      if (!player_.coins) {
        printMessage(" GAME ", " OVER ");
      } else {
        printMessage(" YOU  ", " WIN  ");
      }
      start_game_ = false;
    }

    void startTurn() {
      dealCards();
      player_.coins -= 10;
      arduino_.coins -= 10;
      bank_coins_ = 20;
    }

    void endTurn() {
      if (player_.hand_rank > arduino_.hand_rank) {
        printMessage("< WIN ", "      ");
        player_.coins += bank_coins_;
      } else if (player_.hand_rank < arduino_.hand_rank) {
        printMessage(" WIN >", "      ");
        arduino_.coins += bank_coins_;
      } else {
        printMessage(" DRAW ", "      ");
        player_.coins += bank_coins_ / 2;
        arduino_.coins += bank_coins_ / 2;
      }
      bank_coins_ = 0;
    }
    
    void checkTurnResult() {
      if (player_.coins && arduino_.coins) {
        startTurn();
      } else {
        endGame();
        delay(2000);
      }    
    }

    void fillRateArray(int* rate_array, Card* hand) {
      for (uint8_t i = 0; i < 5; ++i) {
        int count = 0;
        for (uint8_t j = 0; j < 5; ++j) {
          if (hand[i].rank == hand[j].rank) ++count;
        }
        rate_array[i] = count * 100 + hand[i].rank;
      }
    }

    bool isOnePair(Card* hand) {
      int rate_array[5];
      fillRateArray(rate_array, hand);
      int count = 0;
      for (int i = 0; i < 5; ++i) {
        if ((rate_array[i] >= 200) && (rate_array[i] < 300)) ++count;
      }
      return count == 2;
    }

    bool isTwoPairs(Card* hand) {
      int rate_array[5];
      fillRateArray(rate_array, hand);
      int count = 0;
      for (int i = 0; i < 5; ++i) {
        if ((rate_array[i] >= 200) && (rate_array[i] < 300)) ++count;
      }
      return count == 4;
    }

    bool isSet(Card* hand) {
      int rate_array[5];
      fillRateArray(rate_array, hand);
      for (int i = 0; i < 5; ++i) {
        if (rate_array[i] >= 300 && rate_array[i] < 400) return true;
      }
      return false;
    }

    bool isStraight(Card* hand) {
      int num = hand[0].rank;
      if (num < 6) return false;
      for (int i = 1; i < 5; ++i) {
        if (hand[i].rank == num - 1) {
          --num;
        } else {
          return false;
        }
      }
      return true;
    }

    bool isFlush(Card* hand) {
      int suit = hand[0].suit;
      for (int i = 1; i < 5; ++i) {
        if (hand[i].suit != suit) return false;
      }
      return true;
    }

    bool isFullHouse(Card* hand) {
      return isOnePair(hand) && isSet(hand);
    }

    bool isCare(Card* hand) {
      int rate_array[5];
      fillRateArray(rate_array, hand);
      for (int i = 0; i < 5; ++i) {
        if (rate_array[i] >= 400) return true;
      }
      return false;
    }

    bool isFlushStraight(Card* hand) {
      return isFlush(hand) && isStraight(hand);
    }

    bool isFlushRoyal(Card* hand) {
      return isFlushStraight(hand) && hand[0].rank == 12;
    };

    double handRank(Card* hand) {
      double rank = 10000000000.0;
      if (isFlushRoyal(hand)) rank *= 9;
      else if (isFlushStraight(hand)) rank *= 8;
      else if (isCare(hand)) rank *= 7;
      else if (isFullHouse(hand)) rank *= 6;
      else if (isFlush(hand)) rank *= 5;
      else if (isStraight(hand)) rank *= 4;
      else if (isSet(hand)) rank *= 3;
      else if (isTwoPairs(hand)) rank *= 2;
      else if (isOnePair(hand)) rank *= 1;
      else rank = 0;
      rank += hand[0].rank * 100000000.0 + hand[1].rank * 1000000.0 + hand[2].rank * 10000.0 + hand[3].rank * 100.0 + hand[4].rank;
      /*for (int i = 0; i < 5; ++i) {
        rank += i * 13 + hand[i].rank;
      }*/
      return rank;
    };

    void sortHand(Card* hand) {
      int rate_array[5];
      fillRateArray(rate_array, hand);
      for (uint8_t i = 0; i < 5; ++i) {
        for (uint8_t j = 0; j < 5; ++j) {
          if (rate_array[i] > rate_array[j]) {
            uint8_t tmp = hand[i].suit;
            hand[i].suit = hand[j].suit;
            hand[j].suit = tmp;
            
            tmp = hand[i].rank;
            hand[i].rank = hand[j].rank;
            hand[j].rank = tmp;

            tmp = rate_array[i];
            rate_array[i] = rate_array[j];
            rate_array[j] = tmp;
          }
        }
      }
    }
    
    void dealCards() {
      Card hands[10];
      int i = 0;
      while (i < 10) {
        srand((unsigned int)micros());
        hands[i].suit = rand() / 4 % 4;
        hands[i].rank = rand() % 13;
        if (!i) {
          ++i;
          continue;
        };
        bool is_card_unique = true;
        for (int j = 0; j < i; ++j) {
          if (hands[i].suit == hands[j].suit && hands[i].rank == hands[j].rank) {
            is_card_unique = false;
            break;
          }
        }
        if (is_card_unique) ++i;
      }
      for (int i = 0; i < 5; ++i) {
        player_.hand[i].suit = hands[i].suit;
        player_.hand[i].rank = hands[i].rank;
        arduino_.hand[i].suit = hands[5 + i].suit;
        arduino_.hand[i].rank = hands[5 + i].rank;
      }
      sortHand(player_.hand);
      sortHand(arduino_.hand);
      player_.hand_rank = handRank(player_.hand);
      arduino_.hand_rank = handRank(arduino_.hand);
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

    void printArduinoOpenHand() {
      for (int i = 0; i < 5; ++i) {
        printOpenedCard(11 + i, arduino_.hand[i]);
        delay(300);
      }
    }
    
    void playerAdd() {
      if (player_.coins < 10) {
        printMessage("  NO  ", " CASH ");
      } else {
        player_.coins -= 10;
        bank_coins_ += 10;
        printMessage("< ADD ", " +10$ ");      
      }
    }
    
    void playerPass() {
      printMessage("<PASS ", "      ");
      arduino_.coins += bank_coins_;
      bank_coins_ = 0;
    }

    void playerOpen() {
      printMessage("<OPEN ", "      ");
    }

    void arduinoTurn() {
      if (arduino_.coins < 10) {
        arduinoOpen();
        delay(1000);
        endTurn();
        delay(2000);
        checkTurnResult();
        //delay(1000);
        return;
      }
      srand((unsigned int)micros());
      int action = rand() % 10;
      if (action < 6) {
        arduinoAdd();
        delay(2000);
      } else if (action < 8) {
        arduinoOpen();
        delay(1000);
        endTurn();
        delay(2000);
        checkTurnResult();
        //delay(1000);
      } else {
        arduinoPass();
        delay(2000);
        checkTurnResult();
        //delay(1000);
      }
    }

    void arduinoAdd() {
      arduino_.coins -= 10;
      bank_coins_ += 10;
      printMessage(" ADD >", " +10$ ");
    }
    
    void arduinoPass() {
      player_.coins += bank_coins_;
      bank_coins_ = 0;
      printMessage(" PASS>", "      ");
    }
    
    void arduinoOpen() {
      printMessage(" OPEN>", "      ");
      delay(1000);
      printArduinoOpenHand();
    }

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
      if (!PokerSingleton::getInstance().isGameStarted()) {
        PokerSingleton::getInstance().startGame();
        return this;
      }
      PokerSingleton::getInstance().playerOpen();
      delay(1000);
      PokerSingleton::getInstance().printArduinoOpenHand();
      PokerSingleton::getInstance().endTurn();
      delay(2000);
      PokerSingleton::getInstance().checkTurnResult();
      return this;
    }
    
    Screen* Left() {
      if (!PokerSingleton::getInstance().isGameStarted() || PokerSingleton::getInstance().getPlayer().coins < 10) return 0;
      PokerSingleton::getInstance().playerPass();
      delay(2000);
      PokerSingleton::getInstance().checkTurnResult();
      return this;
    }
    
    Screen* Right() {
      if (!PokerSingleton::getInstance().isGameStarted() || PokerSingleton::getInstance().getPlayer().coins < 10) return 0;
      PokerSingleton::getInstance().playerAdd();
      delay(2000);
      PokerSingleton::getInstance().arduinoTurn();
      return this;
    }
  
    void show() override {
      hw::lcd.clear();
      if (!PokerSingleton::getInstance().isGameStarted()) {
        hw::lcd.print(0, 2, "Press SELECT");
        hw::lcd.print(1, 2, "to start game");
        return;
      } else {
        PokerSingleton::getInstance().printPlayerHand();
        PokerSingleton::getInstance().printArduinoCloseHand();
        PokerSingleton::getInstance().printBank();
      }
    }
};

class MoneyScreen : public Screen {
  public:
    void show() override {
      hw::lcd.clear();
      hw::lcd.print(0, 0, "Player");
      hw::lcd.print(1, 0, PokerSingleton::getInstance().getPlayer().coins);
      hw::lcd.print(0, 9, "Arduino");
      hw::lcd.print(1, 12, PokerSingleton::getInstance().getArduino().coins);
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
  controller.showScreen();
}

void loop() {
  controller.run();
}

///////////////////////////////////////////////////////////////////////////////
