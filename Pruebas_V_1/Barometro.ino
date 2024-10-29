

void prueba_barometro(void) {
  contador_bucle = 0;

  //Probar si el MS5611 responde.

  //Inicia la comunicación con el MS5611.
  HWire.beginTransmission(direccion_MS5611);
  //Termina la comunicación y registra el estado de salida.
  error = HWire.endTransmission();
  //Si el estado de salida no es 0, ocurrió un error.
  if (error != 0) {
    //Imprime el error.
    Serial.print("MS5611 no responde a la dirección: ");
    Serial.println(direccion_MS5611, HEX);
    //Establece dato en "s" para salir automáticamente del bucle.
    dato = 's';
  }
  //Si el MS5611 responde normalmente.
  else {
    //Imprime un aviso y la dirección en hexadecimal.
    Serial.print("MS5611 encontrado en la dirección: ");
    Serial.println(direccion_MS5611, HEX);
    
    //Para calcular la presión, es necesario recuperar los 6 valores de calibración de la MS5611.
    //Estos valores de 2 bytes se almacenan en la ubicación de memoria 0xA2 en adelante.

    for (inicio = 1; inicio <= 6; inicio++) {
      //Inicia la comunicación con el MS5611
      HWire.beginTransmission(direccion_MS5611);
      //Enviar la dirección a leer.
      HWire.write(0xA0 + inicio * 2);
      //Termina la comunicación.
      HWire.endTransmission();
      //Ordenar 2 bytes del MS5611.
      HWire.requestFrom(direccion_MS5611, 2);
      //Suma el MSB y el LSB.
      C[inicio] = HWire.read() << 8 | HWire.read();
    }
    //Imprime los valores de calibración.
    Serial.print("C1 = ");
    Serial.println(C[1]);
    Serial.print("C2 = ");
    Serial.println(C[2]);
    Serial.print("C3 = ");
    Serial.println(C[3]);
    Serial.print("C4 = ");
    Serial.println(C[4]);
    Serial.print("C5 = ");
    Serial.println(C[5]);
    Serial.print("C6 = ");
    Serial.println(C[6]);

    //Se precalculan estos valores para aliviar el bucle principal.
    OFF_C2 = C[2] * pow(2, 16);
    SENS_C1 = C[1] * pow(2, 15);

    inicio = 0;
  }

  //Permanecer en este bucle hasta que la variable "dato" contenga una "s".
  while (dato != 's') {
    //Establecer la variable temporizador_bucle con el valor actual de micros() + 4000.
    temporizador_bucle = micros() + 4000;
    //Si hay datos disponibles en el puerto serie.
    if (Serial.available() > 0) {
      //Leer el byte entrante.
      dato = Serial.read();
      //Esperar a que entren otros bytes.
      delay(100);
      //Vaciar el búfer serial.
      while (Serial.available() > 0)contador_bucle = Serial.read();
    }

    //Incrementa la variable contador_barometro para el siguiente bucle.
    contador_barometro ++;

    if (contador_barometro == 1) {
      if (contador_temperatura == 0) {
        //Obtener la temperatura del MS5611.
        HWire.beginTransmission(direccion_MS5611);
        HWire.write(0x00);
        HWire.endTransmission();
        HWire.requestFrom(direccion_MS5611, 3);
        temperatura_bruta = HWire.read() << 16 | HWire.read() << 8 | HWire.read();
      }
      else {
        //Obtener la presión del MS5611.
        HWire.beginTransmission(direccion_MS5611);
        HWire.write(0x00);
        HWire.endTransmission();
        HWire.requestFrom(direccion_MS5611, 3);
        presion_bruta = HWire.read() << 16 | HWire.read() << 8 | HWire.read();
      }

      contador_temperatura ++;
      if (contador_temperatura > 9) {
        contador_temperatura = 0;
        //Pedir el valor de la temperatura.
        HWire.beginTransmission(direccion_MS5611);
        HWire.write(0x58);
        HWire.endTransmission();
      }
      else {
        //Pedir le valor de la presión.
        HWire.beginTransmission(direccion_MS5611);
        HWire.write(0x48);
        HWire.endTransmission();
      }
    }
    if (contador_barometro == 2) {
      //Cálculo de la presión según la hoja de datos del MS5611.
      dT = C[5];
      dT <<= 8;
      dT *= -1;
      dT += temperatura_bruta;

      OFF = OFF_C2 + ((int64_t)dT * (int64_t)C[4]) / pow(2, 7);

      SENS = SENS_C1 + ((int64_t)dT * (int64_t)C[3]) / pow(2, 8);

      P = ((presion_bruta * SENS) / pow(2, 21) - OFF) / pow(2, 15);

      //Para la primera lectura de la presión.
      if (presion_actual == 0) {
        presion_actual = P;
        presion_actual_rapida = P;
        presion_actual_lenta = P;
      }
      
      /*Se usan filtros complementarios para obtener dos señales de presión
      una señal de rápida reacción con picos y una señal de lenta reacción
      sin picos.*/
      presion_actual_rapida = presion_actual_rapida * (float)0.92 + P * (float)0.08;
      presion_actual_lenta = presion_actual_lenta * (float)0.99 + P * (float)0.01;

      //Diferencia entre ambas señales
      diferencia_presion_actual = presion_actual_lenta - presion_actual_rapida;

      //Limita el valor máximo y mínimo de dicha diferencia.
      if (diferencia_presion_actual > 8)diferencia_presion_actual = 8;
      if (diferencia_presion_actual < -8)diferencia_presion_actual = -8;

      //Ajusta la señal lenta para valores fuera del rango [1, -1].
      if (diferencia_presion_actual > 1 || diferencia_presion_actual < -1)presion_actual_lenta -= diferencia_presion_actual / 6.0;
      
      //Establce la presión actual.
      presion_actual = presion_actual_lenta;

      //Estabilización de las lecturas iniciales.
      if (inicio < 200){
        inicio++;
        presion_actual = 0;
      }
      else Serial.println(presion_actual,0);
      
    }

    //Reinicio de la variable contador_barómetro
    if (contador_barometro == 3) {
      contador_barometro = 0;
    }
    while (temporizador_bucle > micros());
  }
  //Reinicio de las variables para el siguiente bucle.
  contador_bucle = 0;
  inicio = 0;
  //Imprime la portada
  portada();
}
