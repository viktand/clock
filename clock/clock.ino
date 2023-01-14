#include <SoftwareSerial.h>

// pins
int led_a = 12;
int led_b = 11;
int led_c = 10;
int led_d = 9;
int led_e = 8;
int led_f = 7;
int led_g = 6;
int seg_1 = 5;
int seg_2 = 4;
int seg_3 = 3;
int seg_4 = 19;
int dp = 14;
int btn = 15;
int snd = 16;
SoftwareSerial Serial1(0, 1); // rx, tx

// time
int tic =0; // time counter
int flashtic = 0; // flash counter
long minutes = 45;
long hours = 21;
long seconds = 0;
int lastSeconds = 0;
bool dots = false; // dots on/off
int editFlash = 0; // счетчик мигания при редактировании
bool edtFl = true; // надо ли погасить разряд, чтобы в целом оно мигало

// correct
int count = 0;
bool correct = true;

// alarm
long minutesA = 30;
long hoursA = 6;
long secondsA = 0;
bool doFunction = false; // флаг включения будильника
int sndTime = 0; // время запуска очередного пилика (счетчик)
bool alarmFlag = false; // флаг того, что будильник пиликает прямо сейчас
bool alarmOn = false; // флаг разрешения пиликать

// button press 
int btnPress; // time
int lastState = 0;
int flash = 0; // редактируемый разряд. 0 - нет такого
bool wait = false; // wait press time
int mode = 0;// 0 - часы, 1 - будильник, 2 - термометр
bool flag = false; // флаг того, что было долгое нажатие

// Температура
long tem;

void setup() {
  pinMode(led_a, OUTPUT);
  pinMode(led_b, OUTPUT);
  pinMode(led_c, OUTPUT);
  pinMode(led_d, OUTPUT);
  pinMode(led_e, OUTPUT);
  pinMode(led_f, OUTPUT);
  pinMode(led_g, OUTPUT);
  pinMode(seg_1, OUTPUT);
  pinMode(seg_2, OUTPUT);
  pinMode(seg_3, OUTPUT);
  pinMode(seg_4, OUTPUT);
  pinMode(dp, OUTPUT);
  pinMode(btn, INPUT);

  digitalWrite(led_a, HIGH);  
  digitalWrite(led_b, HIGH);  
  digitalWrite(led_c, HIGH);  
  digitalWrite(led_d, HIGH);  
  digitalWrite(led_e, HIGH);  
  digitalWrite(led_f, HIGH);
  digitalWrite(led_g, HIGH);    
  digitalWrite(seg_1, HIGH);  
  digitalWrite(seg_2, HIGH);  
  digitalWrite(seg_3, HIGH);  
  digitalWrite(seg_4, HIGH);  
  digitalWrite(dp, HIGH); 
  btnPress = millis();

  Serial.begin (9600);
  Serial.println("Start");
  Serial1.begin (9600);
}

void loop() {      
  int t = millis();
  
  // мерцание при редактировании
  if(t - editFlash > 500){
    editFlash = t;
    edtFl = ! edtFl;
  }
  // time count
  if(t - tic > 1000){
    seconds++;
    if(doFunction){
      checkAlarm(t); // Проверка необходимости подать сигнал будильника.
    }
    if(seconds == 60){
      minutes++;
      seconds = 0;
      if (minutes == 60){
        hours++;
        minutes = 0;
        {
          if(hours == 24){
            hours = 0;
          }
        }
      }
    }
    tic = t;
  }
  
  // dots flash count
  if(t - flashtic > 500){
    dots = ! dots;
    flashtic = t;
  }
  
  //button count
  buttonCase(t);

  // auto reset setting mode
  if((flash > 0 || mode > 0) && t - btnPress > 10000){
    flash = 0;
    mode = 0;
    btnPress = t;
  }
  
  show();
  
  if (Serial1.available() > 0) {
    String data = Serial1.readString();
    checkInput(data);
  }
}

void checkInput(String data){
  Serial.println("input: " + data);
  String key = data.substring(0, 4);
  if(key.equals("time")){ // received time
    String t = data.substring(5);
    long allsec = t.toInt(); 
    hours = allsec / 3600;
    Serial.println("h: " + String(hours));
    minutes = (allsec - (hours * 3600)) / 60;
    Serial.println("m: " + String(minutes));
    seconds = (allsec - (hours * 3600) - (minutes * 60));
    Serial.println("s: " + String(seconds));
    int i = data.indexOf("temp");
    if(i > 0){ // received temper
      String t = data.substring(i + 5);
      tem = t.toInt();         
    }
    return;
  }
  if(key.equals("alrm")){ // received time alrm
    String t = data.substring(10);
    Serial.println("t: " + t);
    long allsec = t.toInt(); 
    Serial.println("allsec: " + String(allsec));
    hoursA = allsec / 3600;
    Serial.println("hA: " + String(hoursA));
    minutesA = (allsec - (hoursA * 3600)) / 60;
    Serial.println("mA: " + String(minutesA));
    secondsA = 0; 
    return;
  }
  if(key.equals("alon")){ // alarm on
    doFunction = true;
    alarmOn = doFunction;
    return;
  }
  if(key.equals("alof")){ // alarm off
    doFunction = false;
    alarmOn = doFunction;
    return;
  }
}

