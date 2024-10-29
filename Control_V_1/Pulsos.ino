/*
| |<--------------------->| |<--------------------->| |<--------------------->| |
Pulsos

La función gestor_canal mide el tiempo transcurrido entre pulsos consecutivos al 
calcular la diferencia entre el valor actual del registro de captura del canal 1 
(CCR1) y un valor inicial previamente almacenado. Si el tiempo medido es negativo,
se ajusta para manejar posibles desbordamientos del contador del temporizador. 

Dependiendo de la duración del pulso medido, la función actualiza un contador de 
selección de canal que almacena los tiempos en variables correspondientes a seis 
canales diferentes. Si el tiempo medido es mayor a un umbral específico (3000 us),
se reinician ciertos parámetros relacionados con el funcionamiento del receptor,
como el contador del selector de canal y la variable de vigilia del receptor. 

Esta función es invocada automáticamente cada vez que ocurre una captura en el 
canal 1 de TIMER2, permitiendo la lectura secuencial de los pulsos PPM.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/


void gestor_canal(void) {
  //Calcula el tiempo medido restando el valor actual del canal 1 (CCR1) del
  // temporizador TIMER2 al valor inicial almacenado en tiempo_medido_inicial.
  tiempo_medido = TIMER2_BASE->CCR1 - tiempo_medido_inicial;

  //Si el tiempo medido negativo, se ajusta sumando 0xFFFF para manejar desbordamientos.
  if (tiempo_medido < 0)tiempo_medido += 0xFFFF;

  //Actualiza el valor inicial del tiempo medido al valor actual del canal 1 del temporizador TIMER2.
  tiempo_medido_inicial = TIMER2_BASE->CCR1;

  //Si el tiempo medido supera los 3000.
  if (tiempo_medido > 3000) {
    //Reinicia contador_selector_canal y vigilia_receptor.
    contador_selector_canal = 0;
    vigilia_receptor = 0;

    //Se reinicia el error si es igual a 8 y el inicio es igual a 2.
    if (error == 8 && inicio == 2)error = 0;
  }

  //En caso contrario, se incrementa el contador del selector de canal.
  else contador_selector_canal++;

  /*Actualiza los valores de los canales 1 al 6 según el contador_selector_canal.
  Cada canal almacena el tiempo medido entre pulsos consecutivos.*/
  if (contador_selector_canal == 1)canal_1 = tiempo_medido;
  if (contador_selector_canal == 2)canal_2 = tiempo_medido;
  if (contador_selector_canal == 3)canal_3 = tiempo_medido;
  if (contador_selector_canal == 4)canal_4 = tiempo_medido;
  if (contador_selector_canal == 5)canal_5 = tiempo_medido;
  if (contador_selector_canal == 6)canal_6 = tiempo_medido;
}
