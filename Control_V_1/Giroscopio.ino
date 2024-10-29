/*
| |<--------------------->| |<--------------------->| |<--------------------->| |

Giroscopio

Esta sección se encarga de configurar y calibrar un giroscopio MPU-6050.
La función configurar_giroscopio establece varios registros del MPU-6050
para configurar la escala del giroscopio y acelerómetro, así como otros
parámetros relacionados. Luego, la función calibrar_giroscopio realiza la
calibración del giroscopio tomando 2000 lecturas y calculando el promedio de
los valores de offset para el rollo, la inclinación y el guiñada.

Finalmente, la función leer_giroscopio lee los datos brutos del giroscopio y
el acelerómetro desde el MPU-6050, realiza ajustes de calibración y proporciona
los valores finales para el roll, pitch y yaw. Además, esta función maneja la
lógica de inversión de direcciones y ajustes de calibración manual en función
de ciertas condiciones.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/

void configurar_giroscopio(void) {
  //Inicia la comunicación con el MPU-6050.
  HWire.beginTransmission(direccion_giroscopio);
  //Se debe escribir en el registro PWR_MGMT_1 (hex 6B).
  HWire.write(0x6B);
  //Configura los bits del registro como 00000000 para activar el giroscopio.
  HWire.write(0x00);
  //Finaliza la comunicación con el MPU-6050.
  HWire.endTransmission();

  //Inicia la comunicación con el MPU-6050.
  HWire.beginTransmission(direccion_giroscopio);
  //Se debe escribir en el registro GYRO_CONFIG (hex 1B).
  HWire.write(0x1B);
  //Configurar los bits del registro como 00001000 (escala completa de 500°/s).
  HWire.write(0x08);
  //Finaliza la comunicación con el MPU-6050.
  HWire.endTransmission();

  //Inicia la comunicación con el MPU-6050.
  HWire.beginTransmission(direccion_giroscopio);
  //Se debe escribir en el registro ACCEL_CONFIG (hex 1C).
  HWire.write(0x1C);
  //Configurar los bits del registro como 00010000 (rango completo de escala de +/- 8g).
  HWire.write(0x10);
  //Finaliza la comunicación con el MPU-6050.
  HWire.endTransmission();

  //Inicia la comunicación con el MPU-6050.
  HWire.beginTransmission(direccion_giroscopio);
  //Se debe escribir en el registro CONFIG (hex 1A).
  HWire.write(0x1A);
  //Configurar los bits del registro como 00000011
  //(Configura el Filtro Pasa Bajos Digital a ~43Hz).
  HWire.write(0x03);
  //Finaliza la comunicación con el MPU-6050.
  HWire.endTransmission();

  //Almacena los datos de la calibración del pitch y del roll.
  acel_pitch_valor_cal  = EEPROM.read(0x16);
  acel_roll_valor_cal  = EEPROM.read(0x17);
}

void calibrar_giroscopio(void) {
  //Inicializa la variable cal_cont en 0.
  cal_cont = 0;

  //Se toman 2000 muestras para realizar el promedio.
  if (cal_cont != 2000) {
    for (cal_cont = 0; cal_cont < 2000 ; cal_cont ++) {

      //Cambiar el estado del LED cada 25 lecturas para indicar la calibración.
      if (cal_cont % 25 == 0) digitalWrite(PB4, !digitalRead(PB4));

      //Lee la salida del giroscopio.
      leer_giroscopio();

      //Suma el valor de roll a cal_roll_giroscopio.
      cal_roll_giroscopio += roll_giroscopio;
      //Suma el valor de pitch a cal_pitch_giroscopio.
      cal_pitch_giroscopio += pitch_giroscopio;
      //Suma el valor de yaw a cal_yaw_giroscopio.
      cal_yaw_giroscopio += yaw_giroscopio;

      //Delay simular un bucle de 250Hz durante la calibración.
      delay(4);
    }

    //Activa el LED rojo.
    led_rojo(HIGH);


    //Se calculan los promedios con las sumas de las 2000 lecturas.
    cal_roll_giroscopio /= 2000;
    cal_pitch_giroscopio /= 2000;
    cal_yaw_giroscopio /= 2000;
  }
}

void leer_giroscopio(void) {
  //Inicia la comunicación con el giroscopio.
  HWire.beginTransmission(direccion_giroscopio);
  //Comenzar la lectura en el registro ACCEL_XOUT_H (hex 3B).
  HWire.write(0x3B);
  //Finaliza la transmisión.
  HWire.endTransmission();
  //Solicita 14 bytes del MPU 6050.
  HWire.requestFrom(direccion_giroscopio, 14);

  //Agrega el MSB y el LSB a la variable acel_y.
  acel_y = HWire.read() << 8 | HWire.read();
  //Agrega el MSB y el LSB a la variable acel_x.
  acel_x = HWire.read() << 8 | HWire.read();
  //Agrega el MSB y el LSB a la variable acel_z.
  acel_z = HWire.read() << 8 | HWire.read();

  //Agrega el MSB y el LSB a la temperatura.
  temperatura = HWire.read() << 8 | HWire.read();

  //Agrega el MSB y el LSB a la variable roll_giroscopio.
  roll_giroscopio = HWire.read() << 8 | HWire.read();
  //Agrega el MSB y el LSB a la variable pitch_giroscopio.
  pitch_giroscopio = HWire.read() << 8 | HWire.read();
  //Agrega el MSB y el LSB a la variable pitch_giroscopio.
  yaw_giroscopio = HWire.read() << 8 | HWire.read();

  //Invierte la dirección de los ejes.
  pitch_giroscopio *= -1;
  yaw_giroscopio *= -1;

  //Resta los valores de calibración manual a los valores del acelerómetro.
  if (calibracion_nivel_activa == 0) {
    acel_y -= acel_pitch_valor_cal;
    acel_x -= acel_roll_valor_cal;
  }

  //Resta los valores de calibración manual a los valores del giroscopio.
  if (cal_cont >= 2000) {
    roll_giroscopio -= cal_roll_giroscopio;
    pitch_giroscopio -= cal_pitch_giroscopio;
    yaw_giroscopio -= cal_yaw_giroscopio;
  }
}
