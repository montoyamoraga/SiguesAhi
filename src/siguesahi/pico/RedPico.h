// RedPico.h
// red para el pico sdk en pico w y pico 2 w:
// chip wifi CYW43, lwIP en segundo plano y TLS con mbedTLS

#ifndef SIGUES_AHI_RED_PICO_H
#define SIGUES_AHI_RED_PICO_H

#if defined(SIGUES_AHI_PICO_SDK)

#include "../Plataforma.h"

struct altcp_pcb;
struct altcp_tls_config;
struct pbuf;

namespace siguesahi {

/// @brief conexión TLS con lwIP. los callbacks de lwIP corren en una
/// interrupción y solo guardan datos; leer() y escribir() esperan en el
/// programa principal
class ConexionPico : public Conexion {
public:
  bool abrir(altcp_tls_config *configuracion, const char *servidor,
             uint16_t puerto, uint32_t limiteMs, bool verificar);

  bool escribir(const uint8_t *datos, size_t largo) override;
  int leer() override;
  void cerrar() override;

private:
  // callbacks de lwIP, definidos en RedPico.cpp con los tipos de lwIP
  friend struct LlamadasLwip;

  bool esperar(volatile bool &bandera);

  altcp_pcb *_pcb = nullptr;
  pbuf *_cola = nullptr;
  uint32_t _limiteMs = 0;

  volatile bool _resuelto = false;
  volatile bool _conectada = false;
  volatile bool _terminada = false;
  // espacio para un ip_addr_t, sin incluir lwIP en este encabezado
  alignas(4) uint8_t _direccion[24];
  bool _direccionValida = false;

  uint8_t _bufer[256];
  size_t _largo = 0;
  size_t _posicion = 0;
};

class RedPico : public Red {
public:
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
  ConexionPico _conexion;
  altcp_tls_config *_configuracionSegura = nullptr;
  altcp_tls_config *_configuracionInsegura = nullptr;
  const char *_certificados = nullptr;
  bool _insegura = false;
  bool _iniciada = false;
};

/// @brief true cuando el chip CYW43 está iniciado (lo necesita el LED)
bool cyw43Listo();

} // namespace siguesahi

#endif // SIGUES_AHI_PICO_SDK

#endif
