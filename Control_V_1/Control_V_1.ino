
/*
| |<--------------------->| |<--------------------->| |<--------------------->| |

Control_V_1

En la fase de configuración inicial, se verifica si el cuadricóptero está
apagado (inicio == 0). En esta etapa, se pueden realizar calibraciones
específicas, como la calibración de la brújula (calibrar_brujula()) y la
calibración de nivel (calibrar_nivel()), activadas por movimientos particulares
de los controles. Además, hay condiciones para cambiar configuraciones cuando
se detectan movimientos específicos en el canal 6.

Luego, se establecen condiciones y modos de vuelo. Dependiendo de la posición de
los canales 5 y 6, se determina el modo de vuelo. También se lleva a cabo el
manejo de la orientación del cuadricóptero mediante cálculos basados en datos del
giroscopio, y se aplican correcciones utilizando datos del acelerómetro para
compensar la deriva.

El código incluye la manipulación de datos provenientes del GPS. Se implementa
una funcionalidad para volver al origen basada en coordenadas GPS, y se realizan
ajustes adicionales según la información recibida del GPS.

Se incorpora la monitorización del voltaje de la batería, utilizado para ajustar
las señales de los motores y compensar la caída de voltaje. Se establecen límites
para evitar situaciones de voltaje peligrosas.

El control de los motores se realiza mediante la generación de señales
(ESC_1, ESC_2, ESC_3, ESC_4) en función de las entradas de control de vuelo y las
salidas del controlador PID. Además, se incorpora una función de telemetría para
enviar datos a una estación terrestre.

Se implementa la gestión de errores, como la detección de un voltaje bajo de la
batería, estableciendo una bandera de error si es necesario. Finalmente, se
monitorea el tiempo del bucle para asegurarse de que se mantenga alrededor
de 4 ms.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/


//Incluye la biblioteca EEPROM.h para leer y escribir datos en la EEPROM.
#include <EEPROM.h>
//Incluye la biblioteca Wire.h para la comunicacion con el giroscopio.
#include <SoftWire.h>
//Inicia el puerto I2C 2 a 400 kHz.
SoftWire HWire(PB10, PB11, I2C_FAST_MODE);

/*
==================================================================================
Configuraciones ajustables

Cambiar "float_simulado" a la configuración que se desee ajustar.
==================================================================================
*/
#define variable_a_ajutar_1 pid_pitch_ganancia_p   
#define variable_a_ajutar_2 pid_pitch_ganancia_i
#define variable_a_ajutar_3 pid_pitch_ganancia_d

/*
==================================================================================
Control PID de ángulos, límites y ganancias
==================================================================================
*/

//Control PID del roll.

//Configuración de ganancia para el controlador P (predeterminado = 2.2).
float pid_roll_ganancia_p = 2.2;
//Configuración de ganancia para el controlador I (predeterminado = 0.04).
float pid_roll_ganancia_i = 0.04;
//Configuración de ganancia para el controlador D (predeterminado = 15.0).
float pid_roll_ganancia_d = 20.0;
//Máxima salida del controlador PID (predeterminado = 400).
int pid_roll_max = 400;

//Control PID del pitch.

//Configuración de ganancia para el controlador P.
float pid_pitch_ganancia_p = pid_roll_ganancia_p;
//Configuración de ganancia para el controlador I.
float pid_pitch_ganancia_i = pid_roll_ganancia_i;
//Configuración de ganancia para el controlador D.
float pid_pitch_ganancia_d = pid_roll_ganancia_d;
//Máxima salida del controlador PID (predeterminado = 400).
int pid_pitch_max = pid_roll_max;

//Control PID del yaw.

//Configuración de ganancia para el controlador P (predeterminado = 4.0).
float pid_yaw_ganancia_p= 4.0;
//Configuración de ganancia para el controlador I (predeterminado = 0.02).
float pid_yaw_ganancia_i = 0.02;
//Configuración de ganancia para el controlador D (predeterminado = 0.0).
float pid_yaw_ganancia_d = 0.0;
//Máxima salida del controlador PID (predeterminado = 400).
int pid_yaw_max = 400;

//Control PID de la altura.

//Configuración de ganancia para el controlador P (predeterminado = 1.4).
float pid_altitud_ganancia_p = 1.4;
//Configuración de ganancia para el controlador I (predeterminado = 0.2).
float pid_altitud_ganancia_i = 0.2;
//Configuración de ganancia para el controlador D (predeterminado = 0.75).
float pid_altitud_ganancia_d = 0.75;
//Máxima salida del controlador PID (predeterminado = 400).
int pid_altitud_max = 400;

//Control PID del gps.

//Configuración de ganancia para el controlador P (predeterminado = 2.7).
float gps_ganancia_p = 2.7;
//Configuración de ganancia para el controlador D (predeterminado = 6.5).
float gps_ganancia_d = 6.5;

