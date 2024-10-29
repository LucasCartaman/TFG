
void gestor_canal_1(void) {
  tiempo_medido = TIMER2_BASE->CCR1 - tiempo_medido_inicial;
  if (tiempo_medido < 0)tiempo_medido += 0xFFFF;
  tiempo_medido_inicial = TIMER2_BASE->CCR1;
  if (tiempo_medido > 3000)contador_selector_canal = 0;
  else contador_selector_canal++;

  if (contador_selector_canal == 1)canal_1 = tiempo_medido;
  if (contador_selector_canal == 2)canal_2 = tiempo_medido;
  if (contador_selector_canal == 3)canal_3 = tiempo_medido;
  if (contador_selector_canal == 4)canal_4 = tiempo_medido;
  if (contador_selector_canal == 5)canal_5 = tiempo_medido;
  if (contador_selector_canal == 6)canal_6 = tiempo_medido;
}
  