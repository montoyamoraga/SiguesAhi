// SalidaParpadeo.h
// muestra el estado del instrumento encendiendo y apagando algo, por
// ejemplo un LED:
// - conectando:         parpadeo rápido
// - existe:             un destello corto cada 3 segundos
// - ya no existe:       encendido fijo
// - desconocido/error:  doble destello cada 2 segundos
// SalidaLed de cada plataforma se basa en esta clase

#ifndef SIGUES_AHI_SALIDA_PARPADEO_H
#define SIGUES_AHI_SALIDA_PARPADEO_H

#include "Salida.h"

namespace siguesahi {

class SalidaParpadeo : public Salida {
public:
  void actualizar(uint32_t ahoraMs) override;
  void notificar(const Informe &informe) override;

protected:
  /// @brief enciende o apaga el LED, lo implementa cada plataforma
  virtual void encender(bool encendido) = 0;

private:
  // duraciones alternadas encendido/apagado en milisegundos, 0 termina
  const uint16_t *_patron = nullptr;
  uint8_t _paso = 0;
  uint32_t _inicioPaso = 0;
};

} // namespace siguesahi

#endif
