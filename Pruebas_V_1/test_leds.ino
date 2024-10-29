
void test_leds(void) {
  dato = 0;
  //Si el puerto serial está disponible.
  if (Serial.available() > 0) {
    //Leer el byte entrante.
    dato = Serial.read();
    //Esperar a que entren otros bytes.
    delay(100);
    //Vaciar el búfer serial.
    while (Serial.available() > 0)contador_bucle = Serial.read();
  }

  Serial.println(F("El LED rojo está encendido por 3 segundos"));
  //Establecer el LED rojo en alto.
  led_rojo(HIGH);
  //Espera 3 segundos.
  delay(3000);
  Serial.println(F(""));


  Serial.println(F("El LED verde está encendido por 3 segundos"));
  //Desactiva el LED rojo.
  led_rojo(LOW);
  //Activa el LED Verde.
  led_verde(HIGH);
  //Espera 3 segundos.
  delay(3000);
  //Desactiva el LED verde.
  led_verde(LOW);

  //Imprime la portada.
  portada();
}
