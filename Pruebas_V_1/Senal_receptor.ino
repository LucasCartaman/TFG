
void leer_senal_receptor(void) {
  //Permanecer en este bucle hasta que la variable "dato" contenga una "s".
  while (dato != 's') {
    //Imprimir los valores del receptor cada 250 ms.
    delay(250);
    //Si el puerto serial está disponible.
    if (Serial.available() > 0) {
      //Leer el byte entrante.
      dato = Serial.read();
      //Esperar a que entren otros bytes.
      delay(100);
      //Vaciar el búfer serial.
      while (Serial.available() > 0)contador_bucle = Serial.read();
    }

    //Iniciar los motores: aceleración bajada y yaw a la izquierda.
    
    if (canal_3 < 1100 && canal_4 < 1100)inicio = 1;

    //Cuando el stick de yaw vuelva al centro.
    if (inicio == 1 && canal_3 < 1100 && canal_4 > 1450)inicio = 2;

    //Parar los motores: aceleracion bajada y yaw a la derecha.
    if (inicio == 2 && canal_3 < 1100 && canal_4 > 1900)inicio = 0;

    //Imprimir el estado de inicio.
    Serial.print("Inicio:");
    Serial.print(inicio);

    //Imprimir la posición del stick del roll.
    Serial.print("  Roll:");
    if (canal_1 - 1480 < 0)Serial.print("<<<");
    else if (canal_1 - 1520 > 0)Serial.print(">>>");
    else Serial.print("-+-");
    Serial.print(canal_1);

    //Imprimir la posición del stick del pitch.
    Serial.print("  Pitch:");
    if (canal_2 - 1480 < 0)Serial.print("^^^");
    else if (canal_2 - 1520 > 0)Serial.print("vvv");
    else Serial.print("-+-");
    Serial.print(canal_2);

    //Imprimir la posición del stick del acelerador.
    Serial.print("  Acelerador:");
    if (canal_3 - 1480 < 0)Serial.print("vvv");
    else if (canal_3 - 1520 > 0)Serial.print("^^^");
    else Serial.print("-+-");
    Serial.print(canal_3);

    //Imprimir la posición del stick del yaw.
    Serial.print("  Yaw:");
    if (canal_4 - 1480 < 0)Serial.print("<<<");
    else if (canal_4 - 1520 > 0)Serial.print(">>>");
    else Serial.print("-+-");
    Serial.print(canal_4);

    //Imprimir el valor del canal 5.
    Serial.print("  C5:");
    Serial.print(canal_5);

    //Imprimir el valor del canal 5.
    Serial.print("  C6:");
    Serial.println(canal_6);

  }
  //Imprime la portada.
  portada();
}
