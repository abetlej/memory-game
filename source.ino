#include <Wire.h>
#include <EEPROM.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

//defining data for characters
#define EMPTY_RECTANGLE 1
byte er[] = {B11111, B10001, B10001, B10001, B10001, B10001, B10001, B11111};
#define FULL_RECTANGLE 2
byte fr[] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111};
#define EMPTY_UP_PART 3
byte eup[] = {B00000, B00000, B00000, B00000, B11111, B10001, B10001, B10001};
#define FULL_UP_PART 4
byte fup[] = {B00000, B00000, B00000, B00000, B11111, B11111, B11111, B11111};
#define EMPTY_DOWN_PART 5
byte edp[] = {B10001, B10001, B10001, B11111, B00000, B00000, B00000, B00000};
#define FULL_DOWN_PART 6
byte fdp[] = {B11111, B11111, B11111, B11111, B00000, B00000, B00000, B00000};
#define UP_AND_DOWN 8
byte uad[] = {B00100, B01010, B10001, B00000, B00000, B10001, B01010, B00100};

//colours
#define RED 0x1
#define TEAL 0x6
#define WHITE 0x7
#define VIOLET 0x5

//defining states
#define WELCOME_R 9 //all states with "_R" at the end are meant for waiting for button release
#define WELCOME 10 //displaying welcome message
#define DEFAULT_EEPROM_SETTINGS_R 11
#define DEFAULT_EEPROM_SETTINGS 12 //changing EEPROM values before first game so ranking is saved correctly
#define PRACTICE_WELCOME_R 14
#define PRACTICE_WELCOME 15//displaying info about PRACTICE mode
#define STORY_WELCOME_R 24
#define STORY_WELCOME 25 //displaying story menu: ranking or level choice
#define CHOOSE_LEVEL_R 26
#define CHOOSE_LEVEL 27 //levels can be: EASY, MEDIUM and HARD 
#define RANKING_R 28
#define RANKING 29 //displaying three highest scores so far 
#define CHANGE_M_R 30
#define CHANGE_M 31 //changing number of elements of a set M
#define M_UP 32
#define M_DOWN 33
#define CHANGE_N_R 34
#define CHANGE_N 35 //changing length of sequence
#define N_UP 36
#define N_DOWN 37
#define CHANGE_T_R 39
#define CHANGE_T 40 //changing time for element input
#define T_UP 41
#define T_DOWN 42
#define CHANGE_D_R 44
#define CHANGE_D 45 //changing display time
#define D_UP 46
#define D_DOWN 47
#define NUMBER_GENERATOR_R 54
#define NUMBER_GENERATOR 55 //generating sequence of length N out of M characters
#define SEQUENCE_INPUT_R 59
#define SEQUENCE_INPUT 60 //time for user input
#define CHECK 65 //checking if sequence generated by program is the same as user's input 
#define SUCCESS 70 //if sequences are the same
#define ERROR_HANDLER 75 //if they are different
#define WINNER 71 //when user reaches M = 4 and N = 12 in story mode (he can save his result)
#define ASK_FOR_INITIALS 72 //asking user to open serial monitor and displaying message asking for initials
#define GET_INITIALS 73 //getting initials from user
#define SAVE_INITIALS 74 //saving initials in the place of a minimum score in previous version of ranking

//time
unsigned long now = 0;
#define passed(t, w) (millis() > (t) + (w))
#define TIME_FOR_DISPLAY 350 //used in message scrolling

//defining other varaibles
int e = 0; //used for scrolling STORY_WELCOME
int g = 0; //used for scrolling ranking
String mode = ""; //game mode
int points = 0; //points = m * n * points_mult
int points_mult = 1; //to multiply points based on level (easy = 1, medium = 2, hard = 3)
//Practice mode menu
String to_start = "SELECT to start ";
String change_settings = "change settings>";
//Selecting level in story mode
String choose_level = "<EASY, ^MEDIUM^, HARD>";
String choose_level2 = choose_level + "   " + choose_level; //for scrolling

