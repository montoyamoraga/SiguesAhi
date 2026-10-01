// Confirmador.h
// exige varias observaciones seguidas antes de cambiar de estado,
// para que un vandalismo pasajero en wikidata no dispare la alarma

#ifndef SIGUES_AHI_CONFIRMADOR_H
#define SIGUES_AHI_CONFIRMADOR_H

#include "Tipos.h"

namespace siguesahi {

class Confirmador {
public:
  /// @brief cantidad de observaciones seguidas para confirmar un cambio
  void configurar(uint8_t necesarias) {
    _necesarias = necesarias < 1 ? 1 : necesarias;
  }

  /// @brief registra una observación, devuelve true si cambió el estado
  bool registrar(Estado observacion) {
    if (observacion == Estado::DESCONOCIDO || observacion == _estado) {
      reiniciar();
      return false;
    }
    // la primera vez que vemos que existe no hace falta confirmar,
    // porque no dispara ninguna alarma
    if (_estado == Estado::DESCONOCIDO && observacion == Estado::EXISTE) {
      _estado = observacion;
      reiniciar();
      return true;
    }
    if (observacion == _candidato) {
      _seguidas++;
    } else {
      _candidato = observacion;
      _seguidas = 1;
    }
    if (_seguidas >= _necesarias) {
      _estado = observacion;
      reiniciar();
      return true;
    }
    return false;
  }

  Estado estado() const { return _estado; }
  uint8_t pendientes() const { return _seguidas; }

private:
  void reiniciar() {
    _candidato = Estado::DESCONOCIDO;
    _seguidas = 0;
  }

  Estado _estado = Estado::DESCONOCIDO;
  Estado _candidato = Estado::DESCONOCIDO;
  uint8_t _seguidas = 0;
  uint8_t _necesarias = 3;
};

} // namespace siguesahi

#endif
