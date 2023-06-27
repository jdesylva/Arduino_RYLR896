// Inclure la bibliothèque utilisée pour contrôler l'afficheur à cristaux liquide :
#include <LiquidCrystal.h>  // Ref: https://www.arduino.cc/reference/en/libraries/liquidcrystal/

// Inclure la bibliothèque utilisée pour communiquer avec le module LoRa
#include <SoftwareSerial.h>

// Initialiser la constante identifiant le port utilisé pour la remise à zéro du module LoRa RLYR896
#define RESET 4

// Initialiser le lien série de LoRa sur les ports 2 & 3
SoftwareSerial lora(2, 3);

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
// On utilise le connecteur J-21 (Port B) du circuit PCB-017
const int rs = 12, en = 13, d4 = 8, d5 = 9, d6 = 10, d7 = 11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void lcd_Effacer();
void lcd_EcrireChaine(char ligne, char* str);

void RYLR890_WriteString(const char* strBuffer);
void RYLR890_Sync();
void RYLR890_Reset();
const char* RYLR890_Stat(unsigned char);
void RYLR890_Configure_L1(unsigned char ucSF, unsigned char ucBW, unsigned char ucCR, unsigned char ucPR);

unsigned int quantite_recue = 0;

void setup() {

  // Initialiser le port série de débogage à 115200 baud
  Serial.begin(115200);
  // Initialiser le port série du RYLR890 à 9600 baud
  lora.begin(9600);

  // Configurer la broche DIO4 en sortie (reset)
  pinMode(RESET, OUTPUT);

  // Initialiser le module LCD's avec le nombre requis de colonnes et de rangées :
  lcd.begin(16, 2);

  //- Attendre 500ms afin de permettre à l’afficher de s’initialiser.
  delay(500);
  //- Effacer l'afficheur
  lcd.clear();
  //- Positionner le curseur de l’afficheur sur la ligne 1, colonne 1.
  lcd.setCursor(0, 0);
  //- Écrire la chaîne « Bienvenue dans le » sur la première ligne.
  lcd.print("Bienvenue dans");
  //- Positionner le curseur de l’afficheur sur la ligne 2, colonne 1.
  lcd.setCursor(0, 1);
  //- Écrire la chaîne « cours 243-Y55 » sur la seconde ligne.}
  lcd.print("le cours 243-Y55");
  delay(1000);
  // - Effacer l'afficheur
  lcd_Effacer();

  // Faire la remise à zéro du module RYLR890
  // RYLR890_Reset();
  RYLR890_Sync();

  delay(2000);

  lcd_EcrireChaine(0, "Recepteur LoRa");
  lcd_EcrireChaine(1, "20230626");

  delay(2000);

  RYLR890_Configure_L1(7, 7, 1, 4);
  lcd_EcrireChaine(0, RYLR890_Stat(0));
  lcd_EcrireChaine(1, RYLR890_Stat(1));

  //delay(5000);



}

void loop() {
  //
  char strBuffer[100];

  while (0) {
    strBuffer[0] = 0;
    RYLR890_ReadString(strBuffer);
    // Si on a reçu des données...
    if (0 != strBuffer[0]) {
      // Incrémenter le nombre de chaines recues
      quantite_recue++;
      //- Effacer l'afficheur
      lcd.clear();
      //- Positionner le curseur de l’afficheur sur la ligne 1, colonne 1.
      lcd.setCursor(0, 0);
      //- Écrire la chaîne le nombre de chaînes sur la première ligne.
      lcd.print(quantite_recue);
      //- Positionner le curseur de l’afficheur sur la ligne 2, colonne 1.
      lcd.setCursor(0, 1);
      //- Écrire la chaîne reçue sur la seconde ligne.}
      lcd.print(strBuffer);
    }
  }
}

void lcd_Effacer() {
  //- Effacer l'afficheur
  lcd.clear();
}

void lcd_EcrireChaine(char ligne, const char* str) {
  //- Positionner le curseur de l’afficheur sur la colonne 1.
  lcd.setCursor(0, ligne);
  // Effacer la ligne
  lcd.print("                ");
  // Écrire la chaîne sur l'afficheur
  lcd.setCursor(0, ligne);
  lcd.print(str);

}


void RYLR890_WriteString(const char* strBuffer) {

  lora.println(strBuffer);
  //lora.println('\r');
  Serial.println(strBuffer);

}


char* RYLR890_ReadString(char* strBuffer) {

  char* ptrStr = strBuffer;

  while (lora.available() > 0) {

    *(ptrStr++) = lora.read();
    //lcd_EcrireChaine(1, (const char *)ptrStr);

  }

  // Si la chaine n'est pas vide...
  if (0 != strBuffer[0]) {

    // Effacer \n
    *(--ptrStr) = 0;
    // Effacer \r
    *(--ptrStr) = 0;
  }
  String str = String(strBuffer);
  str.trim();
  
  strcpy(strBuffer, str.c_str());
  return strBuffer;
  
}

void RYLR890_Sync() {

  char tampon[] = "                    ";
  char* reponse = 0;
  int i = 0;

  RYLR890_WriteString("AT");
  delay(100);
  lcd_EcrireChaine(0, "Recepteur LoRa");

  while (lora.available() == 0) {
    lcd_EcrireChaine(1, String(i++).c_str());
    delay(10);
  }

  if (lora.available() > 0) {
    reponse = RYLR890_ReadString(tampon);
    // lcd_EcrireChaine(1, (const char *)reponse);
    lcd_EcrireChaine(1, (const char *)tampon);
  } else {
    //
    lcd_EcrireChaine(1, "No Sync");
  }
}

void RYLR890_Reset() {

  digitalWrite(RESET, 0);
  delay(500);
  digitalWrite(RESET, 1);

}

const char*  RYLR890_Stat(unsigned char index) {

  char tampon[100];
  char* reponse = 0;

  if (index == 0) {

    RYLR890_WriteString("AT+PARAMETER?");
    delay(100);
    while (lora.available() == 0) {
      delay(10);
      Serial.print(".");
    }

    if (lora.available() > 0) {
      //      return RYLR890_ReadString(tampon);
      RYLR890_ReadString(tampon);
      Serial.println(tampon);
      tampon[12] = 'P';
      tampon[13] = 'a';
      tampon[14] = 'r';
      reponse = &tampon[12];
      Serial.println("Tampon[0] == " + String(tampon[0]));
      Serial.flush();
      delay(100);
      Serial.println("Tampon[1] == " + String(tampon[1]));
      Serial.flush();
      delay(100);
      Serial.println("Tampon[2] == " + String(tampon[2]));
      Serial.flush();
      delay(100);
      Serial.println("Tampon[3] == " + String(tampon[3]));
      Serial.flush();
      delay(100);
      Serial.println("Tampon[4] == " + String(tampon[4]));
      Serial.flush();
      delay(100);
      Serial.println("Tampon[5] == " + String(tampon[5]));
      Serial.println("Tampon[6] == " + String(tampon[6]));
      Serial.println("Tampon[7] == " + String(tampon[7]));
      Serial.flush();
      delay(100);
      Serial.println(reponse);
      Serial.flush();
      return reponse;
    } else {
      //
      return "Erreur";
    }
  } else {
    return "";
  }
}

void RYLR890_Configure_L1(unsigned char ucSF, unsigned char ucBW, unsigned char ucCR, unsigned char ucPR) {

  String str = String("AT+PARAMETER=") + String(ucSF) + String(",") + String(ucBW) + String(",") + String(ucCR) + String(",") + String(ucPR); 
  RYLR890_WriteString(str.c_str());
  delay(10);

}
