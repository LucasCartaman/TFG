/*
| |<--------------------->| |<--------------------->| |<--------------------->| |

GPS


Este código se encarga de configurar y leer datos del módulo GPS. La función
configurar_gps realiza la configuración inicial del módulo GPS, estableciendo
la tasa de actualización, desactivando ciertos mensajes y ajustando la velocidad
de transmisión. La función leer_gps se encarga de leer los datos del módulo GPS,
procesar la información recibida y realizar ajustes, como la corrección de
posición en función de la orientación del multicoptero y el cálculo de
correcciones de pitch y roll para el modo GPS hold. Además, gestiona la detección
de pérdida de señal GPS y realiza ajustes en la orientación y posición del dron
basándose en los datos del GPS.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/


void configurar_gps(void) {

  //Inicia el puerto serial 1.
  Serial2.begin(9600);
  delay(250);

  //Desactivar mensajes GPGSV mediante el protocolo u-blox.
  uint8_t Desactivar_GPGSV[11] = {0xB5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x03, 0x00, 0xFD, 0x15};
  //Envia el comando por el puerto serial.
  Serial2.write(Desactivar_GPGSV, 11);
  //Se agrega un pequeño retraso para dar tiempo al GPS para responder a 9600 bps.
  delay(350);

  //Establece la frecuencia de actualización en 5 Hz utilizando el protocolo u-blox.
  uint8_t Configurar_a_5Hz[14] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xC8, 0x00, 0x01, 0x00, 0x01, 0x00, 0xDE, 0x6A};
  //Envia el comando por el puerto serial.
  Serial2.write(Configurar_a_5Hz, 14);
  //Se agrega un pequeño retraso para dar tiempo al GPS para responder a 9600 bps.
  delay(350);

  //Establece la velocidad de baudios en 57.6 kbps utilizando el protocolo u-blox.
  uint8_t Configurar_a_57kbps[28] = {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00,
                               0x00, 0xE1, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE2, 0xE1
                              };
  //Envia el comando por el puerto serial.
  Serial2.write(Configurar_a_57kbps, 28);
  //Se agrega un pequeño retraso para dar tiempo al GPS para responder a 9600 bps.
  delay(200);

  //Inicia el puerto serial 1.
  Serial2.begin(57600);
  delay(200);
}


void leer_gps(void) {

  //Permanece en este bucle mientras haya información serial disponible desde el GPS.
  while (Serial2.available() && nueva_linea_encontrada == 0) {
    //Guarda un nuevo byte serial en la variable byte_leido.
    char byte_leido = Serial2.read();

    //Si el nuevo byte es igual al carácter $.
    if (byte_leido == '$') {
      //Eliminar los datos antiguos del array de entrada.
      for (contador_mensaje = 0; contador_mensaje <= 99; contador_mensaje ++) {
        //Escribir "-" en todas las entradas.
        mensaje_entrante[contador_mensaje] = '-';
      }
      //Restablecer la variable contador_mensaje porque queremos comenzar a escribir al principio del array.
      contador_mensaje = 0;
    }

    //Si el byte recibido no es igual a un carácter $, incrementa la variable contador_mensaje.
    else if (contador_mensaje <= 99)contador_mensaje ++;
    //Escribe el nuevo byte recibido en la nueva posición del array mensaje_entrante.
    mensaje_entrante[contador_mensaje] = byte_leido;
    //Cada línea NMEA termina con un *. Si se detecta este carácter, la variable nueva_linea_encontrada se establece en 1.
    if (byte_leido == '*') nueva_linea_encontrada = 1;
  }

  //Si el software ha detectado una nueva línea NMEA, verificará si es una línea válida que se puede usar.
  if (nueva_linea_encontrada == 1) {
    //Restablecer la variable nueva_linea_encontrada para la próxima línea.
    nueva_linea_encontrada = 0;

    //Cuando no hay una correción o información de latitud/longitud disponible.
    if (mensaje_entrante[4] == 'L' && mensaje_entrante[5] == 'L' && mensaje_entrante[7] == ',') {
      //Cambia el LED en el STM32 para indicar la recepción del GPS.
      digitalWrite(LED_STM32, !digitalRead(LED_STM32));
      //Reinicia algunas variables si no se encuentra información válida por el módulo GPS.
      l_lat_gps = 0;
      l_lon_gps = 0;
      lat_gps_anterior = 0;
      lon_gps_anterior = 0;
      satelites_usados = 0;
    }

    /*Si la línea comienza con "GA" y hay suficiente calidad de los datos del GPS, 
    se puede escanear la línea en busca de la latitud, longitud y el número de satélites.*/
    if (mensaje_entrante[4] == 'G' && mensaje_entrante[5] == 'A' && (mensaje_entrante[44] == '1' || mensaje_entrante[44] == '2')) {
      //Restar 48 es necesario para pasar de ASCII a números.
      //Filtrar los minutos para la línea GGA y los multiplica por 10000000
      lat_gps_actual = ((int)mensaje_entrante[19] - 48) *  (long)10000000;
      //Filtrar los minutos para la línea GGA y los multiplica por 1000000.
      lat_gps_actual += ((int)mensaje_entrante[20] - 48) * (long)1000000;
      //Filtrar los minutos para la línea GGA y los multiplica por 100000.
      lat_gps_actual += ((int)mensaje_entrante[22] - 48) * (long)100000;
      //Filtrar los minutos para la línea GGA y los multiplica por 10000.
      lat_gps_actual += ((int)mensaje_entrante[23] - 48) * (long)10000;
      //Filtrar los minutos para la línea GGA y los multiplica por 1000.
      lat_gps_actual += ((int)mensaje_entrante[24] - 48) * (long)1000;
      //Filtrar los minutos para la línea GGA y los multiplica por 100.
      lat_gps_actual += ((int)mensaje_entrante[25] - 48) * (long)100;
      //Filtrar los minutos para la línea GGA y los multiplica por 10.
      lat_gps_actual += ((int)mensaje_entrante[26] - 48) * (long)10;
      //Para convertir los minutos a grados, necesitamos dividir los minutos por 6.
      lat_gps_actual /= (long)6;
      //Sumar los grados multiplicados por 10000000.
      lat_gps_actual += ((int)mensaje_entrante[17] - 48) *  (long)100000000;
      //Sumar los grados multiplicados por 1000000.
      lat_gps_actual += ((int)mensaje_entrante[18] - 48) *  (long)10000000;
      //Dividir todo por 10.
      lat_gps_actual /= 10;

      //Filtrar los minutos para la línea GGA y los multiplica por 10000000.
      lon_gps_actual = ((int)mensaje_entrante[33] - 48) *  (long)10000000;
      //Filtrar los minutos para la línea GGA y los multiplica por 1000000.
      lon_gps_actual += ((int)mensaje_entrante[34] - 48) * (long)1000000;
      //Filtrar los minutos para la línea GGA y los multiplica por 100000.
      lon_gps_actual += ((int)mensaje_entrante[36] - 48) * (long)100000;
      //Filtrar los minutos para la línea GGA y los multiplica por 10000.
      lon_gps_actual += ((int)mensaje_entrante[37] - 48) * (long)10000;
      //Filtrar los minutos para la línea GGA y los multiplica por 1000.
      lon_gps_actual += ((int)mensaje_entrante[38] - 48) * (long)1000;
      //Filtrar los minutos para la línea GGA y los multiplica por 100.
      lon_gps_actual += ((int)mensaje_entrante[39] - 48) * (long)100;
      //Filtrar los minutos para la línea GGA y los multiplica por 10.
      lon_gps_actual += ((int)mensaje_entrante[40] - 48) * (long)10;
      //Para convertir los minutos a grados, necesitamos dividir los minutos por 6.
      lon_gps_actual /= (long)6;
      //Sumar los grados multiplicados por 100000000.
      lon_gps_actual += ((int)mensaje_entrante[30] - 48) * (long)1000000000;
      //Sumar los grados multiplicados por 10000000.
      lon_gps_actual += ((int)mensaje_entrante[31] - 48) * (long)100000000;
      //Sumar los grados multiplicados por 1000000.
      lon_gps_actual += ((int)mensaje_entrante[32] - 48) * (long)10000000;
      //Dividir todo por 10.
      lon_gps_actual /= 10;

      //Cuando se vuela al norte del ecuador, la variable latitud_norte se establecerá en 1.
      if (mensaje_entrante[28] == 'N')latitud_norte = 1;
      //Cuando se vuela al sur del ecuador, la variable latitud_norte se establecerá en 0.
      else latitud_norte = 0;

      //Cuando se vuela al este del meridiano de Greenwich, la variable longitud_este se establecerá en 1.
      if (mensaje_entrante[42] == 'E')longitud_este = 1;
      //Cuando se vuela al oeste del meridiano de Greenwich, la variable longitud_este se establecerá en 0.
      else longitud_este = 0;

      //Filtrar el número de satélites de la línea GGA.
      satelites_usados = ((int)mensaje_entrante[46] - 48) * (long)10;
      //Filtrar el número de satélites de la línea GGA.
      satelites_usados += (int)mensaje_entrante[47] - 48;

      //Si es la primera vez que se utiliza el código GPS.
      if (lat_gps_anterior == 0 && lon_gps_anterior == 0) {
        //Guarda el valor actual de la latitud en la variable lat_gps_anterior.
        lat_gps_anterior = lat_gps_actual;
        //Guarda el valor actual de la longitud en la variable lon_gps_anterior.
        lon_gps_anterior = lon_gps_actual;
      }

      //Divide la diferencia entre la latitud nueva y la anterior por diez.
      lat_gps_bucle_suma = (float)(lat_gps_actual - lat_gps_anterior) / 10.0;
      //Divide la diferencia entre la longitud nueva y la anterior por diez.
      lon_gps_bucle_suma = (float)(lon_gps_actual - lon_gps_anterior) / 10.0;

      //Establece la variable l_lat_gps al valor anterior de latitud.
      l_lat_gps = lat_gps_anterior;
      //Establece la variable l_lon_gps al valor anterior de longitud.
      l_lon_gps = lon_gps_anterior;

      //Guarda el nuevo valor de latitud en la variable lat_gps_anterior para el próximo bucle.
      lat_gps_anterior = lat_gps_actual;
      //Guarda el nuevo valor de longitud en la variable lon_gps_anterior para el próximo bucle.
      lon_gps_anterior = lon_gps_actual;

      /*El GPS está configurado para una tasa de actualización de 5 Hz.
      Entre cada 2 mediciones de GPS, se simulan 9 valores de GPS.*/

      //Establece la variable `gps_suma_contador` en 5 como temporizador de cuenta regresiva en el bucle.
      gps_suma_contador = 5;
      //Establece la variable `gps_dato_nuevo_contador` en 9.
      //Este es el número de valores simulados entre 2 mediciones de GPS.
      gps_dato_nuevo_contador = 9;
      //Reinicia la variable lat_gps_suma.
      lat_gps_suma = 0;
      //Reinici la variable lon_gps_suma.
      lon_gps_suma = 0;
      //Establece la variable gps_dato_nuevo_disponible para indicar que hay nuevos datos disponibles.
      gps_dato_nuevo_disponible = 1;
    }

    //Si la línea comienza con SA y hay una corrección GPS,
    //se puede escanear la línea en busca del tipo de corrección (ninguna, 2D o 3D).
    if (mensaje_entrante[4] == 'S' && mensaje_entrante[5] == 'A')tipo_correccion = (int)mensaje_entrante[9] - 48;

  }

  //Después de 5 ciclos de programa, 5 x 4ms = 20ms, el contador gps_suma_contador es 0.

  //Si gps_suma_contador es 0 y se necesitan nuevas simulaciones de GPS.
  if (gps_suma_contador == 0 && gps_dato_nuevo_contador > 0) {
    //Establecer la variable gps_dato_nuevo_disponible para indicar que hay un nuevo dato disponible.
    gps_dato_nuevo_disponible = 1;
    //Decrementar el contador gps_dato_nuevo_contador para que solo haya 9 simulaciones.
    gps_dato_nuevo_contador --;
    //Establecer la variable gps_suma_contador en 5 como temporizador de cuenta regresiva en el bucle.
    gps_suma_contador = 5;
    
    //Agregar la parte simulada a un búfer flotante porque l_lat_gps solo puede contener enteros.
    lat_gps_suma += lat_gps_bucle_suma;
    //Si el valor absoluto de lat_gps_suma es mayor que 1.
    if (abs(lat_gps_suma) >= 1) {
      //Incrementa el valor de lat_gps_suma con el valor de lat_gps_suma como un entero. Sin parte decimal.
      l_lat_gps += (int)lat_gps_suma;
      //Resta el valor de lat_gps_suma como un entero para que el valor decimal permanezca.
      lat_gps_suma -= (int)lat_gps_suma;
    }

    //Agregar la parte simulada a un búfer flotante porque l_lon_gps solo puede contener enteros.
    lon_gps_suma += lon_gps_bucle_suma;
    //Si el valor absoluto de lon_gps_suma es mayor que 1.
    if (abs(lon_gps_suma) >= 1) {
      //Incrementa el valor de lon_gps_suma con el valor de lon_gps_suma como un entero. Sin parte decimal.
      l_lon_gps += (int)lon_gps_suma;
      //Resta el valor de lon_gps_suma como un entero para que el valor decimal permanezca.
      lon_gps_suma -= (int)lon_gps_suma;
    }
  }

  //Si hay un nuevo conjunto de datos de GPS disponible.
  if (gps_dato_nuevo_disponible) {
    //Cambiar el LED en el STM32 para indicar la recepción del GPS.
    if (satelites_usados < 8)digitalWrite(LED_STM32, !digitalRead(LED_STM32));
    //Encender el LED en el STM de manera constante (la función del LED está invertida). Probar el esquemático del STM32.
    else digitalWrite(LED_STM32, LOW);

    //Reinicio de variables
    gps_vigilia_temporizador = millis();
    gps_dato_nuevo_disponible = 0;

    //Si el modo de vuelo es 3 (GPS) y no se han establecido puntos de referencia.
    if (modo_vuelo >= 3 && establecer_ruta == 0) {
      //Indicar que los puntos de referencia están establecidos.
      establecer_ruta = 1;
      //Recordar la latitud actual como punto de referencia.
      l_lat_ruta = l_lat_gps;
      //Recordar la longitud actual como punto de referencia.
      l_lon_ruta = l_lon_gps;
    }

    //Si el modo de retención de GPS y los puntos de referencia están almacenados.
    if (modo_vuelo >= 3 && establecer_ruta == 1) {
      //Ajustes de movimiento.
      if (modo_vuelo == 3 && despegue_detectado == 1) {

        //Correción de la latitud
        if (!latitud_norte) {
          //Correción del sur
          l_lat_gps_ajuste_float += 0.0015 * (((canal_2 - 1500) * cos(gps_ajuste_rumbo_manual * 0.017453)) + ((canal_1 - 1500) * cos((gps_ajuste_rumbo_manual - 90) * 0.017453)));
        }
        else {
          //Correción del norte
          l_lat_gps_ajuste_float -= 0.0015 * (((canal_2 - 1500) * cos(gps_ajuste_rumbo_manual * 0.017453)) + ((canal_1 - 1500) * cos((gps_ajuste_rumbo_manual - 90) * 0.017453)));
        }

        //Correción de la longitud
        if (!longitud_este) {
          //Corección del oeste
          l_lon_gps_ajuste_float -= (0.0015 * (((canal_1 - 1500) * cos(gps_ajuste_rumbo_manual * 0.017453)) + ((canal_2 - 1500) * cos((gps_ajuste_rumbo_manual + 90) * 0.017453)))) / cos(((float)l_lat_gps / 1000000.0) * 0.017453); 
        }

        else {
          //Correción del este
          l_lon_gps_ajuste_float += (0.0015 * (((canal_1 - 1500) * cos(gps_ajuste_rumbo_manual * 0.017453)) + ((canal_2 - 1500) * cos((gps_ajuste_rumbo_manual + 90) * 0.017453)))) / cos(((float)l_lat_gps / 1000000.0) * 0.017453); 
        }
      }

      //Ajuste de la latitud de los puntos de referencia.
      if (l_lat_gps_ajuste_float > 1) {
        l_lat_ruta ++;
        l_lat_gps_ajuste_float --;
      }
      if (l_lat_gps_ajuste_float < -1) {
        l_lat_ruta --;
        l_lat_gps_ajuste_float ++;
      }

      //Ajuste de la longitud de los puntos de referencia.
      if (l_lon_gps_ajuste_float > 1) {
        l_lon_ruta ++;
        l_lon_gps_ajuste_float --;
      }
      if (l_lon_gps_ajuste_float < -1) {
        l_lon_ruta --;
        l_lon_gps_ajuste_float ++;
      }

      //Calcular el error de latitud entre el punto de referencia y la posición actual.
      gps_error_lon = l_lon_ruta - l_lon_gps;
      //Calcular el error de longitud entre el punto de referencia y la posición actual.
      gps_error_lat = l_lat_gps - l_lat_ruta;

      //Calculo de la latitud promedio con memoria rotativa.

      //Resta la posición actual de la memoria para hacer espacio para el nuevo valor.
      gps_lat_prom_total -=  gps_lat_mem_rotativa[ direccion_mem_rotativa_gps];
      //Calcular el cambio nuevo entre la latitud actual y la anterior.
      gps_lat_mem_rotativa[ direccion_mem_rotativa_gps] = gps_error_lat - gps_error_lat_anterior;
      //Suma el nuevo valor al valor promedio.
      gps_lat_prom_total +=  gps_lat_mem_rotativa[ direccion_mem_rotativa_gps];

      //Calculo de la longitud promedio con memoria rotativa.

      //Resta la posición actual de la memoria para hacer espacio para el nuevo valor.
      gps_lon_prom_total -=  gps_lon_mem_rotativa[ direccion_mem_rotativa_gps];
      //Calcular el cambio nuevo entre la longitud actual y la anterior.
      gps_lon_mem_rotativa[ direccion_mem_rotativa_gps] = gps_error_lon - gps_error_lon_anterior;
      //Suma el nuevo valor al valor promedio.
      gps_lon_prom_total +=  gps_lon_mem_rotativa[ direccion_mem_rotativa_gps];
      
      //Incrementa la dirección de la memoria rotativa.
      direccion_mem_rotativa_gps++;
      //Reinicia la dirección de la memoria rotativa cuando ésta llega a 35.
      if ( direccion_mem_rotativa_gps == 35) direccion_mem_rotativa_gps = 0;

      //Guardar el error actual para el siguiente bucle.
      gps_error_lat_anterior = gps_error_lat;
      gps_error_lon_anterior = gps_error_lon;

      /*Calcular la corrección del pitch y del roll del GPS como si la frente del dron 
      estuviera orientada hacia el norte.
      La parte proporcional = (float)gps_error_lat * gps_ganancia_p.
      La parte derivativa = (float)gps_lat_prom_total * gps_ganancia_d.*/

      gps_pitch_ajuste_norte = (float)gps_error_lat * gps_ganancia_p + (float)gps_lat_prom_total * gps_ganancia_d;
      gps_roll_ajuste_norte = (float)gps_error_lon * gps_ganancia_p + (float)gps_lon_prom_total * gps_ganancia_d;

      //Invierte el ajuste del pitch debido a que el cuadricóptero está volando al sur del ecuador.
      if (!latitud_norte)gps_pitch_ajuste_norte *= -1;    
      //Invierte el ajuste del roll debido a que el cuadricóptero está volando al oeste del meridiano de Greenwich.
      if (!longitud_este)gps_roll_ajuste_norte *= -1;

      /*//Dado que la corrección se calcula como si el lado frontal estuviera orientada hacia el
      norte, necesitamos convertirla según la dirección actual.*/

      gps_roll_ajuste = ((float)gps_roll_ajuste_norte * cos(angulo_yaw * 0.017453)) + ((float)gps_pitch_ajuste_norte * cos((angulo_yaw - 90) * 0.017453));

      gps_pitch_ajuste = ((float)gps_pitch_ajuste_norte * cos(angulo_yaw * 0.017453)) + ((float)gps_roll_ajuste_norte * cos((angulo_yaw + 90) * 0.017453));

      //Limita la corrección máxima a 300. De esta manera, aún mantenemos un control con los mandos
      //de pitch y roll en el transmisor.
      if (gps_roll_ajuste > 300) gps_roll_ajuste = 300;
      if (gps_roll_ajuste < -300) gps_roll_ajuste = -300;
      if (gps_pitch_ajuste > 300) gps_pitch_ajuste = 300;
      if (gps_pitch_ajuste < -300) gps_pitch_ajuste = -300;
    }
  }

  //Si se excede el temporizador de vigilancia, significa que falta la señal del GPS.
  if (gps_vigilia_temporizador + 1000 < millis()) {
    //Si el modo de vuelo está configurado en 3 (GPS).
    if (modo_vuelo >= 3 && inicio > 0) {
      //Establecer el modo de vuelo en 2.
      modo_vuelo = 2;
      //Generar un error.
      error = 4;
    }
  }

  //Si el modo de retención GPS está desactivado y los puntos de ruta están configurados.
  if (modo_vuelo < 3 && establecer_ruta > 0) {
    //Restablecer la variable gps_roll_ajuste para desactivar la corrección.
    gps_roll_ajuste = 0;
    //Restablecer la variable gps_pitch_ajuste para desactivar la corrección.
    gps_pitch_ajuste = 0;

    //Si los puntos de ruta están almacenados.
    if (establecer_ruta == 1) {
      //Establecer la variable direccion_mem_rotativa_gps en cero para poder vaciarla.
      direccion_mem_rotativa_gps = 0;
      //Establecer la variable establecer_ruta en 2 como indicación de que el búfer no se ha borrado.
      establecer_ruta = 2;
    }

    //Restablecer la ubicación actual de gps_lon_mem_rotativa.
    gps_lon_mem_rotativa[ direccion_mem_rotativa_gps] = 0;
    //Restablecer la ubicación actual de gps_lon_mem_rotativa.
    gps_lat_mem_rotativa[ direccion_mem_rotativa_gps] = 0;
    //Incrementar la variable direccion_mem_rotativa_gps para el próximo bucle.
    direccion_mem_rotativa_gps++;

    //Si la variable direccion_mem_rotativa_gps es igual a 36, todas las ubicaciones del búfer se han borrado.
    if (direccion_mem_rotativa_gps == 36) {
      //Restablecer la variable establecer_ruta a 0.
      establecer_ruta = 0;
      //Restablecer las variables que se utilizan para el controlador D.
      gps_error_lat_anterior = 0;
      gps_error_lon_anterior = 0;
      gps_lat_prom_total = 0;
      gps_lon_prom_total = 0;
      direccion_mem_rotativa_gps = 0;
      //Reinicia la longitud y latitud de la ruta.
      l_lat_ruta = 0;
      l_lon_ruta = 0;
    }
  }
}
