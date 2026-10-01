// SerialYLed
// revisa cada hora si la unión soviética sigue existiendo,
// muestra el resultado en el monitor serie y en el LED de la placa:
// - parpadeo rápido: conectando
// - destello corto cada 3 segundos: existe
// - encendido fijo: ya no existe
// - doble destello: error o todavía sin respuesta
//
// funciona en arduino nano 33 iot y raspberry pi pico w

#include <SiguesAhi.h>

// crea un archivo secret.h en la carpeta de este ejemplo con:
//   #define NOMBRE_RED "nombre de tu red"
//   #define CLAVE_RED "clave de tu red"
#if __has_include("secret.h")
#include "secret.h"
#endif

#ifndef NOMBRE_RED
#define NOMBRE_RED "nombre de tu red"
#endif

#ifndef CLAVE_RED
#define CLAVE_RED "clave de tu red"
#endif

SiguesAhi sigues;
SalidaSerial salidaSerial(Serial);
SalidaLed salidaLed(LED_BUILTIN);

void setup() {
  Serial.begin(115200);
  // esperar el monitor serie como máximo 3 segundos,
  // para que funcione también sin computador
  while (!Serial && millis() < 3000) {
  }

  sigues.configurarRed(NOMBRE_RED, CLAVE_RED);
  // id de wikidata de la unión soviética, ver https://www.wikidata.org
  sigues.configurarEntidad("Q15180");
  sigues.configurarIntervalo(3600);

  sigues.agregarSalida(salidaSerial);
  sigues.agregarSalida(salidaLed);

  // descomenta para ver los detalles de cada consulta
  // sigues.configurarRegistro(Serial);

  // reinicia la placa si algo se cuelga
  sigues.activarVigilante();

  sigues.comenzar();
}

void loop() { sigues.actualizar(); }
