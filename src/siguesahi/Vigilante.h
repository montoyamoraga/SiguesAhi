// Vigilante.h
// perro guardián (watchdog): reinicia la placa si el programa se cuelga.
//
// el hardware exige alimentarlo cada 8 segundos. algunas operaciones de red
// bloquean más que eso (conectarse a wifi, una consulta lenta), así que
// durante esas operaciones una interrupción periódica lo sigue alimentando,
// pero solo hasta un límite de tiempo declarado de antemano. si la operación
// se pasa del límite, la interrupción deja de alimentarlo y la placa se
// reinicia.
//
// - pico w y pico 2 w: watchdog del rp2040/rp2350 y un temporizador del sdk
// - nano 33 iot y otras samd21: registros WDT y sysTickHook() del núcleo
// - pico w y pico 2 w con el pico sdk: igual que con arduino-pico

#ifndef SIGUES_AHI_VIGILANTE_H
#define SIGUES_AHI_VIGILANTE_H

#include <stdint.h>

namespace siguesahi {

class Vigilante {
public:
  /// @brief tiempo máximo sin alimentar fuera de una operación vigilada
  static const uint32_t PERIODO_MS = 8000;

  /// @brief activa el watchdog de hardware, false si la placa no lo soporta
  bool comenzar();

  bool activo() const { return _activo; }

  /// @brief reinicia la cuenta, llamar seguido desde el programa principal
  void alimentar();

  /// @brief true si el último reinicio lo causó el watchdog
  static bool ultimoReinicioFueVigilante();

  /// @brief permite bloquear hasta limiteMs sin que el watchdog reinicie
  void comenzarBloqueo(uint32_t limiteMs);

  /// @brief termina el bloqueo permitido y alimenta el watchdog
  void terminarBloqueo();

private:
  bool _activo = false;
};

/// @brief marca una operación bloqueante mientras exista este objeto
class BloqueoVigilado {
public:
  BloqueoVigilado(Vigilante &vigilante, uint32_t limiteMs)
      : _vigilante(vigilante) {
    _vigilante.comenzarBloqueo(limiteMs);
  }
  ~BloqueoVigilado() { _vigilante.terminarBloqueo(); }

private:
  Vigilante &_vigilante;
};

} // namespace siguesahi

#endif
