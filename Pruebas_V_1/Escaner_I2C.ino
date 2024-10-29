void escaner_i2c(void) {
  //Imprime la lista de dispositivos necesarios.
  Serial.println("El dron necesita los siguientes dispositivos:");
  Serial.println("0x1E = Brujula: HMC5883L.");
  Serial.println("0x68 = Giroscopio/Acelerómetro: MPU-6050.");
  Serial.println("0x77 = Barómetro: MS5611");
  Serial.println("Deben aparecer en la siguiente lista.");
  Serial.println("");

  //Declaración de variables locales.
  dato = 0;
  uint8_t error, direccion, listo;
  uint16_t nDispositivo;

  //Imprime las direcciones a escanear.
  Serial.println("Escaneando dirección 1 hasta 127...");
  Serial.println("");

  nDispositivo = 0;

  //Bucle de escaneo.
  for (direccion = 1; direccion < 127; direccion++) {
    //Inicia la comunicación.
    HWire.beginTransmission(direccion);
    //Finaliza la comunicación y guarda el estado de salida.
    error = HWire.endTransmission();
    //Si el estado es 0, hay comunicación exitosa.
    if (error == 0) {
      //Imprime el título y la dirección.
      Serial.print("Dispositivo I2C encontrado en la dirección 0x");
      if (direccion < 16)
        Serial.print("0");
      Serial.println(direccion, HEX);

      //Incrementa el contador de dispositivos
      nDispositivo++;
    }
    //Si el error es 4, no hay comunicación exitosa.
    else if (error == 4) {
      //Enviar un mensaje de error.
      Serial.print("Error en la dirección 0x");
      if (direccion < 16)
        Serial.print("0");
      Serial.println(direccion, HEX);
    }
  }

  //Marcar el fin del escaneo.
  listo = 1;

  //Si no se encontraron dispositivos.
  if (nDispositivo == 0)
    //Imprimir un aviso.
    Serial.println("No se encontraron dispositivos I2C");
  else
    //Imprimi "Fin" para indicar el fin del escaneo.
    Serial.println("Fin");
  delay(2000);
  //Imprimir la portada.
  portada();
}
