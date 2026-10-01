// SerialYLed para el pico sdk
// revisa cada hora si la unión soviética sigue existiendo,
// muestra el resultado por USB y en el LED de la placa:
// - parpadeo rápido: conectando
// - destello corto cada 3 segundos: existe
// - encendido fijo: ya no existe
// - doble destello: error o todavía sin respuesta
//
// funciona en raspberry pi pico w y pico 2 w

#include "pico/stdlib.h"

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
SalidaSerial salidaSerial;
SalidaLed salidaLed;

int main() {
  stdio_init_all();
  // esperar la consola USB como máximo 3 segundos,
  // para que funcione también sin computador
  for (int i = 0; i < 30 && !stdio_usb_connected(); i++) {
    sleep_ms(100);
  }

  sigues.configurarRed(NOMBRE_RED, CLAVE_RED);
  // id de wikidata de la unión soviética, ver https://www.wikidata.org
  sigues.configurarEntidad("Q15180");
  sigues.configurarIntervalo(3600);

  sigues.agregarSalida(salidaSerial);
  sigues.agregarSalida(salidaLed);

  // mensajes detallados de cada consulta
  sigues.configurarRegistro();

  // reinicia la placa si algo se cuelga
  sigues.activarVigilante();

  sigues.comenzar();

  while (true) {
    sigues.actualizar();
    sleep_ms(1);
  }
}
