// SalidaParpadeo.cpp

#include "SalidaParpadeo.h"

#include "Plataforma.h"

namespace siguesahi {

// duraciones alternadas encendido, apagado, encendido... terminadas en 0
static const uint16_t PATRON_CONECTANDO[] = {100, 100, 0};
static const uint16_t PATRON_EXISTE[] = {50, 2950, 0};
static const uint16_t PATRON_NO_EXISTE[] = {1000, 0};
static const uint16_t PATRON_PROBLEMA[] = {100, 150, 100, 1650, 0};

void SalidaParpadeo::notificar(const Informe &informe) {
  const uint16_t *patron;
  if (informe.estado == Estado::NO_EXISTE) {
    // la alarma tiene prioridad sobre cualquier problema de red
    patron = PATRON_NO_EXISTE;
  } else if (informe.fase == Fase::CONECTANDO_WIFI ||
             informe.fase == Fase::SINCRONIZANDO_HORA) {
    patron = PATRON_CONECTANDO;
  } else if (informe.error != Error::NINGUNO ||
             informe.estado == Estado::DESCONOCIDO) {
    patron = PATRON_PROBLEMA;
  } else {
    patron = PATRON_EXISTE;
  }

  if (patron != _patron) {
    _patron = patron;
    _paso = 0;
    _inicioPaso = milisegundos();
    encender(true);
  }
}

void SalidaParpadeo::actualizar(uint32_t ahoraMs) {
  if (_patron == nullptr || ahoraMs - _inicioPaso < _patron[_paso]) {
    return;
  }
  _inicioPaso = ahoraMs;
  _paso++;
  if (_patron[_paso] == 0) {
    _paso = 0;
  }
  // los pasos pares encienden, los impares apagan;
  // un patrón de un solo paso queda encendido fijo
  encender(_paso % 2 == 0);
}

} // namespace siguesahi
