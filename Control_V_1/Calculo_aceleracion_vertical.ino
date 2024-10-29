/*
| |<--------------------->| |<--------------------->| |<--------------------->| |

Calculo_aceleracion_vertical

Realiza el cálculo de la aceleración vertical, utilizando promedios a corto y 
largo plazo para suavizar las lecturas. Se emplean memorias rotativas para 
almacenar valores anteriores y corregir posibles anomalías en la aceleración.
La variable alt_acel_integrado acumula la aceleración integrada, permitiendo 
estimar cambios en la altitud.

| |<--------------------->| |<--------------------->| |<--------------------->| |
*/


void calculo_aceleracion_vertical(void) {
  //Incrementa el índice de la memoria rotativa corta.
  acel_z_prom_corto_direccion_mem_rotativa++;
  //Reinicia el valor de la dirección cuando llega a 25.
  if (acel_z_prom_corto_direccion_mem_rotativa == 25)acel_z_prom_corto_direccion_mem_rotativa = 0;

  //Resta el valor actual en la memoria rotativa corta del total acumulado.
  acel_z_prom_corto_total -= acel_z_prom_corto[acel_z_prom_corto_direccion_mem_rotativa];
  //Almacena el valor actual de aceleración vertical en la memoria rotativa corta.
  acel_z_prom_corto[acel_z_prom_corto_direccion_mem_rotativa] = acel_vector_total;
  //Suma el nuevo valor al total acumulado de la memoria rotativa corta.
  acel_z_prom_corto_total += acel_z_prom_corto[acel_z_prom_corto_direccion_mem_rotativa];

  //Una vez completado el promedio corto, se realiza un promedio largo.
  if (acel_z_prom_corto_direccion_mem_rotativa == 0) {
    //Incrementa el índice de la memoria rotativa larga.
    acel_z_prom_largo_direccion_mem_rotativa++;

    //Reinicia el valor de la dirección cuando llega a 50.
    if (acel_z_prom_largo_direccion_mem_rotativa == 50)acel_z_prom_largo_direccion_mem_rotativa = 0;

    //Resta el valor actual en la memoria rotativa larga del total acumulado.
    acel_z_prom_largo_total -= acel_z_prom_largo[acel_z_prom_largo_direccion_mem_rotativa];
    //Calcula y almacena el promedio de la memoria rotativa corta en la memoria rotativa larga.
    acel_z_prom_largo[acel_z_prom_largo_direccion_mem_rotativa] = acel_z_prom_corto_total / 25;
    // Suma el nuevo valor al total acumulado de la memoria rotativa larga.
    acel_z_prom_largo_total += acel_z_prom_largo[acel_z_prom_largo_direccion_mem_rotativa];
  }
  //Calcula el promedio total de la memoria rotativa larga.
  acel_z_prom_total = acel_z_prom_largo_total / 50;

  // Realiza la integración de la aceleración para estimar cambios en la altitud.
  alt_acel_integrado += acel_vector_total - acel_z_prom_total;

  //Para evitar cambios bruscos en la altitud, se establecen límites y bandas muertas.

  if (acel_vector_total - acel_z_prom_total < 400 || acel_vector_total - acel_z_prom_total > 400) {

    if (acel_z_prom_corto_total / 25 - acel_z_prom_total < 500 && acel_z_prom_corto_total / 25 - acel_z_prom_total > -500)

      if (alt_acel_integrado > 200)alt_acel_integrado -= 200;
      else if (alt_acel_integrado < -200)alt_acel_integrado += 200;
  }
}