int m = 2; //number of characters used
int n = 3; //length of a sequence
double t = 2.0; //time to enter next element
double d = 1.0; //time of delay when displaying sequence in seconds
int del = int(1000 * d); //time of delay when displaying sequence in millis
int time_for_input = int(1000 * t); //time for user element input
int state = 10; //start state is WELCOME
String sequence = ""; //sequence generated by program
String input_sequence = ""; //sequence provided by user
int old_buttons = 0; //used for saving users input
String initials = "";
int i = 0; //to save the address of the lowest score in ranking
int ranking_val1; //points of the first user in ranking
int ranking_val2; // -||- second -||-
int ranking_val3; // -||- third -||-
int min_ranking; //value of the lowest score in ranking
char rank1[6]; //first ranking record
char rank2[6]; //second...
char rank3[6]; //third...
String ranking[3]; //array of ranking records
char x = 'X';
int default_points = 0;

//functions used to display characters on 3x2 matrix
void down_arrow() {
  lcd.setCursor(4, 0);
  lcd.write(EMPTY_UP_PART);
  lcd.write(EMPTY_RECTANGLE);
  lcd.write(EMPTY_UP_PART);
  lcd.setCursor(4, 1);
  lcd.write(EMPTY_DOWN_PART);
  lcd.write(FULL_RECTANGLE);
  lcd.write(EMPTY_DOWN_PART);
}

void up_arrow() {
  lcd.setCursor(4, 0);
  lcd.write(EMPTY_UP_PART);
  lcd.write(FULL_RECTANGLE);
  lcd.write(EMPTY_UP_PART);
  lcd.setCursor(4, 1);
  lcd.write(EMPTY_DOWN_PART);
  lcd.write(EMPTY_RECTANGLE);
  lcd.write(EMPTY_DOWN_PART);
}

void left_arrow() {
  lcd.setCursor(4, 0);
  lcd.write(FULL_UP_PART);
  lcd.write(EMPTY_RECTANGLE);
  lcd.write(EMPTY_UP_PART);
  lcd.setCursor(4, 1);
  lcd.write(FULL_DOWN_PART);
  lcd.write(EMPTY_RECTANGLE);
  lcd.write(EMPTY_DOWN_PART);
}

void right_arrow() {
  lcd.setCursor(4, 0);
  lcd.write(EMPTY_UP_PART);
  lcd.write(EMPTY_RECTANGLE);
  lcd.write(FULL_UP_PART);
  lcd.setCursor(4, 1);
  lcd.write(EMPTY_DOWN_PART);
  lcd.write(EMPTY_RECTANGLE);
  lcd.write(FULL_DOWN_PART);
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  //creating characters
  lcd.createChar(EMPTY_RECTANGLE, er);
  lcd.createChar(FULL_RECTANGLE, fr);
  lcd.createChar(EMPTY_UP_PART, eup);
  lcd.createChar(FULL_UP_PART, fup);
  lcd.createChar(EMPTY_DOWN_PART, edp);
  lcd.createChar(FULL_DOWN_PART, fdp);
  lcd.createChar(UP_AND_DOWN, uad);

  randomSeed(analogRead(0));
}

