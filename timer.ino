#define dataPin 2 //data
#define clockPin 4  // пин подключен к входу SH_CP
#define latchPin 3  // пин подключен к входу ST_CP
#define startBtn 5  // кнопка старт/стоп
#define resetBtn 6  // кнопка сброса таймера

unsigned long timing; // Переменная для хранения точки отсчета
unsigned long timingbtn; // Переменная для хранения точки отсчета
uint32_t timeTimer;
int minuts, minutsDec, sec, secDec, state;
int timerSecond = 1000; //время секунды для таймера, указывается в мс
bool blinked;

void setup() {
    pinMode(startBtn, INPUT_PULLUP);
    pinMode(resetBtn, INPUT_PULLUP);
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(latchPin, OUTPUT);
    digitalWrite(latchPin, HIGH);
    minuts = 0;
    minutsDec = 0;
    sec = 0;
    secDec = 0;
    state = 0;
    blinked = false;
    timerZero();
    Serial.begin(9600);
}

byte bt = 0;
int num[] = {
 //ABCDEFGH
  B00000011, //Цифра 0       # AAA #
  B10011111, //Цифра 1       F  #  B
  B00100101, //Цифра 2       # GGG #
  B00001101, //Цифра 3       E  #  C
  B10011001, //Цифра 4       # DDD #  H
  B01001001, //Цифра 5       
  B01000001, //Цифра 6
  B00011111, //Цифра 7
  B00000001, //Цифра 8
  B00001001  //Цифра 9
};
int seg[] = {
  B00010000,  // . . . !
  B00100000,  // . . ! . 
  B01000000,  // . ! . .   
  B10000000   // ! . . .
};

int timerZero(){ // вывод нулей
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, B11110000); // какие индикаторы должны гореть
  shiftOut(dataPin, clockPin, LSBFIRST, num[0]);
  digitalWrite(latchPin, HIGH);
}

int showTime(int minDec,int minut, int secDec, int sec){
  digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, B00010000); // какой индикатор должен гореть
    shiftOut(dataPin, clockPin, LSBFIRST, num[sec]);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, B00100000); // какой индикатор должен гореть
    shiftOut(dataPin, clockPin, LSBFIRST, num[secDec]);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, B01000000); // какой индикатор должен гореть
    //Serial.println(blinked);
    if (blinked){             // индикатор с двоеточием.переменная нужна для моргания 
        bt = num[minut];
        bt &= ~(1 << 0);      // замена последнего бита (что бы зажечь точки)
      } else {
        bt = num[minut];
      }
    shiftOut(dataPin, clockPin, LSBFIRST, bt);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, B10000000); // какой индикатор должен гореть
    shiftOut(dataPin, clockPin, LSBFIRST, num[minDec]);
  digitalWrite(latchPin, HIGH);
}
void timer() {
  if (millis() - timeTimer >= timerSecond) {   // ищем разницу (размер таймера)
        sec++;
        blinked = !blinked;
        if (sec > 9){
          secDec++;
          sec = 0;
          
        }
        if (secDec > 5){
          minuts++;
          secDec = 0;
        }
        if (minutsDec > 9){
          minutsDec = 0;
        }
        if (minuts > 9){
          minutsDec++;
          minuts = 0;
        }
        timeTimer = millis();
      }
      showTime(minutsDec, minuts, secDec, sec);   //что вывести на экран     
      //Serial.println(String(minutsDec) + String(minuts) + " : " + String(secDec) + String(sec));
}
void resettimer() {     // сброс всех переменных в ноль
        minuts = 0;     
        minutsDec = 0;
        sec = 0;
        secDec = 0;
        timerZero();    //вывод на модуль 0000
}
void loop() {
      bool start = digitalRead(startBtn);
      bool reset = digitalRead(resetBtn);
      
      if (!start & millis() - timingbtn > 500) { // кнопка старт
        //Serial.println("START");
        state = 1;
        timingbtn = millis();
        return;
      }
      if (!reset & millis() - timingbtn > 500){ // кнопка стоп
        //Serial.println("RESET");
        if (state == 1) {
          state = 2;
        } else state = 0;
        timingbtn = millis();
        return;
      }

      //  Обработка состояний индикатора. 1 - таймер, 2 - пауза, 3+ - сброс в ноль
      
      if (state == 1){
        timer();
      } else if (state == 2) {
        if (millis() - timeTimer >= timerSecond) {   // ищем разницу (размер таймера)
        blinked = !blinked;
        timeTimer = millis();
        }
        showTime(minutsDec, minuts, secDec, sec);
      } else resettimer();

      
}
