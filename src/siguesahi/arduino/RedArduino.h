// RedArduino.h
// red para arduino:
// - pico w, pico 2 w y otras placas rp2040/rp2350 con chip CYW43 en el
//   núcleo arduino-pico: WiFi.h y TLS con BearSSL
// - nano 33 iot, mkr wifi 1010, nano rp2040 connect y otras con WiFiNINA:
//   TLS hecho por el módulo NINA, con sus propios certificados

#ifndef SIGUES_AHI_RED_ARDUINO_H
#define SIGUES_AHI_RED_ARDUINO_H

#if defined(ARDUINO)

#include <Arduino.h>

#if defined(ARDUINO_ARCH_RP2040) && defined(PICO_CYW43_SUPPORTED)
#define SIGUES_AHI_RED_PICO_W 1
#include <WiFi.h>
#include <WiFiClientSecure.h>
#else
#define SIGUES_AHI_RED_NINA 1
#include <WiFiNINA.h>
#endif

#include "../Plataforma.h"

namespace siguesahi {

/// @brief conexión sobre un Client de arduino, con lectura en bloques
class ConexionArduino : public Conexion {
public:
  void usar(Client *cliente, uint32_t limiteMs);

  bool escribir(const uint8_t *datos, size_t largo) override;
  int leer() override;
  void cerrar() override;

private:
  Client *_cliente = nullptr;
  uint32_t _limiteMs = 0;
  // con WiFiNINA cada read() es una transacción SPI, mejor leer en bloques
  uint8_t _bufer[128];
  size_t _largo = 0;
  size_t _posicion = 0;
};

class RedArduino : public Red {
public:
  ~RedArduino();

  bool hayModulo() override;
  void iniciarConexion(const char *nombre, const char *clave,
                       uint32_t limiteMs) override;
  bool conectada() override;
  void desconectar() override;
  void iniciarHora() override;
  bool horaLista() override;
  bool configurarCertificados(const char *pem) override;
  void permitirInsegura(bool permitir) override;
  Conexion *abrir(const char *servidor, uint16_t puerto,
                  uint32_t limiteMs) override;

private:
#if SIGUES_AHI_RED_PICO_W
  WiFiClientSecure _cliente;
  BearSSL::X509List *_anclas = nullptr;
  const char *_certificados = nullptr;
  bool _horaIniciada = false;
#else
  WiFiSSLClient _cliente;
#endif
  ConexionArduino _conexion;
  bool _insegura = false;
};

} // namespace siguesahi

#endif // ARDUINO

#endif