/*
==================================================================================
Batería

Durante el vuelo, la tensión de la batería disminuye y los motores giran a una 
velocidad más baja. Esto tiene un efecto negativo en la función de mantenimiento
de altitud. Con la variable compensacion_bateria, es posible compensar la caída
de tensión de la batería.
==================================================================================

*/

//Compensación por la caida de voltaje de la batería (predeterminado = 40).
float compensacion_bateria = 40.0;
//Advertencia de batería a 10.5V (predeterminado = 10.5V).
float aviso_bateria_baja = 10.5;
//Offset de calibración para el voltaje de la batería.
float calibracion_voltaje_bateria = 0.0; 

/*
==================================================================================
Direcciones
==================================================================================
*/

//La dirección I2C del MPU-6050 es 0x68 en forma hexadecimal.
uint8_t direccion_giroscopio = 0x68;
//La dirección I2C del MS5611 es 0x77 en forma hexadecimal.
uint8_t direccion_MS5611 = 0x77; 
//La dirección I2C del MPU-HMC5883L es 0x1E en forma hexadecimal.
uint8_t direccion_brujula = 0x1E;

/*
==================================================================================
Definiciones varias
==================================================================================
*/

//Establecer la declinación entre el norte magnético y el norte geográfico.
float declinacion = -17.3;
//Ingresa el punto de despegue cuando no se desea la detección automática de despegue
//(entre 1400 y 1600).
int16_t aceleracion_despegue_manual = 1350;
//Ingresa el pulso mínimo de aceleración de los motores cuando están en ralentí
//(entre 1000 y 1200).
int16_t velocidad_ralenti_motor = 1100;
//Define el GPIO al que va conectado el LED de la placa Blue Pill.
#define LED_STM32 PC13

/*
==================================================================================
Declaración de variables.
==================================================================================
*/

uint8_t canal_1_final, canal_2_final, canal_3_final, canal_4_final;
uint8_t byte_acuse, inicio;
uint8_t error, contador_error, led_error;
uint8_t modo_vuelo, contador_modo_vuelo, led_modo_vuelo;
uint8_t despegue_detectado, cambio_altitud_manual;
uint8_t byte_envio_telemetria, contador_bit_telemetria, contador_bucle_telemetria;
uint8_t contador_selector_canal;
uint8_t calibracion_nivel_activa;

uint32_t byte_buffer_telemetria;

int16_t ESC_1, ESC_2, ESC_3, ESC_4;
int16_t aceleracion_manual;
int16_t aceleracion, aceleracion_despegue, cal_cont;
int16_t temperatura, contador_var;
int16_t acel_x, acel_y, acel_z;
int16_t pitch_giroscopio, roll_giroscopio, yaw_giroscopio;
float ajuste_acc_roll = 0.001;
float ajuste_acc_pitch = -0.0005;

int32_t canal_1_inicio, canal_1, canal_1_base, pid_roll_setpoint_base;
int32_t canal_2_inicio, canal_2, canal_2_base, pid_pitch_setpoint_base;
int32_t canal_3_inicio, canal_3;
int32_t canal_4_inicio, canal_4;
int32_t canal_5_inicio, canal_5;
int32_t canal_6_inicio, canal_6;
int32_t tiempo_medido, tiempo_medido_inicial, vigilia_receptor;
int32_t acel_vector_total, acel_vector_total_inicial;
int32_t cal_roll_giroscopio, cal_pitch_giroscopio, cal_yaw_giroscopio;
int16_t acel_pitch_valor_cal;
int16_t acel_roll_valor_cal;

int32_t acel_z_prom_corto_total, acel_z_prom_largo_total, acel_z_prom_total ;
int16_t acel_z_prom_corto[26], acel_z_prom_largo[51];

uint8_t acel_z_prom_corto_direccion_mem_rotativa, acel_z_prom_largo_direccion_mem_rotativa;

int32_t alt_acel_integrado;

uint32_t temporizador_bucle, temporizador_error, temporizador_modo_vuelo;
uint32_t delay_micros;

float nivel_ajuste_roll, nivel_ajuste_pitch;
float pid_error_temp;
float pid_roll_mem_i, pid_roll_setpoint, entrada_giroscopio_roll, pid_roll_salida, pid_roll_d_ultimo_error;
float pid_pitch_mem_i, pid_pitch_setpoint, entrada_giroscopio_pitch, pid_pitch_salida, pid_pitch_d_ultimo_error;
float pid_yaw_mem_i, pid_yaw_setpoint, entrada_giroscopio_yaw, pid_yaw_salida, pid_yaw_d_ultimo_error;
float acel_angulo_roll, acel_angulo_pitch, angulo_pitch, angulo_roll, angulo_yaw;
float voltaje_bateria, float_simulado;

