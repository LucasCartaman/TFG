/*
| |<--------------------->| |<--------------------->| |<--------------------->| |

Calculos_PID

Esta subrutina se encarga de calcular las salidas de los controles PID para los 
ángulos Roll, Pitch y Yaw.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/

void calculo_pid(void) {

  /*El setpoint del PID en °/s se determina mediante la entrada del roll del receptor.
  En el caso de dividir por 3, la velocidad máxima de roll es aproximadamente 164°/s ((500-8)/3 = 164°/s).*/

  pid_roll_setpoint = 0;

  //Para mejores resultados se usa una banda muerta de 16us.
  if (pid_roll_setpoint_base > 1508)pid_roll_setpoint = pid_roll_setpoint_base - 1508;
  else if (pid_roll_setpoint_base < 1492)pid_roll_setpoint = pid_roll_setpoint_base - 1492;

  //Restar la corrección del ángulo del valor estandarizado de entrada del roll.
  pid_roll_setpoint -= nivel_ajuste_roll;
  //Dividir el setpoint del control de roll por 3 para obtener el angulo en grados.
  pid_roll_setpoint /= 3.0;
  
  /*El setpoint del PID en °/s se determina mediante la entrada del pitch del receptor.
  En el caso de dividir por 3, la velocidad máxima de pitch es aproximadamente 164°/s ((500-8)/3 = 164°/s).*/
  
  pid_pitch_setpoint = 0;

  //Para mejores resultados se usa una banda muerta de 16us.
  if (pid_pitch_setpoint_base > 1508)pid_pitch_setpoint = pid_pitch_setpoint_base - 1508;
  else if (pid_pitch_setpoint_base < 1492)pid_pitch_setpoint = pid_pitch_setpoint_base - 1492;

  //Restar la corrección del ángulo del valor estandarizado de entrada del pitch.
  pid_pitch_setpoint -= nivel_ajuste_pitch;
  //Dividir el setpoint del control de pitch por 3 para obtener el angulo en grados.
  pid_pitch_setpoint /= 3.0;

  pid_yaw_setpoint = 0;

  //El yaw solo funciona cuando los motores están funcionando.
  if (canal_3 > 1050) {
    //Para mejores resultados se usa una banda muerta de 16us.
    if (canal_4 > 1508)pid_yaw_setpoint = (canal_4 - 1508) / 3.0;
    else if (canal_4 < 1492)pid_yaw_setpoint = (canal_4 - 1492) / 3.0;
  }

  //Cálculos para el roll.

  //Error entre el ángulo roll y su respectivo setpoint.
  pid_error_temp = entrada_giroscopio_roll - pid_roll_setpoint;
  //La parte integral del control PID es una suma de errores.
  pid_roll_mem_i += pid_roll_ganancia_i * pid_error_temp;
  //Se limita la salida de la parte integral por seguridad.
  if (pid_roll_mem_i > pid_roll_max)pid_roll_mem_i = pid_roll_max;
  else if (pid_roll_mem_i < pid_roll_max * -1)pid_roll_mem_i = pid_roll_max * -1;

  /*
  Salida total del control PID para el ángulo de Roll:
  P = pid_roll_ganancia_p * pid_error_temp
  I = pid_roll_mem_i
  D = pid_roll_ganancia_d * (pid_error_temp - pid_roll_d_ultimo_error)
  */

  pid_roll_salida = pid_roll_ganancia_p * pid_error_temp + pid_roll_mem_i + pid_roll_ganancia_d * (pid_error_temp - pid_roll_d_ultimo_error);

  //Por seguridad, se limita la salida total del control PID.
  if (pid_roll_salida > pid_roll_max)pid_roll_salida = pid_roll_max;
  else if (pid_roll_salida < pid_roll_max * -1)pid_roll_salida = pid_roll_max * -1;

  //Actualiza el valor de pid_roll_d_ultimo_error para el control D.
  pid_roll_d_ultimo_error = pid_error_temp;

  //Cálculos para el pitch

  //Error entre el ángulo pitch y su respectivo setpoint.
  pid_error_temp = entrada_giroscopio_pitch - pid_pitch_setpoint;
  //La parte integral del control PID es una suma de errores.
  pid_pitch_mem_i += pid_pitch_ganancia_i * pid_error_temp;
  //Se limita la salida de la parte integral por seguridad.
  if (pid_pitch_mem_i > pid_pitch_max)pid_pitch_mem_i = pid_pitch_max;
  else if (pid_pitch_mem_i < pid_pitch_max * -1)pid_pitch_mem_i = pid_pitch_max * -1;

  /*
  Salida total del control PID para el ángulo de Roll:
  P = pid_pitch_ganancia_p * pid_error_temp
  I = pid_pitch_mem_i
  D = pid_pitch_ganancia_d * (pid_error_temp - pid_roll_d_ultimo_error)
  */

  pid_pitch_salida = pid_pitch_ganancia_p * pid_error_temp + pid_pitch_mem_i + pid_pitch_ganancia_d * (pid_error_temp - pid_pitch_d_ultimo_error);
  
  //Por seguridad, se limita la salida total del control PID.
  if (pid_pitch_salida > pid_pitch_max)pid_pitch_salida = pid_pitch_max;
  else if (pid_pitch_salida < pid_pitch_max * -1)pid_pitch_salida = pid_pitch_max * -1;

  //Actualiza el valor de pid_pitch_d_ultimo_error para el control D.
  pid_pitch_d_ultimo_error = pid_error_temp;

  //Cálculos para el yaw.

  //Error entre el ángulo yaw y su respectivo setpoint.
  pid_error_temp = entrada_giroscopio_yaw - pid_yaw_setpoint;
  //La parte integral del control PID es una suma de errores.
  pid_yaw_mem_i += pid_yaw_ganancia_i * pid_error_temp;
  //Se limita la salida de la parte integral por seguridad.
  if (pid_yaw_mem_i > pid_yaw_max)pid_yaw_mem_i = pid_yaw_max;
  else if (pid_yaw_mem_i < pid_yaw_max * -1)pid_yaw_mem_i = pid_yaw_max * -1;


  /*
  Salida total del control PID para el ángulo de Yaw:
  P = pid_yaw_ganancia_p * pid_error_temp
  I = pid_yaw_mem_i
  D = pid_yaw_ganancia_d * (pid_error_temp - pid_yaw_d_ultimo_error)
  */

  pid_yaw_salida = pid_yaw_ganancia_p* pid_error_temp + pid_yaw_mem_i + pid_yaw_ganancia_d * (pid_error_temp - pid_yaw_d_ultimo_error);

  //Por seguridad, se limita la salida total del control PID.
  if (pid_yaw_salida > pid_yaw_max)pid_yaw_salida = pid_yaw_max;
  else if (pid_yaw_salida < pid_yaw_max * -1)pid_yaw_salida = pid_yaw_max * -1;

  //Actualiza el valor de pid_yaw_d_ultimo_error para el control D.
  pid_yaw_d_ultimo_error = pid_error_temp;
}
