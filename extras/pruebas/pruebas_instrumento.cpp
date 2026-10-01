// pruebas_instrumento.cpp
// pruebas del núcleo completo con una red y un reloj simulados

#include "pruebas.h"

#include <deque>
#include <string>

#include "../../src/siguesahi/Instrumento.h"
#include "../../src/siguesahi/SalidaParpadeo.h"
#include "../../src/siguesahi/SalidaTexto.h"

using namespace siguesahi;

// --- plataforma simulada ---

static uint32_t reloj = 0;

static const char *const RESPUESTA_EXISTE =
    "HTTP/1.1 200 OK\r\ncontent-type: application/json\r\n"
    "content-length: 13\r\n\r\n"
    R"({"claims":{}})";

static const char *const RESPUESTA_NO_EXISTE =
    "HTTP/1.1 200 OK\r\ncontent-type: application/json\r\n\r\n"
    R"({"claims":{"P576":[{"mainsnak":{"snaktype":"value","datavalue":)"
    R"({"value":{"time":"+1991-12-26T00:00:00Z","precision":11}}},)"
    R"("rank":"normal"}]}})";

static const char *const RESPUESTA_ENTIDAD =
    "HTTP/1.1 200 OK\r\n\r\n"
    R"({"entities":{"Q15180":{"type":"item","id":"Q15180"}},"success":1})";

static const char *const RESPUESTA_429 =
    "HTTP/1.1 429 Too Many Requests\r\n\r\nlento";

static const char *const RESPUESTA_CORTADA = "HTTP/1.1 200 OK\r\n\r\n"
                                             R"({"claims":{"P576":[{"mainsn)";

// "FALLA" simula que no se pudo abrir la conexión
static const char *const FALLA = "FALLA";

class ConexionFalsa : public Conexion {
public:
  std::string respuesta;
  std::string enviado;
  size_t posicion = 0;
  bool cerrada = true;

  bool escribir(const uint8_t *datos, size_t largo) override {
    enviado.append((const char *)datos, largo);
    return true;
  }
  int leer() override {
    if (posicion >= respuesta.size()) {
      return -1;
    }
    return (uint8_t)respuesta[posicion++];
  }
  void cerrar() override { cerrada = true; }
};

class RedFalsa : public Red {
public:
  bool modulo = true;
  bool enlace = false;
  bool hora = true;
  int intentosWifi = 0;
  int desconexiones = 0;
  std::deque<std::string> respuestas;
  std::string peticiones;
  ConexionFalsa conexion;

  bool hayModulo() override { return modulo; }
  void iniciarConexion(const char *, const char *, uint32_t) override {
    intentosWifi++;
  }
  bool conectada() override { return enlace; }
  void desconectar() override {
    desconexiones++;
    enlace = false;
  }
  void iniciarHora() override {}
  bool horaLista() override { return hora; }
  bool configurarCertificados(const char *) override { return true; }
  void permitirInsegura(bool) override {}
  Conexion *abrir(const char *, uint16_t, uint32_t) override {
    if (respuestas.empty() || respuestas.front() == FALLA) {
      if (!respuestas.empty()) {
        respuestas.pop_front();
      }
      return nullptr;
    }
    peticiones += conexion.enviado;
    conexion.enviado.clear();
    conexion.respuesta = respuestas.front();
    respuestas.pop_front();
    conexion.posicion = 0;
    conexion.cerrada = false;
    return &conexion;
  }
};

namespace siguesahi {
uint32_t milisegundos() { return reloj; }
Red &redDeLaPlataforma() {
  static RedFalsa red;
  return red;
}
} // namespace siguesahi

class DestinoFalso : public Destino {
public:
  std::string texto;
  void escribir(const char *parte) override { texto += parte; }
};

class LedFalso : public SalidaParpadeo {
public:
  bool encendido = false;

protected:
  void encender(bool valor) override { encendido = valor; }
};

// avanza el reloj y llama a actualizar() varias veces
static void avanzar(Instrumento &instrumento, uint32_t milisegundos,
                    int veces = 1) {
  for (int i = 0; i < veces; i++) {
    reloj += milisegundos;
    instrumento.actualizar();
  }
}

// deja el instrumento conectado y con la primera consulta hecha
static void conectar(Instrumento &instrumento, RedFalsa &red) {
  instrumento.configurarRed("mi red", "mi clave");
  instrumento.comenzar();
  avanzar(instrumento, 10); // inicia la conexión wifi
  red.enlace = true;
  avanzar(instrumento, 10); // conectado, sincronizando hora
  avanzar(instrumento, 10); // hora lista, esperando
  avanzar(instrumento, 10); // primera consulta
}

static bool contiene(const std::string &texto, const char *parte) {
  return texto.find(parte) != std::string::npos;
}

// --- pruebas ---

