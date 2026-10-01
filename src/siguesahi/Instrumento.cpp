// Instrumento.cpp

#include "Instrumento.h"

#include <string.h>

#include "Http.h"
#include "Wikidata.h"

namespace siguesahi {

// tiempo máximo para conectarse a wifi antes de reintentar
static const uint32_t LIMITE_WIFI_MS = 20000;
// tiempo máximo para obtener la hora por NTP
static const uint32_t LIMITE_HORA_MS = 30000;
// tiempo máximo de cada espera de datos de wikidata
static const uint32_t LIMITE_RESPUESTA_MS = 15000;
// tiempo máximo de una petición completa a wikidata, antes de que el
// watchdog reinicie la placa: dns, tcp, tls y la respuesta
static const uint32_t LIMITE_PETICION_MS = 6 * LIMITE_RESPUESTA_MS;
// tiempo máximo de otras operaciones de red que bloquean
static const uint32_t LIMITE_OPERACION_MS = 20000;
// primera espera tras una falla, se duplica en cada falla seguida
static const uint32_t REINTENTO_INICIAL_MS = 30000;
// fallas de conexión seguidas antes de reiniciar el wifi
static const uint8_t FALLAS_PARA_RECONECTAR = 3;
// intervalo máximo, para no desbordar los milisegundos
static const uint32_t INTERVALO_MAXIMO_S = 7UL * 24UL * 3600UL;

static const uint16_t PUERTO_HTTPS = 443;
static const size_t LARGO_RUTA = 512;
static const size_t LARGO_PETICION = 800;

Instrumento::Instrumento() : Instrumento(redDeLaPlataforma()) {}

Instrumento::Instrumento(Red &red) : _red(red) {
  copiarTexto(_agente, sizeof(_agente),
              "SiguesAhi/" SIGUES_AHI_VERSION
              " (https://github.com/montoyamoraga/SiguesAhi)");
}

// --- configuración ---

bool Instrumento::configurarRed(const char *nombre, const char *clave) {
  if (nombre == nullptr || nombre[0] == '\0') {
    return false;
  }
  return copiarTexto(_nombreRed, sizeof(_nombreRed), nombre) &&
         copiarTexto(_claveRed, sizeof(_claveRed), clave);
}

bool Instrumento::configurarEntidad(const char *idEntidad) {
  if (!normalizarIdEntidad(idEntidad, _informe.idEntidad,
                           sizeof(_informe.idEntidad))) {
    return false;
  }
  _titulo[0] = '\0';
  return true;
}

bool Instrumento::configurarArticulo(const char *idioma, const char *titulo) {
  if (titulo == nullptr || titulo[0] == '\0' ||
      !construirSitio(idioma, _sitio, sizeof(_sitio)) ||
      !copiarTexto(_titulo, sizeof(_titulo), titulo)) {
    return false;
  }
  // la entidad se busca en la primera consulta
  _informe.idEntidad[0] = '\0';
  return true;
}

void Instrumento::configurarIntervalo(uint32_t segundos) {
  if (segundos < INTERVALO_MINIMO_S) {
    segundos = INTERVALO_MINIMO_S;
  }
  if (segundos > INTERVALO_MAXIMO_S) {
    segundos = INTERVALO_MAXIMO_S;
  }
  _intervaloMs = segundos * 1000UL;
}

void Instrumento::configurarConfirmaciones(uint8_t cantidad) {
  _confirmador.configurar(cantidad);
}

bool Instrumento::configurarAgenteUsuario(const char *agente) {
  if (agente == nullptr || agente[0] == '\0' || strchr(agente, '\r') ||
      strchr(agente, '\n')) {
    return false;
  }
  return copiarTexto(_agente, sizeof(_agente), agente);
}

bool Instrumento::configurarCertificados(const char *pem) {
  return _red.configurarCertificados(pem);
}

void Instrumento::permitirConexionInsegura(bool permitir) {
  _red.permitirInsegura(permitir);
}

void Instrumento::activarVigilante() { _usarVigilante = true; }

void Instrumento::configurarRegistro(Destino &destino) { _registro = &destino; }

bool Instrumento::agregarSalida(Salida &salida) {
  if (_cantidadSalidas >= MAXIMO_SALIDAS) {
    return false;
  }
  _salidas[_cantidadSalidas++] = &salida;
  return true;
}

// --- funcionamiento ---

bool Instrumento::comenzar() {
  // primero el hardware de red: en la pico w el LED depende del chip wifi
  bool hayModulo = _red.hayModulo();

  for (uint8_t i = 0; i < _cantidadSalidas; i++) {
    _salidas[i]->comenzar();
  }

  if (_nombreRed[0] == '\0' ||
      (_informe.idEntidad[0] == '\0' && _titulo[0] == '\0')) {
    _informe.error = Error::SIN_CONFIGURAR;
  } else if (!hayModulo) {
    _informe.error = Error::SIN_MODULO_WIFI;
  }
  if (_informe.error != Error::NINGUNO) {
    _informe.fase = Fase::DETENIDO;
    registrar("error", nombreError(_informe.error));
    notificar();
    return false;
  }

  if (_usarVigilante) {
    _informe.reinicioPorVigilante = Vigilante::ultimoReinicioFueVigilante();
    if (_informe.reinicioPorVigilante) {
      registrar("la placa se reinició por el watchdog", nullptr);
    }
    if (!_vigilante.comenzar()) {
      registrar("esta placa no tiene watchdog", nullptr);
    }
  }

  // la primera consulta se hace apenas haya conexión
  esperar(0);
  conectarWifi();
  return true;
}

void Instrumento::actualizar() {
  uint32_t ahora = milisegundos();
  for (uint8_t i = 0; i < _cantidadSalidas; i++) {
    _salidas[i]->actualizar(ahora);
  }
  _vigilante.alimentar();

  switch (_informe.fase) {
  case Fase::DETENIDO:
  case Fase::CONSULTANDO:
    break;

  case Fase::CONECTANDO_WIFI:
    if (_red.conectada()) {
      registrar("conectado a", _nombreRed);
      {
        // en la pico w busca los servidores NTP por dns
        BloqueoVigilado bloqueo(_vigilante, LIMITE_OPERACION_MS);
        _red.iniciarHora();
      }
      _inicioFase = milisegundos();
      cambiarFase(Fase::SINCRONIZANDO_HORA);
    } else if (ahora - _inicioFase >= LIMITE_WIFI_MS) {
      registrar("conectando a", _nombreRed);
      // en WiFiNINA begin() bloquea hasta LIMITE_WIFI_MS
      BloqueoVigilado bloqueo(_vigilante, LIMITE_WIFI_MS + LIMITE_OPERACION_MS);
      _red.iniciarConexion(_nombreRed, _claveRed, LIMITE_WIFI_MS);
      _inicioFase = milisegundos();
    }
    break;

  case Fase::SINCRONIZANDO_HORA:
    if (!_red.conectada()) {
      conectarWifi();
    } else if (_red.horaLista()) {
      cambiarFase(Fase::ESPERANDO);
    } else if (ahora - _inicioFase >= LIMITE_HORA_MS) {
      _informe.error = Error::HORA;
      registrar("error", nombreError(_informe.error));
      // reconectar también reinicia la sincronización de hora
      BloqueoVigilado bloqueo(_vigilante, LIMITE_OPERACION_MS);
      _red.desconectar();
      conectarWifi();
    }
    break;

  case Fase::ESPERANDO:
    if (!_red.conectada()) {
      registrar("se perdió la conexión con", _nombreRed);
      conectarWifi();
    } else if (ahora - _inicioEspera >= _duracionEspera) {
      consultar();
    }
    break;
  }
}

void Instrumento::consultarAhora() { esperar(0); }

// --- privados ---

void Instrumento::cambiarFase(Fase fase) {
  if (_informe.fase != fase) {
    _informe.fase = fase;
    notificar();
  }
}

void Instrumento::conectarWifi() {
  // forzar un intento inmediato en la próxima actualización
  _inicioFase = milisegundos() - LIMITE_WIFI_MS;
  cambiarFase(Fase::CONECTANDO_WIFI);
}

void Instrumento::esperar(uint32_t duracionMs) {
  _inicioEspera = milisegundos();
  _duracionEspera = duracionMs;
}

void Instrumento::consultar() {
  cambiarFase(Fase::CONSULTANDO);

  Error error = Error::NINGUNO;
  if (_informe.idEntidad[0] == '\0') {
    error = resolverEntidad();
  }

  Observacion observacion;
  if (error == Error::NINGUNO) {
    char ruta[LARGO_RUTA];
    construirRutaDeclaraciones(_informe.idEntidad, ruta, sizeof(ruta));
    JsonDocument filtro;
    crearFiltroDeclaraciones(filtro);
    JsonDocument respuesta;
    error = pedir(ruta, respuesta, filtro);
    if (error == Error::NINGUNO) {
      observacion = interpretarDeclaraciones(respuesta);
      error = observacion.error;
    }
  }

  if (error != Error::NINGUNO) {
    registrarFalla(error);
    return;
  }

  registrar("observación", nombreEstado(observacion.estado));
  _fallasSeguidas = 0;
  _informe.error = Error::NINGUNO;
  _informe.consultasExitosas++;

  _confirmador.registrar(observacion.estado);
  _informe.estado = _confirmador.estado();
  _informe.observacionesPendientes = _confirmador.pendientes();
  // la fecha solo se muestra cuando corresponde al estado confirmado
  if (observacion.estado == _informe.estado) {
    copiarTexto(_informe.fechaDisolucion, sizeof(_informe.fechaDisolucion),
                observacion.fechaDisolucion);
  }

  esperar(_intervaloMs);
  _informe.fase = Fase::ESPERANDO;
  notificar();
}

Error Instrumento::resolverEntidad() {
  char ruta[LARGO_RUTA];
  if (!construirRutaEntidad(_sitio, _titulo, ruta, sizeof(ruta))) {
    return Error::SIN_CONFIGURAR;
  }
  JsonDocument filtro;
  crearFiltroEntidad(filtro);
  JsonDocument respuesta;
  Error error = pedir(ruta, respuesta, filtro);
  if (error == Error::NINGUNO) {
    error = interpretarEntidad(respuesta, _informe.idEntidad,
                               sizeof(_informe.idEntidad));
  }
  if (error == Error::NINGUNO) {
    registrar("entidad encontrada", _informe.idEntidad);
  }
  return error;
}

Error Instrumento::pedir(const char *ruta, JsonDocument &respuesta,
                         const JsonDocument &filtro) {
  // la petición completa se arma antes y se envía de una vez,
  // así viaja en un solo registro TLS
  char peticion[LARGO_PETICION];
  if (!construirPeticion(SERVIDOR_WIKIDATA, ruta, _agente, peticion,
                         sizeof(peticion))) {
    return Error::SIN_CONFIGURAR;
  }

  registrar("pidiendo", ruta);
  // desde aquí hasta el final de la función, el watchdog espera
  // como máximo LIMITE_PETICION_MS
  BloqueoVigilado bloqueo(_vigilante, LIMITE_PETICION_MS);
  Conexion *conexion =
      _red.abrir(SERVIDOR_WIKIDATA, PUERTO_HTTPS, LIMITE_RESPUESTA_MS);
  if (conexion == nullptr) {
    return Error::CONEXION;
  }

  char lineaEstado[48];
  Error error = pedirJson(*conexion, peticion, respuesta, filtro, lineaEstado,
                          sizeof(lineaEstado));
  conexion->cerrar();
  if (error == Error::HTTP) {
    registrar("respuesta http", lineaEstado);
  }
  return error;
}

void Instrumento::registrarFalla(Error error) {
  registrar("error", nombreError(error));
  _informe.error = error;
  _informe.consultasFallidas++;
  if (_fallasSeguidas < 255) {
    _fallasSeguidas++;
  }

  uint32_t espera;
  if (error == Error::ARTICULO_NO_ENCONTRADO ||
      error == Error::ENTIDAD_INVALIDA || error == Error::SIN_CONFIGURAR) {
    // errores de configuración: reintentar rápido no sirve de nada
    espera = _intervaloMs;
  } else {
    // espera exponencial: 30 s, 1 min, 2 min... hasta el intervalo
    uint8_t exponente = _fallasSeguidas - 1;
    if (exponente > 10) {
      exponente = 10;
    }
    espera = REINTENTO_INICIAL_MS << exponente;
    if (espera > _intervaloMs) {
      espera = _intervaloMs;
    }
  }
  esperar(espera);

  // si wikidata no responde varias veces seguidas, puede que el wifi
  // diga estar conectado sin estarlo: reiniciarlo
  if (error == Error::CONEXION &&
      _fallasSeguidas % FALLAS_PARA_RECONECTAR == 0) {
    registrar("reiniciando wifi", nullptr);
    BloqueoVigilado bloqueo(_vigilante, LIMITE_OPERACION_MS);
    _red.desconectar();
    _informe.fase = Fase::CONECTANDO_WIFI;
    _inicioFase = milisegundos() - LIMITE_WIFI_MS;
  } else {
    _informe.fase = Fase::ESPERANDO;
  }
  notificar();
}

void Instrumento::notificar() {
  for (uint8_t i = 0; i < _cantidadSalidas; i++) {
    _salidas[i]->notificar(_informe);
  }
}

void Instrumento::registrar(const char *mensaje, const char *detalle) {
  if (_registro == nullptr) {
    return;
  }
  _registro->escribir("[SiguesAhi] ");
  _registro->escribir(mensaje);
  if (detalle != nullptr) {
    _registro->escribir(": ");
    _registro->escribir(detalle);
  }
  _registro->escribir("\n");
}

} // namespace siguesahi
