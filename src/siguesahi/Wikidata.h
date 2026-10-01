// Wikidata.h
// lógica pura para hablar con la API de wikidata:
// construir rutas, interpretar respuestas y formatear fechas.
// no depende de Arduino.h, así se puede probar en el computador

#ifndef SIGUES_AHI_WIKIDATA_H
#define SIGUES_AHI_WIKIDATA_H

#include <ArduinoJson.h>
#include <stddef.h>
#include <stdint.h>

#include "Tipos.h"

namespace siguesahi {

/// @brief servidor de la API
extern const char *const SERVIDOR_WIKIDATA;

/// @brief propiedad "fecha de disolución, abolición o demolición"
extern const char *const PROPIEDAD_DISOLUCION;

/// @brief resultado de interpretar una respuesta de wikidata
struct Observacion {
  Error error = Error::NINGUNO;
  Estado estado = Estado::DESCONOCIDO;
  char fechaDisolucion[16] = {0};
};

/// @brief copia con terminador nulo, false si no cabe
bool copiarTexto(char *destino, size_t capacidad, const char *origen);

/// @brief normaliza "q15180" a "Q15180", false si no es un id válido
bool normalizarIdEntidad(const char *entrada, char *salida, size_t capacidad);

/// @brief convierte "es" en "eswiki", false si el idioma no es válido
bool construirSitio(const char *idioma, char *salida, size_t capacidad);

/// @brief codifica texto UTF-8 para una URL, false si no cabe
bool codificarUrl(const char *entrada, char *salida, size_t capacidad);

/// @brief ruta para consultar la propiedad P576 de una entidad
bool construirRutaDeclaraciones(const char *idEntidad, char *salida,
                                size_t capacidad);

/// @brief ruta para encontrar la entidad de un artículo de wikipedia
bool construirRutaEntidad(const char *sitio, const char *titulo, char *salida,
                          size_t capacidad);

/// @brief extrae el código de "HTTP/1.1 200 OK", -1 si no se entiende
int codigoEstadoHttp(const char *lineaEstado);

/// @brief filtro para leer solo lo necesario de wbgetclaims
void crearFiltroDeclaraciones(JsonDocument &filtro);

/// @brief filtro para leer solo lo necesario de wbgetentities
void crearFiltroEntidad(JsonDocument &filtro);

/// @brief interpreta la respuesta de wbgetclaims
Observacion interpretarDeclaraciones(const JsonDocument &respuesta);

/// @brief interpreta la respuesta de wbgetentities y copia el id encontrado
Error interpretarEntidad(const JsonDocument &respuesta, char *idEntidad,
                         size_t capacidad);

/// @brief convierte "+1991-12-26T00:00:00Z" según la precisión de wikidata
bool formatearFecha(const char *tiempo, int precision, char *salida,
                    size_t capacidad);

} // namespace siguesahi

#endif