static void probarSinConfigurar() {
  RedFalsa red;
  Instrumento instrumento(red);
  VERIFICAR(!instrumento.comenzar());
  VERIFICAR(instrumento.informe().error == Error::SIN_CONFIGURAR);
  VERIFICAR(instrumento.informe().fase == Fase::DETENIDO);

  RedFalsa sinModulo;
  sinModulo.modulo = false;
  Instrumento otro(sinModulo);
  otro.configurarRed("red", "clave");
  otro.configurarEntidad("Q1");
  VERIFICAR(!otro.comenzar());
  VERIFICAR(otro.informe().error == Error::SIN_MODULO_WIFI);

  // configuración inválida
  VERIFICAR(!instrumento.configurarEntidad("X1"));
  VERIFICAR(!instrumento.configurarRed("", "clave"));
  VERIFICAR(!instrumento.configurarAgenteUsuario("malo\r\nX-Inyectado: 1"));
  VERIFICAR(!instrumento.configurarArticulo("es", ""));
}

static void probarFlujoNormal() {
  RedFalsa red;
  Instrumento instrumento(red);
  DestinoFalso texto;
  SalidaTexto salidaTexto(texto);
  instrumento.agregarSalida(salidaTexto);
  instrumento.configurarEntidad("Q15180");
  instrumento.configurarIntervalo(3600);
  red.respuestas.push_back(RESPUESTA_EXISTE);

  conectar(instrumento, red);
  VERIFICAR(red.intentosWifi == 1);
  VERIFICAR(instrumento.existe());
  VERIFICAR(instrumento.informe().fase == Fase::ESPERANDO);
  VERIFICAR(instrumento.informe().consultasExitosas == 1);
  VERIFICAR(red.conexion.cerrada);

  // la petición HTTP enviada
  const std::string &enviado = red.conexion.enviado;
  VERIFICAR(contiene(enviado, "GET /w/api.php?action=wbgetclaims&format=json"
                              "&entity=Q15180&property=P576 HTTP/1.0\r\n"));
  VERIFICAR(contiene(enviado, "Host: www.wikidata.org\r\n"));
  VERIFICAR(contiene(enviado, "User-Agent: SiguesAhi/" SIGUES_AHI_VERSION));
  VERIFICAR(contiene(enviado, "\r\n\r\n"));

  // no consulta antes del intervalo
  red.respuestas.push_back(RESPUESTA_EXISTE);
  avanzar(instrumento, 60000, 59);
  VERIFICAR(instrumento.informe().consultasExitosas == 1);
  avanzar(instrumento, 60000);
  VERIFICAR(instrumento.informe().consultasExitosas == 2);

  // consultarAhora() adelanta la consulta
  red.respuestas.push_back(RESPUESTA_EXISTE);
  instrumento.consultarAhora();
  avanzar(instrumento, 10);
  VERIFICAR(instrumento.informe().consultasExitosas == 3);

  VERIFICAR(contiene(texto.texto, "[SiguesAhi] conectando a wifi\n"));
  VERIFICAR(contiene(texto.texto, "[SiguesAhi] Q15180: existe\n"));
}

static void probarVandalismo() {
  RedFalsa red;
  Instrumento instrumento(red);
  DestinoFalso texto;
  SalidaTexto salidaTexto(texto);
  LedFalso led;
  instrumento.agregarSalida(salidaTexto);
  instrumento.agregarSalida(led);
  instrumento.configurarEntidad("Q15180");
  instrumento.configurarConfirmaciones(3);
  red.respuestas.push_back(RESPUESTA_EXISTE);
  conectar(instrumento, red);
  VERIFICAR(instrumento.existe());

  // dos observaciones de disolución y luego se revierte
  red.respuestas.push_back(RESPUESTA_NO_EXISTE);
  red.respuestas.push_back(RESPUESTA_NO_EXISTE);
  red.respuestas.push_back(RESPUESTA_EXISTE);
  for (int i = 0; i < 2; i++) {
    instrumento.consultarAhora();
    avanzar(instrumento, 10);
  }
  VERIFICAR(instrumento.existe());
  VERIFICAR(instrumento.informe().observacionesPendientes == 2);
  VERIFICAR_TEXTO(instrumento.fechaDisolucion(), "");
  instrumento.consultarAhora();
  avanzar(instrumento, 10);
  VERIFICAR(instrumento.existe());
  VERIFICAR(instrumento.informe().observacionesPendientes == 0);

  // tres seguidas sí cambian el estado
  for (int i = 0; i < 3; i++) {
    red.respuestas.push_back(RESPUESTA_NO_EXISTE);
    instrumento.consultarAhora();
    avanzar(instrumento, 10);
  }
  VERIFICAR(instrumento.yaNoExiste());
  VERIFICAR_TEXTO(instrumento.fechaDisolucion(), "1991-12-26");
  VERIFICAR(contiene(
      texto.texto, "[SiguesAhi] Q15180: ya no existe, disuelta en 1991-12-26"));

  // el LED queda encendido fijo
  VERIFICAR(led.encendido);
  avanzar(instrumento, 700, 10);
  VERIFICAR(led.encendido);
}