void loop() {
  uint8_t buttons = lcd.readButtons();

  switch (state) {
    case WELCOME_R:
      lcd.setBacklight(WHITE);
      if (buttons == 0) {
        state = WELCOME;
      }
      break;
    case WELCOME:
      ranking_val1 = EEPROM.read(20); //reading from EEPROM
      ranking_val2 = EEPROM.read(30);
      ranking_val3 = EEPROM.read(40);
      min_ranking = min(ranking_val1, min(ranking_val2, ranking_val3)); //finding the lowest score value
      if (min_ranking == ranking_val1) { //finding the address where data corresponding to lowest score value is saved
        i = 20;
      }
      else if (min_ranking == ranking_val2) {
        i = 30;
      }
      else if (min_ranking == ranking_val3) {
        i = 40;
      }
      points = 0;
      mode = "";
      lcd.setCursor(0, 0);
      lcd.print("<STORY   EEPROM^");
      lcd.setCursor(0, 1);
      lcd.print("       PRACTICE>");
      if (buttons & BUTTON_RIGHT) {
        Serial.println("practice mode");
        state = PRACTICE_WELCOME_R;
      }
      else if (buttons & BUTTON_LEFT) {
        Serial.println("story mode");
        state = STORY_WELCOME_R;
      }
      else if (buttons & BUTTON_UP) {
        Serial.println("defualt EEPROM settings");
        state = DEFAULT_EEPROM_SETTINGS_R;
      }
      break;
    case DEFAULT_EEPROM_SETTINGS_R:
      if (buttons == 0) {
        state = DEFAULT_EEPROM_SETTINGS;
      }
      break;
    case DEFAULT_EEPROM_SETTINGS: //changing initial values in ranking to "XXX: 0"
      for (int l = 20; l < 41; l += 10) {
        EEPROM.write(l, default_points);
        for (int k = 1; k < 4; k++) {
          EEPROM.write((l + k), x);
        }
      }
      lcd.setCursor(0, 0);
      lcd.print("Please reset    ");
      lcd.setCursor(0, 1);
      lcd.print("    your Arduino");
      break;
    case PRACTICE_WELCOME_R:
      mode = "practice";
      if (buttons == 0) {
        now = millis();
        state = PRACTICE_WELCOME;
      }
      break;
    case PRACTICE_WELCOME:
      if (buttons & BUTTON_RIGHT) {
        Serial.println("going to change M");
        state = CHANGE_M_R;
      }
      else if (buttons & BUTTON_SELECT) { //starting with default settings (M = 2 and N = 3) or previous user's settings
        Serial.println("straight to start");
        state = NUMBER_GENERATOR_R;
      }
      else {
        lcd.setCursor(0, 0);
        lcd.print(change_settings);
        lcd.setCursor(0, 1);
        lcd.print(to_start);
      }
      break;
    case CHANGE_M_R:
      lcd.setBacklight(WHITE);
      if (buttons == 0) {
        state = CHANGE_M;
        now = millis();
      }
      break;
    case CHANGE_M:
      if (passed(now, 750)) {
        if (buttons & BUTTON_UP) {
          //Serial.println("M up");
          state = M_UP;
        }
        else if (buttons & BUTTON_DOWN) {
          //Serial.println("M down");
          state = M_DOWN;
        }
        else if (buttons & BUTTON_LEFT) {
          Serial.println("back to practice welcome");
          state = WELCOME_R;
        }
        else if (buttons & BUTTON_RIGHT) {
          Serial.println("going to change N");
          state = CHANGE_N_R;
        }
        else if (buttons & BUTTON_SELECT) {
          Serial.println("start");
          state = NUMBER_GENERATOR_R;
        }
        else {
          lcd.setCursor (0, 0);
          lcd.print("<back  M : " + String(m));
          lcd.write(UP_AND_DOWN);
          lcd.print(" N>");
          lcd.setCursor (0, 1);
          lcd.print(to_start);
        }
      }
      else {
        lcd.setCursor (0, 0);
        lcd.print("<back  M : " + String(m));
        lcd.write(UP_AND_DOWN);
        lcd.print(" N>");
        lcd.setCursor (0, 1);
        lcd.print(to_start);
      }
      break;
    case M_UP:
      lcd.setCursor(11, 0);
      m += 1;
      delay(200);
      if (m > 4) {
        m = 2;
      }
      lcd.print(m);
      state = CHANGE_M;
      break;
    case M_DOWN:
      lcd.setCursor(11, 0);
      m -= 1;
      delay(200);
      if (m < 2) {
        m = 4;
      }
      lcd.print(m);
      state = CHANGE_M;
      break;
    case CHANGE_N_R:
      if (buttons == 0) {
        state = CHANGE_N;
        now = millis();
      }
      break;
    case CHANGE_N:
      if (passed(now, 750)) {
        if (buttons & BUTTON_UP) {
          //Serial.println("N up");
          state = N_UP;
        }
        else if (buttons & BUTTON_DOWN) {
          //Serial.println("N down");
          state = N_DOWN;
        }
        else if (buttons & BUTTON_LEFT) {
          Serial.println("back to change M");
          state = CHANGE_M_R;
        }
        else if (buttons & BUTTON_RIGHT) {
          Serial.println("going to change T");
          state = CHANGE_T_R;
        }
        else if (buttons & BUTTON_SELECT) {
          Serial.println("start");
          state = NUMBER_GENERATOR_R;
        }
        else {
          lcd.setCursor (0, 0);
          lcd.print("<M    N :" + String(n));
          lcd.write(UP_AND_DOWN);
          lcd.print("  T> ");
          lcd.print(to_start);
        }
      }
      else {
        lcd.setCursor (0, 0);
        lcd.print("<M    N :" + String(n));
        lcd.write(UP_AND_DOWN);
        lcd.print("  T> ");
        lcd.setCursor (0, 1);
        lcd.print(to_start);
      }
      break;
    case N_UP:
      lcd.setCursor(9, 0);
      n += 1;
      delay(200);
      if (n > 99) {
        n = 1;
      }
      lcd.print(n);
      state = CHANGE_N;
      break;
    case N_DOWN:
      lcd.setCursor(9, 0);
      n -= 1;
      delay(200);
      if (n == 0) {
        n = 99;
      }
      lcd.print(n);
      state = CHANGE_N;
      break;
    case CHANGE_T_R:
      if (buttons == 0) {
        state = CHANGE_T;
        now = millis();
      }
      break;
    case CHANGE_T:
      if (passed(now, 750)) {
        if (buttons & BUTTON_UP) {
          //Serial.println("T up");
          state = T_UP;
        }
        else if (buttons & BUTTON_DOWN) {
          //Serial.println("T down");
          state = T_DOWN;
        }
        else if (buttons & BUTTON_LEFT) {
          Serial.println("back to change N");
          state = CHANGE_N_R;
        }
        else if (buttons & BUTTON_RIGHT) {
          Serial.println("going to change D");
          state = CHANGE_D_R;
        }
        else if (buttons & BUTTON_SELECT) {
          Serial.println("start");
          state = NUMBER_GENERATOR_R;
        }
        else {
          lcd.setCursor (0, 0);
          lcd.print("<N  T : " + String(t));
          lcd.write(UP_AND_DOWN);
          lcd.print(" D>");
          lcd.setCursor (0, 1);
          lcd.print(to_start);
        }
      }
      else {
        lcd.setCursor (0, 0);
        lcd.print("<N  T : " + String(t));
        lcd.write(UP_AND_DOWN);
        lcd.print(" D>");
        lcd.setCursor (0, 1);
        lcd.print(to_start);
      }
      break;
    case T_UP:
      lcd.setCursor(8, 0);
      t += 0.1;
      delay(200);
      if (t > 5.0) {
        t = 0.1;
      }
      lcd.print(t);
      state = CHANGE_T;
      break;
    case T_DOWN:
      lcd.setCursor(8, 0);
      t -= 0.1;
      delay(200);
      if (t < 0.1) {
        t = 5.0;
      }
      lcd.print(t);
      state = CHANGE_T;
      break;
    case CHANGE_D_R:
      if (buttons == 0) {
        state = CHANGE_D;
        now = millis();
      }
      break;
    case CHANGE_D:
      if (passed(now, 750)) {
        if (buttons & BUTTON_UP) {
          //Serial.println("D up");
          state = D_UP;
        }
        else if (buttons & BUTTON_DOWN) {
          // Serial.println("D down");
          state = D_DOWN;
        }
        else if (buttons & BUTTON_LEFT) {
          Serial.println("back to change T");
          state = CHANGE_T_R;
        }
        else if (buttons & BUTTON_RIGHT) {
          Serial.println("back to change M");
          state = CHANGE_M_R;
        }
        else if (buttons & BUTTON_SELECT) {
          Serial.println("start");
          state = NUMBER_GENERATOR_R;
        }
        else {
          lcd.setCursor (0, 0);
          lcd.print("<T  D : " + String(d));
          lcd.write(UP_AND_DOWN);
          lcd.print(" M>");
          lcd.setCursor (0, 1);
          lcd.print(to_start);
        }
      }
      else {
        lcd.setCursor (0, 0);
        lcd.print("<T  D : " + String(d));
        lcd.write(UP_AND_DOWN);
        lcd.print(" M>");
        lcd.setCursor (0, 1);
        lcd.print(to_start);
      }
      break;
    case D_UP:
      lcd.setCursor(8, 0);
      d += 0.1;
      delay(200);
      if (d > 5.0) {
        d = 0.1;
      }
      lcd.print(d);
      state = CHANGE_D;
      break;
    case D_DOWN:
      lcd.setCursor(8, 0);
      d -= 0.1;
      delay(200);
      if (d < 0.1) {
        d = 5.0;
      }
      lcd.print(d);
      state = CHANGE_D;
      break;
    case NUMBER_GENERATOR_R:
      if (buttons == 0) {
        state = NUMBER_GENERATOR;
      }
      break;
    case NUMBER_GENERATOR:
      lcd.setBacklight(WHITE);
      del = int(1000 * d);
      time_for_input = int(1000 * t);
      sequence = "";
      lcd.clear();
      Serial.println("m : " + String(m));
      Serial.println("n : " + String(n));
      Serial.println("d : " + String(d));
      Serial.println("del : " + String(del));
      Serial.println("t : " + String(t));
      Serial.println("time_for_input : " + String(time_for_input));
      for (int c = 0; c < n; c++) { //generating numbers and displaying symbols
        lcd.clear();
        delay(100);
        int rand_num = random(1, m + 1);
        if (rand_num == 1) {
          sequence += "1";
          left_arrow();
          delay(del);
        }
        else if (rand_num == 2) {
          sequence += "2";
          up_arrow();
          delay(del);
        }
        else if (rand_num == 3) {
          sequence += "3";
          right_arrow();
          delay(del);
        }
        else {
          sequence += "4";
          down_arrow();
          delay(del);
        }
      }
      //Serial.println("Repeat this sequence: " + sequence); //TESTS: printing the sequence on Serial monitor makes life easier :)
      now = millis();
      lcd.clear();
      input_sequence = "";
      state = SEQUENCE_INPUT;
      break;
    case SEQUENCE_INPUT_R:
      if (buttons == 0) {
        lcd.clear();
        input_sequence += String(old_buttons); //adding last pressed button to sequence
        now = millis();
        state = SEQUENCE_INPUT;
      }
      break;
    case SEQUENCE_INPUT:
      if (input_sequence.length() < n) {
        if (passed(now, time_for_input)) {
          Serial.println(input_sequence);
          state = ERROR_HANDLER;
        }
        else {
          if (buttons & BUTTON_LEFT) {
            old_buttons = 1;
            lcd.clear();
            left_arrow();
            state = SEQUENCE_INPUT_R;
          }
          else if (buttons & BUTTON_UP) {
            old_buttons = 2;
            lcd.clear();
            up_arrow();
            state = SEQUENCE_INPUT_R;
          }
          else if (buttons & BUTTON_RIGHT) {
            old_buttons = 3;
            lcd.clear();
            right_arrow();
            state = SEQUENCE_INPUT_R;
          }
          else if (buttons & BUTTON_DOWN) {
            old_buttons = 4;
            lcd.clear();
            down_arrow();
            state = SEQUENCE_INPUT_R;
          }
          else {
            lcd.setCursor(0, 0);
            lcd.print("ENTER ANSWER");
            old_buttons = 0;
          }
        }
      }
      else if (input_sequence.length() == n) {
        //Serial.println(input_sequence);
        state = CHECK;
      }
      break;
    case CHECK:
      lcd.clear();
      if (input_sequence == sequence) {
        Serial.println("correct");
        state = SUCCESS;
      }
      else {
        Serial.println("error");
        state = ERROR_HANDLER;
      }
      break;
    case SUCCESS:
      lcd.setBacklight(TEAL);
      if (mode == "practice") { //in practice mode user can either go back to menu or "play again" with a new sequence of lenght N
        lcd.setCursor(0, 0);
        lcd.print("<menu           ");
        lcd.setCursor(0, 1);
        lcd.print("    play again >");
        if (buttons & BUTTON_LEFT) {
          state = WELCOME_R;
        }
        else if (buttons & BUTTON_RIGHT) {
          state = NUMBER_GENERATOR_R;
        }
      }
      else { //story mode
        points = m * n * points_mult; //points calculating
        //Serial.println(points);
        if (points == 144) {
          state = WINNER;
        }
        if (points > min_ranking) { //if user's points value is higher than minimal points value in ranking he can choose to either save his
          //result or play further
          lcd.setCursor(0, 0);
          lcd.print("<back to game   ");
          lcd.setCursor(0, 1);
          lcd.print(" save my result>");
          if (buttons & BUTTON_RIGHT) {
            state = ASK_FOR_INITIALS;
          }
          else if (buttons & BUTTON_LEFT) {
            d -= 0.1; //time for display in the next round will be shorter
            n += 1;
            if (n > 12) { //after correct input for sequence of lenght N = 12, M id changed to M+1 and N to 6
              n = 6;
              m += 1;
              state = NUMBER_GENERATOR_R;
            }
            else {
              state = NUMBER_GENERATOR_R;
            }
          }
        }
        else {
          delay(1000);
          d -= 0.1;
          n += 1;
          if (n > 12) {
            n = 6;
            m += 1;
            state = NUMBER_GENERATOR_R;
          }
          else {
            state = NUMBER_GENERATOR_R;
          }
        }
      }
      break;
    case ERROR_HANDLER:
      lcd.setBacklight(RED);
      if (mode == "practice") {
        lcd.setCursor (0, 0);
        lcd.print("<menu    ERROR.  ");
        lcd.setCursor(0, 1);
        lcd.print("    play again >");
        if (buttons & BUTTON_RIGHT) {
          state = NUMBER_GENERATOR_R;
        }
        if (buttons & BUTTON_LEFT) {
          state = WELCOME_R;
        }
      }
      else { //story mode
        lcd.setCursor (0, 0);
        lcd.print("ERROR.          ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        delay(1500);
        lcd.setBacklight(WHITE);
        state = WELCOME; //in story mode user goes back to menu after an error
      }
      break;
    case WINNER:
      lcd.setBacklight(VIOLET);
      lcd.setCursor(0, 0);
      lcd.print("YOU WON!!!      ");
      lcd.setCursor(0, 1);
      lcd.print("  save initials>"); //user will be asked to save initials
      if (buttons & BUTTON_RIGHT) {
        state = ASK_FOR_INITIALS;
      }
      break;
    case STORY_WELCOME_R:
      mode = "story";
      lcd.setCursor(0, 0);
      lcd.print("LEFT          ");
      if (buttons == 0) {
        state = STORY_WELCOME;
      }
      break;
    case STORY_WELCOME:
      lcd.setCursor(0, 0);
      lcd.print("<see ranking    ");
      lcd.setCursor(0, 1);
      lcd.print("  choose level >");
      if (buttons & BUTTON_LEFT) {
        state = RANKING_R;
      }
      else if (buttons & BUTTON_RIGHT) {
        state = CHOOSE_LEVEL_R;
      }
      break;
    case CHOOSE_LEVEL_R:
      if (buttons == 0) {
        state = CHOOSE_LEVEL;
      }
      break;
    case CHOOSE_LEVEL:
      if (buttons & BUTTON_LEFT) {
        points_mult = 1; //settings for easy mode
        t = 3.0;
        d = 2.5;
        m = 2;
        n = 5;
        state = NUMBER_GENERATOR_R;
      }
      else if (buttons & BUTTON_UP) {
        points_mult = 2; //settings for medium mode
        t = 2.0;
        d = 1.8;
        m = 3;
        n = 6;
        state = NUMBER_GENERATOR_R;
      }
      //comment out the part below for TESTS
      else if (buttons & BUTTON_RIGHT) {
        points_mult = 3; //settings for hard mode
        t = 1.5;
        d = 1.2;
        m = 4;
        n = 7;
        state = NUMBER_GENERATOR_R;
      }
      //end of part that should be commented out
      /* else if (buttons & BUTTON_RIGHT) { //TESTS: so you do not need to actually play
        points_mult = 3; //settings for hard mode
        t = 4.5;
        d = 1.2;
        m = 4;
        n = 12;
        state = NUMBER_GENERATOR_R;
        } */
      else {
        if (passed(now, TIME_FOR_DISPLAY)) {
          now += TIME_FOR_DISPLAY;
          e++;
          if (e == ((choose_level.length()) + 3)) {
            e = 1;
          }
          lcd.setCursor(0, 0);
          lcd.print("Choose level:   ");
          lcd.setCursor(0, 1);
          lcd.print(choose_level2.substring(e - 1, 15 + e));
        }
      }
      break;
    case ASK_FOR_INITIALS:
      lcd.setCursor(0, 0);
      lcd.print(" Use serial     ");
      lcd.setCursor(0, 1);
      lcd.print("monitor         ");
      Serial.println("ENTER YOUR INITIALS (3 letters) and press ENTER:");
      state = GET_INITIALS;
      break;
    case GET_INITIALS:
      if (Serial.available()) {
        initials = Serial.readString();
        Serial.println(initials + "please reset your Arduino");
        state = SAVE_INITIALS;
      }
      break;
    case SAVE_INITIALS:
      EEPROM.write(i, points); //saving data in the place of previous ranking's lowest score
      if ((initials[0] >= 65 && initials[0] <= 90) || (initials[0] >= 97 && initials[0] <= 122)) {
        EEPROM.write((i + 1), initials[0]);
      }
      else {
        EEPROM.write((i + 1), x);
      }
      if ((initials[1] >= 65 && initials[1] <= 90) || (initials[1] >= 97 && initials[1] <= 122)) {
        EEPROM.write((i + 2), initials[1]);
      }
      else {
        EEPROM.write((i + 2), x);
      }
      if ((initials[2] >= 65 && initials[2] <= 90) || (initials[2] >= 97 && initials[2] <= 122)) {
        EEPROM.write((i + 3), initials[2]);
      }
      else {
        EEPROM.write((i + 3), x);
      }
      break;
    case RANKING_R:
      if (buttons == 0) {
        state = RANKING;
      }
      break;
    case RANKING:
      lcd.setCursor(0, 0);
      lcd.print("RANKING:   back>");
      //creating a string out of chars read from EEPROM
      rank1[0] = EEPROM.read(21);
      rank1[1] = EEPROM.read(22);
      rank1[2] = EEPROM.read(23);
      rank1[3] = ':';
      rank1[4] = '\0';
      int rank1_points = EEPROM.read(20);
      rank2[0] = EEPROM.read(31);
      rank2[1] = EEPROM.read(32);
      rank2[2] = EEPROM.read(33);
      rank2[3] = ':';
      rank2[4] = '\0';
      int rank2_points = EEPROM.read(30);
      rank3[0] = EEPROM.read(41);
      rank3[1] = EEPROM.read(42);
      rank3[2] = EEPROM.read(42);
      rank3[3] = ':';
      rank3[4] = '\0';
      int rank3_points = EEPROM.read(40);
      ranking[0] = "  " + String(rank1) + String(rank1_points) + "          ";
      ranking[1] = "  " + String(rank2) + String(rank2_points) + "          ";
      ranking[2] = "  " + String(rank3) + String(rank3_points) + "          ";
      lcd.setCursor(0, 1);
      lcd.write(UP_AND_DOWN);
      lcd.setCursor(1, 1);
      lcd.print(ranking[g]);
      //scrolling ranking by using UP and DOWN buttons
      if (buttons & BUTTON_DOWN) {
        g++;
        if (g > 2) {
          g = 0;
        }
      }
      else if (buttons & BUTTON_UP) {
        g--;
        if (g < 0) {
          g = 2;
        }
      }
      else if (buttons & BUTTON_RIGHT) {
        state = STORY_WELCOME_R;
      }
      break;
  }
}
