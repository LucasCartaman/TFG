
void prueba_vibraciones_motores(void) {
  //Declaración de variables locales.
  int32_t array_vibraciones[20], nivel_vibracion_prom, resultado_vibracion_total;
  uint8_t contador_array, aceleracion_inicial_ok, contador_vibracion;
  uint32_t temporizador_espera;
  aceleracion_inicial_ok = 0;

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

    //Si se detecta la aceleración en la posición más baja.
    if (aceleracion_inicial_ok) {
      //Obtener los datos brutos del giroscopio y el acelerómetro.
      leer_giroscopio();
      //Calcular el vector total del acelerómetro.
      array_vibraciones[0] = sqrt((eje_acel[1] * eje_acel[1]) + (eje_acel[2] * eje_acel[2]) + (eje_acel[3] * eje_acel[3]));

      //Realizar este bucle 16 veces para crear un array de vectores del acelerómetro.
      for (contador_array = 16; contador_array > 0; contador_array--) {
        //Desplazar cada variable una posición en el array.
        array_vibraciones[contador_array] = array_vibraciones[contador_array - 1];
        //Sumar el valor del array a la variable nivel_vibracion_prom.
        nivel_vibracion_prom += array_vibraciones[contador_array];
      }
      //Dividir nivel_vibracion_prom por 17 para obtener el vector promedio total del acelerómetro.
      nivel_vibracion_prom /= 17;

      //Si el contador_vibracion es menor que 20.
      if (contador_vibracion < 20) {
        //Incrementa la variable contador_vibracion.
        contador_vibracion ++;
        //Sumar la diferencia absoluta entre el vector promedio y el vector actual a la variable resultado_vibracion_total.
        resultado_vibracion_total += abs(array_vibraciones[0] - nivel_vibracion_prom);
      }
      //Caso contrario.
      else {
        //Reiniciar la variable contador_vibracion.
        contador_vibracion = 0;
        //Imprimir el vector total del acelerómetro dividido por 50 en el monitor serie.
        Serial.println(resultado_vibracion_total / 50);
        //Reiniciar la variable resultado_vibracion_total.
        resultado_vibracion_total = 0;
      }

      //Si se envía el carácter "1".
      if (dato == '1') {
        //Establecer el pulso de salida del ESC 1 igual a la entrada de aceleración.
        TIMER4_BASE->CCR1 = canal_3;
        //Mantener el pulso del ESc 2 en 1000 us.
        TIMER4_BASE->CCR2 = 1000;
        //Mantener el pulso del ESc 3 en 1000 us.
        TIMER4_BASE->CCR3 = 1000;
        //Mantener el pulso del ESc 4 en 1000 us.
        TIMER4_BASE->CCR4 = 1000;
      }

      //Si se envía el carácter "2"
      if (dato == '2') {
        //Mantener el pulso del ESc 1 en 1000 us.
        TIMER4_BASE->CCR1 = 1000;
        //Establecer el pulso de salida del ESC 2 igual a la entrada de aceleración.
        TIMER4_BASE->CCR2 = canal_3;
        //Mantener el pulso del ESc 3 en 1000 us.
        TIMER4_BASE->CCR3 = 1000;
        //Mantener el pulso del ESc 4 en 1000 us.
        TIMER4_BASE->CCR4 = 1000;
      }

      //Si se envía el carácter "3"
      if (dato == '3') {
        //Mantener el pulso del ESc 1 en 1000 us.
        TIMER4_BASE->CCR1 = 1000;
        //Mantener el pulso del ESc 2 en 1000 us.
        TIMER4_BASE->CCR2 = 1000;
        //Establecer el pulso de salida del ESC 3 igual a la entrada de aceleración.
        TIMER4_BASE->CCR3 = canal_3;
        //Mantener el pulso del ESc 4 en 1000 us.
        TIMER4_BASE->CCR4 = 1000;
      }

      //Si se envía el carácter "4"
      if (dato == '4') {
        //Mantener el pulso del ESc 1 en 1000 us.
        TIMER4_BASE->CCR1 = 1000;
        //Mantener el pulso del ESc 2 en 1000 us.
        TIMER4_BASE->CCR2 = 1000;
        //Mantener el pulso del ESc 3 en 1000 us.
        TIMER4_BASE->CCR3 = 1000;
        //Establecer el pulso de salida del ESC 4 igual a la entrada de aceleración.
        TIMER4_BASE->CCR4 = canal_3;
      }

      //Si se envía el carácter "5"
      if (dato == '5') {
        //Establecer el pulso de salida del todos los ESCs igual a la entrada de aceleración.
        TIMER4_BASE->CCR1 = canal_3;
        TIMER4_BASE->CCR2 = canal_3;
        TIMER4_BASE->CCR3 = canal_3;
        TIMER4_BASE->CCR4 = canal_3;
      }

    }

    //Si el acelerador no está en la posición más baja.
    else {
      //Establecer la variable temporizador_espera con el valor actual de millis() incrementado en 10 segundos.
      temporizador_espera = millis() + 10000;
      //Si el canal del acelerador no está en la posición más baja.
      if (canal_3 > 1050) {
        //Imprimir mensaje de aviso.
        Serial.println(F("El acelerador no está en la posición más baja."));
        Serial.print(F("El valor del acelerador es: "));
        Serial.println(canal_3);
        Serial.print(F(""));
        Serial.print(F("Esperando 10 segundos:"));
      }
      //Permanecer en este bucle de espera durante 10 segundos.
      while (temporizador_espera > millis() && !aceleracion_inicial_ok) {
        //If the aceleracion is in the lowest position set the aceleracion_inicial_ok variable en 1.
        if (canal_3 < 1050)aceleracion_inicial_ok = 1;
        //Esperar 500 ms.
        delay(500);
        //Imprimir un punto para mostrar que hay un proceso en ejecución.
        Serial.print(F("."));
      }
    }

    //Si la aceleración no se detecta baja después de los 10 segundos.
    if (!aceleracion_inicial_ok) {
      //Salir de este bucle y regresar al menú principal.
      dato = 's';
    }
    //Crea un bucle de 250 Hz.
    while (temporizador_bucle > micros());
  }
  //Reinicia la variable contador_bucle.
  contador_bucle = 0;
  //Imprime la portada.
  portada();
}
