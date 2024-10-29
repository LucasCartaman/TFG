
void prueba_brujula(void) {
  //Reinicia el contador_bucle.
  contador_bucle = 0;
  //Lee el voltaje de la bateria.
  voltaje_bateria = analogRead(4);

  //Probar si la brujula responde.

  //Inicia la comunicación con la brújula.
  HWire.beginTransmission(direccion_brujula);
  //Termina la comunicación y registra el estado de salida.
  error = HWire.endTransmission();
  //Si el estado de salida no es 0, ocurrió un error.
  if (error != 0) {
    //Imprimir el error.
    Serial.println("La brújula no responde");
  }
  //Si la brújula responde normalmente.
  else {
    //Probar los valores del HMC5883.
    //Las dirección son de acuerdo a la hoja de datos.
    HWire.beginTransmission(direccion_brujula);
    HWire.write(0x00);
    HWire.write(0x10);
    HWire.write(0x60);
    HWire.write(0x00);
    HWire.endTransmission();
    delay(250);
    HWire.beginTransmission(direccion_brujula);
    HWire.write(0x00);
    HWire.write(0x11);
    HWire.write(0x60);
    HWire.write(0x01);
    HWire.endTransmission();
    Serial.print("Prueba con bias positivo: ");
    delay(10);
    leer_brujula();
    Serial.print("x: ");
    Serial.print(brujula_x);
    Serial.print("  y: ");
    Serial.print(brujula_y);
    Serial.print("  z: ");
    Serial.println(brujula_z);

    HWire.beginTransmission(direccion_brujula);
    HWire.write(0x00);
    HWire.write(0x12);
    HWire.write(0x60);
    HWire.write(0x01);
    HWire.endTransmission();
    Serial.print("Prueba con bias negativo: ");
    delay(10);
    leer_brujula();
    Serial.print("x: ");
    Serial.print(brujula_x);
    Serial.print("  y: ");
    Serial.print(brujula_y);
    Serial.print("  z: ");
    Serial.println(brujula_z);

    HWire.beginTransmission(direccion_brujula);
    HWire.write(0x00);
    HWire.write(0x10);
    HWire.write(0x60);
    HWire.write(0x00);
    HWire.endTransmission();
    delay(2000);
  }

  //Permanecer en este bucle hasta que la variable "dato" contenga una "s".
  while (dato != 's') {
    //Esperar 4000 ms para simular un bucle de 250 Hz.
    delayMicroseconds(3700);
    //Si el puerto seerial está disponible.
    if (Serial.available() > 0) {
      //Leer el byte entrante.
      dato = Serial.read();
      //Esperar a que entren otros bytes.
      delay(100);
      //Vaciar el búfer serial.
      while (Serial.available() > 0)contador_bucle = Serial.read();
    }

    //Iniciar la lectura de los datos del HMC5883.
    HWire.beginTransmission(direccion_brujula);
    HWire.write(0x03); //select register 3, X MSB register.
    HWire.endTransmission();

    leer_brujula();
    contador_bucle++;

    //Imprimir los valores de la brújula.
    if (contador_bucle == 125) {
      Serial.print("Eje X:");
      Serial.print (brujula_x);
      Serial.print(" Eje Z:");
      Serial.print(brujula_z);
      Serial.print(" Eje Y:");
      Serial.println(brujula_y);
      contador_bucle = 0;
    }
  }
  //Reinicia el contador_bucle.
  contador_bucle = 0;
  //Imprime la portada.
  portada();
}

void leer_brujula() {
  //Pide 6 bytes del HMC5883.
  HWire.requestFrom(direccion_brujula, 6);
  //Suma los MSB con los LSB.
  brujula_x = (HWire.read() << 8) | HWire.read();
  brujula_z = (HWire.read() << 8) | HWire.read();
  brujula_y = (HWire.read() << 8) | HWire.read();
}
