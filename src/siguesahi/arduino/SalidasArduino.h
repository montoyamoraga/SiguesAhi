// SalidasArduino.h
// salidas y destinos de texto para arduino

#ifndef SIGUES_AHI_SALIDAS_ARDUINO_H
#define SIGUES_AHI_SALIDAS_ARDUINO_H

#if defined(ARDUINO)

#include <Arduino.h>

#include "../Plataforma.h"
#include "../SalidaParpadeo.h"
#include "../SalidaTexto.h"

namespace siguesahi {

/// @brief destino de texto sobre cualquier Print, por ejemplo Serial
class DestinoPrint : public Destino {
public:
  DestinoPrint() {}
  explicit DestinoPrint(Print &destino) : _destino(&destino) {}

  void usar(Print &destino) { _destino = &destino; }

  void escribir(const char *texto) override {
    if (_destino != nullptr) {
      _destino->print(texto);
    }
  }

private:
  Print *_destino = nullptr;
};

} // namespace siguesahi

/// @brief escribe cada cambio del instrumento, por ejemplo en Serial
class SalidaSerial : public siguesahi::SalidaTexto {
public:
  /// @param destino cualquier Print, por ejemplo Serial o Serial1
  explicit SalidaSerial(Print &destino)
      : siguesahi::SalidaTexto(_destinoPrint), _destinoPrint(destino) {}

private:
  siguesahi::DestinoPrint _destinoPrint;
};

/// @brief muestra el estado del instrumento con un LED, ver SalidaParpadeo.h
class SalidaLed : public siguesahi::SalidaParpadeo {
public:
  /// @param pin pin del LED, por ejemplo LED_BUILTIN
  /// @param activoEnAlto false si el LED se enciende con LOW
  explicit SalidaLed(uint8_t pin, bool activoEnAlto = true)
      : _pin(pin), _activoEnAlto(activoEnAlto) {}

  void comenzar() override {
    pinMode(_pin, OUTPUT);
    _iniciado = true;
    encender(false);
  }

protected:
  void encender(bool encendido) override {
    if (_iniciado) {
      digitalWrite(_pin, encendido == _activoEnAlto ? HIGH : LOW);
    }
  }

private:
  uint8_t _pin;
  bool _activoEnAlto;
  bool _iniciado = false;
};

#endif // ARDUINO

#endif
