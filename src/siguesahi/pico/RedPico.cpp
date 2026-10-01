// RedPico.cpp

#if defined(SIGUES_AHI_PICO_SDK)

#include "RedPico.h"

#include <string.h>
#include <time.h>

#include "hardware/sync.h"
#include "lwip/altcp.h"
#include "lwip/altcp_tls.h"
#include "lwip/apps/sntp.h"
#include "lwip/dns.h"
#include "lwip/pbuf.h"
#include "mbedtls/ssl.h"
#include "pico/cyw43_arch.h"
#include "pico/time.h"

#include "../Certificados.h"

// --- hora del sistema, compartida con lwIP (SNTP) y mbedTLS ---

// cualquier hora posterior a 2024 se considera sincronizada
static const uint32_t HORA_MINIMA = 1704067200;

static volatile uint32_t segundosBase = 0;
static volatile uint64_t microsegundosBase = 0;

// lwIP la llama al recibir la hora por SNTP, ver lwipopts.h
extern "C" void siguesahi_pico_fijar_hora(uint32_t segundos) {
  uint32_t estado = save_and_disable_interrupts();
  segundosBase = segundos;
  microsegundosBase = time_us_64();
  restore_interrupts(estado);
}

// mbedTLS la usa para revisar la vigencia de los certificados,
// ver mbedtls_config.h
extern "C" time_t siguesahi_pico_hora(time_t *hora) {
  uint32_t estado = save_and_disable_interrupts();
  uint32_t base = segundosBase;
  uint64_t inicio = microsegundosBase;
  restore_interrupts(estado);
  time_t ahora = 0;
  if (base != 0) {
    ahora = (time_t)base + (time_t)((time_us_64() - inicio) / 1000000ULL);
  }
  if (hora != nullptr) {
    *hora = ahora;
  }
  return ahora;
}

namespace siguesahi {

uint32_t milisegundos() { return to_ms_since_boot(get_absolute_time()); }

Red &redDeLaPlataforma() {
  static RedPico red;
  return red;
}

bool cyw43Listo() { return cyw43_is_initialized(&cyw43_state); }

// --- callbacks de lwIP: corren en una interrupción, solo guardan datos ---

struct LlamadasLwip {
  static_assert(sizeof(ip_addr_t) <= sizeof(ConexionPico::_direccion),
                "_direccion es muy chica para ip_addr_t");

  static void alResolver(const char *, const ip_addr_t *direccion,
                         void *argumento) {
    ConexionPico *conexion = static_cast<ConexionPico *>(argumento);
    if (direccion != nullptr) {
      memcpy(conexion->_direccion, direccion, sizeof(ip_addr_t));
      conexion->_direccionValida = true;
    }
    conexion->_resuelto = true;
  }

  static err_t alConectar(void *argumento, altcp_pcb *, err_t) {
    // con altcp_tls esto ocurre cuando termina el saludo TLS
    static_cast<ConexionPico *>(argumento)->_conectada = true;
    return ERR_OK;
  }

  static err_t alRecibir(void *argumento, altcp_pcb *, pbuf *datos, err_t) {
    ConexionPico *conexion = static_cast<ConexionPico *>(argumento);
    if (datos == nullptr) {
      // el servidor cerró la conexión
      conexion->_terminada = true;
      return ERR_OK;
    }
    // no se avisa a lwIP que se leyeron hasta que leer() los consuma,
    // así la ventana TCP frena al servidor si no damos abasto
    if (conexion->_cola == nullptr) {
      conexion->_cola = datos;
    } else {
      pbuf_cat(conexion->_cola, datos);
    }
    return ERR_OK;
  }

