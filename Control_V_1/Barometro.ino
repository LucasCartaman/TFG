/*
| |<--------------------->| |<--------------------->| |<--------------------->| |

Barometro

Lee datos de un barómetro MS-5611, calcula la temperatura y presión atmosférica,
y controla la altitud de un cuadricóptero utilizando un controlador PID. 
Implementa filtros para suavizar las lecturas y ajusta dinámicamente los parámetros
del controlador. Además, detecta cambios manuales en la altitud del cuadricóptero.

Cada llamada de esta función incrementa la variable contador_barometro, para
cada valor se ejecuta una serie de pasos, cuando se llama la funcion leer_barometro
por tercera vez, se obtendrán los resultados del control PID.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/

void leer_barometro(void) {
  //Incrementa contado_barómetro.
  contador_barometro ++;

  //En la primera llamada se obtienen los valores de temperatura y presion.
  if (contador_barometro == 1) {     
    /*Si contador_temperatura es 0, aun no se tiene el valor de la temperatura
    por lo que se debe adquirir.*/                                      
    if (contador_temperatura == 0) {
      //Inicia la transmisión con el MS5611.
      HWire.beginTransmission(direccion_MS5611);
      //Activa el registro de salida, de acuerdo con el datasheet.
      HWire.write(0x00);
      //Finaliza la transmisión con el MS5611.
      HWire.endTransmission();
      /*Guarda los 3 bytes enviados por el MS5611. El comando responsable de
      requerir dichos bytes se encuentra más abajo*/
      HWire.requestFrom(direccion_MS5611, 3);
      //Para evitar picos, se hará un promedio de 5 lecturas usando una memoria rotativa.

      //Resta la posición de memoria actual para hacer espacio para el nuevo valor.
      temperatura_prom_total_bruta -= mem_rotativa_temperatura_bruta[direccion_mem_temperatura_prom];
      //Calcula el nuevo cambio entre la presión actual y la medición anterior.
      mem_rotativa_temperatura_bruta[direccion_mem_temperatura_prom] = HWire.read() << 16 | HWire.read() << 8 | HWire.read();
      //Añade el nuevo valor al valor promedio a largo plazo.
      temperatura_prom_total_bruta += mem_rotativa_temperatura_bruta[direccion_mem_temperatura_prom];
      //Incrementa la dirección de la memoria rotativa.
      direccion_mem_temperatura_prom++;
      //Reinicia la dirección cuando esta llega a la posición 5.
      if (direccion_mem_temperatura_prom == 5)direccion_mem_temperatura_prom = 0;
      //Calcula el promedio de las últimas 5 mediciones.
      temperatura_bruta = temperatura_prom_total_bruta / 5;
    }
    //Si ya se midió la temperatura, se mide la presión.
    else {
      //Inicia la transmisión con el MS5611
      HWire.beginTransmission(direccion_MS5611);
      //Activa el registro de salida, de acuerdo con el datasheet.
      HWire.write(0x00);
      //Finaliza la transmisión con el MS5611.
      HWire.endTransmission();
      /*Guarda los 3 bytes enviados por el MS5611. El comando responsable de
      requerir dichos bytes se encuentra más abajo*/
      HWire.requestFrom(direccion_MS5611, 3);
      /*Se desplaza los 3 bytes guardados para ubicarlos en la posición
      correcta y luego se suman para obtener el valor bruto.*/
      presion_bruta = HWire.read() << 16 | HWire.read() << 8 | HWire.read();
    }
    /*Aumenta el contador de la temperatura, indica cuantas veces se leyó
    el barómetro*/
    contador_temperatura ++;
    //Si la temperatura no se leyó en 20 bucles, se debe actualizar el valor de la temperatura
    if (contador_temperatura == 20) {
      //Reinicia el contador de la temperatura.
      contador_temperatura = 0;
      //Inicia la comunicación con el MS5611
      HWire.beginTransmission(direccion_MS5611);
      //Comando para requerir los bytes de temperatura, según el datasheet.
      HWire.write(0x58);
      //Finaliza la comunicación con el MS5611
      HWire.endTransmission();
    }
    //Si aún no se hicieron 20 lecturas del sensor, se puede guardar la presión
    else {
      //Inicia la comunicación con el MS5611
      HWire.beginTransmission(direccion_MS5611);
      //Comando para requerir los bytes de presión, segundo el datasheet.
      HWire.write(0x48);
      //Finaliza la comunicación con el MS5611
      HWire.endTransmission();
    }
  }

  //En la segunda llamada se calcula el valor de la presión con la formula del fabricante.
  if (contador_barometro == 2) {
    //Para más información, revisar el datasheet.
    dT = C[5];
    dT <<= 8;
    dT *= -1;
    dT += temperatura_bruta;

    OFF = OFF_C2 + ((int64_t)dT * (int64_t)C[4]) / pow(2, 7);
    SENS = SENS_C1 + ((int64_t)dT * (int64_t)C[3]) / pow(2, 8);

    //Valor de presión final
    P = ((presion_bruta * SENS) / pow(2, 21) - OFF) / pow(2, 15);

    /*Para tener una señal más limpia se hace un promedio
    de 20 valores con una memoria rotativa*/

    //Resta la posición de memoria actual para hacer espacio para el nuevo valor.
    presion_prom_total -= mem_rotativa_presion[direccion_mem_rotativa_presion];
    //Calcula el nuevo cambio entre la presión actual y la medición anterior.
    mem_rotativa_presion[direccion_mem_rotativa_presion] = P;
    //Añade el nuevo valor al valor promedio a largo plazo.
    presion_prom_total += mem_rotativa_presion[direccion_mem_rotativa_presion];
    //Incrementa la dirección de la memoria rotativa.
    direccion_mem_rotativa_presion++;
    //Reinicia la dirección cuando ésta llega a la posición 20.
    if (direccion_mem_rotativa_presion == 20)direccion_mem_rotativa_presion = 0;
    //Calcula el promedio de las 20 lecturas.
    presion_actual_rapida = (float)presion_prom_total / 20.0;

    
    /*Para obtener mejores resultados, se usa un filtro complementario que puede
    ajustarse mediante el promedio rápido.*/
    presion_actual_lenta = presion_actual_lenta * (float)0.985 + presion_actual_rapida * (float)0.015;
    //Calcula la diferencia entre el promedio lento y el rápido.
    diferencia_presion_actual = presion_actual_lenta - presion_actual_rapida;
    //Limite superior de 8.
    if (diferencia_presion_actual > 8)diferencia_presion_actual = 8;
    //Limite inferior de -8.
    if (diferencia_presion_actual < -8)diferencia_presion_actual = -8;
    /*Si la diferencia está fuera del rango [1, -1], el promedio lento se ajusta en función
    del error entre el promedio rápido y el lento.*/
    if (diferencia_presion_actual > 1 || diferencia_presion_actual < -1)presion_actual_lenta -= diferencia_presion_actual / 6.0;
    
    //Valor de presión usado para los cálculos.
    presion_actual = presion_actual_lenta;
    
  }
  

  //En la tercera llamada de la función, se calcula todo lo relacionado con el PID
  if (contador_barometro == 3) { 
    //Reinicio de la variable contador_barometro.
    contador_barometro = 0;

    /*En la siguiente parte, se utiliza un búfer rotativo para calcular el cambio a largo plazo entre las diversas
    mediciones de presión. Este valor total se puede utilizar para detectar la dirección (arriba/abajo) y la 
    velocidad del cuadricóptero, y funciona como la parte derivativa del controlador PID total, el cual actua
    amortiguando la salida total del control PID.*/
    
    //Durante un cambio de altitud manual, se desactiva la detección arriba/abajo
    if (cambio_altitud_manual == 1)presion_amortiguador_anterior = presion_actual * 10;
    //Resta la posición de memoria actual para hacer espacio para el nuevo valor.
    aceleracion_amortiguador -= buffer_amortiguador[direccion_mem_rotativa_amortiguador];   
    //Calcula el nuevo cambio entre la presión actual y la medición anterior.
    buffer_amortiguador[direccion_mem_rotativa_amortiguador] = presion_actual * 10 - presion_amortiguador_anterior;
    //Añade el nuevo valor al valor promedio a largo plazo.
    aceleracion_amortiguador += buffer_amortiguador[direccion_mem_rotativa_amortiguador];
    //Almacena la medición actual para el próximo bucle.
    presion_amortiguador_anterior = presion_actual * 10;
    //Incrementa la dirección de la memoria rotativa.
    direccion_mem_rotativa_amortiguador++;
    //Reinicia la dirección cuando ésta llega a la posición 30.
    if (direccion_mem_rotativa_amortiguador == 30)direccion_mem_rotativa_amortiguador = 0;

    // Si el cuadricóptero está en modo de altitud y volando.
    if (modo_vuelo >= 2 && despegue_detectado == 1) {
      // Si aún no está establecido, establece el punto de ajuste de altitud del PID.
      if (pid_altitud_setpoint == 0)pid_altitud_setpoint = presion_actual;

      /* Cuando la posición del joystick de aceleración se incrementa o disminuye, la función 
      de mantenimiento de altitud se deshabilita parcialmente. La variable "cambio_altitud_manual"
      indicará si la altitud del cuadricóptero ha sido cambiada por el piloto.*/

      //Inicialización de variables.
      cambio_altitud_manual = 0;
      aceleracion_manual = 0;

      //Si el acelerador se incrementa por encima de 1600 μs (60%).
      if (canal_3 > 1600) {
        //Establece la variable "cambio_altitud_manual" en 1 para indicar que la altitud ha sido ajustada.
        cambio_altitud_manual = 1;
        //Ajusta el setpoint al valor de presión actual para que la salida de los controladores P e I sea 0.
        pid_altitud_setpoint = presion_actual;
        //Para evitar cambios muy rápidos en la altura, se limita la aceleración.
        aceleracion_manual = (canal_3 - 1600) / 3;
      }

      // Si el acelerador se reduce por debajo de 1400 μs (40%).
      if (canal_3 < 1400) {
        //Establece la variable "cambio_altitud_manual" en 1 para indicar que la altitud ha sido ajustada.
        cambio_altitud_manual = 1;
        //Ajusta el setpoint al valor de presión actual para que la salida de los controladores P e I sea 0.
        pid_altitud_setpoint = presion_actual;
        //Para evitar cambios muy rápidos en la altura, se limita la aceleración.
        aceleracion_manual = (canal_3 - 1400) / 5;
      }

      //Calcula la salida del PID para el mantenimiento de altitud.

      //Establece la entrada del control PID.
      pid_altitud_entrada = presion_actual;

      //Calcula el error entre el setpoint y el valor actual.
      pid_error_temp = pid_altitud_entrada - pid_altitud_setpoint;

      /*Para obtener mejores resultados, se aumenta la ganancia P cuando el error aumenta.
      La variable "pid_altitud_ganancia_error" se utilizará para ajustar la ganancia P del controlador PID.*/

      pid_altitud_ganancia_error = 0;

      //Si el error está fuera del rango [10, -10]
      if (pid_error_temp > 10 || pid_error_temp < -10) {
        //Se calcula el valor positivo de pid_altitud_ganancia_error en base al error.
        pid_altitud_ganancia_error = (abs(pid_error_temp) - 10) / 20.0;
        //Para evitar ganancias muy elevadas, se limita pid_altitud_ganancia_error a 3.
        if (pid_altitud_ganancia_error > 3)pid_altitud_ganancia_error = 3;
      }

      /*En la siguiente sección se calcula la salida I, la cual es una acumulación de errores
      a lo largo del tiempo.Se elimina el factor tiempo ya que el bucle del programa se ejecuta a 250Hz. */

      pid_altitud_mem_i += (pid_altitud_ganancia_i / 100.0) * pid_error_temp;

      //Para evitar salidas muy elevadas se establecen los límites de saturación.
      if (pid_altitud_mem_i > pid_altitud_max)pid_altitud_mem_i = pid_altitud_max;
      else if (pid_altitud_mem_i < pid_altitud_max * -1)pid_altitud_mem_i = pid_altitud_max * -1;

      /*La siguiente linea calcula la salida total del control PID
      P = (pid_altitud_ganancia_p + pid_altitud_ganancia_error) * pid_error_temp.
      I = pid_altitud_mem_i += (pid_altitud_ganancia_i / 100.0) * pid_error_temp
      D = pid_altitud_ganancia_d * aceleracion_amortiguador.*/

      pid_altitud_salida = (pid_altitud_ganancia_p + pid_altitud_ganancia_error) * pid_error_temp + pid_altitud_mem_i + pid_altitud_ganancia_d * aceleracion_amortiguador;

      //Para evitar salidas muy elevadas se establecen los límites de saturación.
      if (pid_altitud_salida > pid_altitud_max)pid_altitud_salida = pid_altitud_max;
      else if (pid_altitud_salida < pid_altitud_max * -1)pid_altitud_salida = pid_altitud_max * -1;
    }

    /* Si la función de mantenimiento de altitud está desactivada, algunas variables deben reiniciarse
    para garantizar un inicio sin problemas cuando la función de mantenimiento de altitud se vuelva a activar.*/
    else if (modo_vuelo < 2 && pid_altitud_setpoint != 0) {
      pid_altitud_setpoint = 0;
      pid_altitud_salida = 0;
      pid_altitud_mem_i = 0;
      aceleracion_manual = 0;
      cambio_altitud_manual = 1;
    }
  }
}
