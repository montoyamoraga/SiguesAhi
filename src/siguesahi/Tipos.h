// Tipos.h
// tipos públicos de SiguesAhi: estado, fase, error e informe

#ifndef SIGUES_AHI_TIPOS_H
#define SIGUES_AHI_TIPOS_H

#include <stdint.h>

namespace siguesahi {

/// @brief lo que sabemos de la institución
enum class Estado : uint8_t {
  DESCONOCIDO = 0, // todavía no hay una consulta exitosa
  EXISTE,          // wikidata no registra fecha de disolución (P576)
  NO_EXISTE        // wikidata registra fecha de disolución (P576)
};

/// @brief qué está haciendo el instrumento en este momento
enum class Fase : uint8_t {
  DETENIDO = 0,       // antes de comenzar() o sin configuración válida
  CONECTANDO_WIFI,    // intentando conectarse a la red
  SINCRONIZANDO_HORA, // esperando la hora por NTP, necesaria para TLS
  ESPERANDO,          // conectado, esperando la próxima consulta
  CONSULTANDO         // hablando con wikidata
};

/// @brief motivo de la última falla, NINGUNO si todo funcionó
enum class Error : uint8_t {
  NINGUNO = 0,
  SIN_CONFIGURAR,         // falta red o entidad/artículo
  SIN_MODULO_WIFI,        // el hardware de wifi no responde
  WIFI,                   // no se pudo conectar a la red
  HORA,                   // no se pudo obtener la hora por NTP
  CONEXION,               // no se pudo abrir la conexión TLS con wikidata
  HTTP,                   // wikidata respondió con un código distinto de 200
  RESPUESTA,              // respuesta incompleta o JSON inválido
  ARTICULO_NO_ENCONTRADO, // el artículo no existe o no tiene entidad
  ENTIDAD_INVALIDA        // wikidata no reconoce el identificador
};

/// @brief resumen del estado del instrumento, que reciben las salidas
struct Informe {
  Fase fase = Fase::DETENIDO;
  Estado estado = Estado::DESCONOCIDO;
  Error error = Error::NINGUNO;
  // identificador de wikidata, por ejemplo "Q15180"
  char idEntidad[16] = {0};
  // fecha de disolución, "AAAA", "AAAA-MM" o "AAAA-MM-DD", vacía si no hay
  char fechaDisolucion[16] = {0};
  // observaciones seguidas que apuntan a un cambio de estado aún sin confirmar
  uint8_t observacionesPendientes = 0;
  uint32_t consultasExitosas = 0;
  uint32_t consultasFallidas = 0;
  // true si el watchdog reinició la placa antes de este arranque
  bool reinicioPorVigilante = false;
};

const char *nombreEstado(Estado estado);
const char *nombreFase(Fase fase);
const char *nombreError(Error error);

} // namespace siguesahi

#endif
