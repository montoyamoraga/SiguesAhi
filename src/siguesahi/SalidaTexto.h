// SalidaTexto.h
// escribe en texto cada cambio del instrumento en un Destino.
// SalidaSerial de cada plataforma se basa en esta clase

#ifndef SIGUES_AHI_SALIDA_TEXTO_H
#define SIGUES_AHI_SALIDA_TEXTO_H

#include "Plataforma.h"
#include "Salida.h"

namespace siguesahi {

class SalidaTexto : public Salida {
public:
  explicit SalidaTexto(Destino &destino) : _destino(destino) {}

  void notificar(const Informe &informe) override;

private:
  void linea(const char *a, const char *b = nullptr, const char *c = nullptr,
             const char *d = nullptr);

  Destino &_destino;
  Informe _anterior;
  bool _primera = true;
};

} // namespace siguesahi

#endif