  static void alFallar(void *argumento, err_t) {
    // lwIP ya liberó la conexión
    ConexionPico *conexion = static_cast<ConexionPico *>(argumento);
    conexion->_pcb = nullptr;
    conexion->_terminada = true;
    conexion->_resuelto = true;
  }
};

// --- ConexionPico ---

bool ConexionPico::esperar(volatile bool &bandera) {
  uint32_t inicio = milisegundos();
  while (!bandera && !_terminada) {
    if (milisegundos() - inicio >= _limiteMs) {
      return false;
    }
    sleep_ms(1);
  }
  return bandera;
}

bool ConexionPico::abrir(altcp_tls_config *configuracion, const char *servidor,
                         uint16_t puerto, uint32_t limiteMs, bool verificar) {
  cerrar();
  _limiteMs = limiteMs;
  _resuelto = false;
  _conectada = false;
  _terminada = false;
  _direccionValida = false;

  // dns
  ip_addr_t *direccion = reinterpret_cast<ip_addr_t *>(_direccion);
  cyw43_arch_lwip_begin();
  err_t error =
      dns_gethostbyname(servidor, direccion, LlamadasLwip::alResolver, this);
  cyw43_arch_lwip_end();
  if (error == ERR_OK) {
    _direccionValida = true;
  } else if (error != ERR_INPROGRESS || !esperar(_resuelto)) {
    return false;
  }
  if (!_direccionValida) {
    return false;
  }
  _terminada = false;

  // tcp y tls
  cyw43_arch_lwip_begin();
  _pcb = altcp_tls_new(configuracion, IP_GET_TYPE(direccion));
  if (_pcb != nullptr) {
    altcp_arg(_pcb, this);
    altcp_recv(_pcb, LlamadasLwip::alRecibir);
    altcp_err(_pcb, LlamadasLwip::alFallar);
    // el nombre se usa para SNI y para verificar el certificado
    mbedtls_ssl_set_hostname(
        static_cast<mbedtls_ssl_context *>(altcp_tls_context(_pcb)), servidor);
    error = altcp_connect(_pcb, direccion, puerto, LlamadasLwip::alConectar);
  }
  cyw43_arch_lwip_end();
  if (_pcb == nullptr || error != ERR_OK || !esperar(_conectada)) {
    cerrar();
    return false;
  }

  // lwIP no exige un certificado válido (ALTCP_MBEDTLS_AUTHMODE es
  // OPTIONAL), así que se revisa aquí: cadena, nombre y vigencia
  if (verificar) {
    cyw43_arch_lwip_begin();
    uint32_t problemas =
        _pcb == nullptr
            ? 0xFFFFFFFFu
            : mbedtls_ssl_get_verify_result(
                  static_cast<mbedtls_ssl_context *>(altcp_tls_context(_pcb)));
    cyw43_arch_lwip_end();
    if (problemas != 0) {
      cerrar();
      return false;
    }
  }
  return true;
}

bool ConexionPico::escribir(const uint8_t *datos, size_t largo) {
  cyw43_arch_lwip_begin();
  err_t error = ERR_CONN;
  if (_pcb != nullptr) {
    error = altcp_write(_pcb, datos, (u16_t)largo, TCP_WRITE_FLAG_COPY);
    if (error == ERR_OK) {
      error = altcp_output(_pcb);
    }
  }
  cyw43_arch_lwip_end();
  return error == ERR_OK;
}

int ConexionPico::leer() {
  if (_posicion < _largo) {
    return _bufer[_posicion++];
  }
  uint32_t inicio = milisegundos();
  while (true) {
    size_t copiados = 0;
    bool terminada;
    cyw43_arch_lwip_begin();
    if (_cola != nullptr) {
      copiados =
          _cola->tot_len < sizeof(_bufer) ? _cola->tot_len : sizeof(_bufer);
      pbuf_copy_partial(_cola, _bufer, (u16_t)copiados, 0);
      _cola = pbuf_free_header(_cola, (u16_t)copiados);
      if (_pcb != nullptr) {
        altcp_recved(_pcb, (u16_t)copiados);
      }
    }
    terminada = _terminada || _pcb == nullptr;
    cyw43_arch_lwip_end();

    if (copiados > 0) {
      _largo = copiados;
      _posicion = 1;
      return _bufer[0];
    }
    if (terminada || milisegundos() - inicio >= _limiteMs) {
      return -1;
    }
    sleep_ms(1);
  }
}

void ConexionPico::cerrar() {
  cyw43_arch_lwip_begin();
  if (_pcb != nullptr) {
    altcp_arg(_pcb, nullptr);
    altcp_recv(_pcb, nullptr);
    altcp_err(_pcb, nullptr);
    if (altcp_close(_pcb) != ERR_OK) {
      altcp_abort(_pcb);
    }
    _pcb = nullptr;
  }
  if (_cola != nullptr) {
    pbuf_free(_cola);
    _cola = nullptr;
  }
  cyw43_arch_lwip_end();
  _largo = 0;
  _posicion = 0;
}

// --- RedPico ---

bool RedPico::hayModulo() {
  if (!_iniciada) {
    // el programa puede haber iniciado el chip por su cuenta
    if (!cyw43_is_initialized(&cyw43_state) && cyw43_arch_init() != 0) {
      return false;
    }
    cyw43_arch_enable_sta_mode();
    _iniciada = true;
  }
  return true;
}

void RedPico::iniciarConexion(const char *nombre, const char *clave,
                              uint32_t limiteMs) {
  (void)limiteMs;
  bool abierta = clave == nullptr || clave[0] == '\0';
  // no bloquea: conectada() avisa cuando hay conexión e IP
  cyw43_arch_wifi_connect_async(nombre, abierta ? nullptr : clave,
                                abierta ? CYW43_AUTH_OPEN
                                        : CYW43_AUTH_WPA2_AES_PSK);
}

bool RedPico::conectada() {
  return _iniciada &&
         cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP;
}

void RedPico::desconectar() {
  _conexion.cerrar();
  cyw43_arch_lwip_begin();
  if (sntp_enabled()) {
    sntp_stop();
  }
  cyw43_wifi_leave(&cyw43_state, CYW43_ITF_STA);
  cyw43_arch_lwip_end();
}

void RedPico::iniciarHora() {
  cyw43_arch_lwip_begin();
  if (!sntp_enabled()) {
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_setservername(1, "time.nist.gov");
    sntp_init();
  }
  cyw43_arch_lwip_end();
}

bool RedPico::horaLista() {
  return _insegura || siguesahi_pico_hora(nullptr) > (time_t)HORA_MINIMA;
}

bool RedPico::configurarCertificados(const char *pem) {
  _certificados = pem;
  if (_configuracionSegura != nullptr) {
    _conexion.cerrar();
    altcp_tls_free_config(_configuracionSegura);
    _configuracionSegura = nullptr;
  }
  return true;
}

void RedPico::permitirInsegura(bool permitir) { _insegura = permitir; }

Conexion *RedPico::abrir(const char *servidor, uint16_t puerto,
                         uint32_t limiteMs) {
  altcp_tls_config *configuracion;
  cyw43_arch_lwip_begin();
  if (_insegura) {
    if (_configuracionInsegura == nullptr) {
      _configuracionInsegura = altcp_tls_create_config_client(nullptr, 0);
    }
    configuracion = _configuracionInsegura;
  } else {
    if (_configuracionSegura == nullptr) {
      const char *pem = _certificados ? _certificados : CERTIFICADOS_RAIZ;
      // mbedTLS exige contar el terminador nulo del PEM
      _configuracionSegura = altcp_tls_create_config_client(
          reinterpret_cast<const u8_t *>(pem), strlen(pem) + 1);
    }
    configuracion = _configuracionSegura;
  }
  cyw43_arch_lwip_end();
  if (configuracion == nullptr) {
    return nullptr;
  }
  if (!_conexion.abrir(configuracion, servidor, puerto, limiteMs, !_insegura)) {
    return nullptr;
  }
  return &_conexion;
}

} // namespace siguesahi

#endif // SIGUES_AHI_PICO_SDK
