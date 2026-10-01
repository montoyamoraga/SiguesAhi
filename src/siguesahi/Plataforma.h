// Plataforma.h
// lo que el núcleo de SiguesAhi necesita de cada plataforma.
// cada plataforma (arduino, pico sdk, pruebas en el computador) implementa
// estas clases y define las funciones milisegundos() y redDeLaPlataforma()

#ifndef SIGUES_AHI_PLATAFORMA_H
#define SIGUES_AHI_PLATAFORMA_H

#include <stddef.h>
#include <stdint.h>

namespace siguesahi {

/// @brief destino de texto, por ejemplo el puerto serie
class Destino {
public:
  virtual ~Destino() {}
  virtual void escribir(const char *texto) = 0;
};

/// @brief conexión TLS abierta, vista como un flujo de bytes
class Conexion {
public:
  virtual ~Conexion() {}

  /// @brief envía todos los bytes, false si falla
  virtual bool escribir(const uint8_t *datos, size_t largo) = 0;

  /// @brief siguiente byte recibido, o -1 si se cerró o pasó el límite
  /// de tiempo indicado al abrir la conexión
  virtual int leer() = 0;

  virtual void cerrar() = 0;
};

/// @brief wifi, hora y conexiones TLS
class Red {
public:
  virtual ~Red() {}

  /// @brief inicia el hardware, false si el módulo wifi no responde
  virtual bool hayModulo() = 0;

  /// @brief empieza a conectarse, clave vacía para redes abiertas.
  /// puede bloquear hasta limiteMs
  virtual void iniciarConexion(const char *nombre, const char *clave,
                               uint32_t limiteMs) = 0;

  virtual bool conectada() = 0;

  virtual void desconectar() = 0;

  /// @brief empieza a sincronizar la hora, necesaria para verificar TLS
  virtual void iniciarHora() = 0;

  /// @brief true cuando la hora es confiable
  virtual bool horaLista() = 0;

  /// @brief reemplaza los certificados raíz (PEM), false si no se puede
  virtual bool configurarCertificados(const char *pem) = 0;

  /// @brief no verificar el certificado del servidor, solo para pruebas
  virtual void permitirInsegura(bool permitir) = 0;

  /// @brief abre una conexión TLS, nullptr si falla.
  /// limiteMs es el tiempo máximo de cada espera, al abrir y al leer
  virtual Conexion *abrir(const char *servidor, uint16_t puerto,
                          uint32_t limiteMs) = 0;
};

/// @brief milisegundos desde el arranque, da la vuelta cada 49 días
uint32_t milisegundos();

/// @brief la red de esta plataforma
Red &redDeLaPlataforma();

} // namespace siguesahi

#endif