//Variables de la brújula.
uint8_t calibracion_brujula_activa, bloqueo_rumbo;
int16_t brujula_x, brujula_y, brujula_z;
int16_t brujula_valor_cal[6];
float brujula_x_horizontal, brujula_y_horizontal, rumbo_actual_brujula;
float brujula_escala_y, brujula_escala_z;
int16_t brujula_offset_x, brujula_offset_y, brujula_offset_z;
float curso_a, curso_b, curso_c, curso_base_reflejado, curso_actual_reflejado;
float curso_bloqueo_rumbo, desviacion_curso_bloqueo_rumbo;

//Variable de la presión.
float pid_altitud_ganancia_error;
uint16_t C[7];
uint8_t contador_barometro, contador_temperatura, direccion_mem_temperatura_prom;
int64_t OFF, OFF_C2, SENS, SENS_C1, P;
uint32_t presion_bruta, temperatura_bruta, mem_rotativa_temperatura_bruta[6], temperatura_prom_total_bruta;
float presion_actual, presion_actual_lenta, presion_actual_rapida, diferencia_presion_actual;
float presion_suelo, presion_mantener_altitud, volver_al_origen_descenso;
int32_t dT;

//Variables del PID de altitud.
float pid_altitud_mem_i, pid_altitud_setpoint, pid_altitud_entrada, pid_altitud_salida;
uint8_t direccion_mem_rotativa_amortiguador;
int32_t buffer_amortiguador[35], aceleracion_amortiguador;
float presion_amortiguador_anterior;
int32_t mem_rotativa_presion[50], presion_prom_total;
uint8_t direccion_mem_rotativa_presion;

//Variables del GPS
uint8_t byte_leido, mensaje_entrante[100], satelites_usados, tipo_correccion;
uint8_t establecer_ruta, latitud_norte, longitud_este ;
uint16_t contador_mensaje;
int16_t gps_suma_contador;
int32_t l_lat_gps, l_lon_gps, lat_gps_anterior, lon_gps_anterior;
int32_t lat_gps_actual, lon_gps_actual, l_lat_ruta, l_lon_ruta;
float gps_pitch_ajuste_norte, gps_pitch_ajuste, gps_roll_ajuste_norte, gps_roll_ajuste;
float lat_gps_bucle_suma, lon_gps_bucle_suma, lat_gps_suma, lon_gps_suma;
uint8_t nueva_linea_encontrada, gps_dato_nuevo_disponible, gps_dato_nuevo_contador;
uint8_t direccion_mem_rotativa_gps, pasos_volver_al_origen;
int32_t gps_lat_prom_total, gps_lon_prom_total;
int32_t gps_lat_mem_rotativa[40], gps_lon_mem_rotativa[40];
int32_t gps_error_lat, gps_error_lon;
int32_t gps_error_lat_anterior, gps_error_lon_anterior;
uint32_t gps_vigilia_temporizador;

float l_lon_gps_ajuste_float, l_lat_gps_ajuste_float, gps_ajuste_rumbo_manual;
float volver_al_origen_factor_latitud, volver_al_origen_factor_longitud, volver_al_origen_factor_movimiento;
uint8_t punto_origen_registrado;
int32_t lat_gps_origen, lon_gps_origen;


//Software serial.
uint8_t ss_byte_acuse;
//uint8_t byte_temporal;
int8_t ss_flanco_subida_activo;
int16_t ss_array_tiempo[200];
int8_t ss_marca_impresion = 1;
uint8_t ss_array_tiempo_contador, ss_array_tiempo_contador_2, ss_bits_recibidos_contador;
uint8_t ss_bytes_recibidos[30], ss_nivel, ss_byte_contador, nueva_ruta_disponible;
int32_t lat_gps_ruta, lon_gps_ruta;
int32_t ss_tiempo_medido, ss_tiempo_medido_inicial, ss_ultimo_cambio_entrada, ss_ultimo_cambio_entrada_anterior;

//Navegación.
uint8_t volar_nuevo_punto, volar_nuevo_punto_paso, monitor_ruta;
float volar_ruta_factor_movimiento, volar_ruta_factor_lat, volar_ruta_factor_lon;


//Variables de configuración.
uint32_t conf_temporizador_ajuste;
uint16_t conf_contador_click;
uint8_t canal_6_anterior;
float conf_ajustable_1, conf_ajustable_2, conf_ajustable_3;

