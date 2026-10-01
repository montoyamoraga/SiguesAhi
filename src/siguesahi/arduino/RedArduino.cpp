// RedArduino.cpp

#if defined(ARDUINO)

#include "RedArduino.h"

#if SIGUES_AHI_RED_PICO_W
#include "../Certificados.h"
#include <time.h>
#endif

namespace siguesahi {

uint32_t milisegundos() { return millis(); }

Red &redDeLaPlataforma() {
  static RedArduino red;
  return red;
}

// --- ConexionArduino ---

void ConexionArduino::usar(Client *cliente, uint32_t limiteMs) {
  _cliente = cliente;
  _limiteMs = limiteMs;
  _largo = 0;
  _posicion = 0;
}

bool ConexionArduino::escribir(const uint8_t *datos, size_t largo) {
  return _cliente != nullptr && _cliente->write(datos, largo) == largo;
}

int ConexionArduino::leer() {
  if (_posicion < _largo) {
    return _bufer[_posicion++];
  }
  if (_cliente == nullptr) {
    return -1;
  }
  uint32_t inicio = millis();
  while (true) {
    int disponibles = _cliente->available();
    if (disponibles > 0) {
      int leidos = _cliente->read(_bufer, sizeof(_bufer));
      if (leidos > 0) {
        _largo = (size_t)leidos;
        _posicion = 1;
        return _bufer[0];
      }
    } else if (!_cliente->connected()) {
      return -1;
    }
    if (millis() - inicio >= _limiteMs) {
      return -1;
    }
    delay(1);
  }
}

void ConexionArduino::cerrar() {
  if (_cliente != nullptr) {
    _cliente->stop();
  }
  usar(nullptr, 0);
}

// --- RedArduino ---

#if SIGUES_AHI_RED_PICO_W
// cualquier hora posterior a 2024 se considera sincronizada
static const time_t HORA_MINIMA = 1704067200;
#endif

RedArduino::~RedArduino() {
#if SIGUES_AHI_RED_PICO_W
  delete _anclas;
#endif
}

bool RedArduino::hayModulo() {
#if SIGUES_AHI_RED_PICO_W
  return true;
#else
  return WiFi.status() != WL_NO_MODULE;
#endif
}

void RedArduino::iniciarConexion(const char *nombre, const char *clave,
                                 uint32_t limiteMs) {
  bool abierta = clave == nullptr || clave[0] == '\0';
#if SIGUES_AHI_RED_PICO_W
  (void)limiteMs;
  WiFi.mode(WIFI_STA);
  if (abierta) {
    WiFi.beginNoBlock(nombre);
  } else {
    WiFi.beginNoBlock(nombre, clave);
  }
#else
  // en WiFiNINA begin() bloquea hasta conectarse o fallar,
  // 50 segundos por omisión
  WiFi.setTimeout(limiteMs);
  if (abierta) {
    WiFi.begin(nombre);
  } else {
    WiFi.begin(nombre, clave);
  }
#endif
}

bool RedArduino::conectada() {
#if SIGUES_AHI_RED_PICO_W
  return WiFi.connected();
#else
  return WiFi.status() == WL_CONNECTED;
#endif
}

void RedArduino::desconectar() {
  _conexion.cerrar();
  WiFi.disconnect();
#if SIGUES_AHI_RED_PICO_W
  _horaIniciada = false;
#endif
}

void RedArduino::iniciarHora() {
#if SIGUES_AHI_RED_PICO_W
  if (!_horaIniciada) {
    NTP.begin("pool.ntp.org", "time.nist.gov");
    _horaIniciada = true;
  }
#endif
}

bool RedArduino::horaLista() {
#if SIGUES_AHI_RED_PICO_W
  return _insegura || time(nullptr) > HORA_MINIMA;
#else
  // el módulo NINA verifica los certificados con su propia hora
  return true;
#endif
}

bool RedArduino::configurarCertificados(const char *pem) {
#if SIGUES_AHI_RED_PICO_W
  _certificados = pem;
  delete _anclas;
  _anclas = nullptr;
  return true;
#else
  // WiFiNINA usa los certificados guardados en el firmware del módulo
  (void)pem;
  return false;
#endif
}

void RedArduino::permitirInsegura(bool permitir) { _insegura = permitir; }

Conexion *RedArduino::abrir(const char *servidor, uint16_t puerto,
                            uint32_t limiteMs) {
  _conexion.cerrar();
  _cliente.setTimeout(limiteMs);
#if SIGUES_AHI_RED_PICO_W
  if (_insegura) {
    _cliente.setInsecure();
  } else {
    if (_anclas == nullptr) {
      _anclas = new BearSSL::X509List(_certificados ? _certificados
                                                    : CERTIFICADOS_RAIZ);
    }
    _cliente.setTrustAnchors(_anclas);
    _cliente.setX509Time(time(nullptr));
  }
#endif
  if (!_cliente.connect(servidor, puerto)) {
    _cliente.stop();
    return nullptr;
  }
  _conexion.usar(&_cliente, limiteMs);
  return &_conexion;
}

} // namespace siguesahi

#endif // ARDUINO
