/*
| |<--------------------->| |<--------------------->| |<--------------------->| |

LED

Este código gestiona LEDs rojos y verdes. Las funciones led_rojo y led_verde
controlan los LEDs rojo y verde, respectivamente. La función senal_error genera
la señal de LED para indicar un error, y la función senal_modo_vuelo genera
la señal de LED para indicar el modo de vuelo. Ambas funciones controlan el
parpadeo de los LEDs basándose en condiciones específicas y temporizadores.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/


//Establece el estado del LED rojo.
void led_rojo(int8_t nivel) {
  digitalWrite(PB4, nivel);
}

//Establece el estado del LED verde.
void led_verde(int8_t nivel) {
  digitalWrite(PB3, nivel);
}

//Manejo de la señal de errores.
void senal_error(void) {
  //Cuando el error es 100, el LED rojo siempre estará encendido.
  if (error >= 100) led_rojo(HIGH);
  //Si el valor de temporizador_error es menor que la función millis().
  else if (temporizador_error < millis()) {
    //Establece el próximo intervalo de temporizador_error en 250 ms.
    temporizador_error = millis() + 250;
    /*Si se detecta un error y el valor de contador_error es mayor que error + 3, se reinicia el 
    contador de errores. Se añade 3 a la comparación para que, al completar todos los parpadeos, 
    haya una espera adicional de 750 ms antes de reiniciar el contador. Esto asegura que la 
    secuencia de parpadeo tenga una pausa clara entre ciclos, permitiendo distinguir mejor cada 
    ciclo de parpadeo.*/
    if (error > 0 && contador_error > error + 3) contador_error = 0;
    //Si la secuencia de destello de error no ha terminado (contador_error < error) y el LED está apagado.
    if (contador_error < error && led_error == 0 && error > 0) {
      //Enciende el LED rojo.
      led_rojo(HIGH);
      //Establece led_error en 1 para indicar que el LED está encendido.
      led_error = 1;
    }
    //Si la secuencia de destello de error no ha terminado (contador_error < error) y el LED está encendido.
    else {
      //Apaga el LED rojo.
      led_rojo(LOW);
      //Incrementar la variable contador_error en 1 para llevar un seguimiento de los destellos.
      contador_error++;
      //Establece led_error en 0 para indicar que el LED está apagado.
      led_error = 0;
    }
  }
}

//En esta parte se genera la señal del LED del modo de vuelo.
void senal_modo_vuelo(void) {
  //Si el valor de temporizador_modo_vuelo es menor que la función millis().
  if (temporizador_modo_vuelo < millis()) {
    //Establece el próximo intervalo de temporizador_modo_vuelo en 250 ms.
    temporizador_modo_vuelo = millis() + 250;
    /*Si hay un modo de vuelo y el contador_modo_vuelo es mayor que modo_vuelo + 3, 
    reinicia el error. Se añade 3 a la comparación para que, al completar todos los parpadeos, 
    haya una espera adicional de 750 ms antes de reiniciar el contador. Esto asegura que la 
    secuencia de parpadeo tenga una pausa clara entre ciclos, permitiendo distinguir mejor cada 
    ciclo de parpadeo.*/
    if (modo_vuelo > 0 && contador_modo_vuelo > modo_vuelo + 3) contador_modo_vuelo = 0;
    //Si la secuencia de destello no ha terminado (contador_modo_vuelo < modo_vuelo) y el LED está apagado.
    if (contador_modo_vuelo < modo_vuelo && led_modo_vuelo == 0 && modo_vuelo > 0) {
      //Enciende el LED verde.
      led_verde(HIGH);
      //Establece led_modo_vuelo en 1 para indicar que el LED está encendido.
      led_modo_vuelo = 1;
    }

    //Si la secuencia de destello no ha terminado (contador_modo_vuelo < modo_vuelo) y el LED está encendido.
    else {
      //Apaga el LED verde.
      led_verde(LOW);
      //Incrementar la variable contador_modo_vuelo en 1 para llevar un seguimiento de los destellos.
      contador_modo_vuelo++;
      //Establece led_modo_vuelo en 1 para indicar que el LED está apagado.
      led_modo_vuelo = 0;
    }
  }
}
