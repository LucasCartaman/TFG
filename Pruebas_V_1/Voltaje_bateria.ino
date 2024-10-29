
void prueba_voltaje_bateria(void) {
  //Inicializa la variable contador_bucle.
  contador_bucle = 0;
  //Leer el voltaje de la batería.
  voltaje_bateria = analogRead(4);

  //Permanecer en este bucle hasta que la variable "dato" contenga una "s".
  while (dato != 's') {
    //Esperar 4000 ms para crear un bucle de 250 Hz.
    delayMicroseconds(4000);

    //Si hay datos disponibles en el puerto serie.
    if (Serial.available() > 0) {
      //Leer el byte entrante.
      dato = Serial.read();
      //Esperar a que entren otros bytes.
      delay(100);
      //Vaciar el búfer serial.
      while (Serial.available() > 0)contador_bucle = Serial.read();
    }
    //Incrementa el contador de bucle.
    contador_bucle++;

    //Imprimir el voltaje de la batería cada segundo.
    if (contador_bucle == 250) {
      Serial.print("Voltaje = ");
      //Imprimir el voltaje promedio de la batería en el monitor serie.
      Serial.print(voltaje_bateria / 112.81, 1);
      Serial.println("V");
      //Reinicia el contador de bucle.
      contador_bucle = 0;
    }

    //Se utiliza un filtro complementario para filtrar los picos de voltaje causados por los ESC.
    voltaje_bateria = (voltaje_bateria * 0.99) + ((float)analogRead(4) * 0.01);
  }
  //Reinicia el contador de bucle.
  contador_bucle = 0;
  //Imprime la portada.
  portada();
}
