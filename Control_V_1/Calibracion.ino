/*
| |<--------------------->| |<--------------------->| |<--------------------->| |

Calibracion

En esta parte se manejan los procedimientos de calibración de nivel y brújula.
Se envian datos por telemetría y se maneja el encendido de los LEDs.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/

void calibrar_brujula(void) {
  //Establecer la variable calibracion_brujula_activa para deshabilitar el ajuste 
  //de los valores brutos de la brújula.
  calibracion_brujula_activa = 1;
  //El LED rojo indicará que la calibración de la brújula está activa.
  led_rojo(HIGH);
  //Apagar el LED verde.
  led_verde(LOW);
  //Permanecer en este bucle hasta que el piloto baje la palanca de pitch del transmisor.
  while (canal_2 < 1900) {
    //Enviar datos de telemetría a la estación terrestre.
    envio_telemetria();
    //Simular un bucle de programa a 250Hz.
    delayMicroseconds(3700);
    //Leer los valores brutos de la brújula.
    leer_brujula();
    
    //En las siguientes líneas se detectan y almacenan los valores máximos y mínimos de la brújula.
    if (brujula_x < brujula_valor_cal[0])brujula_valor_cal[0] = brujula_x;
    if (brujula_x > brujula_valor_cal[1])brujula_valor_cal[1] = brujula_x;
    if (brujula_y < brujula_valor_cal[2])brujula_valor_cal[2] = brujula_y;
    if (brujula_y > brujula_valor_cal[3])brujula_valor_cal[3] = brujula_y;
    if (brujula_z < brujula_valor_cal[4])brujula_valor_cal[4] = brujula_z;
    if (brujula_z > brujula_valor_cal[5])brujula_valor_cal[5] = brujula_z;
  }
  //Reinicia la variable calibracion_brujula_activa.
  calibracion_brujula_activa = 0;

  //Los valores máximos y mínimos se almacenan son necesarios para el próximo inicio y.
  for (error = 0; error < 6; error ++) EEPROM.write(0x10 + error, brujula_valor_cal[error]);

  //Inicializa la brújula y configura los registros.
  configurar_brujula();
  //Lee y calcula los datos de la brújula.
  leer_brujula();
  //Establecer el rumbo inicial de la brújula.
  angulo_yaw = rumbo_actual_brujula;

  //Desactiva el LED rojo.
  led_rojo(LOW);
  //Parpadea el LED verde 16 veces.
  for (error = 0; error < 15; error ++) {
    led_verde(HIGH);
    delay(50);
    led_verde(LOW);
    delay(50);
  }

  error = 0;
  //Configurar el temporizador para el próximo bucle.
  temporizador_bucle = micros();
}


void calibrar_nivel(void) {
  //Activa la variable calibracion_nivel_activa.
  calibracion_nivel_activa = 1;

  //Permanecer en este bucle hasta que el piloto aumente la palanca de pitch del transmisor.
  while (canal_2 < 1100) {
    //Enviar datos de telemetría a la estación terrestre.
    envio_telemetria();
    delay(10);
  }

  //Establece el estado de los LEDs para indicar la calibración.
  led_rojo(HIGH);
  led_verde(LOW);

  //Inicializa las variables del acelerómetro.
  acel_pitch_valor_cal = 0;
  acel_roll_valor_cal = 0;

  //Se calculará un promedio para los valores de pitch y roll.
  for (error = 0; error < 64; error ++) {
    //Enviar datos de telemetría a la estación terrestre.
    envio_telemetria();

    //Lee los datos del giroscopio.
    leer_giroscopio();
    //Agrega los datos leidos.
    acel_pitch_valor_cal += acel_y;
    acel_roll_valor_cal += acel_x;

    //Limita el valor del acelerómetro.
    if (acel_y > 500 || acel_y < -500)error = 80;
    if (acel_x > 500 || acel_x < -500)error = 80;

    delayMicroseconds(3700);
  }

  //Calcula el valor promedio de pitch y roll
  acel_pitch_valor_cal /= 64;
  acel_roll_valor_cal /= 64;

  //Desactiva el LED rojo
  led_rojo(LOW);

  if (error < 80) {
    //Guarda los valores en la EEPROM
    EEPROM.write(0x16, acel_pitch_valor_cal);
    EEPROM.write(0x17, acel_roll_valor_cal);
    //EEPROM.write(0x10 + error, brujula_valor_cal[error]);

    //Parpadea el LED verde 16 veces.
    for (error = 0; error < 15; error ++) {
      led_verde(HIGH);
      delay(50);
      led_verde(LOW);
      delay(50);
    }
    error = 0;
  }

  else error = 3;
  //Desactiva la variable calibracion_nivel_activa.
  calibracion_nivel_activa = 0;
  //Lee los valores del giroscopio.
  leer_giroscopio();
  
  //Calcula la magnitud del vector total del acelerómetro.
  acel_vector_total = sqrt((acel_x * acel_x) + (acel_y * acel_y) + (acel_z * acel_z));

  //Evitar que la función asin produzca un NaN.
  
  if (abs(acel_y) < acel_vector_total) {
    //Calcula el ángulo pitch.
    //180/pi = 57.296.
    acel_angulo_pitch = asin((float)acel_y / acel_vector_total) * 57.296;
  }

  //Evitar que la función asin produzca un NaN.
  if (abs(acel_x) < acel_vector_total) {   
    //Calcula el ángulo roll.
    //180/pi = 57.296.
    acel_angulo_roll = asin((float)acel_x / acel_vector_total) * 57.296;
  }
  //Establece el ángulo pitch del giroscopio igual al ángulo de pitch del acelerómetro cuando se inicia el cuadricóptero.
  angulo_pitch = acel_angulo_pitch;
  //Establece el ángulo roll del giroscopio igual al ángulo de roll del acelerómetro cuando se inicia el cuadricóptero.
  angulo_roll = acel_angulo_roll;

  //Configurar el temporizador para el próximo bucle.
  temporizador_bucle = micros();
}
