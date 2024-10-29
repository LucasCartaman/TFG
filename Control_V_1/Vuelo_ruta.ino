/*
| |<--------------------->| |<--------------------->| |<--------------------->| |
volar_ruta

Esta función permite al dron volar a un punto siguiendo dos pasos. El primer
paso es calcular el factor de movimiento tanto para la longitud como para
la latitud, dependiendo de cual coordenada tenga un mayor error. El segundo paso
ajusta la latitud y la longitud del dron dinámicamente hasta llegar al punto de
consigna
| |<--------------------->| |<--------------------->| |<--------------------->| |
*/


void volar_ruta(void) {
  // Inicializa la variable de monitoreo de la ruta en 0.
  monitor_ruta = 0;

  if (modo_vuelo != 3 && inicio == 2){
    // Resetea el indicador de nuevo punto de ruta si no está en el modo de vuelo adecuado.
    volar_nuevo_punto = 0;
    // Reinicia el monitoreo de la ruta.
    monitor_ruta = 0;
  }
  
  
  //Si hay un nuevo punto al que volar.
  if (volar_nuevo_punto == 1) {
    //Paso 0: cálculos básicos
    if (volar_nuevo_punto_paso == 0) {
      // Inicializa el factor de movimiento de la ruta en 0.0.
      volar_ruta_factor_movimiento = 0.0;

      // Si ya se han calculado los factores de latitud o longitud, pasa al siguiente paso.
      if (volar_ruta_factor_lat == 1 || volar_ruta_factor_lon == 1)volar_nuevo_punto_paso = 1;
      //cos(((float)l_lat_gps / 1000000.0)

      // Determina cuál de los errores, latitud o longitud, es mayor y ajusta los factores en consecuencia.

      // Si el error en la latitud es mayor o igual que el de la longitud,
      if (abs(lat_gps_ruta - l_lat_ruta) >= abs(lon_gps_ruta - l_lon_ruta)) {
        // Calcula el factor de longitud basado en la relación con la latitud y establece la latitud en 1.
        volar_ruta_factor_lon = (float)abs(lon_gps_ruta - l_lon_ruta) / (float)abs(lat_gps_ruta - l_lat_ruta);
        volar_ruta_factor_lat = 1;
      }
      // Si el error en la longitud es mayor que el de la latitud,
      else {
        // calcula el factor de latitud basado en la relación con la longitud y establece la longitud en 1.
        volar_ruta_factor_lon = 1;
        volar_ruta_factor_lat = (float)abs(lat_gps_ruta - l_lat_ruta) / (float)abs(lon_gps_ruta - l_lon_ruta);
      }
    }

    //Paso 1: Volar al siguiente punto.
    if (volar_nuevo_punto_paso == 1) {
      // Indica que se está monitoreando la ruta.
      monitor_ruta = 6;
      // Si el dron ya está en el punto de ruta, se desactiva el indicador de nuevo punto.
      if (lat_gps_ruta == l_lat_ruta && lon_gps_ruta == l_lon_ruta)volar_nuevo_punto = 0;

      // Ajusta el factor de movimiento dependiendo de la cercanía al punto de ruta.

      // Si el dron está cerca del punto de ruta y el factor de movimiento es mayor que 0.05, disminuye el factor.
      if (abs(lat_gps_ruta - l_lat_ruta) < 160 && abs(lon_gps_ruta - l_lon_ruta) < 160 && volar_ruta_factor_movimiento > 0.05)volar_ruta_factor_movimiento -= 0.00015;
      // Si el factor de movimiento es menor que 0.20, aumenta el factor para acelerar el acercamiento.
      else if (volar_ruta_factor_movimiento < 0.20)volar_ruta_factor_movimiento += 0.0001;

      // Ajusta la latitud actual en función del factor de movimiento calculado.

      if (lat_gps_ruta != l_lat_ruta) {
        // Si la latitud objetivo es mayor que la actual, incrementa el ajuste.
        if (lat_gps_ruta > l_lat_ruta) l_lat_gps_ajuste_float += volar_ruta_factor_movimiento * volar_ruta_factor_lat;
        // Si la latitud objetivo es menor que la actual, disminuye el ajuste.
        if (lat_gps_ruta < l_lat_ruta) l_lat_gps_ajuste_float -= volar_ruta_factor_movimiento * volar_ruta_factor_lat;
      }

      // Ajusta la longitud actual en función del factor de movimiento calculado.

      if (lon_gps_ruta != l_lon_ruta) {
        // Si la longitud objetivo es mayor que la actual, incrementa el ajuste.
        if (lon_gps_ruta > l_lon_ruta) l_lon_gps_ajuste_float += volar_ruta_factor_movimiento * volar_ruta_factor_lon;
        // Si la longitud objetivo es menor que la actual, disminuye el ajuste.
        if (lon_gps_ruta < l_lon_ruta) l_lon_gps_ajuste_float -= volar_ruta_factor_movimiento * volar_ruta_factor_lon;
      }
    }
  }
}
