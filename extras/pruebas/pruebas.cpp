// pruebas.cpp
// pruebas de SiguesAhi, se compilan en el computador:
//   make -C extras/pruebas

#include "pruebas.h"

#include "../../src/siguesahi/Confirmador.h"
#include "../../src/siguesahi/Wikidata.h"

using namespace siguesahi;

int fallas = 0;
int pruebas = 0;

static Observacion declaraciones(const char *json) {
  JsonDocument filtro;
  crearFiltroDeclaraciones(filtro);
  JsonDocument respuesta;
  DeserializationError error =
      deserializeJson(respuesta, json, DeserializationOption::Filter(filtro));
  if (error) {
    Observacion observacion;
    observacion.error = Error::RESPUESTA;
    return observacion;
  }
  return interpretarDeclaraciones(respuesta);
}

static Error entidad(const char *json, char *id, size_t capacidad) {
  JsonDocument filtro;
  crearFiltroEntidad(filtro);
  JsonDocument respuesta;
  if (deserializeJson(respuesta, json, DeserializationOption::Filter(filtro))) {
    return Error::RESPUESTA;
  }
  return interpretarEntidad(respuesta, id, capacidad);
}

static void probarIdEntidad() {
  char id[16];
  VERIFICAR(normalizarIdEntidad("Q15180", id, sizeof(id)));
  VERIFICAR_TEXTO(id, "Q15180");
  VERIFICAR(normalizarIdEntidad("q42", id, sizeof(id)));
  VERIFICAR_TEXTO(id, "Q42");
  VERIFICAR(!normalizarIdEntidad("Q", id, sizeof(id)));
  VERIFICAR(!normalizarIdEntidad("Q0123", id, sizeof(id)));
  VERIFICAR(!normalizarIdEntidad("P576", id, sizeof(id)));
  VERIFICAR(!normalizarIdEntidad("Q12a", id, sizeof(id)));
  VERIFICAR(!normalizarIdEntidad("Q1234567890123", id, sizeof(id)));
  VERIFICAR(!normalizarIdEntidad(nullptr, id, sizeof(id)));
  VERIFICAR(!normalizarIdEntidad("Q12345", id, 4));
}

static void probarSitio() {
  char sitio[20];
  VERIFICAR(construirSitio("es", sitio, sizeof(sitio)));
  VERIFICAR_TEXTO(sitio, "eswiki");
  VERIFICAR(construirSitio("EN", sitio, sizeof(sitio)));
  VERIFICAR_TEXTO(sitio, "enwiki");
  VERIFICAR(construirSitio("zh-yue", sitio, sizeof(sitio)));
  VERIFICAR_TEXTO(sitio, "zh_yuewiki");
  VERIFICAR(!construirSitio("e", sitio, sizeof(sitio)));
  VERIFICAR(!construirSitio("es&x=1", sitio, sizeof(sitio)));
  VERIFICAR(!construirSitio("es", sitio, 6));
}

static void probarCodificarUrl() {
  char salida[64];
  VERIFICAR(
      codificarUrl("Uni\xC3\xB3n Sovi\xC3\xA9tica", salida, sizeof(salida)));
  VERIFICAR_TEXTO(salida, "Uni%C3%B3n%20Sovi%C3%A9tica");
  VERIFICAR(codificarUrl("A&B=C?#", salida, sizeof(salida)));
  VERIFICAR_TEXTO(salida, "A%26B%3DC%3F%23");
  VERIFICAR(codificarUrl("National_Rifle_Association", salida, sizeof(salida)));
  VERIFICAR_TEXTO(salida, "National_Rifle_Association");
  // justo cabe y justo no cabe
  VERIFICAR(codificarUrl("%", salida, 4));
  VERIFICAR(!codificarUrl("%", salida, 3));
}

static void probarRutas() {
  char ruta[256];
  VERIFICAR(construirRutaDeclaraciones("Q15180", ruta, sizeof(ruta)));
  VERIFICAR_TEXTO(ruta, "/w/api.php?action=wbgetclaims&format=json"
                        "&entity=Q15180&property=P576");
  VERIFICAR(construirRutaEntidad("eswiki", "Uni\xC3\xB3n_Sovi\xC3\xA9tica",
                                 ruta, sizeof(ruta)));
  VERIFICAR_TEXTO(ruta, "/w/api.php?action=wbgetentities&format=json"
                        "&props=info&redirects=yes&sites=eswiki"
                        "&titles=Uni%C3%B3n_Sovi%C3%A9tica");
  VERIFICAR(!construirRutaDeclaraciones("Q15180", ruta, 20));
  VERIFICAR(!construirRutaEntidad("eswiki", "x", ruta, 20));
}

