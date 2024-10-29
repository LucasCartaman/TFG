/*
| |<--------------------->| |<--------------------->| |<--------------------->| |

Configuraciones.

Este código se encarga de cambiar la configuración del sistema. Al principio,
establece ciertas variables ajustables según otras variables específicas.
Luego, espera durante un período y envía telemetría. Durante este tiempo,
monitorea las señales de los canales del transmisor y ajusta las variables
ajustables en consecuencia. El bucle continúa hasta que se recibe una señal
específica en el canal 6. Finalmente, se restablece un temporizador para el
próximo ciclo y se apaga un indicador LED rojo.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/


void cambiar_configuracion(void) {
  //Configura los LEDs para indicar el cambio de configuraciones.
  led_rojo(HIGH);
  led_verde(LOW);

  //Define que variables se van a ajustar.
  //variable_a_ajutar se encuentran en Control_V_1.ino.
  conf_ajustable_1 = variable_a_ajutar_1;
  conf_ajustable_2 = variable_a_ajutar_2;
  conf_ajustable_3 = variable_a_ajutar_3;

  //Envio de datos por telemetría.
  for (error = 0; error < 150; error ++) {
    delay(20);
    envio_telemetria();
  }
  error = 0;

  //El ajuste se hará cuando el canal 6 se active.
  while (canal_6 >= 1900) {
    //Simula un bucle de 250 Hz. Se resta el tiempo de delay total del for anterior
    delayMicroseconds(3700);
    //Se envían y reciben los cambios por telemetría.
    envio_telemetria();

    //El canal 1 modifica el valor de conf_ajustable_1.
    if (canal_1 > 1550)conf_ajustable_1 += (float)(canal_1 - 1550) * 0.000001;
    if (canal_1 < 1450)conf_ajustable_1 -= (float)(1450 - canal_1) * 0.000001;
    if (conf_ajustable_1 < 0)conf_ajustable_1 = 0;
    //Guarda el valor final ajustado en variable_a_ajutar_1.
    variable_a_ajutar_1 = conf_ajustable_1;
    pid_roll_ganancia_p = pid_pitch_ganancia_p;
    
    //El canal 2 modifica el valor de conf_ajustable_2.
    if (canal_2 > 1550)conf_ajustable_2 += (float)(canal_2 - 1550) * 0.000001;
    if (canal_2 < 1450)conf_ajustable_2 -= (float)(1450 - canal_2) * 0.000001;
    if (conf_ajustable_2 < 0)conf_ajustable_2 = 0;
    //Guarda el valor final ajustado en variable_a_ajutar_2.
    variable_a_ajutar_2 = conf_ajustable_2;
    pid_roll_ganancia_i = pid_pitch_ganancia_i;

    //El canal 4 modifica el valor de conf_ajustable_3.
    if (canal_4 > 1550)conf_ajustable_3 += (float)(canal_4 - 1550) * 0.000001;
    if (canal_4 < 1450)conf_ajustable_3 -= (float)(1450 - canal_4) * 0.000001;
    if (conf_ajustable_3 < 0)conf_ajustable_3 = 0;
    //Guarda el valor final ajustado en variable_a_ajutar_3.
    variable_a_ajutar_3 = conf_ajustable_3;
    pid_roll_ganancia_d = pid_pitch_ganancia_d;
  }
  //Establecer el temporizador para el próximo bucle.
  temporizador_bucle = micros();
  //Desactiva el LED rojo para indicar el fin del ajuste.
  led_rojo(LOW);
}