static void probarFallasYReintentos() {
  RedFalsa red;
  Instrumento instrumento(red);
  instrumento.configurarEntidad("Q15180");
  instrumento.configurarIntervalo(3600);
  red.respuestas.push_back(FALLA);
  conectar(instrumento, red);
  VERIFICAR(instrumento.informe().error == Error::CONEXION);
  VERIFICAR(instrumento.estado() == Estado::DESCONOCIDO);
  VERIFICAR(instrumento.informe().consultasFallidas == 1);

  // primer reintento a los 30 segundos, no antes
  red.respuestas.push_back(FALLA);
  avanzar(instrumento, 29000);
  VERIFICAR(instrumento.informe().consultasFallidas == 1);
  avanzar(instrumento, 1000);
  VERIFICAR(instrumento.informe().consultasFallidas == 2);

  // segundo reintento a los 60 segundos; a la tercera falla de conexión
  // seguida se reinicia el wifi
  red.respuestas.push_back(FALLA);
  avanzar(instrumento, 59000);
  VERIFICAR(instrumento.informe().consultasFallidas == 2);
  avanzar(instrumento, 1000);
  VERIFICAR(instrumento.informe().consultasFallidas == 3);
  VERIFICAR(red.desconexiones == 1);
  VERIFICAR(instrumento.informe().fase == Fase::CONECTANDO_WIFI);

  // al reconectar, la siguiente consulta funciona y borra el error
  red.enlace = true;
  red.respuestas.push_back(RESPUESTA_EXISTE);
  avanzar(instrumento, 10, 3);
  avanzar(instrumento, 120000);
  VERIFICAR(instrumento.existe());
  VERIFICAR(instrumento.informe().error == Error::NINGUNO);
}

static void probarErroresDeRespuesta() {
  RedFalsa red;
  Instrumento instrumento(red);
  instrumento.configurarEntidad("Q15180");
  red.respuestas.push_back(RESPUESTA_429);
  conectar(instrumento, red);
  VERIFICAR(instrumento.informe().error == Error::HTTP);

  red.respuestas.push_back(RESPUESTA_CORTADA);
  instrumento.consultarAhora();
  avanzar(instrumento, 10);
  VERIFICAR(instrumento.informe().error == Error::RESPUESTA);
  VERIFICAR(instrumento.estado() == Estado::DESCONOCIDO);
}

static void probarArticulo() {
  RedFalsa red;
  Instrumento instrumento(red);
  VERIFICAR(instrumento.configurarArticulo("es", "Unión_Soviética"));
  red.respuestas.push_back(RESPUESTA_ENTIDAD);
  red.respuestas.push_back(RESPUESTA_EXISTE);
  conectar(instrumento, red);
  VERIFICAR_TEXTO(instrumento.informe().idEntidad, "Q15180");
  VERIFICAR(instrumento.existe());
  VERIFICAR(contiene(red.peticiones,
                     "action=wbgetentities&format=json&props=info"
                     "&redirects=yes&sites=eswiki"
                     "&titles=Uni%C3%B3n_Sovi%C3%A9tica HTTP/1.0"));
  VERIFICAR(contiene(red.conexion.enviado, "entity=Q15180"));
}

static void probarWifiPerdido() {
  RedFalsa red;
  Instrumento instrumento(red);
  instrumento.configurarEntidad("Q15180");
  red.respuestas.push_back(RESPUESTA_EXISTE);
  conectar(instrumento, red);
  VERIFICAR(red.intentosWifi == 1);

  red.enlace = false;
  avanzar(instrumento, 10);
  VERIFICAR(instrumento.informe().fase == Fase::CONECTANDO_WIFI);
  avanzar(instrumento, 10);
  VERIFICAR(red.intentosWifi == 2);
  // sin respuesta en 20 segundos, reintenta
  avanzar(instrumento, 20000);
  VERIFICAR(red.intentosWifi == 3);
  // el estado conocido se mantiene mientras no hay red
  VERIFICAR(instrumento.existe());
}

static void probarHora() {
  RedFalsa red;
  red.hora = false;
  Instrumento instrumento(red);
  instrumento.configurarEntidad("Q15180");
  instrumento.configurarRed("red", "clave");
  instrumento.comenzar();
  avanzar(instrumento, 10);
  red.enlace = true;
  avanzar(instrumento, 10);
  VERIFICAR(instrumento.informe().fase == Fase::SINCRONIZANDO_HORA);
  // sin hora en 30 segundos: error y reconexión
  avanzar(instrumento, 30000);
  VERIFICAR(instrumento.informe().error == Error::HORA);
  VERIFICAR(red.desconexiones == 1);
  VERIFICAR(instrumento.informe().fase == Fase::CONECTANDO_WIFI);
}

void probarInstrumento() {
  probarSinConfigurar();
  probarFlujoNormal();
  probarVandalismo();
  probarFallasYReintentos();
  probarErroresDeRespuesta();
  probarArticulo();
  probarWifiPerdido();
  probarHora();
}