static void probarCodigoHttp() {
  VERIFICAR(codigoEstadoHttp("HTTP/1.1 200 OK\r") == 200);
  VERIFICAR(codigoEstadoHttp("HTTP/1.0 429 Too Many Requests") == 429);
  VERIFICAR(codigoEstadoHttp("HTTP/2 503") == 503);
  VERIFICAR(codigoEstadoHttp("") == -1);
  VERIFICAR(codigoEstadoHttp("HTTP/1.1") == -1);
  VERIFICAR(codigoEstadoHttp("HTTP/1.1 2") == -1);
  VERIFICAR(codigoEstadoHttp("garbage 200") == -1);
}

static void probarFechas() {
  char fecha[16];
  VERIFICAR(formatearFecha("+1991-12-26T00:00:00Z", 11, fecha, sizeof(fecha)));
  VERIFICAR_TEXTO(fecha, "1991-12-26");
  VERIFICAR(formatearFecha("+1991-12-00T00:00:00Z", 10, fecha, sizeof(fecha)));
  VERIFICAR_TEXTO(fecha, "1991-12");
  VERIFICAR(formatearFecha("+1991-00-00T00:00:00Z", 9, fecha, sizeof(fecha)));
  VERIFICAR_TEXTO(fecha, "1991");
  VERIFICAR(formatearFecha("+1900-00-00T00:00:00Z", 7, fecha, sizeof(fecha)));
  VERIFICAR_TEXTO(fecha, "1900");
  VERIFICAR(formatearFecha("-0044-03-15T00:00:00Z", 11, fecha, sizeof(fecha)));
  VERIFICAR_TEXTO(fecha, "-0044-03-15");
  VERIFICAR(
      formatearFecha("+00000001806-01-01T00:00:00Z", 9, fecha, sizeof(fecha)));
  VERIFICAR_TEXTO(fecha, "1806");
  VERIFICAR(!formatearFecha(nullptr, 11, fecha, sizeof(fecha)));
  VERIFICAR(!formatearFecha("basura", 11, fecha, sizeof(fecha)));
  VERIFICAR(!formatearFecha("+1991-12-26T00:00:00Z", 11, fecha, 8));
}

static void probarDeclaraciones() {
  // respuesta real para la unión soviética (Q15180), con referencias
  Observacion o = declaraciones(
      R"({"claims":{"P576":[{"mainsnak":{"snaktype":"value","property":"P576",)"
      R"("hash":"6654","datavalue":{"value":{"time":"+1991-12-26T00:00:00Z",)"
      R"("timezone":0,"before":0,"after":0,"precision":11,"calendarmodel":)"
      R"("http://www.wikidata.org/entity/Q1985727"},"type":"time"},)"
      R"("datatype":"time"},"type":"statement","id":"q15180$e1e8",)"
      R"("rank":"normal","references":[{"hash":"4680","snaks":{"P92":[]}}]}]}})");
  VERIFICAR(o.error == Error::NINGUNO);
  VERIFICAR(o.estado == Estado::NO_EXISTE);
  VERIFICAR_TEXTO(o.fechaDisolucion, "1991-12-26");

  // respuesta real para la NRA (Q863259): sin P576
  o = declaraciones(R"({"claims":{}})");
  VERIFICAR(o.error == Error::NINGUNO);
  VERIFICAR(o.estado == Estado::EXISTE);
  VERIFICAR_TEXTO(o.fechaDisolucion, "");

  // una declaración obsoleta no cuenta
  o = declaraciones(R"({"claims":{"P576":[{"rank":"deprecated","mainsnak":)"
                    R"({"snaktype":"value","datavalue":{"value":{"time":)"
                    R"("+2020-01-01T00:00:00Z","precision":11}}}}]}})");
  VERIFICAR(o.estado == Estado::EXISTE);

  // "novalue" afirma que no hay disolución
  o = declaraciones(
      R"({"claims":{"P576":[{"rank":"normal","mainsnak":{"snaktype":"novalue"}}]}})");
  VERIFICAR(o.estado == Estado::EXISTE);

  // "somevalue": disuelta en fecha desconocida
  o = declaraciones(R"({"claims":{"P576":[{"rank":"normal","mainsnak":)"
                    R"({"snaktype":"somevalue"}}]}})");
  VERIFICAR(o.estado == Estado::NO_EXISTE);
  VERIFICAR_TEXTO(o.fechaDisolucion, "");

  // una preferida con "novalue" gana sobre una normal con fecha
  o = declaraciones(
      R"({"claims":{"P576":[)"
      R"({"rank":"normal","mainsnak":{"snaktype":"value","datavalue":)"
      R"({"value":{"time":"+2020-01-01T00:00:00Z","precision":11}}}},)"
      R"({"rank":"preferred","mainsnak":{"snaktype":"novalue"}}]}})");
  VERIFICAR(o.estado == Estado::EXISTE);

  // con varias fechas del mejor rango se usa la primera
  o = declaraciones(
      R"({"claims":{"P576":[)"
      R"({"rank":"normal","mainsnak":{"snaktype":"value","datavalue":)"
      R"({"value":{"time":"+1990-00-00T00:00:00Z","precision":9}}}},)"
      R"({"rank":"normal","mainsnak":{"snaktype":"value","datavalue":)"
      R"({"value":{"time":"+1991-12-26T00:00:00Z","precision":11}}}}]}})");
  VERIFICAR(o.estado == Estado::NO_EXISTE);
  VERIFICAR_TEXTO(o.fechaDisolucion, "1990");

  // error de la API por id inexistente
  o = declaraciones(R"({"error":{"code":"no-such-entity","info":"x"}})");
  VERIFICAR(o.error == Error::ENTIDAD_INVALIDA);

  // respuesta sin "claims" ni "error"
  o = declaraciones(R"({"algo":1})");
  VERIFICAR(o.error == Error::RESPUESTA);

  // JSON cortado a la mitad
  o = declaraciones(R"({"claims":{"P576":[{"rank":"nor)");
  VERIFICAR(o.error == Error::RESPUESTA);
}

