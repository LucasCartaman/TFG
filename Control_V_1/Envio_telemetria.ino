/*
| |<--------------------->| |<--------------------->| |<--------------------->| |

Envio_telemetria

Esta parte envía el dato de telemetría a la estación terrestre. La salida para 
el monitor serie es PB0. El protocolo es 1 bit de inicio, 8 bits de dato, 
sin paridad, 1 bit de parada. Variables de más de 8 bits se envían por partes,
el envío de todas las variables se realiza 2 veces por segundo (2Hz).
Se utiliza un byte de acuse para comprobar la integridad de los datos.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/

void envio_telemetria(void) {
  
  //Incrementa la variable contador_bucle_telemetria.
  contador_bucle_telemetria++;

  //Enviar una L como firma de inicio.
  if (contador_bucle_telemetria == 1)byte_envio_telemetria = 'L';
  //Enviar una C como firma de inicio.
  if (contador_bucle_telemetria == 2)byte_envio_telemetria = 'C';

  if (contador_bucle_telemetria == 3) {
    //Enviar el error como un byte.
    byte_acuse = 0;
    byte_envio_telemetria = error;
  }

  //Enviar el modo de vuelo, el paso del modo volver al origen y la ruta como un byte.
  if (contador_bucle_telemetria == 4)byte_envio_telemetria = modo_vuelo + pasos_volver_al_origen + monitor_ruta;

  //Enviar el voltaje de la batería como un byte.
  if (contador_bucle_telemetria == 5)byte_envio_telemetria = voltaje_bateria * 10;

  //Envio de la temperatura del giroscopio
  if (contador_bucle_telemetria == 6) {
    //Almacenar la temperatura ya que puede cambiar durante el próximo bucle.
    byte_buffer_telemetria = temperatura;
    //Enviar el primer byte de la variable de temperatura.
    byte_envio_telemetria = byte_buffer_telemetria;
  }
  //Enviar el último byte de la variable de temperatura.
  if (contador_bucle_telemetria == 7)byte_envio_telemetria = byte_buffer_telemetria >> 8;

  //Enviar el ángulo de roll como un byte. Agregar 100 y multiplicar por 100 por evita números float negativos.
  if (contador_bucle_telemetria == 8)byte_envio_telemetria = angulo_roll + 100;
 
  //Enviar el ángulo de pitch como un byte. Agregar 100 y multiplicar por 100 por evita números float negativos.
  if (contador_bucle_telemetria == 9)byte_envio_telemetria = angulo_pitch + 100;

  //Enviar el valor de inicio como un byte.
  if (contador_bucle_telemetria == 10)byte_envio_telemetria = inicio; 

  //Envio de la altitud.
  if (contador_bucle_telemetria == 11) {
    //La altitud solo se envia cuando el dron esté en vuelo.
    if (inicio == 2) {
      //Calcular la altitud y sumar 1000 para evitar números negativos.
      byte_buffer_telemetria = 1000 + ((float)(presion_suelo - presion_actual) * 0.117);
    }
    else {
      //Enviar una altitud de 0 metros si el cuadricóptero no está volando.
      byte_buffer_telemetria = 1000;
    }
    //Enviar el primer byte de la variable de altitud.
    byte_envio_telemetria = byte_buffer_telemetria;
  }
  //Enviar el último byte de la variable de altitud.
  if (contador_bucle_telemetria == 12)byte_envio_telemetria = byte_buffer_telemetria >> 8;

  //Envio de la aceleración.
  if (contador_bucle_telemetria == 13) {
    //Almacenar la aceleración de despegue ya que puede cambiar durante el próximo bucle.
    byte_buffer_telemetria = 1500 + aceleracion_despegue;
    //Enviar el primer byte de la variable de aceleracion_despegue.
    byte_envio_telemetria = byte_buffer_telemetria;
  }
  //Enviar el último byte de la variable de aceleracion_despegue.
  if (contador_bucle_telemetria == 14)byte_envio_telemetria = byte_buffer_telemetria >> 8;

  //Envío del rumbo del dron (ángulo yaw).
  if (contador_bucle_telemetria == 15) {
    //Almacenar el rumbo de la brújula ya que puede cambiar durante el próximo bucle.
    byte_buffer_telemetria = angulo_yaw;
    //Enviar el primer byte del angulo yaw
    byte_envio_telemetria = byte_buffer_telemetria;
  }
  //Enviar el último byte del angulo yaw
  if (contador_bucle_telemetria == 16)byte_envio_telemetria = byte_buffer_telemetria >> 8;

  //Enviar la variable de bloqueo_rumbo como un byte.
  if (contador_bucle_telemetria == 17)byte_envio_telemetria = bloqueo_rumbo;

  //Enviar la variable satelites_usados como un byte.
  if (contador_bucle_telemetria == 18)byte_envio_telemetria = satelites_usados;

  //Enviar la variable tipo_correccion como un byte.
  if (contador_bucle_telemetria == 19)byte_envio_telemetria = tipo_correccion;

  //Envio de la latitud.
  if (contador_bucle_telemetria == 20) {
    //Almacenar la posición de latitud ya que puede cambiar durante el próximo bucle.
    if (latitud_norte == 0) byte_buffer_telemetria = -l_lat_gps;
    else byte_buffer_telemetria = l_lat_gps;
    //Enviar el primer byte de la latitud.
    byte_envio_telemetria = byte_buffer_telemetria;
  }
  //Enviar el siguiente byte de la latitud.
  if (contador_bucle_telemetria == 21)byte_envio_telemetria = byte_buffer_telemetria >> 8;
  //Enviar el siguiente byte de la latitud.
  if (contador_bucle_telemetria == 22)byte_envio_telemetria = byte_buffer_telemetria >> 16;
  //Enviar el siguiente byte de la latitud.
  if (contador_bucle_telemetria == 23)byte_envio_telemetria = byte_buffer_telemetria >> 24;

  //Envio de la longitud.
  if (contador_bucle_telemetria == 24) {
    //Almacenar la posición de longitud ya que puede cambiar durante el próximo bucle.
    if (longitud_este == 0) byte_buffer_telemetria = -l_lon_gps;
    else byte_buffer_telemetria = l_lon_gps;
    //Enviar el primer byte de la longitud.
    byte_envio_telemetria = byte_buffer_telemetria;
  }
  //Enviar el siguiente byte de la longitud.
  if (contador_bucle_telemetria == 25)byte_envio_telemetria = byte_buffer_telemetria >> 8;
  //Enviar el siguiente byte de la longitud.
  if (contador_bucle_telemetria == 26)byte_envio_telemetria = byte_buffer_telemetria >> 16;
  //Enviar el siguiente byte de la longitud.
  if (contador_bucle_telemetria == 27)byte_envio_telemetria = byte_buffer_telemetria >> 24;

  //Envio de variable ajustable 1.
  if (contador_bucle_telemetria == 28) {
    //Almacenar la configuración ajustable 1, ya que puede cambiar durante el próximo bucle.
    //Multiplica por 100 y suma 500 para evitar numeros flotantes negativos
    byte_buffer_telemetria = (variable_a_ajutar_1 + 500) * 100;
    //Envia el primer byte de la configuración ajustable 1.
    byte_envio_telemetria = byte_buffer_telemetria;
  }
  //Envia el último byte de la configuración ajustable 1.
  if (contador_bucle_telemetria == 29)byte_envio_telemetria = byte_buffer_telemetria >> 8;

  //Envio de variable ajustable 2.
  if (contador_bucle_telemetria == 30) {
    //Almacenar la configuración ajustable 2, ya que puede cambiar durante el próximo bucle.
    byte_buffer_telemetria = (variable_a_ajutar_2 + 500) * 100;;
    //Envia el primer byte de la configuración ajustable 2.
    byte_envio_telemetria = byte_buffer_telemetria;
  }
  //Envia el último byte de la configuración ajustable 2.
  if (contador_bucle_telemetria == 31)byte_envio_telemetria = byte_buffer_telemetria >> 8;
  
  //Envio de variable ajustable 3.
  if (contador_bucle_telemetria == 32) {
    //Almacenar la configuración ajustable 3, ya que puede cambiar durante el próximo bucle.
    byte_buffer_telemetria = (variable_a_ajutar_3 + 500) * 100;
    //Envia el primer byte de la configuración ajustable 3.
    byte_envio_telemetria = byte_buffer_telemetria;
  }
  //Envia el último byte de la configuración ajustable 3.
  if (contador_bucle_telemetria == 33)byte_envio_telemetria = byte_buffer_telemetria >> 8;

  //Envia el byte de acuse.
  if (contador_bucle_telemetria == 34)byte_envio_telemetria = byte_acuse;


  //Después de 125 ciclos, se restablece la variable contador_bucle_telemetria.
  //De esta manera, los datos de telemetría se envían cada medio segundo.
  if (contador_bucle_telemetria == 125)contador_bucle_telemetria = 0;

  //Envía el byte_envio_telemetria a través del protocolo serie mediante la salida PB0.
  //Envía un bit de inicio primero.
  if (contador_bucle_telemetria <= 34) {
    byte_acuse ^= byte_envio_telemetria;
    //Restablece la salida PB0 a 0 para crear un bit de inicio.
    GPIOB_BASE->BSRR = 0b1 << 16;
    //Actualiza el valor de delay_micros.
    delay_micros = micros() + 104;
    //Retrasa 104 microsegundos (1 s / 9600 bps).
    while (delay_micros > micros());
    //Crea un bucle para cada bit.
    for (contador_bit_telemetria = 0; contador_bit_telemetria < 8; contador_bit_telemetria ++) {
      //Si el bit específico es 1, establece la salida PB0 en 1.
      if (byte_envio_telemetria >> contador_bit_telemetria & 0b1) GPIOB_BASE->BSRR = 0b1 << 0;
      //Si el bit específico es 0, restablece la salida PB0 a 0.
      else GPIOB_BASE->BSRR = 0b1 << 16;
      //Actualiza el valor de delay_micros.
      delay_micros = micros() + 104;
      //Retrasa 104 microsegundos (1 s / 9600 bps).
      while (delay_micros > micros());
    }
    //Envía un bit de parada, activando la salida PB0.
    GPIOB_BASE->BSRR = 0b1 << 0;
  }
}