void checkAlarm(int moment){
  if(hours == hoursA && minutes == minutesA){  
    Serial.println("alarm!");
    if(flash == 0 && alarmOn){
      int s = moment - sndTime;
      Serial.println("alarm do: s=" + String(s));
      if(s > 1000 || s < 0){
        Serial.println("alarm on");
        tone(snd, 800, 100);
        sndTime = moment;
        alarmFlag = true;
      }
    }
  }else
  {
     alarmOn = true;
  }
}

void countUp(){
  int ml = 0;
  int hl = 0;
  switch(flash){
    case 1:
      ml = minutes % 10;
      minutes -= ml;
      ml++;
      if(ml == 10){
        ml = 0;
      }
      minutes += ml;
      break; 
    case 2:
      ml = minutes % 10;
      minutes += 10;
      if(minutes > 60 ){
        minutes = ml;
      }    
      break; 
    case 3:
      hl = hours % 10;
      hours -= hl;
      hl++;
      if(hours < 20 && hl == 10){
        hl = 0;
      }
      if(hours == 20 && hl > 4){
        hl = 0;
      }
      hours += hl;
      break; 
    case 4:
      hl = hours % 10;
      hours += 10;
      if(hours > 23 ){
        hours = hl;
      }    
      break; 
  }
}

void countUpAlarm(){
  int ml = 0;
  int hl = 0;
  switch(flash){
    case 1:
      doFunction = ! doFunction;
      alarmOn = doFunction;
      break;   
    case 2:
      ml = minutesA % 10;
      minutesA -= ml;
      ml++;
      if(ml == 10){
        ml = 0;
      }
      minutesA += ml;
      break; 
    case 3:
      ml = minutesA % 10;
      minutesA += 10;
      if(minutesA > 59 ){
        minutesA = ml;
      }    
      break; 
    case 4:
      hl = hoursA % 10;
      hoursA -= hl;
      hl++;
      if(hoursA < 20 && hl == 10){
        hl = 0;
      }
      if(hoursA == 20 && hl > 4){
        hl = 0;
      }
      hoursA += hl;
      break; 
    case 5:
      hl = hoursA % 10;
      hoursA += 10;
      if(hoursA > 23 ){
        hoursA = hl;
      }    
      break;    
  }
}

void show(){
  if(mode == 1){
    showAlarm();
    return;
  }
  if(mode == 2){
    showTem();
    return;
  }
  int ml = minutes % 10;
  int mh = (minutes - ml) / 10;
  int hl = hours % 10;
  int hh = (hours - hl) / 10;
  showDots();
  if(flash != 1 || edtFl) displey(seg_4, ml);
  if(flash != 2 || edtFl) displey(seg_3, mh);
  if(flash != 3 || edtFl) displey(seg_2, hl);
  if(flash != 4 || edtFl) displey(seg_1, hh);  
}

void showTem(){
  int t = abs(tem);
  int ml = t % 10;
  int mh = (t - ml) / 10;
  digitalWrite(dp, LOW);
  displey(seg_4, ml);
  displey(seg_3, mh);
  displey(seg_1, 12);
  if(tem < 0){
    displey(seg_2, 16);
  }
}

void showAlarm(){
  int ml = minutesA % 10;
  int mh = (minutesA - ml) / 10;
  int hl = hoursA % 10;
  int hh = (hoursA - hl) / 10;
  if(!doFunction && (flash == 0 || (edtFl && flash == 1))){
    digitalWrite(dp, HIGH);
  }else{
    digitalWrite(dp, LOW);
  }
  if(flash != 2 || edtFl) displey(seg_4, ml);
  if(doFunction && (flash == 0 || (edtFl && flash == 1))){
    digitalWrite(dp, HIGH);
  }else{
    digitalWrite(dp, LOW);
  }
  if(flash != 3 || edtFl) displey(seg_3, mh);
  if(flash != 4 || edtFl) displey(seg_2, hl);
  if(flash != 5 || edtFl) displey(seg_1, hh);  
}

