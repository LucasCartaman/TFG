/*
| |<--------------------->| |<--------------------->| |<--------------------->| |

Brujula

La función leer_brujula obtiene lecturas de la brújula, ajusta el rumbo
considerando inclinaciones y realiza la calibración. La función configurar_brujula
inicializa la brújula y calcula valores de calibración.
La subrutina desviacion_curso calcula la diferencia mínima entre dos rumbos, 
ajustándola para considerar rangos diferentes.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/

void leer_brujula() {
  //Inicia la comunicación con la brújula.
  HWire.beginTransmission(direccion_brujula);
  //Inicia la lectura en la ubicación hexadecimal 0x03.
  HWire.write(0x03);
  //Finaliza la transmisión.
  HWire.endTransmission();

  //Guarda 6 bytes enviados por la brújula.
  HWire.requestFrom(direccion_brujula, 6);

  //Suma el MSB(primeros 8 bits) y el LSB(últimos 8 bits) a brujula_y.
  brujula_y = HWire.read() << 8 | HWire.read();          
  //Invierte la dirección del eje.       
  brujula_y *= -1;
  //Suma el MSB(primeros 8 bits) y el LSB(últimos 8 bits) a brujula_z.
  brujula_z = HWire.read() << 8 | HWire.read();
  //Suma el MSB(primeros 8 bits) y el LSB(últimos 8 bits) a brujula_x.
  brujula_x = HWire.read() << 8 | HWire.read();
  //Invierte la dirección del eje.
  brujula_x *= -1;

  /*Antes de que la brújula pueda proporcionar mediciones precisas, necesita ser calibrada.
  Al inicio, se calculan las variables brujula_offset y brujula_escala.
  La siguiente parte ajustará los valores brutos del compás para que puedan ser utilizados
  en el cálculo de la dirección.*/

  //Cuando la brújula no se esté calibrando.
  if (calibracion_brujula_activa == 0) {
    //Suma el offset al valor bruto de y.
    brujula_y += brujula_offset_y;
    //Escalar el valor de y para que coincida con los otros ejes.
    brujula_y *= brujula_escala_y;
    //Suma el offset al valor bruto de z.
    brujula_z += brujula_offset_z;
    //Escalar el valor de z para que coincida con los otros ejes.
    brujula_z *= brujula_escala_z;
    //Suma el offset al valor bruto de z. No se escala ya que se toma este valor como referencia.
    brujula_x += brujula_offset_x;
  }

  /*Los valores de la brújula cambian cuando el ángulo de inclinación y balanceo del cuadricóptero cambia.
  Por eso, los valores de x e y necesitan calcularse para una posición horizontal virtual.
  El valor 0.0174533 es pi/180 ya que las funciones están en radianes en lugar de grados.*/

  brujula_x_horizontal = (float)brujula_x * cos(angulo_pitch * -0.0174533) + (float)brujula_y * sin(angulo_roll * 0.0174533) * sin(angulo_pitch * -0.0174533) - (float)brujula_z * cos(angulo_roll * 0.0174533) * sin(angulo_pitch * -0.0174533);
  brujula_y_horizontal = (float)brujula_y * cos(angulo_roll * 0.0174533) + (float)brujula_z * sin(angulo_roll * 0.0174533);

  /*Ahora que se conocen los valores horizontales, se puede calcular la dirección. Con las siguientes
  líneas de código, la dirección se calcula en grados. atan2 utiliza radianes en lugar de grados,
  por eso se utiliza 180/3.14.*/

  if (brujula_y_horizontal < 0) rumbo_actual_brujula = 180 + (180 + ((atan2(brujula_y_horizontal, brujula_x_horizontal)) * (180 / 3.14)));
  else rumbo_actual_brujula = (atan2(brujula_y_horizontal, brujula_x_horizontal)) * (180 / 3.14);

  
  //Suma la declinación al rumbo magnético de la brújula para obtener el norte geográfico.
  rumbo_actual_brujula += declinacion;
  //Si el rumbo de la brújula se vuelve menor que 0, se le suma 360 para mantenerlo en el rango de 0 a 360 grados.
  if (rumbo_actual_brujula < 0) rumbo_actual_brujula += 360;
  //Si el rumbo de la brújula se vuelve mayor que 360, se le resta 360 para mantenerlo en el rango de 0 a 360 grados.
  else if (rumbo_actual_brujula >= 360) rumbo_actual_brujula -= 360;
}

/*Al iniciar, es necesario configurar los registros de la brújula. Después de eso,
se calculan los valores de offset y escala para la calibración.*/

void configurar_brujula() {
  //Inicia la comunicación.
  HWire.beginTransmission(direccion_brujula);
  //Se debe escribir en el Registro de Configuración A (00 hex).
  HWire.write(0x00);
  //Configura los bits del Registro de Configuración A como 01111000 para establecer la tasa de muestreo
  //(promedio de 8 muestreos y frecuencia de 75Hz).
  HWire.write(0x78);
  //Configuramos los bits del Registro de Configuración B como 00100000 para establecer la ganancia en +/-1.3Ga.
  HWire.write(0x20);
  //Configuramos los bits del Registro de Modo como 00000000 para establecer el Modo de Medición Continua.
  HWire.write(0x00);
  //Finaliza la comunicación.
  HWire.endTransmission();

  //Leer los valores de calibración de la EEPROM.
  for (error = 0; error < 6; error ++)brujula_valor_cal[error] = EEPROM.read(0x10 + error);

  error = 0;
  //Calcular los valores de offset y escala para la calibración.
  brujula_escala_y = ((float)brujula_valor_cal[1] - brujula_valor_cal[0]) / (brujula_valor_cal[3] - brujula_valor_cal[2]);
  brujula_escala_z = ((float)brujula_valor_cal[1] - brujula_valor_cal[0]) / (brujula_valor_cal[5] - brujula_valor_cal[4]);

  brujula_offset_x = (brujula_valor_cal[1] - brujula_valor_cal[0]) / 2 - brujula_valor_cal[1];
  brujula_offset_y = (((float)brujula_valor_cal[3] - brujula_valor_cal[2]) / 2 - brujula_valor_cal[3]) * brujula_escala_y;
  brujula_offset_z = (((float)brujula_valor_cal[5] - brujula_valor_cal[4]) / 2 - brujula_valor_cal[5]) * brujula_escala_z;
}


//La siguiente subrutina calcula la diferencia más pequeña entre dos valores de dirección.

float desviacion_curso(float curso_b, float curso_c) {
  
  curso_a = curso_b - curso_c;

  if (curso_a < -180 || curso_a > 180) {

    if (curso_c > 180)curso_base_reflejado = curso_c - 180;
    else curso_base_reflejado = curso_c + 180;

    if (curso_b > 180)curso_actual_reflejado = curso_b - 180;
    else curso_actual_reflejado = curso_b + 180;

    curso_a = curso_actual_reflejado - curso_base_reflejado;

  }
  return curso_a;
}
