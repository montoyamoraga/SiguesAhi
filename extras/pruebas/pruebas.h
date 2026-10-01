// pruebas.h
// utilidades compartidas por las pruebas en el computador

#ifndef SIGUES_AHI_PRUEBAS_H
#define SIGUES_AHI_PRUEBAS_H

#include <stdio.h>
#include <string.h>

extern int fallas;
extern int pruebas;

#define VERIFICAR(condicion)                                                   \
  do {                                                                         \
    pruebas++;                                                                 \
    if (!(condicion)) {                                                        \
      fallas++;                                                                \
      printf("FALLA %s:%d: %s\n", __FILE__, __LINE__, #condicion);             \
    }                                                                          \
  } while (0)

#define VERIFICAR_TEXTO(obtenido, esperado)                                    \
  do {                                                                         \
    pruebas++;                                                                 \
    if (strcmp((obtenido), (esperado)) != 0) {                                 \
      fallas++;                                                                \
      printf("FALLA %s:%d: \"%s\" != \"%s\"\n", __FILE__, __LINE__,            \
             (obtenido), (esperado));                                          \
    }                                                                          \
  } while (0)

void probarInstrumento();

#endif
