// Wikidata.cpp

#include "Wikidata.h"

#include <stdio.h>
#include <string.h>

namespace siguesahi {

const char *const SERVIDOR_WIKIDATA = "www.wikidata.org";
const char *const PROPIEDAD_DISOLUCION = "P576";

// precisiones de tiempo en wikidata
static const int PRECISION_ANIO = 9;
static const int PRECISION_MES = 10;

// rangos de las declaraciones, de peor a mejor
static const int RANGO_INVALIDO = -1;
static const int RANGO_OBSOLETO = 0;
static const int RANGO_NORMAL = 1;
static const int RANGO_PREFERIDO = 2;

bool copiarTexto(char *destino, size_t capacidad, const char *origen) {
  if (destino == nullptr || capacidad == 0) {
    return false;
  }
  if (origen == nullptr) {
    destino[0] = '\0';
    return true;
  }
  size_t largo = strlen(origen);
  if (largo >= capacidad) {
    destino[0] = '\0';
    return false;
  }
  memcpy(destino, origen, largo + 1);
  return true;
}

bool normalizarIdEntidad(const char *entrada, char *salida, size_t capacidad) {
  if (entrada == nullptr || (entrada[0] != 'Q' && entrada[0] != 'q')) {
    return false;
  }
  size_t digitos = strlen(entrada + 1);
  // los ids de wikidata no empiezan con cero y hoy tienen hasta 9 dígitos
  if (digitos < 1 || digitos > 12 || entrada[1] == '0') {
    return false;
  }
  for (size_t i = 1; i <= digitos; i++) {
    if (entrada[i] < '0' || entrada[i] > '9') {
      return false;
    }
  }
  if (digitos + 2 > capacidad) {
    return false;
  }
  salida[0] = 'Q';
  memcpy(salida + 1, entrada + 1, digitos + 1);
  return true;
}

bool construirSitio(const char *idioma, char *salida, size_t capacidad) {
  if (idioma == nullptr) {
    return false;
  }
  size_t largo = strlen(idioma);
  if (largo < 2 || largo > 12 || largo + 5 > capacidad) {
    return false;
  }
  for (size_t i = 0; i < largo; i++) {
    char c = idioma[i];
    if (c >= 'A' && c <= 'Z') {
      c = c - 'A' + 'a';
    } else if (c == '-') {
      // "zh-yue" es "zh_yuewiki"
      c = '_';
    } else if (c < 'a' || c > 'z') {
      return false;
    }
    salida[i] = c;
  }
  memcpy(salida + largo, "wiki", 5);
  return true;
}

bool codificarUrl(const char *entrada, char *salida, size_t capacidad) {
  static const char DIGITOS_HEX[] = "0123456789ABCDEF";
  if (entrada == nullptr || salida == nullptr || capacidad == 0) {
    return false;
  }
  size_t j = 0;
  for (size_t i = 0; entrada[i] != '\0'; i++) {
    unsigned char c = (unsigned char)entrada[i];
    bool reservado = !((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
                       (c >= '0' && c <= '9') || c == '-' || c == '_' ||
                       c == '.' || c == '~');
    size_t necesario = reservado ? 3 : 1;
    if (j + necesario >= capacidad) {
      salida[0] = '\0';
      return false;
    }
    if (reservado) {
      salida[j++] = '%';
      salida[j++] = DIGITOS_HEX[c >> 4];
      salida[j++] = DIGITOS_HEX[c & 0x0F];
    } else {
      salida[j++] = (char)c;
    }
  }
  salida[j] = '\0';
  return true;
}

bool construirRutaDeclaraciones(const char *idEntidad, char *salida,
                                size_t capacidad) {
  int n = snprintf(salida, capacidad,
                   "/w/api.php?action=wbgetclaims&format=json"
                   "&entity=%s&property=%s",
                   idEntidad, PROPIEDAD_DISOLUCION);
  return n > 0 && (size_t)n < capacidad;
}

bool construirRutaEntidad(const char *sitio, const char *titulo, char *salida,
                          size_t capacidad) {
  int n = snprintf(salida, capacidad,
                   "/w/api.php?action=wbgetentities&format=json"
                   "&props=info&redirects=yes&sites=%s&titles=",
                   sitio);
  if (n <= 0 || (size_t)n >= capacidad) {
    return false;
  }
  return codificarUrl(titulo, salida + n, capacidad - n);
}

int codigoEstadoHttp(const char *lineaEstado) {
  if (lineaEstado == nullptr || strncmp(lineaEstado, "HTTP/", 5) != 0) {
    return -1;
  }
  const char *espacio = strchr(lineaEstado, ' ');
  if (espacio == nullptr) {
    return -1;
  }
  int codigo = 0;
  for (int i = 1; i <= 3; i++) {
    char c = espacio[i];
    if (c < '0' || c > '9') {
      return -1;
    }
    codigo = codigo * 10 + (c - '0');
  }
  return codigo;
}

void crearFiltroDeclaraciones(JsonDocument &filtro) {
  filtro.clear();
  JsonObject declaracion =
      filtro["claims"][PROPIEDAD_DISOLUCION].add<JsonObject>();
  declaracion["rank"] = true;
  declaracion["mainsnak"]["snaktype"] = true;
  declaracion["mainsnak"]["datavalue"]["value"]["time"] = true;
  declaracion["mainsnak"]["datavalue"]["value"]["precision"] = true;
  filtro["error"]["code"] = true;
}

void crearFiltroEntidad(JsonDocument &filtro) {
  filtro.clear();
  filtro["entities"]["*"]["id"] = true;
  filtro["entities"]["*"]["missing"] = true;
  filtro["error"]["code"] = true;
}

static int valorRango(const char *rango) {
  if (rango == nullptr) {
    return RANGO_INVALIDO;
  }
  if (strcmp(rango, "preferred") == 0) {
    return RANGO_PREFERIDO;
  }
  if (strcmp(rango, "normal") == 0) {
    return RANGO_NORMAL;
  }
  if (strcmp(rango, "deprecated") == 0) {
    return RANGO_OBSOLETO;
  }
  return RANGO_INVALIDO;
}

Observacion interpretarDeclaraciones(const JsonDocument &respuesta) {
  Observacion observacion;

  if (!respuesta["error"].isNull()) {
    observacion.error = Error::ENTIDAD_INVALIDA;
    return observacion;
  }
  JsonObjectConst declaraciones = respuesta["claims"];
  if (declaraciones.isNull()) {
    observacion.error = Error::RESPUESTA;
    return observacion;
  }

  // como en las "declaraciones verdaderas" de wikidata, solo cuentan las del
  // mejor rango disponible, y las obsoletas nunca cuentan
  JsonArrayConst lista = declaraciones[PROPIEDAD_DISOLUCION];
  int mejorRango = RANGO_NORMAL;
  for (JsonObjectConst declaracion : lista) {
    int rango = valorRango(declaracion["rank"]);
    if (rango > mejorRango) {
      mejorRango = rango;
    }
  }

  observacion.estado = Estado::EXISTE;
  for (JsonObjectConst declaracion : lista) {
    if (valorRango(declaracion["rank"]) != mejorRango) {
      continue;
    }
    const char *tipo = declaracion["mainsnak"]["snaktype"];
    // "novalue" afirma explícitamente que no hay fecha de disolución
    if (tipo == nullptr || strcmp(tipo, "novalue") == 0) {
      continue;
    }
    observacion.estado = Estado::NO_EXISTE;
    // "somevalue" significa disuelta en fecha desconocida
    if (strcmp(tipo, "value") == 0 && observacion.fechaDisolucion[0] == '\0') {
      JsonObjectConst valor = declaracion["mainsnak"]["datavalue"]["value"];
      formatearFecha(valor["time"], valor["precision"] | 11,
                     observacion.fechaDisolucion,
                     sizeof(observacion.fechaDisolucion));
    }
  }
  return observacion;
}

Error interpretarEntidad(const JsonDocument &respuesta, char *idEntidad,
                         size_t capacidad) {
  if (!respuesta["error"].isNull()) {
    return Error::ARTICULO_NO_ENCONTRADO;
  }
  JsonObjectConst entidades = respuesta["entities"];
  if (entidades.isNull()) {
    return Error::RESPUESTA;
  }
  for (JsonPairConst par : entidades) {
    JsonObjectConst entidad = par.value();
    if (entidad["missing"].is<const char *>()) {
      return Error::ARTICULO_NO_ENCONTRADO;
    }
    if (normalizarIdEntidad(entidad["id"], idEntidad, capacidad)) {
      return Error::NINGUNO;
    }
  }
  return Error::ARTICULO_NO_ENCONTRADO;
}

bool formatearFecha(const char *tiempo, int precision, char *salida,
                    size_t capacidad) {
  if (salida == nullptr || capacidad == 0) {
    return false;
  }
  salida[0] = '\0';
  if (tiempo == nullptr) {
    return false;
  }
  // el formato es "+AAAA-MM-DDThh:mm:ssZ", el año puede tener más dígitos
  // y el signo "-" indica antes de cristo
  bool negativo = tiempo[0] == '-';
  if (tiempo[0] == '+' || tiempo[0] == '-') {
    tiempo++;
  }
  const char *finAnio = strchr(tiempo, '-');
  if (finAnio == nullptr || finAnio == tiempo || strlen(finAnio) < 6) {
    return false;
  }
  // quitar ceros a la izquierda del año, dejando al menos cuatro dígitos
  while (finAnio - tiempo > 4 && tiempo[0] == '0') {
    tiempo++;
  }
  size_t largo = finAnio - tiempo;
  if (precision >= PRECISION_MES) {
    largo += 3;
  }
  if (precision > PRECISION_MES) {
    largo += 3;
  }
  if (precision < PRECISION_ANIO) {
    // décadas, siglos, etc: basta con el año
    largo = finAnio - tiempo;
  }
  if (largo + (negativo ? 1 : 0) >= capacidad) {
    return false;
  }
  size_t j = 0;
  if (negativo) {
    salida[j++] = '-';
  }
  memcpy(salida + j, tiempo, largo);
  salida[j + largo] = '\0';
  return true;
}

} // namespace siguesahi
