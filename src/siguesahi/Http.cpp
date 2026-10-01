// Http.cpp

#include "Http.h"

#include <stdio.h>
#include <string.h>

#include "Wikidata.h"

namespace siguesahi {

// adaptador para que ArduinoJson lea directamente de la conexión
class LectorConexion {
public:
  explicit LectorConexion(Conexion &conexion) : _conexion(conexion) {}

  int read() { return _conexion.leer(); }

  size_t readBytes(char *destino, size_t largo) {
    size_t leidos = 0;
    while (leidos < largo) {
      int c = _conexion.leer();
      if (c < 0) {
        break;
      }
      destino[leidos++] = (char)c;
    }
    return leidos;
  }

private:
  Conexion &_conexion;
};

bool construirPeticion(const char *servidor, const char *ruta,
                       const char *agente, char *salida, size_t capacidad) {
  int largo = snprintf(salida, capacidad,
                       "GET %s HTTP/1.0\r\n"
                       "Host: %s\r\n"
                       "User-Agent: %s\r\n"
                       "Accept: application/json\r\n"
                       "Connection: close\r\n\r\n",
                       ruta, servidor, agente);
  return largo > 0 && (size_t)largo < capacidad;
}

// lee hasta el fin de línea, sin incluirlo, false si se cortó antes
static bool leerLinea(Conexion &conexion, char *linea, size_t capacidad) {
  size_t largo = 0;
  while (true) {
    int c = conexion.leer();
    if (c < 0) {
      linea[largo] = '\0';
      return false;
    }
    if (c == '\n') {
      break;
    }
    if (c != '\r' && largo + 1 < capacidad) {
      linea[largo++] = (char)c;
    }
  }
  linea[largo] = '\0';
  return true;
}

// avanza hasta la línea vacía que separa los encabezados del cuerpo
static bool saltarEncabezados(Conexion &conexion) {
  // estado: cuántos caracteres de "\r\n\r\n" llevamos, contando desde el
  // "\n" final de la línea de estado que ya se leyó
  static const char FIN[] = "\r\n\r\n";
  size_t coincidencias = 2;
  while (coincidencias < 4) {
    int c = conexion.leer();
    if (c < 0) {
      return false;
    }
    if (c == FIN[coincidencias]) {
      coincidencias++;
    } else if (c == '\r') {
      coincidencias = 1;
    } else {
      coincidencias = 0;
    }
  }
  return true;
}

Error pedirJson(Conexion &conexion, const char *peticion,
                JsonDocument &respuesta, const JsonDocument &filtro,
                char *lineaEstado, size_t capacidadLinea) {
  if (!conexion.escribir((const uint8_t *)peticion, strlen(peticion))) {
    return Error::CONEXION;
  }
  if (!leerLinea(conexion, lineaEstado, capacidadLinea)) {
    return Error::RESPUESTA;
  }
  if (codigoEstadoHttp(lineaEstado) != 200) {
    return Error::HTTP;
  }
  if (!saltarEncabezados(conexion)) {
    return Error::RESPUESTA;
  }
  LectorConexion lector(conexion);
  DeserializationError error =
      deserializeJson(respuesta, lector, DeserializationOption::Filter(filtro));
  if (error) {
    return Error::RESPUESTA;
  }
  return Error::NINGUNO;
}

} // namespace siguesahi
