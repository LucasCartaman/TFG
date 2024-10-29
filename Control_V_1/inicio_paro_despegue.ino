/*
| |<--------------------->| |<--------------------->| |<--------------------->| |

Inicio_paro_despegue

Este código gestiona el inicio, parada y detección de despegue en un sistema.
La función inicio_paro_despegue monitorea las señales de entrada de un receptor
y toma decisiones en función de ellas. Inicia los motores si se cumplen ciertas
condiciones, detecta el despegue automático y realiza ajustes en la aceleración
y los controladores PID en consecuencia. Además, detiene los motores si se
cumplen ciertas condiciones y reinicia los controladores PID para un despegue
suave. También maneja la detección de despegue automático y ajusta la
aceleración de acuerdo con ciertos criterios.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/


void inicio_paro_despegue(void) {
  //Para iniciar los motores: aceleración baja y yaw a la izquierda.
  if (canal_3 < 1050 && canal_4 < 1050)inicio = 1;
  //Cuando la palanca de yaw vuelve a la posición central.
  if (inicio == 1 && canal_4 > 1450) {

    //Establecer la aceleración base a la variable velocidad_ralenti_motor. 
    aceleracion = velocidad_ralenti_motor;
    //Establecer el pitch del giroscopio igual al pitch del acelerómetro.
    angulo_pitch = acel_angulo_pitch;
    //Establecer el roll del giroscopio igual al roll del acelerómetro.
    angulo_roll = acel_angulo_roll;
    //Guardar la presión a nivel del suelo para cálculos de altitud.
    presion_suelo = presion_actual;
    //Establecer el yaw actual como la dirección de bloqueo de curso.
    curso_bloqueo_rumbo = angulo_yaw;
    //Guardar la aceleración cuando se inicia el cuadricóptero.
    acel_vector_total_inicial = acel_vector_total;
    
    //Cuando se tenga los satélites suficientes.
    if(satelites_usados >= 5){
      //Guardar la latitud inicial.
      lat_gps_origen = l_lat_gps;
      //Guardar la longitud inicial.
      lon_gps_origen = l_lon_gps;
      //Establecer la variable punto_origen_registrado en 1 para indicar que se guardó el punto inicial.
      punto_origen_registrado = 1;
    }
    //Si no hay satélites suficientes no se guarda el punto de origen.
    else punto_origen_registrado = 0;

    
    //Establecer la variable inicio en 2 para indicar que el cuadricóptero está iniciado.
    inicio = 2;
    //Restablecer el valor de aceleración integrado.
    alt_acel_integrado = 0;
    //Si se utiliza y es válida la aceleración manual de despegue (pulso de 1400 µs a 1600 µs).
    if (aceleracion_despegue_manual > 1300 && aceleracion_despegue_manual < 1600) {
      //Usar la aceleración manual de despegue.
      aceleracion_despegue = aceleracion_despegue_manual - 1500;
      //Establecer la detección automática de despegue en 1, indicando que el cuadricóptero está volando.
      despegue_detectado = 1;
      //Restablecer los controladores PID para un despegue suave.
      pid_roll_mem_i = 0;
      pid_roll_d_ultimo_error = 0;
      pid_roll_salida = 0;
      pid_pitch_mem_i = 0;
      pid_pitch_d_ultimo_error = 0;
      pid_pitch_salida = 0;
      pid_yaw_mem_i = 0;
      pid_yaw_d_ultimo_error = 0;
      pid_yaw_salida = 0;
    }
    //Si el valor manual de aceleración de despegue no es válido.
    else if (aceleracion_despegue_manual) {
      //Establecer el error en 5.
      error = 5;
      //No compensar la aceleración de despegue.
      aceleracion_despegue = 0;
      //Establecer la variable inicio en 0 para detener los motores.
      inicio = 0;
    }
  }

  //Deteniendo los motores: aceleradir abajo y yaw a la derecha.
  if (inicio == 2 && canal_3 < 1050 && canal_4 > 1950) {
    //Establecer la variable inicio en 0 para detener los motores.
    inicio = 0;
    //Restablecer la detección automática de despegue.
    despegue_detectado = 0; 
  }

  //Cuando el cuadricóptero está iniciado y no se detecta despegue.
  if (despegue_detectado == 0 && inicio == 2) {

    //Cuando la aceleración está a la mitad o más, aumentar la aceleración.
    if (canal_3 > 1480 && aceleracion < 1750) aceleracion++;

    //Si no se detecta el despegue cuando la aceleración ha alcanzado los 1700: error = 6.
    if (aceleracion == 1750)error = 6;

    //Cuando la aceleración está por debajo de la posición central de la palanca.
    if (canal_3 <= 1480) {
      
      //Reducir la aceleración a la variable velocidad_ralenti_motor.
      if (aceleracion > velocidad_ralenti_motor)aceleracion--;

      //Cuando la aceleración vuelve a la velocidad de ralenti, restablecer los controladores PID.
      else {
        //Restablecer los controladores PID para un despegue suave.
        pid_roll_mem_i = 0;
        pid_roll_d_ultimo_error = 0;
        pid_roll_salida = 0;
        pid_pitch_mem_i = 0;
        pid_pitch_d_ultimo_error = 0;
        pid_pitch_salida = 0;
        pid_yaw_mem_i = 0;
        pid_yaw_d_ultimo_error = 0;
        pid_yaw_salida = 0;
      }
    }

    //Se detecta un despegue cuando el cuadricóptero está acelerando.
    if (acel_z_prom_corto_total / 25 - acel_vector_total_inicial > 800) {
      //Establecer la variable de detección de despegue en 1 para indicar un despegue.
      despegue_detectado = 1;
      //Establecer el punto de ajuste de altitud en el nivel del suelo + aproximadamente 2.2 metros.
      pid_altitud_setpoint = presion_suelo - 22;

      //Si la aceleración está entre 1400 y 1700 µs durante el despegue, calcular la aceleración de despegue.
      if (aceleracion > 1400 && aceleracion < 1700) aceleracion_despegue = aceleracion - 1530;
      //De lo contrario
      else {
        //No se calcula la aceleración de despegue.
        aceleracion_despegue = 0;
        //Establecer el error en 7.
        error = 7;
      }
    }
  }
}
