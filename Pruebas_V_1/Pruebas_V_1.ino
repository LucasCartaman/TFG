
#include <SoftWire.h>
SoftWire HWire(PB10, PB11, I2C_FAST_MODE);

//Declaración de variables globales.
uint8_t deshabilitar_acelerador;
uint8_t error;
uint32_t temporizador_bucle;
float acel_angulo_roll, acel_angulo_pitch, angulo_pitch, angulo_roll;
float voltaje_bateria;
int16_t contador_bucle;
uint8_t dato, inicio, warning;
int16_t eje_acel[4], eje_giroscopio[4], temperatura;
int32_t cal_eje_giroscopio[4], cal_eje_acel[4];
int32_t cal_int;
int32_t canal_1_inicio, canal_1;
int32_t canal_2_inicio, canal_2;
int32_t canal_3_inicio, canal_3;
int32_t canal_4_inicio, canal_4;
int32_t canal_5_inicio, canal_5;
int32_t canal_6_inicio, canal_6;
int32_t tiempo_medido, tiempo_medido_inicial;
uint8_t contador_selector_canal;

//Variables del barómetro.
uint16_t C[7];
uint8_t contador_barometro, contador_temperatura;
int64_t OFF, OFF_C2, SENS, SENS_C1, P;
uint32_t presion_bruta, temperatura_bruta, temp;
float presion_actual, presion_actual_lenta, presion_actual_rapida, diferencia_presion_actual;
float presion_suelo, presion_mantener_altitud;
int32_t dT, dT_C5;

//Variables de la brújula.
int16_t brujula_x, brujula_y, brujula_z;

//Direcciones.
uint8_t direccion_giroscopio = 0x68;
uint8_t direccion_MS5611 = 0x77;
uint8_t direccion_brujula = 0x1E;

void setup() {
  pinMode(4, INPUT_ANALOG);
  //Los puertos PB3 y PB4 se utilizan como JTDO y JNTRST de forma predeterminada.
  //La siguiente función conecta PB3 y PB4 a la función de salida alternativa.
  afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY);

  //Establece PB3 y PB4 como salida.
  pinMode(PB3, OUTPUT);
  pinMode(PB4, OUTPUT);

  //Inicializa los pines PB3 y PB4 en bajo.
  led_verde(LOW);
  led_rojo(LOW);

  //Inicia el puerto serial en 57600 kbps.
  Serial.begin(57600);
  //Dar tiempo puerto serie para iniciar y prevenir la pérdida de datos.
  delay(100);
  //Configurar los temporizadores para las entradas del receptor y las salidas de los ESC.
  conf_timer();
  //Darle tiempo a los temporizadores para que inicien.
  delay(50);

  //Inicia el puerto I2C maestro.
  HWire.begin();

  //Inicia la comunicación con el MPU6050.
  HWire.beginTransmission(direccion_giroscopio);
  //Escribe en el registro PWR_MGMT_1 (hexadecimal 6B).
  HWire.write(0x6B);
  //Establecer los bits del registro como 00000000 para activar el giroscopio.
  HWire.write(0x00);
  //Finaliza la comunicación.
  HWire.endTransmission();

  //Inicia la comunicación con el MPU6050.
  HWire.beginTransmission(direccion_giroscopio);
  //Escribe en el registro GYRO_CONFIG (hexadecimal 1B).
  HWire.write(0x1B);
  //Establecer los bits del registro como 00001000 (500dps a escala completa).
  HWire.write(0x08);
  //Finaliza la comunicación.
  HWire.endTransmission();

  //Inicia la comunicación con el MPU6050.
  HWire.beginTransmission(direccion_giroscopio);
  //Escribe en el registro ACCEL_CONFIG (hexadecimal 1C).
  HWire.write(0x1C);
  //Establecer los bits del registro como 00010000 (+/- 8g rango a escala completa).
  HWire.write(0x10);
  //Finaliza la comunicación.
  HWire.endTransmission();

  //Inicia la comunicación con el MPU6050.
  HWire.beginTransmission(direccion_giroscopio);
  //Escribe en el registro CONFIG (hexadecimal 1A).
  HWire.write(0x1A);
  //Establecer los bits del registro como 00000011 (Configurar el Filtro Paso Bajo Digital a ~43Hz).
  HWire.write(0x03);
  //Finaliza la comunicación.
  HWire.endTransmission();

  //Imprime la portada
  portada();
}