void showDots(){
  if(dots){
    digitalWrite(dp, HIGH);
    return;
  }
  digitalWrite(dp, LOW);
}

void displey(int seg, int dig){  
  digitalWrite(seg, LOW); 
  switch(dig){
    case 0:
      digitalWrite(led_a, HIGH); 
      digitalWrite(led_b, HIGH); 
      digitalWrite(led_c, HIGH); 
      digitalWrite(led_d, HIGH); 
      digitalWrite(led_e, HIGH); 
      digitalWrite(led_f, HIGH); 
      break;
    case 1:
      digitalWrite(led_b, HIGH); 
      digitalWrite(led_c, HIGH); 
      break;
    case 2:
      digitalWrite(led_a, HIGH); 
      digitalWrite(led_b, HIGH); 
      digitalWrite(led_g, HIGH); 
      digitalWrite(led_e, HIGH); 
      digitalWrite(led_d, HIGH); 
      break;
    case 3:
      digitalWrite(led_a, HIGH);   
      digitalWrite(led_b, HIGH); 
      digitalWrite(led_g, HIGH); 
      digitalWrite(led_c, HIGH); 
      digitalWrite(led_d, HIGH); 
      break;
    case 4:
      digitalWrite(led_f, HIGH); 
      digitalWrite(led_b, HIGH); 
      digitalWrite(led_g, HIGH); 
      digitalWrite(led_c, HIGH); 
      break;
    case 5:
      digitalWrite(led_a, HIGH); 
      digitalWrite(led_f, HIGH); 
      digitalWrite(led_g, HIGH); 
      digitalWrite(led_c, HIGH); 
      digitalWrite(led_d, HIGH); 
      break;
    case 6:
      digitalWrite(led_a, HIGH); 
      digitalWrite(led_f, HIGH); 
      digitalWrite(led_g, HIGH); 
      digitalWrite(led_c, HIGH); 
      digitalWrite(led_d, HIGH); 
      digitalWrite(led_e, HIGH); 
      break;
    case 7:
      digitalWrite(led_a, HIGH);   
      digitalWrite(led_b, HIGH); 
      digitalWrite(led_c, HIGH); 
      break;
    case 8:
      digitalWrite(led_a, HIGH); 
      digitalWrite(led_b, HIGH); 
      digitalWrite(led_f, HIGH); 
      digitalWrite(led_g, HIGH); 
      digitalWrite(led_c, HIGH); 
      digitalWrite(led_d, HIGH); 
      digitalWrite(led_e, HIGH); 
      break;
    case 9:
      digitalWrite(led_a, HIGH); 
      digitalWrite(led_b, HIGH); 
      digitalWrite(led_f, HIGH); 
      digitalWrite(led_g, HIGH); 
      digitalWrite(led_c, HIGH); 
      digitalWrite(led_d, HIGH); 
      break;
    case 12:
      digitalWrite(led_a, HIGH);       
      digitalWrite(led_d, HIGH); 
      digitalWrite(led_e, HIGH); 
      digitalWrite(led_f, HIGH);
      break;
    case 16:          
      digitalWrite(led_g, HIGH);      
      break;
  }
  allOff();  
}

void allOff(){
  digitalWrite(led_a, LOW);  
  digitalWrite(led_b, LOW);  
  digitalWrite(led_c, LOW);  
  digitalWrite(led_d, LOW);  
  digitalWrite(led_e, LOW);  
  digitalWrite(led_f, LOW);
  digitalWrite(led_g, LOW);    
  digitalWrite(seg_1, HIGH);  
  digitalWrite(seg_2, HIGH);  
  digitalWrite(seg_3, HIGH);  
  digitalWrite(seg_4, HIGH);   
}

void buttonCase(int moment){
  int buttonState  = 1 - digitalRead(btn);
  if(buttonState == 1){
    if(alarmFlag){
      alarmOn = false;
    }
    if(lastState == 1){
      if(moment - btnPress > 2000){ // долгое нажатие
         btnPress = moment; 
         flag = true;       
         if(flash > 0){
           flash++;
           if((flash == 5 && mode == 0) || (flash == 6 && mode == 1)){
             flash = 0;
           }
         }else{
           flash = 1;
         }
      }
    }else{
      btnPress = moment;
    }
  }else{
    if(lastState == 1){
      btnPress = moment;    
      if(moment - btnPress < 1000){
        if(flag){
          flag = false;
          lastState = buttonState;
          return;
        }        
        if(flash > 0){
          if(mode == 1){
            countUpAlarm();
          }
          if(mode == 0){
            countUp();
          }          
        }else{         
          if(++
          mode > 2){
            mode = 0;                  
          }
        }
      }
    }
  }
  lastState = buttonState;
}
