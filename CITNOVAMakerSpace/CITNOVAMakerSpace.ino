#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>
#include <Keypad.h>

// ------------------------------------------
// ------------- CONEXIONES -----------------
//
// Teclado 4x4 Filas:  4, 5, 6, 7
// Teclado 4x4 Columnas:  8, 9, 10, 11
// LCD con I2C: SCL = 21, SDA = 20
// Serial RDM6300: Serial1
// MAX485: Serial2

#define DENTRO 1
#define FUERA 2

const int MOT1_2 = 2;
const int MOT1_3 = 3;
const int MOT2_4 = 4;
const int MOT2_5 = 5;
const int MOT1_LIM_I = 8;
const int MOT1_LIM_S = 9;
const int MOT2_LIM_I = 10;
const int MOT2_LIM_S = 11;



// RS, E, D4, D5, D6, D7
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
LiquidCrystal lcd2(A8, A9, A10, A11, A12, A13);


const byte ROWS = 4;         // 4 filas
const byte COLS = 4;         // 4 columnas
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Teclado interior
byte rowPins[ROWS] = {22, 23, 24, 25};
byte colPins[COLS] = {26, 27, 28, 29};
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Teclado exterior
byte rowPins2[ROWS] = {32, 33, 34, 35};
byte colPins2[COLS] = {36, 37, 38, 39};
Keypad kpd2 = Keypad(makeKeymap(keys), rowPins2, colPins2, ROWS, COLS);


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial.println("--------------------- ");
  Serial.println("   Iniciando CITNOVA  ");
  Serial.println("--------------------- ");
  initLcd();
  initLcd2();

  // Motor 1
  pinMode(MOT1_2, OUTPUT);
  pinMode(MOT1_3, OUTPUT);
  pinMode(MOT2_4, OUTPUT);
  pinMode(MOT2_5, OUTPUT);
  pinMode(MOT1_LIM_I, INPUT);
  pinMode(MOT1_LIM_S, INPUT);
  pinMode(MOT2_LIM_I, INPUT);
  pinMode(MOT2_LIM_S, INPUT);

  
  digitalWrite(MOT1_2, 1);
  digitalWrite(MOT1_3, 1);

  //  expand(1);
    contract();
}

char keyInput[6];
String strKeyInput;
int loc = 0;


/**
 * Contrae los motores al mismo tiempo, parando por separado
 * cada motor.
 */
void contract() {
  int count = 0;
  if (digitalRead(MOT1_LIM_S) == 0) {
    int x = digitalRead(MOT1_LIM_I);
    int y = digitalRead(MOT2_LIM_I);
    while (true){
      x = digitalRead(MOT1_LIM_I);
      y = digitalRead(MOT2_LIM_I);
      if(x == 0){
        digitalWrite(MOT1_2, 1);
        digitalWrite(MOT1_3, 1);
        count++;
      }
      if(y == 0){
        digitalWrite(MOT2_4, 1);
        digitalWrite(MOT2_5, 1);
        count++;
      }
      if(count == 2)
        break;
      digitalWrite(MOT1_2, 1);
      digitalWrite(MOT1_3, 0);
      digitalWrite(MOT2_4, 1);
      digitalWrite(MOT2_5, 0);
    }
  }
}


void expand(int motor) {
  if (motor == 1) {
//    Serial.println(digitalRead(MOT1_LIM_I));
    if (digitalRead(MOT1_LIM_I) == 1) {
      int x = digitalRead(MOT1_LIM_S);
      while (x != 1) {
        x = digitalRead(MOT1_LIM_S);
//        Serial.println(x);
        digitalWrite(MOT1_2, 0);
        digitalWrite(MOT1_3, 1);
      }
      digitalWrite(MOT1_2, 1);
      digitalWrite(MOT1_3, 1);
    }
  }
}

