/*
| |<--------------------->| |<--------------------->| |<--------------------->| |

Volver_origen

Esta función implementa un programa de retorno al origen, dividido en 4 pasos.
Inicia con cálculos básicos para determinar si el vehículo está cerca de su 
punto de inicio. Luego, eleva la altitud a 20 metros sobre el suelo, retorna
a la posición de inicio ajustando la posición en función de la diferencia GPS,
desciende ajustando la presión y finalmente detiene los motores. Este proceso
se ejecuta cuando el modo de vuelo está configurado en 4.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/

void volver_al_origen(void) {

  if (modo_vuelo == 4) {
    //Paso 0: cálculos básicos.
    if (pasos_volver_al_origen == 0) {
      //Si el cuadricóptero está cerca del origen, se ejecuta el paso 3 (aterrizaje).
      if (abs(lat_gps_origen - l_lat_ruta) < 90 && abs(lon_gps_origen - l_lon_ruta) < 90)pasos_volver_al_origen = 3;
      //De lo contrario se realizan los cálculos.
      else {
        //Inicializa la variable volver_al_origen_factor_movimiento.
        volver_al_origen_factor_movimiento = 0.0;

        //Si los calculos ya se realizaron, se pasa al siguiente paso
        if (volver_al_origen_factor_latitud == 1 || volver_al_origen_factor_longitud == 1)pasos_volver_al_origen = 1;
        //cos(((float)l_lat_gps / 1000000.0)
        
        //Si el error de la latitud es mayor o igual que el error de la longitud.
        if (abs(lat_gps_origen - l_lat_ruta) >= abs(lon_gps_origen - l_lon_ruta)) {
          //Se calcula el factor de la longitud, mientras que el factor de la latitud es la unidad.
          volver_al_origen_factor_longitud = (float)abs(lon_gps_origen - l_lon_ruta) / (float)abs(lat_gps_origen - l_lat_ruta);
          volver_al_origen_factor_latitud = 1;
        }

        //Si el error de la latitud es menor que el error de la longitud.
        else {
          //Se calcula el factor de la longitud, mientras que el factor de la latitud es la unidad.
          volver_al_origen_factor_longitud = 1;
          volver_al_origen_factor_latitud = (float)abs(lat_gps_origen - l_lat_ruta) / (float)abs(lon_gps_origen - l_lon_ruta);
        }

        //Si el error en la presion es menor a 170, da un valor positivo a volver_al_origen_descenso.
        if (presion_suelo - presion_actual < 170)volver_al_origen_descenso = 170 - (presion_suelo - presion_actual);
        //De lo contrario asigna 0 a volver_al_origen_descenso.
        else volver_al_origen_descenso = 0;
      }
    }

    //Paso 1: Incrementa la altitud a 20 metros sobre el nivel del suelo.
    
    if (pasos_volver_al_origen == 1) {
      /*Si volver_al_origen_descenso no es positivo, no se requiere ajuste de altura 
      y se ejecuta el siguiente paso.*/
      if (volver_al_origen_descenso <= 0)pasos_volver_al_origen = 2;

      //Si volver_al_origen_descenso es positivo, se ajusta la altura.
      if (volver_al_origen_descenso > 0) {
        pid_altitud_setpoint -= 0.035;
        volver_al_origen_descenso -= 0.035;
      }
    }

    //Paso 2: Regresar a la posición de origen.

    if (pasos_volver_al_origen == 2) {
      //Si el cuadricóptero ya está en la posición de origen, ejecutar el siguiente paso.
      if (lat_gps_origen == l_lat_ruta && lon_gps_origen == l_lon_ruta)pasos_volver_al_origen = 3;

      //Si el error en la longitud y la latitud es pequeño, se disminuye el factor de movimiento.
      if (abs(lat_gps_origen - l_lat_ruta) < 160 && abs(lon_gps_origen - l_lon_ruta) < 160 && volver_al_origen_factor_movimiento > 0.05)volver_al_origen_factor_movimiento -= 0.00015;
      //De lo contrario se aumenta el factor de movimiento hasta un máximo de 0.2.
      else if (volver_al_origen_factor_movimiento < 0.20)volver_al_origen_factor_movimiento += 0.0001;

      //Si existe un error entre la latitud inicial y la latitud actual, se debe ajustar su valor.
      if (lat_gps_origen != l_lat_ruta) {
        if (lat_gps_origen > l_lat_ruta) l_lat_gps_ajuste_float += volver_al_origen_factor_movimiento * volver_al_origen_factor_latitud;
        if (lat_gps_origen < l_lat_ruta) l_lat_gps_ajuste_float -= volver_al_origen_factor_movimiento * volver_al_origen_factor_latitud;
      }

      //Si existe un error entre la longitud inicial y la longitd actual, se debe ajustar su valor.
      if (lon_gps_origen != l_lon_ruta) {
        if (lon_gps_origen > l_lon_ruta) l_lon_gps_ajuste_float += volver_al_origen_factor_movimiento * volver_al_origen_factor_longitud;
        if (lon_gps_origen < l_lon_ruta) l_lon_gps_ajuste_float -= volver_al_origen_factor_movimiento * volver_al_origen_factor_longitud;
      }
    }

    //Paso 3:Disminuir la altitud, aumentando el punto de ajuste de presión.
    
    if (pasos_volver_al_origen == 3) {

      //Si el cuadricóptero ya está a baja altitud, ejecutar al siguiente paso.
      if (pid_altitud_setpoint > presion_actual + 150)pasos_volver_al_origen = 4;
      //Aumenta el valor del setpoint.
      pid_altitud_setpoint += 0.035;
    }

    //Paso 4: Desactivar los motores.
    
    if (pasos_volver_al_origen == 4) {
      //Establecer la variable inicio en 0 para parar los motores.
      inicio = 0;
      //Establece pasos_volver_al_origen en 5 para evitar entrar en los demás condicionales.
      pasos_volver_al_origen = 5;
    }

  }
}
