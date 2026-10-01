// PorArticulo
// en vez del id de wikidata, usa el título de un artículo de wikipedia.
// la biblioteca busca la entidad de wikidata en la primera consulta

#include <SiguesAhi.h>

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

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) {
  }

  sigues.configurarRed(NOMBRE_RED, CLAVE_RED);

  // idioma de wikipedia y título del artículo, tal como aparece en la URL
  if (!sigues.configurarArticulo(
          "es", "Constitución_Política_de_la_República_de_Chile_de_1980")) {
    Serial.println("artículo inválido");
  }

  // pedir 3 consultas seguidas con el mismo resultado antes de cambiar,
  // para ignorar vandalismos pasajeros en wikidata
  sigues.configurarConfirmaciones(3);

  sigues.agregarSalida(salidaSerial);
  sigues.configurarRegistro(Serial);
  sigues.comenzar();
}

void loop() {
  sigues.actualizar();

  // también se puede leer el estado directamente
  static bool avisado = false;
  if (sigues.yaNoExiste() && !avisado) {
    Serial.print("fecha de disolución: ");
    Serial.println(sigues.fechaDisolucion());
    avisado = true;
  }
}