void loop() {
  printToLcd("Captura tu clave",
             "                ");
  printToLcd2("Captura tu clave",
              "                ");
              
  readKeyStringUntil(keyInput, 6, '#', 0, 1);         // Véase metodo
  Serial.print("keyInput: "); Serial.println(keyInput);
  Serial.print("Ubicacion: "); Serial.println(loc);
  Serial.println();

  strKeyInput = String(keyInput);
  Serial.print("strKeyInput: "); Serial.println(strKeyInput);

  

  if (strKeyInput.equals("13250")) {
    Serial.println("Abrir puerta");

    if(loc == FUERA){
      printToLcd2("OK Administrador",
                  "Abriendo        ");
      printToLcd("  Espere un     ",
                 "  momento       ");
    }

    if(loc == DENTRO){
      printToLcd("OK Administrador",
                  "Abriendo        ");
      printToLcd2("  Espere un     ",
                  "  momento       ");
    }
    
//    expand();
//    delay(5000);
//    contract();

    printToLcd("Captura tu clave",
               "                ");
    printToLcd2("Captura tu clave",
                "                ");
  }

  for (int i = 0; i <= 6; i++)
    keyInput[i] = ' ';
  
  strKeyInput = "" ;
  asm volatile ("  jmp 0"); 
  
}





/**
   Crea un ciclo hasta presionar la tecla #, los caracteres presionados
   se almacenan en el array provisto.
     loc. Si se trata del teclado de entrada o salida
 *   *pStr. Array de tipo char donde almacenar los caracteres.
     len. Longitud del array *pStr.
     endChar. Caracter que indica terminacion de captura.
     col. Columna donde comenzar a mostrar los caracteres.
     row. Fila donde comenzar a mostrar los caracteres
*/
void readKeyStringUntil(char *pStr, int len, char endChar, int col, int row) {
  char k = 0;
  char k2 = 0;
  int j = 0;
  int colCount = 0;
  boolean loopOk = true;
  char maskedKeyIn[len + 1];
  loc = 0;
  
  for (int i = 0; i < len; i++) {
    maskedKeyIn[i] = ' ';
  }
  maskedKeyIn[len] = '\0';

  while (loopOk) {
    k = kpd.getKey();      // Interior
    delay(30);
    if (k) {
      while (loopOk) {
        printToLcd2("  Espere un     ",
                    "  momento       ");
        Serial.print(k);
        if (k == endChar) {
          loc = DENTRO;
          loopOk = false;
          break;
        }
        if (k) {
          if (j < len - 1) {        // Para eviar simbolo raro en LCD
            pStr[j] = k;
            maskedKeyIn[j] = '*';
            //            Serial.print("j: "); Serial.print(j);
            //            Serial.print("pStr: "); Serial.println(pStr);
            lcd.setCursor(col, row);
            lcd.print(maskedKeyIn);
            colCount++;
            j++;
          }
        }
        k = '\0';
        k = kpd.getKey();
      }
    }
    k2 = kpd2.getKey();      // Exterior
    delay(30);
    if (k2) {
      while (loopOk) {
        printToLcd("  Espere un     ",
                   "  momento       ");
        Serial.print(k2);
        if (k2 == endChar) {
          loc = FUERA;
          loopOk = false;
          break;
        }
        if (k2) {
          if (j < len - 1) {        // Para eviar simbolo raro en LCD
            pStr[j] = k2;
            maskedKeyIn[j] = '*';
            //            Serial.print("j2: "); Serial.print(j);
            //            Serial.print("pStr2: "); Serial.println(pStr);
            lcd2.setCursor(col, row);
            lcd2.print(maskedKeyIn);
            colCount++;
            j++;
          }
        }
        k2 = '\0';
        k2 = kpd2.getKey();
      }
    }
  }
}


void initLcd() {
  lcd.begin(16, 2);
  printToLcd("Captura tu clave",
             "                ");
}

void initLcd2() {
  lcd2.begin(16, 2);
  printToLcd2("Captura tu clave",
              "                ");
}






/**
   Imprime los strings en cada fila de la pantalla LCD de 20x4
*/
void printToLcd(char row0[16], char row1[16]) {
  lcd.setCursor(0, 0);
  lcd.print(row0);

  lcd.setCursor(0, 1);
  lcd.print(row1);
}

void printToLcd2(char row0[16], char row1[16]) {
  lcd2.setCursor(0, 0);
  lcd2.print(row0);

  lcd2.setCursor(0, 1);
  lcd2.print(row1);
}