static void probarEntidad() {
  char id[16];
  // respuesta real para "National_Rifle_Association" en enwiki
  VERIFICAR(entidad(R"({"entities":{"Q863259":{"pageid":814694,"ns":0,)"
                    R"("title":"Q863259","type":"item","id":"Q863259"}},)"
                    R"("success":1})",
                    id, sizeof(id)) == Error::NINGUNO);
  VERIFICAR_TEXTO(id, "Q863259");

  // respuesta real para un artículo inexistente
  VERIFICAR(entidad(R"({"entities":{"-1":{"site":"enwiki","title":"Nada",)"
                    R"("missing":""}},"success":1})",
                    id, sizeof(id)) == Error::ARTICULO_NO_ENCONTRADO);

  VERIFICAR(entidad(R"({"error":{"code":"param-missing"}})", id, sizeof(id)) ==
            Error::ARTICULO_NO_ENCONTRADO);
  VERIFICAR(entidad(R"({"success":1})", id, sizeof(id)) == Error::RESPUESTA);
}

static void probarConfirmador() {
  Confirmador c;
  c.configurar(3);
  VERIFICAR(c.estado() == Estado::DESCONOCIDO);

  // la primera observación de que existe se acepta de inmediato
  VERIFICAR(c.registrar(Estado::EXISTE));
  VERIFICAR(c.estado() == Estado::EXISTE);

  // un vandalismo pasajero no cambia el estado
  VERIFICAR(!c.registrar(Estado::NO_EXISTE));
  VERIFICAR(c.pendientes() == 1);
  VERIFICAR(!c.registrar(Estado::NO_EXISTE));
  VERIFICAR(c.pendientes() == 2);
  VERIFICAR(!c.registrar(Estado::EXISTE));
  VERIFICAR(c.pendientes() == 0);
  VERIFICAR(c.estado() == Estado::EXISTE);

  // tres seguidas confirman
  VERIFICAR(!c.registrar(Estado::NO_EXISTE));
  VERIFICAR(!c.registrar(Estado::NO_EXISTE));
  VERIFICAR(c.registrar(Estado::NO_EXISTE));
  VERIFICAR(c.estado() == Estado::NO_EXISTE);
  VERIFICAR(c.pendientes() == 0);

  // volver a existir también requiere confirmación
  VERIFICAR(!c.registrar(Estado::EXISTE));
  VERIFICAR(c.estado() == Estado::NO_EXISTE);

  // ver que no existe desde el comienzo también requiere confirmación
  Confirmador d;
  d.configurar(2);
  VERIFICAR(!d.registrar(Estado::NO_EXISTE));
  VERIFICAR(d.estado() == Estado::DESCONOCIDO);
  VERIFICAR(d.registrar(Estado::NO_EXISTE));
  VERIFICAR(d.estado() == Estado::NO_EXISTE);

  // con 1 (o 0) los cambios son inmediatos
  Confirmador e;
  e.configurar(0);
  VERIFICAR(e.registrar(Estado::NO_EXISTE));
  VERIFICAR(e.estado() == Estado::NO_EXISTE);
}

int main() {
  probarIdEntidad();
  probarSitio();
  probarCodificarUrl();
  probarRutas();
  probarCodigoHttp();
  probarFechas();
  probarDeclaraciones();
  probarEntidad();
  probarConfirmador();
  probarInstrumento();
  printf("%d pruebas, %d fallas\n", pruebas, fallas);
  return fallas == 0 ? 0 : 1;
}
