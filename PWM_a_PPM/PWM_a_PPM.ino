/*
| |<------------------------->| |<------------------------->| |<------------------------->| |

PWM_a_PPM

Este código está diseñado para generar señales PPM en el pin 2 del Arduino.
El programa utiliza un bucle principal que verifica el estado de cada entrada analógica y 
genera pulsos PPM correspondientes a cada canal. Cada pulso se genera activando el pin 2, 
esperando 100 microsegundos y luego desactivando el pin. Este proceso se repite para cada canal.
El bucle principal espera a que la entrada analógica correspondiente esté en un estado
específico antes de ejecutar el siguiente canal. La última parte del código genera un pulso
de parada después de procesar todos los canales. 

Canal 1 = entrada analógica 0 (A0).
Canal 2 = entrada analógica 1 (A1).
Canal 3 = entrada analógica 2 (A2).
Canal 4 = entrada analógica 3 (A3).
Canal 5 = entrada analógica 4 (A4).
Canal 6 = entrada analógica 5 (A5).
Salida PPM = pin 2 (2).

| |<------------------------->| |<------------------------->| |<------------------------->| |
*/
void setup(){
  //Desactivar el temporizador 0 ya que consumirá mucho tiempo.
  TIMSK0 &= ~_BV(TOIE0);
  //El pin 2 será la salida PPM.
  pinMode(2, OUTPUT);
  //Mientras la entrada analógica 0 esté baja, no se ejecutará el bucle principal (loop).
  while(PINC & B00000001);
}

void loop(){
  
  //Canal 1.

  //Mientras la entrada analógica 0 esté baja, no se ejecuta las demás líneas.
  while(!(PINC & B00000001));
  //Establecer la salida 2 en alto.
  PORTD |= B00000100;
  //Retrasar durante 100 microsegundos.
  delayMicroseconds(100);
  //Establecer el pin 2 en bajo.
  PORTD &= B11111011;

  //Canal 2.

  //Mientras la entrada analógica 1 esté baja, no se ejecuta las demás líneas.
  while(!(PINC & B00000010));
  //Establecer la salida 2 en alto.
  PORTD |= B00000100;
  //Retrasar durante 100 microsegundos.
  delayMicroseconds(100);
  //Establecer el pin 2 en bajo.
  PORTD &= B11111011;

  //Canal 3.

  //Mientras la entrada analógica 2 esté baja, no se ejecuta las demás líneas.
  while(!(PINC & B00000100));
  //Establecer la salida 2 en alto.
  PORTD |= B00000100;
  //Retrasar durante 100 microsegundos.
  delayMicroseconds(100);
  //Establecer el pin 2 en bajo.
  PORTD &= B11111011;
  
  //Canal 4.

  //Mientras la entrada analógica 3 esté baja, no se ejecuta las demás líneas.
  while(!(PINC & B00001000));
  //Establecer la salida 2 en alto.
  PORTD |= B00000100;
  //Retrasar durante 100 microsegundos.
  delayMicroseconds(100);
  //Establecer el pin 2 en bajo.
  PORTD &= B11111011;

  //Canal 5.

  //Mientras la entrada analógica 4 esté baja, no se ejecuta las demás líneas.
  while(!(PINC & B00010000));
  //Establecer la salida 2 en alto.
  PORTD |= B00000100;
  //Retrasar durante 100 microsegundos.
  delayMicroseconds(100);
  //Establecer el pin 2 en bajo.
  PORTD &= B11111011;
  
  //Canal 6.

  //Mientras la entrada analógica 5 esté baja, no se ejecuta las demás líneas.
  while(!(PINC & B00100000));
  //Establecer la salida 2 en alto.
  PORTD |= B00000100;
  //Retrasar durante 100 microsegundos.
  delayMicroseconds(100);
  //Establecer el pin 2 en bajo.
  PORTD &= B11111011;

  //Pulso de parada.

  //Mientras la entrada analógica 5 esté baja, no se ejecuta las demás líneas.
  while(PINC & B00100000);
  //Establecer la salida 2 en alto.
  PORTD |= B00000100;
  //Retrasar durante 100 microsegundos.
  delayMicroseconds(100);
  //Establecer el pin 2 en bajo.
  PORTD &= B11111011;
}