void loop() {
  delay(10);
  //Si el puerto serial esta disponible.
  if (Serial.available() > 0) {
    //Lee el byte entrante
    dato = Serial.read();
    //Esperar a que entren otros bytes.
    delay(100);
    //Vaciar el búfer serial.
    while (Serial.available() > 0)contador_bucle = Serial.read();
    //Deshabilitar el acelerador.
    deshabilitar_acelerador = 1;
  }

  //Si la aceleración no está deshabilitada.
  if (!deshabilitar_acelerador) {
    //Establecer el pulso de entrada de aceleración del receptor al pulso de salida de los ESCs.
    TIMER4_BASE->CCR1 = canal_3;
    TIMER4_BASE->CCR2 = canal_3;
    TIMER4_BASE->CCR3 = canal_3;
    TIMER4_BASE->CCR4 = canal_3;
  }
  //Si la aceleración está deshabilitada.
  else {
    //Establecer la salida de los ESCs a 1000us para desactivar los motores.
    TIMER4_BASE->CCR1 = 1000;
    TIMER4_BASE->CCR2 = 1000;
    TIMER4_BASE->CCR3 = 1000;
    TIMER4_BASE->CCR4 = 1000;
  }

  //Si se envía el carácter "a".
  if (dato == 'a') {
    Serial.println(F("Leyendo de pulsos de entrada del receptor."));
    Serial.println(F("Puede salir enviando un s (salir)."));
    delay(2500);
    //Leer la señal dle receptor.
    leer_senal_receptor();
  }


//Si se envía el carácter "b".
  if (dato == 'b') {
    Serial.println(F("Iniciando escáner I2C"));
    //Escanear las direcciones I2C.
    escaner_i2c();
  }

  //Si se envía el carácter "c".
  if (dato == 'c') {
    Serial.println(F("Leyendo valores brutos del giroscopio."));
    Serial.println(F("Puede salir enviando un s (salir)."));
    //Lee los valores brutos del giroscopio.
    leer_valores_giroscopio();
  }

  //Si se envía el carácter "d".
  if (dato == 'd') {
    Serial.println(F("Leyendo valores brutos del acelerómetro."));
    Serial.println(F("Puede salir enviando un s (salir)."));
    delay(2500);
    //Lee los valores brutos del acelerómetro.
    leer_valores_giroscopio();
  }

  //Si se envía el carácter "e".
  if (dato == 'e') {
    Serial.println(F("Leyendo ángulos de la IMU."));
    Serial.println(F("Puede salir enviando un s (salir)."));
    //Lee los ángulos de la IMU.
    prueba_angulos_imu();
  }

  //Si se envía el carácter "f".
  if (dato == 'f') {
    Serial.println(F("Probando los LEDs."));
    //Prueba los LEDs.
    test_leds();
  }

  //Si se envía el carácter "g".
  if (dato == 'g') {
    Serial.println(F("Leyendo el voltaje de la batería."));
    Serial.println(F("Puede salir enviando un s (salir)."));
    //Prueba la lectura del voltaje de la batería.
    prueba_voltaje_bateria();
  }

  //Si se envía el carácter "h".
  if (dato == 'h') {
    Serial.println(F("Probando barómetro MS-5611."));
    Serial.println(F("Puede salir enviando un s (salir)."));
    delay(2500);
    //Prueba el barómetro.
    prueba_barometro();
  }

  //Si se envía el carácter "i".
  if (dato == 'i') {
    Serial.println(F("Probando valores brutos del GPS."));
    //Prueba los valores brutos del GPs.
    prueba_gps();
  }

  //Si se envía el carácter "j".
  if (dato == 'j') {
    Serial.println(F("Probando brújula HMC5883L."));
    Serial.println(F("Puede salir enviando un s (salir)."));
    delay(2500);
    //Prueba los datos de la brújula.
    prueba_brujula();
  }

  ////Si se envía el carácter "1".
  if (dato == '1') {
    Serial.println(F("Probando motor 1 (delantero derecho, en sentido antihorario.)."));
    Serial.println(F("Puede salir enviando un s (salir)."));
    delay(2500);
    //Prueba la vibración del motor 1.
    prueba_vibraciones_motores();
  }

  //Si se envía el carácter "2".
  if (dato == '2') {
    Serial.println(F("Probando motor 2 (posterior derecho, en sentido horario.)."));
    Serial.println(F("Puede salir enviando un s (salir)."));
    delay(2500);
    //Prueba la vibración del motor 2.
    prueba_vibraciones_motores();
  }

  //Si se envía el carácter "3".
  if (dato == '3') {
    Serial.println(F("Probando motor 3 (posterior izquierdo, en sentido antihorario.)."));
    Serial.println(F("Puede salir enviando un s (salir)."));
    delay(2500);
    //Prueba la vibración del motor 3.
    prueba_vibraciones_motores();
  }

  //Si se envía el carácter "4".
  if (dato == '4') {
    Serial.println(F("Probando motor 4 (front lefft, clockwise direction)."));
    Serial.println(F("Puede salir enviando un s (salir)."));
    delay(2500);
    //Prueba la vibración del motor 4.
    prueba_vibraciones_motores();
  }

  //Si se envía el carácter "5".
  if (dato == '5') {
    Serial.println(F("Probando todos los motores."));
    Serial.println(F("Puede salir enviando un s (salir)."));
    delay(2500);
    //Prueba la vibración de todos los motores
    prueba_vibraciones_motores();
  }
}

