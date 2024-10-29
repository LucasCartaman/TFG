
void prueba_angulos_imu(void) {
  uint8_t primer_angulo = 0;
  contador_bucle = 0;
  primer_angulo = false;
  cal_int = 0;

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

    //Si no se utiliza la calibración manual.
    if (cal_int == 0) {
      //Restablecer las variables de calibración para la próxima calibración.
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
        //Lee los valores del giroscopio.
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
      //Desactiva el LED verde
      led_verde(LOW);

      //Se divide el total por 2000 para tener el valor promedio.
      cal_eje_giroscopio[1] /= 2000;
      cal_eje_giroscopio[2] /= 2000;
      cal_eje_giroscopio[3] /= 2000;
    }

    //Leer el valor actual del giroscopio.
    leer_giroscopio();

    //Cálculo de ángulos.
    // 1 / (250Hz / 65.5) = 0.0000611

    //Calcular el ángulo de pitch recorrido y sumarlo a la variable angulo_pitch.
    angulo_pitch += eje_giroscopio[2] * 0.0000611;
    //Calcular el ángulo de roll recorrido y sumarlo a la variable angulo_roll.
    angulo_roll += eje_giroscopio[1] * 0.0000611;

    //La función sin de Arduino opera en radianes.
    //0.0000611 * (3.142 / 180degr) = 0.000001066

    //Si la IMU ha girado, transferir el ángulo de roll al ángulo de pitch.
    angulo_pitch -= angulo_roll * sin(eje_giroscopio[3] * 0.000001066);
    //Si la IMU ha girado, transferir el ángulo de pitch al ángulo de roll.
    angulo_roll += angulo_pitch * sin(eje_giroscopio[3] * 0.000001066);

    //Cálculos del ángulo del acelerómetro.

    //Límitar los valores máximo y mínimos del acelerómetro.
    if (eje_acel[1] > 4096)eje_acel[1] = 4096;
    if (eje_acel[1] < -4096)eje_acel[1] = -4096;
    if (eje_acel[2] > 4096)eje_acel[2] = 4096;
    if (eje_acel[2] < -4096)eje_acel[2] = -4096;

    //La función sin de Arduino opera en radianes.
    //1 / (3.142 / 180) = 57.296

    //Cálculo del ángulo de pitch.
    acel_angulo_pitch = asin((float)eje_acel[1] / 4096) * 57.296;
    //Cálculo del ángulo de roll.
    acel_angulo_roll = asin((float)eje_acel[2] / 4096) * 57.296;

    //Si es la primera medición
    if (!primer_angulo) {
      //Establecer el ángulo de pitch como el ángulo del acelerómetro.
      angulo_pitch = acel_angulo_pitch;
      //Establecer el ángulo de roll como el ángulo del acelerómetro.
      angulo_roll = acel_angulo_roll;
      primer_angulo = true;
    }
    //Si no es la primera medición.
    else {
      //Corregir el desvío con el ángulo del acelerómetro
      angulo_pitch = angulo_pitch * 0.9996 + acel_angulo_pitch * 0.0004;
      angulo_roll = angulo_roll * 0.9996 + acel_angulo_roll * 0.0004;
    }

    /*Imprimir todas las lecturas a la vez lleva mucho tiempo y las lecturas 
    estarían desfasadas, por lo que se imprime un dato por bucle. */

    if (contador_bucle == 0)Serial.print("Pitch: ");
    if (contador_bucle == 1)Serial.print(angulo_pitch , 1);
    if (contador_bucle == 2)Serial.print(" Roll: ");
    if (contador_bucle == 3)Serial.print(angulo_roll , 1);
    if (contador_bucle == 4)Serial.print(" Yaw: ");
    if (contador_bucle == 5)Serial.print(eje_giroscopio[3] / 65.5 , 0);
    if (contador_bucle == 6)Serial.print(" Temp: ");
    if (contador_bucle == 7)Serial.println(temperatura / 340.0 + 35.0 , 1);
    contador_bucle ++;
    if (contador_bucle == 60)contador_bucle = 0;

    while (temporizador_bucle > micros());
  }
  //Reinicia el contador.
  contador_bucle = 0;
  //Imprime la portada
  portada();
}

