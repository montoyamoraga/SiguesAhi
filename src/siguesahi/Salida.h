// Salida.h
// clase base para todas las salidas del instrumento.
// para crear una salida propia, hereda de siguesahi::Salida
// y sobrescribe los métodos que necesites

#ifndef SIGUES_AHI_SALIDA_H
#define SIGUES_AHI_SALIDA_H

#include <stdint.h>

#include "Tipos.h"

namespace siguesahi {

class Salida {
public:
  virtual ~Salida() {}

  /// @brief se llama una vez desde SiguesAhi::comenzar()
  virtual void comenzar() {}

  /// @brief se llama en cada SiguesAhi::actualizar(), para animaciones
  virtual void actualizar(uint32_t ahoraMs) { (void)ahoraMs; }

  /// @brief se llama cada vez que cambia algo del informe
  virtual void notificar(const Informe &informe) { (void)informe; }
};

} // namespace siguesahi

#endif