/*
==================================================================================
Rutina de Setup
==================================================================================
*/
void setup() {

  //Configuraciones de pines

  //Esto es necesario para leer el valor analógico del puerto A4.
  pinMode(4, INPUT_ANALOG);

  /*
  El puerto PB3 y PB4 se utilizan como JTDO y JNTRST de forma predeterminada.
  La siguiente función conecta PB3 y PB4 a la función de salida alternativa.
  */

  //Conecta PB3 y PB4 a la función de salida.
  afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY);

  //Configura PB3 como salida para el LED verde.
  pinMode(PB3, OUTPUT);
  //Configura PB4 como salida para el LED rojo.
  pinMode(PB4, OUTPUT);
  //Este es el LED en la placa STM32. Se utiliza para indicar la señal del GPS.
  pinMode(LED_STM32, OUTPUT);

  //Apaga el LED en la STM32. La función del LED está invertida. Ver el esquema del STM32.
  digitalWrite(LED_STM32, HIGH);
  //Establece la salida PB3 en bajo.
  led_verde(LOW);
  //Establece la salida PB4 en alto.
  led_rojo(HIGH);

  //Establece PB0 como salida para la transmisión de telemetría (TX).
  pinMode(PB0, OUTPUT);

  //Configuración de emulación de EEPROM.
  EEPROM.PageBase0 = 0x801F000;
  EEPROM.PageBase1 = 0x801F800;
  EEPROM.PageSize  = 0x400;

  //Configurar la salida serial a 57600 kbps. (Solo para depuración)
  Serial.begin(57600);
  //Dar al puerto serial algún tiempo para iniciar y evitar pérdida de datos.
  delay(250);

  //Configura los temporizadores para las entradas del receptor y las salidas de los ESC.
  conf_timer();
  //Da tiempo a los temporizadores para que inicien.
  delay(50);

  //Configura la velocidad de baudios y la frecuencia de actualización de salida del módulo GPS.
  configurar_gps();

  //Verificar si el MPU-6050 está respondiendo.

  //Inicia el I2C como maestro.
  HWire.begin();
  //Inicia la comunicación con el MPU-6050.
  HWire.beginTransmission(direccion_giroscopio);
  //Finaliza la transmisión y registra el estado de salida.
  error = HWire.endTransmission();
  
  //Permanece en este bucle porque el MPU-6050 no respondió.
  while (error != 0) {
    //Establece el estado de error en 1.
    error = 1;
    //Muestra el error a través del LED rojo.
    senal_error();
    //Simula una frecuencia de actualización de 250Hz como en el bucle principal.
    delay(4);
  }

  //Comprobar si la brújula responde.

  //Iniciar la comunicación con el HMC5883L.
  HWire.beginTransmission(direccion_brujula);
  //Finalizar la transmisión y registrar el estado de salida.
  error = HWire.endTransmission();

  //Permanecer en este bucle porque el HMC5883L no respondió.
  while (error != 0) {
    //Establecer el estado de error en 2.
    error = 2;
    //Mostrar el error a través del LED rojo.
    senal_error();
    //Simula una frecuencia de actualización de 250Hz como en el bucle principal.
    delay(4);
  }

  //Verificar si el barómetro MS5611 está respondiendo.

  //Iniciar la comunicación con el MS5611.
  HWire.beginTransmission(direccion_MS5611);
  //Finalizar la transmisión y registrar el estado de salida.
  error = HWire.endTransmission();

  //Permanecer en este bucle porque el MS5611 no respondió.
  while (error != 0) {
    //Establecer el estado de error en 3.
    error = 3;
    //Mostrar el error a través del LED rojo.
    senal_error();
    //Simula una frecuencia de actualización de 250Hz como en el bucle principal.
    delay(4);
  }

  //Inicializar el giroscopio y establecer los registros correctos.
  configurar_giroscopio();
  //Inicializar la brújula y establecer los registros correctos.
  configurar_brujula();
  //Leer y calcular el dato de la brújula.
  leer_brujula();
  //Establecer el rumbo inicial de la brújula.
  angulo_yaw = rumbo_actual_brujula;

  //Crear un retraso de 5 segundos antes de la calibración.

  //1250 bucles de 4 ms = 5 s.
  for (contador_var = 0; contador_var < 1250; contador_var++) {
    //Cada 125 bucles (500 ms).
    if (contador_var % 125 == 0) {
      //Cambia el estado del LED
      digitalWrite(PB4, !digitalRead(PB4));
    }
    //Simula una frecuencia de actualización de 250Hz como en el bucle principal.
    delay(4);
  }

  //Establecer el inicio nuevamente en 0.
  contador_var = 0;
  //Calibrar el offset del giroscopio.
  calibrar_giroscopio();

  //Esperar hasta que el receptor esté activo.
  while (canal_1 < 990 || canal_2 < 990 || canal_3 < 990 || canal_4 < 990)  {
    //Establecer el estado de error en 4.
    error = 4;
    //Mostrar el error a través del LED rojo.
    senal_error();
    //Simula una frecuencia de actualización de 250Hz como en el bucle principal.
    delay(4);
  }
  //Reinicia el estado de error en 0.
  error = 0;

  //Cuando todo esté listo, apagar el LED.

  //Establecer la salida PB4 en bajo.
  led_rojo(LOW);

  /*
  Cargar el voltaje de la batería en la variable "voltaje_bateria".

  El STM32 utiliza un convertidor analógico a digital de 12 bits.
  analogRead => 0 = 0V ..... 4095 = 3.3V

  El divisor de voltaje (1k y 10k) es de 1:11.
  analogRead => 0 = 0V ..... 4095 = 36.3V

  4095 / 36.3 = 112.81.
  */

  voltaje_bateria = (float)analogRead(4) / 112.81;

  //Para calcular la presión, es necesario extraer los 6 valores de calibración del MS5611.
  //Estos valores de 2 bytes se almacenan en la ubicación de memoria 0xA2 en adelante.
  for (inicio = 1; inicio <= 6; inicio++) {
    //Iniciar comunicación con el MPU-6050.
    HWire.beginTransmission(direccion_MS5611);
    //Enviar la dirección que queremos leer.
    HWire.write(0xA0 + inicio * 2);
    //Finalizar la transmisión.
    HWire.endTransmission();

    //Solicitar 2 bytes del MS5611.
    HWire.requestFrom(direccion_MS5611, 2);
    //Sumar el byte bajo y alto a la variable de calibración C[x].
    C[inicio] = HWire.read() << 8 | HWire.read();
  }
  
  //Estos valores están precalculados para aliviar el bucle principal.
  OFF_C2 = C[2] * pow(2, 16);
  SENS_C1 = C[1] * pow(2, 15);

  //El MS5611 necesita realizar algunas lecturas para estabilizarse.

  //El bucle se ejecuta 100 veces.
  for (inicio = 0; inicio < 100; inicio++) {
    //Lee el dato del barómetro.
    leer_barometro();
    //El bucle también se ejecuta a 250Hz (4ms por bucle).
    delay(4);
  }
  //Reinicia el valor de la presión.
  presion_actual = 0;

  //Antes de comenzar, el valor promedio del acelerómetro se gaurda en las variables.
  for (inicio = 0; inicio <= 24; inicio++)acel_z_prom_corto[inicio] = acel_z;
  for (inicio = 0; inicio <= 49; inicio++)acel_z_prom_largo[inicio] = acel_z;
  acel_z_prom_corto_total = acel_z * 25;
  acel_z_prom_largo_total = acel_z * 50;
  inicio = 0;

  //Establece los límites iniciales de la velocidad del motor.
  if (velocidad_ralenti_motor < 1000)velocidad_ralenti_motor = 1000;
  if (velocidad_ralenti_motor > 1200)velocidad_ralenti_motor = 1200;

  //Inicia el temporizador para el primer bucle.
  temporizador_bucle = micros();
}

