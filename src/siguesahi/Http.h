// Http.h
// peticiones HTTP/1.0 sobre una Conexion, con respuesta JSON.
// HTTP/1.0 garantiza que la respuesta no venga por partes (chunked)

#ifndef SIGUES_AHI_HTTP_H
#define SIGUES_AHI_HTTP_H

#include <ArduinoJson.h>

#include "Plataforma.h"
#include "Tipos.h"

namespace siguesahi {

/// @brief arma la petición GET completa, false si no cabe
bool construirPeticion(const char *servidor, const char *ruta,
                       const char *agente, char *salida, size_t capacidad);

/// @brief envía la petición y lee la respuesta JSON aplicando el filtro.
/// no cierra la conexión
Error pedirJson(Conexion &conexion, const char *peticion,
                JsonDocument &respuesta, const JsonDocument &filtro,
                char *lineaEstado, size_t capacidadLinea);

} // namespace siguesahi

#endif
