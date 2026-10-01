// Vigilante.cpp

#include "Vigilante.h"

#include "Plataforma.h"

// el núcleo arduino-pico está construido sobre el pico sdk,
// así que ambos usan las mismas funciones del sdk
#if defined(ARDUINO_ARCH_RP2040) || defined(SIGUES_AHI_PICO_SDK)
#define SIGUES_AHI_VIGILANTE_RP2 1
#include <hardware/watchdog.h>
#include <pico/time.h>
#elif defined(ARDUINO_ARCH_SAMD)
#include <Arduino.h>
// GCLK_GENDIV_ID solo existe en samd21, no en samd51
#if defined(GCLK_GENDIV_ID)
#define SIGUES_AHI_VIGILANTE_SAMD21 1
#endif
#endif

namespace siguesahi {

// compartidos con la interrupción: el bloqueo permitido y su límite
static volatile bool bloqueoPermitido = false;
static volatile uint32_t finBloqueo = 0;

#if SIGUES_AHI_VIGILANTE_RP2 || SIGUES_AHI_VIGILANTE_SAMD21

// cada cuánto alimenta la interrupción durante un bloqueo permitido
static const uint32_t ALIMENTAR_CADA_MS = 500;

static void alimentarHardware();

// llamada desde la interrupción periódica
static void alimentarSiCorresponde() {
  if (bloqueoPermitido && (int32_t)(finBloqueo - milisegundos()) > 0) {
    alimentarHardware();
  }
}

#endif

#if SIGUES_AHI_VIGILANTE_RP2

static repeating_timer_t temporizador;

static bool alInterrumpir(repeating_timer_t *) {
  alimentarSiCorresponde();
  return true;
}

static void alimentarHardware() { watchdog_update(); }

static bool activarHardware() {
  if (!add_repeating_timer_ms(ALIMENTAR_CADA_MS, alInterrumpir, nullptr,
                              &temporizador)) {
    return false;
  }
  // true: pausar el watchdog mientras se depura con un debugger
  watchdog_enable(Vigilante::PERIODO_MS, true);
  return true;
}

bool Vigilante::ultimoReinicioFueVigilante() {
  return watchdog_enable_caused_reboot();
}

#elif SIGUES_AHI_VIGILANTE_SAMD21

static volatile bool hardwareActivo = false;

static void alimentarHardware() {
  // escribir mientras se sincroniza bloquearía el bus, mejor saltarse una
  if (!WDT->STATUS.bit.SYNCBUSY) {
    WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
  }
}

static bool activarHardware() {
  // generador de reloj 2: oscilador de 32 kHz dividido por 32 = 1024 Hz
  GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(4);
  GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_GENEN |
                      GCLK_GENCTRL_SRC_OSCULP32K | GCLK_GENCTRL_DIVSEL;
  while (GCLK->STATUS.bit.SYNCBUSY) {
  }
  GCLK->CLKCTRL.reg =
      GCLK_CLKCTRL_ID_WDT | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2;

  WDT->CTRL.reg = 0;
  while (WDT->STATUS.bit.SYNCBUSY) {
  }
  WDT->INTENCLR.bit.EW = 1;
  // 8192 ciclos a 1024 Hz = 8 segundos
  WDT->CONFIG.bit.PER = WDT_CONFIG_PER_8K_Val;
  WDT->CTRL.bit.WEN = 0;
  while (WDT->STATUS.bit.SYNCBUSY) {
  }
  alimentarHardware();
  WDT->CTRL.bit.ENABLE = 1;
  while (WDT->STATUS.bit.SYNCBUSY) {
  }
  hardwareActivo = true;
  return true;
}

bool Vigilante::ultimoReinicioFueVigilante() {
  return (PM->RCAUSE.reg & PM_RCAUSE_WDT) != 0;
}

} // namespace siguesahi

// el núcleo samd llama a esta función en cada milisegundo, desde SysTick.
// devolver 0 deja que el núcleo siga con su manejo normal
extern "C" int sysTickHook(void) {
  static uint16_t cuenta = 0;
  if (siguesahi::hardwareActivo && ++cuenta >= siguesahi::ALIMENTAR_CADA_MS) {
    cuenta = 0;
    siguesahi::alimentarSiCorresponde();
  }
  return 0;
}

namespace siguesahi {

#else

// plataforma sin watchdog, por ejemplo las pruebas en el computador
static void alimentarHardware() {}

static bool activarHardware() { return false; }

bool Vigilante::ultimoReinicioFueVigilante() { return false; }

#endif

bool Vigilante::comenzar() {
  if (!_activo) {
    _activo = activarHardware();
  }
  return _activo;
}

void Vigilante::alimentar() {
  if (_activo) {
    alimentarHardware();
  }
}

void Vigilante::comenzarBloqueo(uint32_t limiteMs) {
  if (!_activo) {
    return;
  }
  alimentarHardware();
  finBloqueo = milisegundos() + limiteMs;
  bloqueoPermitido = true;
}

void Vigilante::terminarBloqueo() {
  if (!_activo) {
    return;
  }
  bloqueoPermitido = false;
  alimentarHardware();
}

} // namespace siguesahi