//Lectura del giroscopio.
void leer_giroscopio(void) {
  //Inicia la comunicación con el giroscopio.
  HWire.beginTransmission(direccion_giroscopio);
  //Comenzar a leer en el registro 3B y autoincrementar con cada lectura.
  HWire.write(0x3B);
  //Finaliza la comunicación.
  HWire.endTransmission();
  //Solicitar 14 bytes del MPU6050.
  HWire.requestFrom(direccion_giroscopio, 14);

  //Suma el HSB y el LSB del eje x del acelerómetro.
  eje_acel[1] = HWire.read() << 8 | HWire.read();
  //Suma el HSB y el LSB del eje y del acelerómetro.
  eje_acel[2] = HWire.read() << 8 | HWire.read();
  //Suma el HSB y el LSB del eje z del acelerómetro.
  eje_acel[3] = HWire.read() << 8 | HWire.read();
  //Suma el HSB y el LSB de la temperatura.
  temperatura = HWire.read() << 8 | HWire.read();
  //Suma el HSB y el LSB del eje x del giroscopio.
  eje_giroscopio[1] = HWire.read() << 8 | HWire.read();
  //Suma el HSB y el LSB del eje y del giroscopio.
  eje_giroscopio[2] = HWire.read() << 8 | HWire.read();
  //Suma el HSB y el LSB del eje z del giroscopio.
  eje_giroscopio[3] = HWire.read() << 8 | HWire.read();
  //Invierte la dirección de los ejes z e y.
  eje_giroscopio[2] *= -1;
  eje_giroscopio[3] *= -1;

  //Para la calibración manual.
  if (cal_int >= 2000) {
    //Resta el valor de calibración manual del roll.
    eje_giroscopio[1] -= cal_eje_giroscopio[1];
    //Resta el valor de calibración manual del pitch.
    eje_giroscopio[2] -= cal_eje_giroscopio[2];
    //Resta el valor de calibración manual del yaw.
    eje_giroscopio[3] -= cal_eje_giroscopio[3];
  }
}

  //Manejo del estado de los LEDs.
  void led_rojo(int8_t nivel) {
    digitalWrite(PB4, nivel);
  }
  void led_verde(int8_t nivel) {
    digitalWrite(PB3, nivel);
  }
