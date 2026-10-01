// Tipos.cpp

#include "Tipos.h"

namespace siguesahi {

const char *nombreEstado(Estado estado) {
  switch (estado) {
  case Estado::DESCONOCIDO:
    return "desconocido";
  case Estado::EXISTE:
    return "existe";
  case Estado::NO_EXISTE:
    return "ya no existe";
  }
  return "?";
}

const char *nombreFase(Fase fase) {
  switch (fase) {
  case Fase::DETENIDO:
    return "detenido";
  case Fase::CONECTANDO_WIFI:
    return "conectando a wifi";
  case Fase::SINCRONIZANDO_HORA:
    return "sincronizando hora";
  case Fase::ESPERANDO:
    return "esperando";
  case Fase::CONSULTANDO:
    return "consultando wikidata";
  }
  return "?";
}

const char *nombreError(Error error) {
  switch (error) {
  case Error::NINGUNO:
    return "ninguno";
  case Error::SIN_CONFIGURAR:
    return "falta configurar red y entidad o artículo";
  case Error::SIN_MODULO_WIFI:
    return "no se encontró el módulo wifi";
  case Error::WIFI:
    return "no se pudo conectar a wifi";
  case Error::HORA:
    return "no se pudo obtener la hora";
  case Error::CONEXION:
    return "no se pudo conectar a wikidata";
  case Error::HTTP:
    return "wikidata respondió con un error http";
  case Error::RESPUESTA:
    return "respuesta inválida de wikidata";
  case Error::ARTICULO_NO_ENCONTRADO:
    return "artículo no encontrado en wikidata";
  case Error::ENTIDAD_INVALIDA:
    return "entidad inválida en wikidata";
  }
  return "?";
}

} // namespace siguesahi
