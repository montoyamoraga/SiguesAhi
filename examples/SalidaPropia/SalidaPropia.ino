// SalidaPropia
// cómo crear una salida propia: aquí, un relé o zumbador en un pin
// que se activa solo cuando la institución deja de existir

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

class SalidaAlarma : public siguesahi::Salida {
public:
  explicit SalidaAlarma(uint8_t pin) : _pin(pin) {}

  void comenzar() override {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
  }

  void notificar(const siguesahi::Informe &informe) override {
    bool activar = informe.estado == siguesahi::Estado::NO_EXISTE;
    digitalWrite(_pin, activar ? HIGH : LOW);
  }

private:
  uint8_t _pin;
};

SiguesAhi sigues;
SalidaLed salidaLed(LED_BUILTIN);
SalidaAlarma salidaAlarma(2);

void setup() {
  sigues.configurarRed(NOMBRE_RED, CLAVE_RED);
  sigues.configurarEntidad("Q863259");
  sigues.agregarSalida(salidaLed);
  sigues.agregarSalida(salidaAlarma);
  // reinicia la placa si algo se cuelga
  sigues.activarVigilante();

  sigues.comenzar();
}

void loop() { sigues.actualizar(); }
