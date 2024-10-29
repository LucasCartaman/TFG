void conf_timer(void) {
   // Configuración de Timer2 para la lectura de pulsos del receptor
  Timer2.attachCompare1Interrupt(gestor_canal_1);
  TIMER2_BASE->CR1 = TIMER_CR1_CEN;
  TIMER2_BASE->CR2 = 0;
  TIMER2_BASE->SMCR = 0;
  TIMER2_BASE->DIER = TIMER_DIER_CC1IE;
  TIMER2_BASE->EGR = 0;
  TIMER2_BASE->CCMR1 = TIMER_CCMR1_CC1S_INPUT_TI1;
  TIMER2_BASE->CCMR2 = 0;
  TIMER2_BASE->CCER = TIMER_CCER_CC1E;
  //Detectar flanco de bajada.
  //TIMER2_BASE->CCER |= TIMER_CCER_CC1P;
  //Detectar flanco de subida.
  TIMER2_BASE->CCER &= ~TIMER_CCER_CC1P;
  TIMER2_BASE->PSC = 71;
  TIMER2_BASE->ARR = 0xFFFF;
  TIMER2_BASE->DCR = 0;

  /*Se necesita una prueba para verificar si la entrada de aceleración está 
  activa y válida. De lo contrario, los ESC podrían iniciar sin previo aviso.*/

  contador_bucle = 0;

  /*Permanecer en este bucle mientras la entrada de aceleración esté fuera
  del rango [1000, 2000]-*/
  while ((canal_3 > 2100 || canal_3 < 900) && warning == 0) {
    delay(100);
    contador_bucle++;

    //Imprime avisos dependiendo del contador.
    if (contador_bucle == 40) {
      Serial.println(F("Esperando una señal de entrada válida del canal 3."));
      Serial.println(F(""));
      Serial.println(F("El pulso de entrada debe estar entre 1000us y 2000us."));
      Serial.print(F("Valor de entrada actual del canal 3 = "));
      Serial.println(canal_3);
      Serial.println(F(""));
      Serial.println(F("¿El receptor está conectado y el transmisor encendido?."));
      Serial.println(F(""));
      Serial.print(F("Esperando 5 segundos."));
    }
    if (contador_bucle > 40 && contador_bucle % 10 == 0)Serial.print(F("."));

    if (contador_bucle == 90) {
      Serial.println(F(""));
      Serial.println(F(""));
      Serial.println(F("¡Las salidas de los ESCs están desactivadas por seguridad!."));
      warning = 1;
    }
  }

  //Si la entrada de aceleración es válida
  if (warning == 0) {
    //Configuración de Timer4 para generar pulsos PWM para ESC
    TIMER4_BASE->CR1 = TIMER_CR1_CEN | TIMER_CR1_ARPE;
    TIMER4_BASE->CR2 = 0;
    TIMER4_BASE->SMCR = 0;
    TIMER4_BASE->DIER = 0;
    TIMER4_BASE->EGR = 0;
    TIMER4_BASE->CCMR1 = (0b110 << 4) | TIMER_CCMR1_OC1PE | (0b110 << 12) | TIMER_CCMR1_OC2PE;
    TIMER4_BASE->CCMR2 = (0b110 << 4) | TIMER_CCMR2_OC3PE | (0b110 << 12) | TIMER_CCMR2_OC4PE;
    TIMER4_BASE->CCER = TIMER_CCER_CC1E | TIMER_CCER_CC2E | TIMER_CCER_CC3E | TIMER_CCER_CC4E;
    TIMER4_BASE->PSC = 71;
    TIMER4_BASE->ARR = 4000;
    TIMER4_BASE->DCR = 0;
    TIMER4_BASE->CCR1 = 1000;

    //Configuración de pines PWM para los ESCs.
    TIMER4_BASE->CCR1 = canal_3;
    TIMER4_BASE->CCR2 = canal_3;
    TIMER4_BASE->CCR3 = canal_3;
    TIMER4_BASE->CCR4 = canal_3;
    pinMode(PB6, PWM);
    pinMode(PB7, PWM);
    pinMode(PB8, PWM);
    pinMode(PB9, PWM);
  }
}
