
/*
| |<--------------------->| |<--------------------->| |<--------------------->| |

Timers

Esta sección configura los temporizadores para leer los pulsos del receptor y 
generar los pulsos de salida para los ESC. La función conf_timer realiza la
configuración de los temporizadores, asignando interrupciones y ajustando varios
parámetros para garantizar la lectura precisa de los pulsos del receptor y la
generación adecuada de pulsos de salida para los ESC. Además, se establecen
algunos valores predeterminados para los temporizadores y se configuran los pines
PWM para la salida de los pulsos hacia los ESC.

Timer 2: Configurado para capturar los pulsos del receptor, detectando flancos 
de subida y utilizando un prescaler que ajusta la frecuencia a 1 MHz para medir 
los pulsos PWM de entrada con alta precisión.

Timer 3: Configurado para gestionar entradas seriales, con ajustes similares a 
los de Timer 2, permitiendo la captura de pulsos con detección de flancos de bajada.

Timer 4: Configurado para generar señales PWM a los ESCs que controlan los 
motores del cuadricóptero. Incluye habilitación de pre-carga para los valores de 
comparación y configuración de los pines PB6-PB9 como salidas PWM.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/


void conf_timer(void) {
  // Configuración de Timer2 para la lectura de pulsos del receptor

  // Se adjunta la interrupción al canal 1 del Timer 2 para gestionar las señales del receptor.
  Timer2.attachCompare1Interrupt(gestor_canal);
  // Habilita el temporizador.
  TIMER2_BASE->CR1 = TIMER_CR1_CEN;
  // Desactiva configuraciones adicionales.
  TIMER2_BASE->CR2 = 0;
  // No usa ningún modo esclavo de sincronización.
  TIMER2_BASE->SMCR = 0;
  // Habilita la interrupción por captura en el canal 1.
  TIMER2_BASE->DIER = TIMER_DIER_CC1IE;
  // Desactiva la generación de eventos.
  TIMER2_BASE->EGR = 0;
  // Configura el canal 1 del Timer 2 como entrada que captura el valor de TI1 (Timer Input 1).
  TIMER2_BASE->CCMR1 = TIMER_CCMR1_CC1S_INPUT_TI1;
   // Sin configuraciones adicionales en los canales 3 y 4.
  TIMER2_BASE->CCMR2 = 0;
  // Habilita la captura en el canal 1 del Timer 2.
  TIMER2_BASE->CCER = TIMER_CCER_CC1E;

  // Configura el Timer 2 para detectar el flanco de subida de las señales del receptor.

  //Detectar flanco de bajada.
  //TIMER2_BASE->CCER |= TIMER_CCER_CC1P;
  //Detectar flanco de subida.
  TIMER2_BASE->CCER &= ~TIMER_CCER_CC1P;
  // Establece el prescaler a 71, lo cual reduce la frecuencia del reloj base a 1 MHz
  // (72 MHz / (71 + 1)), para permitir medidas precisas de los pulsos de entrada
  TIMER2_BASE->PSC = 71;
  // Establece el valor máximo de recarga del temporizador a 0xFFFF (65535).
  TIMER2_BASE->ARR = 0xFFFF;
  // No utiliza el registro de control directo.
  TIMER2_BASE->DCR = 0;

  // Configuración de Timer 3 para la entrada serial
  
  // Se adjunta a la interrupción al canal 1 del Timer 3 la función gestor_entrada_serial.
  Timer3.attachCompare1Interrupt(gestor_entrada_serial);
  // Habilita el temporizador.
  TIMER3_BASE->CR1 = TIMER_CR1_CEN;
  // Desactiva configuraciones adicionales.
  TIMER3_BASE->CR2 = 0;
  // No usa ningún modo esclavo de sincronización.
  TIMER3_BASE->SMCR = 0;
  // Habilita la interrupción por captura en el canal 1.
  TIMER3_BASE->DIER = TIMER_DIER_CC1IE;
  // Desactiva la generación de eventos.
  TIMER3_BASE->EGR = 0;

  // Configura el canal 1 del Timer 3 como entrada que captura el valor de TI1 (Timer Input 1),
  // con la configuración para leer la entrada serial del pin A6.
  TIMER3_BASE->CCMR1 = TIMER_CCMR1_CC1S_INPUT_TI1;
  // Sin configuraciones adicionales en los canales 3 y 4.
  TIMER3_BASE->CCMR2 = 0;
  // Habilita la captura en el canal 1 del Timer 3.
  TIMER3_BASE->CCER = TIMER_CCER_CC1E;
  // Configura Timer 3 para detectar flancos de bajada para la captura de los pulsos.
  TIMER3_BASE->CCER |= TIMER_CCER_CC1P;
  // Establece el prescaler a 71, lo cual reduce la frecuencia del reloj base a 1 MHz
  // (72 MHz / (71 + 1)), para permitir medidas precisas de los pulsos de entrada.
  TIMER3_BASE->PSC = 71;
  // Establece el valor máximo de recarga del temporizador a 0xFFFF (65535).
  // Esto permite medir pulsos largos sin sobrepasar el contador.
  TIMER3_BASE->ARR = 0xFFFF;
  // No utiliza el registro de control directo.
  TIMER3_BASE->DCR = 0;


  // Configuración de Timer 4 para generar pulsos PWM para los ESCs.
  // Este temporizador controla los motores del cuadricóptero a través de señales PWM.

  // Configura Timer 4 para iniciar su contador, habilitar la salida del temporizador y utilizar
  // una pre-carga automática (ARPE) para los valores de comparación (CCR).
  TIMER4_BASE->CR1 = TIMER_CR1_CEN | TIMER_CR1_ARPE;
  // Desactiva configuraciones adicionales.
  TIMER4_BASE->CR2 = 0;
  // No usa ningún modo esclavo de sincronización.
  TIMER4_BASE->SMCR = 0;
  // No requiere interrupciones directas para la generación de PWM.
  TIMER4_BASE->DIER = 0;
  // Desactiva la generación de eventos.
  TIMER4_BASE->EGR = 0;

  // Configura los canales 1 y 2 del Timer 4 en modo de comparación de salida PWM (OC Mode),
  // activando la precarga de comparación para que los valores CCR se actualicen de forma segura.
  TIMER4_BASE->CCMR1 = (0b110 << 4) | TIMER_CCMR1_OC1PE | (0b110 << 12) | TIMER_CCMR1_OC2PE;
  // Configura los canales 3 y 4 del Timer 4 en modo de comparación de salida PWM (OC Mode),
  // activando la precarga de comparación para que los valores CCR se actualicen de forma segura. 
  TIMER4_BASE->CCMR2 = (0b110 << 4) | TIMER_CCMR2_OC3PE | (0b110 << 12) | TIMER_CCMR2_OC4PE;
  // Habilita los canales de comparación de salida (OC) 1 a 4, para enviar las señales PWM
  // a los pines configurados para los ESCs.
  TIMER4_BASE->CCER = TIMER_CCER_CC1E | TIMER_CCER_CC2E | TIMER_CCER_CC3E | TIMER_CCER_CC4E;
  // Establece el prescaler a 71, lo cual reduce la frecuencia del reloj base a 1 MHz
  // (72 MHz / (71 + 1)), para permitir la generación de PWM en una frecuencia de 1 kHz.
  TIMER4_BASE->PSC = 71;
  // Establece el valor máximo de recarga del temporizador a 5000,
  // lo cual define el periodo del ciclo PWM en 5 ms (1 kHz).
  TIMER4_BASE->ARR = 5000;
  // No utiliza el registro de control directo.
  TIMER4_BASE->DCR = 0;
  // Establece los valores iniciales de los registros de comparación (CCR) de Timer 4,
  // configurando el ancho del pulso PWM inicial para los ESCs en 1000 microsegundos.
  TIMER4_BASE->CCR1 = 1000;

  /*
  Configuración de pines PWM para los ESCs.
  Los pines PB6, PB7, PB8 y PB9 están configurados como salidas PWM, vinculados a los canales
  correspondientes de Timer 4, los cuales controlan los motores a través de los ESCs.
  */
  TIMER4_BASE->CCR1 = 1000;
  TIMER4_BASE->CCR2 = 1000;
  TIMER4_BASE->CCR3 = 1000;
  TIMER4_BASE->CCR4 = 1000;
  pinMode(PB6, PWM);
  pinMode(PB7, PWM);
  pinMode(PB8, PWM);
  pinMode(PB9, PWM);
}
