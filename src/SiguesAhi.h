// SiguesAhi.h

// un proyecto de aarón montoya-moraga
// github.com/montoyamoraga/SiguesAhi
// comenzado en septiembre de 2020

// instrumento que pregunta periódicamente a wikidata si una institución
// sigue existiendo, revisando su fecha de disolución (propiedad P576).
//
// funciona como biblioteca de arduino y como biblioteca del pico sdk,
// con la misma interfaz:
//   SiguesAhi sigues;
//   SalidaSerial salidaSerial(...);
//   SalidaLed salidaLed(...);

#ifndef SIGUES_AHI_H
#define SIGUES_AHI_H

#include "siguesahi/Instrumento.h"
#include "siguesahi/Plataforma.h"
#include "siguesahi/Salida.h"
#include "siguesahi/SalidaParpadeo.h"
#include "siguesahi/SalidaTexto.h"
#include "siguesahi/Tipos.h"

#if defined(ARDUINO)

#include "siguesahi/arduino/SalidasArduino.h"

class SiguesAhi : public siguesahi::Instrumento {
public:
  using siguesahi::Instrumento::configurarRegistro;

  /// @brief mensajes detallados de diagnóstico, por ejemplo en Serial
  void configurarRegistro(Print &destino) {
    _destinoRegistro.usar(destino);
    configurarRegistro(_destinoRegistro);
  }

private:
  siguesahi::DestinoPrint _destinoRegistro;
};

#elif defined(SIGUES_AHI_PICO_SDK)

#include "siguesahi/pico/SalidasPico.h"

class SiguesAhi : public siguesahi::Instrumento {
public:
  using siguesahi::Instrumento::configurarRegistro;

  /// @brief mensajes detallados de diagnóstico en la salida estándar
  void configurarRegistro() { configurarRegistro(_consola); }

private:
  siguesahi::DestinoConsola _consola;
};

#else

// otras plataformas, por ejemplo las pruebas en el computador, usan
// siguesahi::Instrumento directamente y definen su propia red
using SiguesAhi = siguesahi::Instrumento;

#endif

#endif
