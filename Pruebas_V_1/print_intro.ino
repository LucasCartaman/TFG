
void portada(void) {
  Serial.println(F(""));
  Serial.println(F("| |<--------------------->| |<--------------------->| |<--------------------->| |"));
  Serial.println(F("                      Configuraciones y Pruebas Principales                      "));
  Serial.println(F("| |<--------------------->| |<--------------------->| |<--------------------->| |"));
  Serial.println(F("a = Leer los pulsos de entrada del receptor."));
  Serial.println(F("b = Buscar dispositivos I2C."));
  Serial.println(F("c = Leer valores brutos del giroscopio."));
  Serial.println(F("d = Leer valores brutos del acelerómetro."));
  Serial.println(F("e = Probar ángulos de la IMU."));
  Serial.println(F("f = Probar los LEDs."));
  Serial.println(F("g = Leer voltaje de la batería."));
  Serial.println(F("h = Probar barómetro."));
  Serial.println(F("i = Probar GPS."));
  Serial.println(F("j = Probar brújula HMC5883L."));
  Serial.println(F("| |<--------------------->| |<--------------------->| |<--------------------->| |"));
  Serial.println(F("1 = Probar motor 1 (delantero derecho, en sentido antihorario.)"));
  Serial.println(F("2 = Probar motor 2 (posterior derecho, en sentido horario.)"));
  Serial.println(F("3 = Probar motor 3 (posterior izquierdo, en sentido antihorario.)"));
  Serial.println(F("4 = Probar motor 4 (delantero izquierdo, en sentido horario.)"));
  Serial.println(F("5 = Probar todos los motores."));
  Serial.println(F("| |<--------------------->| |<--------------------->| |<--------------------->| |"));
  Serial.println(F(""));
  if (!deshabilitar_acelerador) {
    Serial.println(F("!!! EL ACELERADOR ESTÁ HABILITADO !!!"));
  }
}
