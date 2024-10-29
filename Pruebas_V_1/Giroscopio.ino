
void leer_valores_giroscopio(void) {
  //Si no se usa la calibración manual.
  cal_int = 0;

  //Permanecer en este bucle hasta que la variable "dato" contenga una "s".
  while (dato != 's') {
    //Imprime los datos cada 250 ms.
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

    //Si se envia el carácter "c".
    if (dato == 'c') {
      if (cal_int != 2000) {
        //Reinicia los valores de calibración.
        cal_eje_giroscopio[1] = 0;
        cal_eje_giroscopio[2] = 0;
        cal_eje_giroscopio[3] = 0;
        Serial.print("Calibrando el giroscopio");

        //Tomar 2000 muestras del giroscopio para poder determinar el promedio del offset.
        for (cal_int = 0; cal_int < 2000 ; cal_int ++) {
          //Después de cada 125 lecturas.
          if (cal_int % 125 == 0) {
            //Cambiar el estado del LED para indicar la calibración.
            digitalWrite(PB3, !digitalRead(PB3));
            Serial.print(".");
          }

          //Lee la salida del giroscopio.
          leer_giroscopio();
          //Suma el valor del roll a cal_eje_giroscopio.
          cal_eje_giroscopio[1] += eje_giroscopio[1];
          //Suma el valor del pitch a cal_eje_giroscopio.
          cal_eje_giroscopio[2] += eje_giroscopio[2];
          //Suma el valor del yaw a cal_eje_giroscopio.
          cal_eje_giroscopio[3] += eje_giroscopio[3];
          //Simular un bucle de 250 Hz.
          delay(4);
        }
        Serial.println(".");

        //Se divide el total por 2000 para tener el valor promedio.
        cal_eje_giroscopio[1] /= 2000;
        cal_eje_giroscopio[2] /= 2000;
        cal_eje_giroscopio[3] /= 2000;

        //Imprime los valores de calibración obtenidos.
        Serial.print("Valor de calibración X:");
        Serial.println(cal_eje_giroscopio[1]);
        Serial.print("Valor de calibración Y:");
        Serial.println(cal_eje_giroscopio[2]);
        Serial.print("Valor de calibración Z:");
        Serial.println(cal_eje_giroscopio[3]);
      }

      //Read the gyro output.
      leer_giroscopio();
      //Imprime los valores del giroscopio.
      Serial.print("Giroscopio_Z = ");
      Serial.print(eje_giroscopio[1]);
      Serial.print(" Giroscopio_Y = ");
      Serial.print(eje_giroscopio[2]);
      Serial.print(" Giroscopio_Z = ");
      Serial.println(eje_giroscopio[3]);
    }

    //Si se envia el carácter "d".
    else {
      //Leer la salida del acelerómetro.
      leer_giroscopio();

      //Imprime los valores del acelerómetro.
      Serial.print("Acelerómetro_X = ");
      Serial.print(eje_acel[1]);
      Serial.print(" Acelerómetro_Y = ");
      Serial.print(eje_acel[2]);
      Serial.print(" Acelerómetro_Z = ");
      Serial.println(eje_acel[3]);
    }
  }
  //Imprimir la portada.
  portada();

}