/*
==================================================================================
Rutina principal
==================================================================================
*/

void loop() {
  /*
  Incrementa la variable 'vigilia_receptor' mientras su valor sea menor a 750.
  Esta variable actúa como un contador para asegurarse de que la señal del receptor
  esté activa y funcionando correctamente durante el vuelo.
  */
  
  if(vigilia_receptor < 750)vigilia_receptor ++;
  //Si 'vigilia_receptor' ha alcanzado 750 (señal perdida) y si el cuadricóptero está en vuelo
  if(vigilia_receptor >= 750 && inicio == 2){
    //Neutraliza todos los canales y establece el error en 8
    canal_1 = 1500;
    canal_2 = 1500;
    canal_3 = 1100;
    canal_4 = 1500;
    error = 8;

    //Si hay suficientes satélites
    if (satelites_usados > 5){
      //Si el origen está establecido, activa el modo de vuelo para volver al origen.
      if(punto_origen_registrado == 1)canal_5 = 2000;
      //Si el origen no está establecido, activa el modo de vuelo para mantener la posición.
      else canal_5 = 1750;
    }

    //Si no hay suficientes satélites, activa el modo de vuelo para mantener la altitud.
    else canal_5 = 1500;  
    inicio = 0;

  }
  
  //Algunas funciones solo son accesibles cuando el cuadricóptero está desactivado.
  if (inicio == 0) {
    //Para la calibración de la brújula, mueve ambos sticks hacia la esquina superior derecha.
    if (canal_1 > 1900 && canal_2 < 1100 && canal_3 > 1900 && canal_4 > 1900)calibrar_brujula();
    //Para la calibración del nivel, mueve ambos sticks hacia la esquina superior izquierda.
    if (canal_1 < 1100 && canal_2 < 1100 && canal_3 > 1900 && canal_4 < 1100)calibrar_nivel();
    
    //Cambio de configuraciones.
    if (canal_6 >= 1900 && canal_6_anterior == 0) {
      canal_6_anterior = 1;
      if (conf_temporizador_ajuste > millis())conf_contador_click ++;
      else conf_contador_click = 0;
      conf_temporizador_ajuste = millis() + 1000;
      if (conf_contador_click > 3) {
        conf_contador_click = 0;
        cambiar_configuracion();
      }
    }
    if (canal_6 < 1900)canal_6_anterior = 0;
  }

  //Modos de vuelo.

  bloqueo_rumbo = 0;
  //Si el canal 6 está entre 1200 µs y 1600 µs, el modo de vuelo es 2.
  if (canal_6 > 1200)bloqueo_rumbo = 1;
  //En todas las demás situaciones, el modo de vuelo es 1.
  modo_vuelo = 1;
  //Si el canal 6 está entre 1200us y 1600us, el modo de vuelo es 2.
  if (canal_5 >= 1200 && canal_5 < 1600)modo_vuelo = 2;
  //Si el canal 6 está entre 1600us y 1900us, el modo de vuelo es 3.
  if (canal_5 >= 1600 && canal_5 < 1950)modo_vuelo = 3;
  //Si el canal 6 está entre 1950us y 2100us.
  if (canal_5 >= 1950 && canal_5 < 2100) {
    //Y se cumplen las demás condiciones, el modo de vuelo es 4.
    if (establecer_ruta == 1 && punto_origen_registrado == 1 && inicio == 2)modo_vuelo = 4;
    //De lo contrario el modo de vuelo seguirá siendo 3.
    else modo_vuelo = 3;
  }

  //Cuando termine el modo de vuelo 4, se reinician ciertas variables.
  if (modo_vuelo != 4) {
    pasos_volver_al_origen = 0;
    volver_al_origen_factor_latitud = 0;
    volver_al_origen_factor_longitud = 0;
  }

  //Realizar las siguientes subrutinas.

  //Subrutina para seguir la ruta.
  volar_ruta(); 
  //Programa de retorno a casa.
  volver_al_origen();
  //Mostrar el modo de vuelo a través del LED verde.
  senal_modo_vuelo();
  //Mostrar el error a través del LED rojo.
  senal_error();
  //Leer el dato del giroscopio y el acelerómetro.
  leer_giroscopio();
  //Leer y calcular el dato del barómetro.
  leer_barometro();
  //Leer y calcular el dato de la brújula.
  leer_brujula();
  //Lectura del puerto serial.
  ss_mapeo_byte();

  if (gps_suma_contador >= 0)gps_suma_contador --;

  //Leer y calcular el dato del GPS.
  leer_gps();

  //Para un fondo de escala de ± 500 °/s, la sensibilidad es
  //65.5 LSB/°/s (verificar la hoja de datos del MPU-6050 para más información).
  
  //Entradas PID del giroscopio en °/s.
  entrada_giroscopio_roll = (entrada_giroscopio_roll * 0.7) + (((float)roll_giroscopio / 65.5) * 0.3);
  entrada_giroscopio_pitch = (entrada_giroscopio_pitch * 0.7) + (((float)pitch_giroscopio / 65.5) * 0.3);
  entrada_giroscopio_yaw = (entrada_giroscopio_yaw * 0.7) + (((float)yaw_giroscopio / 65.5) * 0.3);


  //Cálculos de ángulos del giroscopio.
  //1 / (250Hz * 65.5 LSB/°/s) = 0.0000611 °/LSB

  //Calcular el ángulo de pitch recorrido y sumarlo a la variable angulo_pitch.
  angulo_pitch += (float)pitch_giroscopio * 0.0000611;
  //Calcular el ángulo de roll recorrido y sumarlo a la variable angulo_roll.
  angulo_roll += (float)roll_giroscopio * 0.0000611;
  //Calcular el ángulo de yaw recorrido y sumarlo a la variable angulo_yaw.
  angulo_yaw += (float)yaw_giroscopio * 0.0000611;

  //Mantiene el ángulo en el rango de [0,360°]
  if (angulo_yaw < 0) angulo_yaw += 360;
  else if (angulo_yaw >= 360) angulo_yaw -= 360;

  //La función sin de Arduino está en radianes y no en grados. 0.0000611 * (3.142 / 180grados) = 0.000001066 

  //Si la IMU ha experimentado un giro, transfiere el ángulo de roll al ángulo de pitch.
  angulo_pitch -= angulo_roll * sin((float)yaw_giroscopio * 0.000001066);
  //Agrega el ajuste del acelerómetro para comensar cualquier desnivel en el módulo MPU6050
  angulo_pitch += ajuste_acc_pitch;
  
  //Si la IMU ha girado, transfiere el ángulo de pitch al ángulo de roll.
  angulo_roll += angulo_pitch * sin((float)yaw_giroscopio * 0.000001066);
  //Agrega el ajuste del acelerómetro para comensar cualquier desnivel en el módulo MPU6050
  angulo_roll += ajuste_acc_roll;
  //Calcula la diferencia entre el rumbo del giroscopio y la brújula y realiza una pequeña corrección.
  angulo_yaw -= desviacion_curso(angulo_yaw, rumbo_actual_brujula) / 1200.0;

  //Mantiene el ángulo en el rango de [0,360°]
  if (angulo_yaw < 0) angulo_yaw += 360;
  else if (angulo_yaw >= 360) angulo_yaw -= 360;

  //Cálculos del ángulo del acelerómetro

  //Calcular el vector total del acelerómetro.
  acel_vector_total = sqrt((acel_x * acel_x) + (acel_y * acel_y) + (acel_z * acel_z));

  //Previene que la función asin genere un NaN.
  //57.296 = 180/3.142
  if (abs(acel_y) < acel_vector_total) {
    //Calcula el ángulo de pitch.
    acel_angulo_pitch = asin((float)acel_y / acel_vector_total) * 57.296;
  }
  //Previene que la función asin genere un NaN.
  if (abs(acel_x) < acel_vector_total) {
    //Calcula el ángulo de roll.
    acel_angulo_roll = asin((float)acel_x / acel_vector_total) * 57.296;
  }

  //Corregir la deriva del ángulo de pitch del giroscopio con el ángulo de pitch del acelerómetro.
  angulo_pitch = angulo_pitch * 0.9996 + acel_angulo_pitch * 0.0004;
  //Corregir la deriva del ángulo de roll del giroscopio con el ángulo de roll del acelerómetro.
  angulo_roll = angulo_roll * 0.9996 + acel_angulo_roll * 0.0004;
  
  //Calcular la corrección del ángulo de pitch.
  nivel_ajuste_pitch = angulo_pitch * 15;
  //Calcular la corrección del ángulo de roll.
  nivel_ajuste_roll = angulo_roll * 15;
 
  //Calcular la aceleración vertical.
  calculo_aceleracion_vertical();

  //Normalmente, canal_1 es el setpoint del PID roll.
  canal_1_base = canal_1;
  //Normalmente, canal_2 es el setpoint del PID pitch.
  canal_2_base = canal_2;
  gps_ajuste_rumbo_manual = angulo_yaw;
  
  /*
  Cuando el modo bloqueo_rumbo está activado, los setpoints de roll y pitch dependen del rumbo.
  Al iniciar, el rumbo se registra en la variable curso_bloqueo_rumbo. 
  Primero, se calcula la desviación del rumbo entre el rumbo actual y curso_bloqueo_rumbo.
  Con base en esta desviación, se calculan los controles de pitch y roll para que la respuesta
  sea la misma que al inicio.
  */

  if (bloqueo_rumbo == 1) {
    desviacion_curso_bloqueo_rumbo = desviacion_curso(angulo_yaw, curso_bloqueo_rumbo);
    canal_1_base = 1500 + ((float)(canal_1 - 1500) * cos(desviacion_curso_bloqueo_rumbo * 0.017453)) 
                    + ((float)(canal_2 - 1500) * cos((desviacion_curso_bloqueo_rumbo - 90) * 0.017453));

    canal_2_base = 1500 + ((float)(canal_2 - 1500) * cos(desviacion_curso_bloqueo_rumbo * 0.017453)) 
                    + ((float)(canal_1 - 1500) * cos((desviacion_curso_bloqueo_rumbo + 90) * 0.017453));

    gps_ajuste_rumbo_manual = curso_bloqueo_rumbo;

  }

  //Establecer el setpoint de los controles de roll y pitch según el modo de vuelo.
  if (modo_vuelo >= 3 && establecer_ruta == 1) {
    pid_roll_setpoint_base = 1500 + gps_roll_ajuste;
    pid_pitch_setpoint_base = 1500 + gps_pitch_ajuste;
  }
  else {
    pid_roll_setpoint_base = canal_1_base;
    pid_pitch_setpoint_base = canal_2_base;
  }

  //Limita el setpoint de los controles PID.
  if (pid_roll_setpoint_base > 2000)pid_roll_setpoint_base = 2000;
  if (pid_roll_setpoint_base < 1000)pid_roll_setpoint_base = 1000;
  if (pid_pitch_setpoint_base > 2000)pid_pitch_setpoint_base = 2000;
  if (pid_pitch_setpoint_base < 1000)pid_pitch_setpoint_base = 1000;

  //Calcular las salidas del PID basándose en las entradas del receptor.
  calculo_pid();

  //Detección de inicio, parada y despegue.
  inicio_paro_despegue();

  
  //Se necesita la tensión de la batería para la compensación. Se utiliza un filtro complementario para reducir el ruido.
  
  voltaje_bateria = voltaje_bateria * 0.92 + ((((float)analogRead(4) / 112.81) + calibracion_voltaje_bateria) * 0.08);

  //Enciende el LED si la tensión de la batería es demasiado baja. La configuración predeterminada es 10.5V.
  if (voltaje_bateria > 6.0 && voltaje_bateria < aviso_bateria_baja && error == 0)error = 1;
  if (voltaje_bateria > 6.0 && voltaje_bateria > aviso_bateria_baja && error == 1)error = 0;

  //Si el dron está activo y volando.
  if (despegue_detectado == 1 && inicio == 2) {
    //La aceleración base es la suma del canal 3 y la aceleración de despegue.
    aceleracion = canal_3 + aceleracion_despegue;
    //Si el modo de altitud está activo.
    if (modo_vuelo >= 2){
      //La aceleración base es la suma del canal de aceleración del receptor, la aceleración de despegue
      //y la salida del control PID.
      aceleracion = 1500 + aceleracion_manual + aceleracion_despegue + pid_altitud_salida;
      Serial.print("aceleracion_manual: ");
      Serial.print(aceleracion_manual);
      Serial.print("\taceleracion_despegue: ");
      Serial.print(aceleracion_despegue);
      Serial.print("\tpid_altitud_salida: ");
      Serial.println(pid_altitud_salida);
    }
  }

  //Creación de los pulsos de los ESCs.

  //Motores iniciados.
  if (inicio == 2) {
    //Necesitamos cierto margen para mantener el control total con máxima aceleración.
    if (aceleracion > 1800) aceleracion = 1800;
    //Calcular el pulso para el ESC 1 (frontal-derecha - CCW).
    ESC_1 = aceleracion - pid_pitch_salida + pid_roll_salida - pid_yaw_salida;
    //Calcular el pulso para el ESC 2 (posterior-derecha - CW).
    ESC_2 = aceleracion + pid_pitch_salida + pid_roll_salida + pid_yaw_salida;
    //Calcular el pulso para el ESC 3 (posterior-izquierda - CCW).
    ESC_3 = aceleracion + pid_pitch_salida - pid_roll_salida - pid_yaw_salida;
    //Calcular el pulso para el ESC 4 (frontal-izquierda - CW).
    ESC_4 = aceleracion - pid_pitch_salida - pid_roll_salida + pid_yaw_salida;

    //Si la batería está conectada.
    if (voltaje_bateria < 12.40 && voltaje_bateria > 6.0) {
      //Compensa la salida de los ESCs por la caída del voltaje.
      ESC_1 += (12.40 - voltaje_bateria) * compensacion_bateria;
      ESC_2 += (12.40 - voltaje_bateria) * compensacion_bateria;
      ESC_3 += (12.40 - voltaje_bateria) * compensacion_bateria;
      ESC_4 += (12.40 - voltaje_bateria) * compensacion_bateria;
    }

    //Limita la salida mínima de los ESCs para mantener los motores girando.
    if (ESC_1 < velocidad_ralenti_motor) ESC_1 = velocidad_ralenti_motor;
    if (ESC_2 < velocidad_ralenti_motor) ESC_2 = velocidad_ralenti_motor;
    if (ESC_3 < velocidad_ralenti_motor) ESC_3 = velocidad_ralenti_motor;
    if (ESC_4 < velocidad_ralenti_motor) ESC_4 = velocidad_ralenti_motor;

    //Limita la salida máxima de los ESCs
    if (ESC_1 > 1500)ESC_1 = 1500;
    if (ESC_2 > 1500)ESC_2 = 1500;
    if (ESC_3 > 1500)ESC_3 = 1500;
    if (ESC_4 > 1500)ESC_4 = 1500;
  }

  //Si el dron no está activo, lleva la salida de los ESCs al mínimo.
  else {
    ESC_1 = 1000;
    ESC_2 = 1000;
    ESC_3 = 1000;
    ESC_4 = 1000;
  }

  //Establecer el pulso de entrada de aceleración del receptor al pulso de salida de los ESCs.
  TIMER4_BASE->CCR1 = ESC_1;
  TIMER4_BASE->CCR2 = ESC_2;
  TIMER4_BASE->CCR3 = ESC_3;
  TIMER4_BASE->CCR4 = ESC_4;
  //Esto reiniciará el temporizador 4 y los pulsos del ESC se crearán directamente.
  TIMER4_BASE->CNT = 5000;

  //Enviar datos de telemetría a la estación terrestre.
  envio_telemetria();

  /*
  Debido al cálculo del ángulo, el tiempo del bucle se vuelve muy importante.
  Si el tiempo del bucle es más largo o más corto que 4000us, 
  el cálculo del ángulo es incorrecto.
  */

 //Generar un error si el tiempo del bucle supera los 4050us.
  if (micros() - temporizador_bucle > 4050)error = 2;
  //Esperar hasta que hayan pasado 4000us.
  while (micros() - temporizador_bucle < 4000);
  //Establecer el temporizador para el próximo bucle.
  temporizador_bucle = micros();
}
