// SalidasPico.h
// salidas y destinos de texto para el pico sdk

#ifndef SIGUES_AHI_SALIDAS_PICO_H
#define SIGUES_AHI_SALIDAS_PICO_H

#if defined(SIGUES_AHI_PICO_SDK)

#include <stdio.h>

#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"

#include "../Plataforma.h"
#include "../SalidaParpadeo.h"
#include "../SalidaTexto.h"
#include "RedPico.h"

namespace siguesahi {

/// @brief destino de texto en la salida estándar del pico sdk (stdio),
/// que puede ser USB o UART según CMakeLists.txt
class DestinoConsola : public Destino {
public:
  void escribir(const char *texto) override { fputs(texto, stdout); }
};

} // namespace siguesahi

/// @brief escribe cada cambio del instrumento en la salida estándar
class SalidaSerial : public siguesahi::SalidaTexto {
public:
  SalidaSerial() : siguesahi::SalidaTexto(_consola) {}

private:
  siguesahi::DestinoConsola _consola;
};

/// @brief muestra el estado del instrumento con un LED, ver SalidaParpadeo.h
class SalidaLed : public siguesahi::SalidaParpadeo {
public:
  /// @brief LED de la placa, conectado al chip wifi CYW43
  SalidaLed() : _gpio(-1), _activoEnAlto(true) {}

  /// @param gpio pin del LED
  /// @param activoEnAlto false si el LED se enciende con 0
  explicit SalidaLed(unsigned gpio, bool activoEnAlto = true)
      : _gpio((int)gpio), _activoEnAlto(activoEnAlto) {}

  void comenzar() override {
    if (_gpio >= 0) {
      gpio_init((unsigned)_gpio);
      gpio_set_dir((unsigned)_gpio, GPIO_OUT);
    }
    _iniciado = true;
    encender(false);
  }

protected:
  void encender(bool encendido) override {
    if (!_iniciado) {
      return;
    }
    bool nivel = encendido == _activoEnAlto;
    if (_gpio >= 0) {
      gpio_put((unsigned)_gpio, nivel);
    } else if (siguesahi::cyw43Listo()) {
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, nivel);
    }
  }

private:
  int _gpio;
  bool _activoEnAlto;
  bool _iniciado = false;
};

#endif // SIGUES_AHI_PICO_SDK

#endif
