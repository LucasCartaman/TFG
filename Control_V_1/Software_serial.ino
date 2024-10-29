/*
| |<--------------------->| |<--------------------->| |<--------------------->| |

Software_serial

Esta sección está diseñada para el manejo de datos serie y la gestión de pulsos
de entrada. La función ss_mapeo_byte se encarga de interpretar y procesar bytes
de datos recibidos. La integridad de los datos se verifica mediante cálculos de
paridad, y si se detecta un conjunto de datos válido, se actualizan variables
relacionadas con la posición y se activan ciertos eventos de vuelo.

Por otro lado, la función gestor_entrada_serial está asociada con la gestión de
la entrada serial, ajustando la configuración del temporizador en respuesta a
flancos de subida o bajada detectados en el canal de entrada. Esta función
contribuye al seguimiento preciso del tiempo entre pulsos de entrada,
facilitando la sincronización y procesamiento adecuado de los datos serie.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/
  
  
/*se ejecuta despues de gestor_entrada_serial, ya que en esa función se actualiza
 el valor de ss_ultimo_cambio_entrada, permitiendo el reinicio de ss_marca_impresion*/
void ss_mapeo_byte(void) {
  //Si el mensaje ya finalizó y ya terminó el procesamiento de bytes anterior.
  if (  millis() - ss_ultimo_cambio_entrada > 50 && ss_marca_impresion == 0) {
    /*Ajusta el último valor en el array sumando 104 us al penúltimo tiempo registrado,
    para compensar el desfase temporal del bit de inicio y alinear la secuencia de bits*/
    ss_array_tiempo[ss_array_tiempo_contador] = ss_array_tiempo[ss_array_tiempo_contador - 1] + 104;
    
    //Reiniciar variables.
    ss_nivel = 0;
    ss_bits_recibidos_contador = 0;
    ss_array_tiempo_contador_2 = 1;
    ss_byte_contador = 0;

    //Detección del cambio de nivel
    while (ss_bits_recibidos_contador <= 130) {
      /*Si el tiempo esperado para que cambie el bit cambie de nivel es mayor 
      que el tiempo almacenado en el array, hubo un cambio de nivel*/
      if (5.0 + 104.1666667 * (float)ss_bits_recibidos_contador > ss_array_tiempo[ss_array_tiempo_contador_2]) { 
        //Alternar el valor de ss_nivel.
        if (ss_nivel)ss_nivel = 0;
        else ss_nivel = 1;
        //Ir al siguiente índice del array
        ss_array_tiempo_contador_2 ++;
      }

      //Guardar ss_nivel en el bit del byte correspondiente
      if (ss_bits_recibidos_contador == 1 + ss_byte_contador * 10)ss_bytes_recibidos[ss_byte_contador] = ss_nivel << 0;
      if (ss_bits_recibidos_contador == 2 + ss_byte_contador * 10)ss_bytes_recibidos[ss_byte_contador] += ss_nivel << 1;
      if (ss_bits_recibidos_contador == 3 + ss_byte_contador * 10)ss_bytes_recibidos[ss_byte_contador] += ss_nivel << 2;
      if (ss_bits_recibidos_contador == 4 + ss_byte_contador * 10)ss_bytes_recibidos[ss_byte_contador] += ss_nivel << 3;
      if (ss_bits_recibidos_contador == 5 + ss_byte_contador * 10)ss_bytes_recibidos[ss_byte_contador] += ss_nivel << 4;
      if (ss_bits_recibidos_contador == 6 + ss_byte_contador * 10)ss_bytes_recibidos[ss_byte_contador] += ss_nivel << 5;
      if (ss_bits_recibidos_contador == 7 + ss_byte_contador * 10)ss_bytes_recibidos[ss_byte_contador] += ss_nivel << 6;
      if (ss_bits_recibidos_contador == 8 + ss_byte_contador * 10)ss_bytes_recibidos[ss_byte_contador] += ss_nivel << 7;
      //Pasar al siguiente byte.
      if (ss_bits_recibidos_contador == 9 + ss_byte_contador * 10)ss_byte_contador ++;
      //Pasar al siguiente bit, hasta un límite de 130, suficiente para 13 mensajes (10 bits por mensaje).
      ss_bits_recibidos_contador ++;
    }

    //Reiniciar el byte de acuse.
    ss_byte_acuse = 0x00;
    //Calcular el byte de acuse con los bytes recibidos.
    for (contador_var = 0; contador_var <= 10; contador_var++)ss_byte_acuse ^= ss_bytes_recibidos[contador_var];

    //Verificar la igual entre el byte de acuse calculado y el recibido.
    if (ss_byte_acuse == ss_bytes_recibidos[11]) {

      //Si el mensaje inicia con la firma P y R (Punto de Ruta).
      if (ss_bytes_recibidos[0] == 'P' && ss_bytes_recibidos[1] == 'R') {
        //Activar la variable nueva_ruta_disponible.
        nueva_ruta_disponible = 1;
        //Reiniciar el primer byte recibido.
        ss_bytes_recibidos[0] = 0x00;

        //Guarda la latitud y la longitud.
        lat_gps_ruta = (int32_t)ss_bytes_recibidos[2] | (int32_t)ss_bytes_recibidos[3] << 8 | (int32_t)ss_bytes_recibidos[4] << 16 | (int32_t)ss_bytes_recibidos[5] << 24;
        lon_gps_ruta = (int32_t)ss_bytes_recibidos[6] | (int32_t)ss_bytes_recibidos[7] << 8 | (int32_t)ss_bytes_recibidos[8] << 16 | (int32_t)ss_bytes_recibidos[9] << 24;
       
        //Reestablecer las variables para el vuelo.
        if (establecer_ruta == 1 && punto_origen_registrado == 1 && modo_vuelo == 3) {
          volar_nuevo_punto = 1;
          volar_nuevo_punto_paso = 0;
          volar_ruta_factor_lat = 0;
          volar_ruta_factor_lon = 0;
        }
      }
    }
    //Activar ss_marca_impresion para inidicar el fin.
    ss_marca_impresion = 1;
  }

  //Cuando se procese un nuevo byte y ya haya finalizado el mapeo anterior.
  if (millis() - ss_ultimo_cambio_entrada < 4 && ss_marca_impresion == 1) {
    //Reiniciar ss_marca_impresion.
    ss_marca_impresion = 0;
  }
}


