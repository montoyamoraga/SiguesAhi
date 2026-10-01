// SalidaTexto.cpp

#include "SalidaTexto.h"

#include <stdio.h>

namespace siguesahi {

void SalidaTexto::linea(const char *a, const char *b, const char *c,
                        const char *d) {
  _destino.escribir("[SiguesAhi] ");
  const char *partes[] = {a, b, c, d};
  for (const char *parte : partes) {
    if (parte != nullptr) {
      _destino.escribir(parte);
    }
  }
  _destino.escribir("\n");
}

void SalidaTexto::notificar(const Informe &informe) {
  if (_primera && informe.reinicioPorVigilante) {
    linea("la placa se reinició por el watchdog");
  }

  if (_primera || informe.fase != _anterior.fase) {
    linea(nombreFase(informe.fase));
  }

  if (informe.error != Error::NINGUNO &&
      (_primera || informe.error != _anterior.error ||
       informe.consultasFallidas != _anterior.consultasFallidas)) {
    linea("error: ", nombreError(informe.error));
  }

  if (informe.observacionesPendientes > 0 &&
      informe.observacionesPendientes != _anterior.observacionesPendientes) {
    char numero[4];
    snprintf(numero, sizeof(numero), "%u",
             (unsigned)informe.observacionesPendientes);
    linea("posible cambio de estado, observación ", numero);
  }

  if ((_primera && informe.estado != Estado::DESCONOCIDO) ||
      informe.estado != _anterior.estado) {
    const char *detalle = nullptr;
    const char *fecha = nullptr;
    if (informe.estado == Estado::NO_EXISTE) {
      if (informe.fechaDisolucion[0] != '\0') {
        detalle = ", disuelta en ";
        fecha = informe.fechaDisolucion;
      } else {
        detalle = ", fecha de disolución desconocida";
      }
    }
    char encabezado[24];
    snprintf(encabezado, sizeof(encabezado), "%s: ", informe.idEntidad);
    linea(encabezado, nombreEstado(informe.estado), detalle, fecha);
  }

  _anterior = informe;
  _primera = false;
}

} // namespace siguesahi
