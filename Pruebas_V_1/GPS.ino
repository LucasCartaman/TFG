
void prueba_gps(void) {
  dato = 0;
  contador_bucle = 0;

  //Inicializa el puerto serial a 9600bps.
  Serial2.begin(9600);
  //Tiempo de espera.
  delay(250);

  //Ejecutar el bucle 500 veces
  while (contador_bucle < 500) {
    if (contador_bucle < 1000)contador_bucle ++;
    //Esperar 4000 us para crear un bucle de 250 Hz.
    delayMicroseconds(4000);

    //Para los primeros 250 ciclos.
    if (contador_bucle == 1) {
      //Imprimir encabezado.
      Serial.println("");
      Serial.println("| |<--------------------->| |<--------------------->| |<--------------------->| |");
      Serial.println("Prueba de gps a 9600bps (tiempo de prueba: 1 seg.)");
      Serial.println("| |<--------------------->| |<--------------------->| |<--------------------->| |");
    }
    //Imprimir los datos recibidos
    if (contador_bucle > 1 && contador_bucle < 250)while (Serial2.available())Serial.print((char)Serial2.read());

    //Para los siguientes 250 ciclos.
    if (contador_bucle == 250) {
      //Imprimir encabezado.
      Serial.println("");
      Serial.println("| |<--------------------->| |<--------------------->| |<--------------------->| |");
      Serial.println("Prueba de gps a 57600bps (tiempo de prueba: 1 seg.)");
      Serial.println("| |<--------------------->| |<--------------------->| |<--------------------->| |");
      delay(200);
      
      //Deshabilitar mensajes GPGSV.
      uint8_t Desactivar_GPGSV[11] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x03, 0x00, 0xFD, 0x15};
      Serial2.write(Desactivar_GPGSV, 11);
      delay(350);
      //Establecer la tasa de actualización en 5Hz.
      uint8_t Configurar_a_5Hz[14] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00, 0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A};
      Serial2.write(Configurar_a_5Hz, 14);
      delay(350);
      //Establece la velocidad de baudios en 57kbps.
      uint8_t Configurar_a_57kbps[28] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00,
                                   0x00, 0xE1, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE2, 0xE1
                                  };
      Serial2.write(Configurar_a_57kbps, 28);
      delay(200);

      //Inicializa el puerto serial a 57600bps.
      Serial2.begin(57600);
      //Tiempo de espera.
      delay(200);

      //Leer el puerto serial.
      while (Serial2.available())Serial2.read();
    }
    //Imprimir el los datos obtenidos.
    if (contador_bucle > 250 && contador_bucle < 500)while (Serial2.available())Serial.print((char)Serial2.read());

  }
  //Reinicia el contador.
  contador_bucle = 0;
  //Muestra la portada.
  portada();
}
