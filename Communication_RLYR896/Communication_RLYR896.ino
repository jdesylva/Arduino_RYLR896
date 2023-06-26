#include <SoftwareSerial.h>

SoftwareSerial lora(2, 3);

void setup() {
  // Initialiser le port série du RYLR890 à 115200 baud
  Serial.begin(115200);
  lora.begin(9600);
}

void loop() {

  String rsp = "";
  String cmd = "";

  while (Serial.available()) {
    // Lire la commande reçue
    cmd += String(char(Serial.read()));
    delay(10);
  }
  if (cmd.length() > 0) {
    //Envoyer la commande au module
    Serial.println(cmd);
    lora.println(cmd);
    cmd = "";
  }

  while (lora.available()) {
    if (lora.available()) {
      // Lire la réponse reçue
      rsp += String(char(lora.read()));
      delay(10);
    }
  }

  if (rsp.length() > 0) {
    Serial.println(rsp);
    rsp = "";
  }
}
