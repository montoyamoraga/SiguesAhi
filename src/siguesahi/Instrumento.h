// Instrumento.h
// núcleo de SiguesAhi, igual en todas las plataformas: pregunta
// periódicamente a wikidata si una institución sigue existiendo, revisando
// su fecha de disolución (propiedad P576), y avisa a las salidas.
// cada plataforma lo usa a través de la clase SiguesAhi, en SiguesAhi.h

#ifndef SIGUES_AHI_INSTRUMENTO_H
#define SIGUES_AHI_INSTRUMENTO_H

#include <ArduinoJson.h>

#include "Confirmador.h"
#include "Plataforma.h"
#include "Salida.h"
#include "Tipos.h"
#include "Vigilante.h"

#define SIGUES_AHI_VERSION "0.1.0"

namespace siguesahi {

class Instrumento {
public:
  static const uint8_t MAXIMO_SALIDAS = 4;
  static const uint32_t INTERVALO_MINIMO_S = 60;

  /// @brief usa la red de la plataforma
  Instrumento();

  /// @brief usa otra red, por ejemplo una simulada para pruebas
  explicit Instrumento(Red &red);

  // --- configuración, antes de comenzar() ---

  /// @brief nombre y clave de la red wifi, clave vacía para redes abiertas
  bool configurarRed(const char *nombre, const char *clave);

  /// @brief institución por su id de wikidata, por ejemplo "Q15180"
  bool configurarEntidad(const char *idEntidad);

  /// @brief institución por su artículo de wikipedia, por ejemplo
  /// configurarArticulo("es", "Unión_Soviética")
  bool configurarArticulo(const char *idioma, const char *titulo);

  /// @brief segundos entre consultas, mínimo 60, por omisión 3600
  void configurarIntervalo(uint32_t segundos);

  /// @brief consultas seguidas que deben coincidir para cambiar de estado,
  /// por omisión 3, para ignorar vandalismos pasajeros en wikidata
  void configurarConfirmaciones(uint8_t cantidad);

  /// @brief User-Agent enviado a wikidata, idealmente con un contacto,
  /// ver https://meta.wikimedia.org/wiki/User-Agent_policy
  bool configurarAgenteUsuario(const char *agente);

  /// @brief certificados raíz en formato PEM, en placas sin almacén propio.
  /// el texto debe seguir existiendo mientras se use
  bool configurarCertificados(const char *pem);

  /// @brief no verificar el certificado de wikidata, solo para pruebas.
  /// sin efecto en placas WiFiNINA, que siempre verifican
  void permitirConexionInsegura(bool permitir = true);

  /// @brief activa el watchdog: reinicia la placa si el programa se cuelga.
  /// desde comenzar(), hay que llamar a actualizar() al menos cada
  /// 8 segundos, así que evita esperas largas en tu código
  void activarVigilante();

  /// @brief mensajes detallados de diagnóstico
  void configurarRegistro(Destino &destino);

  /// @brief agrega una salida, hasta MAXIMO_SALIDAS
  bool agregarSalida(Salida &salida);

  // --- funcionamiento ---

  /// @brief llamar una vez al comenzar, false si falta configuración
  bool comenzar();

  /// @brief llamar continuamente. bloquea solo mientras consulta a wikidata
  /// (unos segundos) y, en placas WiFiNINA, mientras se conecta a wifi
  void actualizar();

  /// @brief adelanta la próxima consulta
  void consultarAhora();

  // --- lectura ---

  const Informe &informe() const { return _informe; }
  Estado estado() const { return _informe.estado; }
  bool existe() const { return _informe.estado == Estado::EXISTE; }
  bool yaNoExiste() const { return _informe.estado == Estado::NO_EXISTE; }
  const char *fechaDisolucion() const { return _informe.fechaDisolucion; }

private:
  void cambiarFase(Fase fase);
  void conectarWifi();
  void esperar(uint32_t duracionMs);
  void consultar();
  Error resolverEntidad();
  Error pedir(const char *ruta, JsonDocument &respuesta,
              const JsonDocument &filtro);
  void registrarFalla(Error error);
  void notificar();
  void registrar(const char *mensaje, const char *detalle);

  Red &_red;
  Confirmador _confirmador;
  Informe _informe;
  Vigilante _vigilante;
  bool _usarVigilante = false;

  Salida *_salidas[MAXIMO_SALIDAS] = {nullptr};
  uint8_t _cantidadSalidas = 0;
  Destino *_registro = nullptr;

  char _nombreRed[33] = {0};
  char _claveRed[64] = {0};
  char _sitio[20] = {0};
  char _titulo[128] = {0};
  char _agente[128] = {0};

  uint32_t _intervaloMs = 3600UL * 1000UL;
  uint32_t _inicioEspera = 0;
  uint32_t _duracionEspera = 0;
  uint32_t _inicioFase = 0;
  uint8_t _fallasSeguidas = 0;
};

} // namespace siguesahi

#endif