//Esta funcion es llamada por TIM3 cuando hay un cambio de nivel en el pin A0.
void gestor_entrada_serial(void) {
  //Si el pulso de entrada del canal 1 del receptor en A0 está en alto.
  if (ss_flanco_subida_activo) {
    //Cambiar el modo de captura de entrada al flanco de bajada del pulso.
    TIMER3_BASE->CCER |= TIMER_CCER_CC1P;
    ss_flanco_subida_activo = 0;
  }
  //Si el pulso de entrada del canal 1 del receptor en A0 está en bajo.
  else {
    //Cambiar el modo de captura de entrada al flanco de subida del pulso.
    TIMER3_BASE->CCER &= ~TIMER_CCER_CC1P;
    ss_flanco_subida_activo = 1;
  }

  //Calcular tiempo usando el valor de CCR1.
  ss_tiempo_medido = TIMER3_BASE->CCR1 - ss_tiempo_medido_inicial;
  //Si el tiempo medido es negativo, sumar 0xFFFF para que sea positivo.
  if (ss_tiempo_medido < 0)ss_tiempo_medido += 0xFFFF;

  //Actualizar el tiempo del último cambio en A0
  ss_ultimo_cambio_entrada = millis();

  /*Si la diferencia de tiempo del cambio actual y el cambio anterior
  es mayor a 50 ms, inició la comunicación*/
  if (ss_ultimo_cambio_entrada - ss_ultimo_cambio_entrada_anterior > 50) {
    //Reiniciar el contador
    ss_array_tiempo_contador = 1;
    //Actualiza el tiempo medido inicial
    ss_tiempo_medido_inicial = TIMER3_BASE->CCR1;
  }
  //Si la diferencia de tiempo es menor a 50 ms.
  else {
    //Guardar el tiempo medido en el array.
    ss_array_tiempo[ss_array_tiempo_contador] = ss_tiempo_medido;
    /*Aumentar el contador para guardar el tiempo en otro ínidice en el
    siguiente ciclo.*/
    if (ss_array_tiempo_contador < 180)ss_array_tiempo_contador++;
  }

  //Actualizar el tiempo de cambio anterior.
  ss_ultimo_cambio_entrada_anterior = ss_ultimo_cambio_entrada;
}
