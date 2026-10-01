# SiguesAhi

Biblioteca de Arduino para construir instrumentos que revisan si una institución sigue existiendo, y avisan cuando deja de existir.

SiguesAhi es un proyecto de código abierto de Aarón Montoya-Moraga, comenzado en septiembre de 2020.

## Cómo funciona

Cada cierto tiempo (por omisión, cada hora) el instrumento consulta a [Wikidata](https://www.wikidata.org) la propiedad [P576, "fecha de disolución, abolición o demolición"](https://www.wikidata.org/wiki/Property:P576) de la institución:

- si no tiene fecha de disolución, la institución **existe**
- si tiene fecha de disolución, la institución **ya no existe**

Para que un vandalismo pasajero en Wikidata no dispare la alarma, el cambio de estado se confirma solo después de varias consultas seguidas con el mismo resultado (por omisión, 3).

## Plataformas

SiguesAhi funciona de dos maneras, con la misma interfaz:

| Plataforma | Placas | Wifi y TLS |
| --- | --- | --- |
| Biblioteca de Arduino | Arduino Nano 33 IoT, MKR WiFi 1010 | WiFiNINA |
| Biblioteca de Arduino, con el núcleo [arduino-pico](https://github.com/earlephilhower/arduino-pico) | Raspberry Pi Pico W, Pico 2 W | WiFi y BearSSL del núcleo |
| Biblioteca del [Pico SDK](https://github.com/raspberrypi/pico-sdk) en C++ | Raspberry Pi Pico W, Pico 2 W | CYW43, lwIP y mbedTLS |

### Instalación en Arduino

Instala SiguesAhi y sus dependencias, [ArduinoJson](https://arduinojson.org) (7 o posterior) y WiFiNINA, desde el gestor de bibliotecas del Arduino IDE.

Para la Pico W, agrega esta URL en *Preferencias → URLs adicionales del gestor de placas* del Arduino IDE, y luego instala "Raspberry Pi Pico/RP2040/RP2350" en el gestor de placas:

```text
https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
```

### Instalación en el Pico SDK

Agrega SiguesAhi al `CMakeLists.txt` de tu programa, después de `pico_sdk_init()`:

```cmake
add_subdirectory(ruta/a/SiguesAhi siguesahi)
target_link_libraries(mi_programa siguesahi pico_stdlib)
```

CMake descarga ArduinoJson automáticamente (solo encabezados, no depende de Arduino). SiguesAhi incluye su propio `lwipopts.h` y `mbedtls_config.h`. Si tu programa necesita los suyos, usa `-DSIGUES_AHI_CONFIGURACION=OFF` y copia las líneas marcadas "SiguesAhi" de [src/siguesahi/pico/configuracion/](src/siguesahi/pico/configuracion/).

El ejemplo completo está en [pico/ejemplos/SerialYLed/](pico/ejemplos/SerialYLed/):

```sh
cd pico/ejemplos/SerialYLed
cmake -S . -B build -DPICO_BOARD=pico2_w   # o pico_w
cmake --build build
```

Luego conecta la placa con el botón BOOTSEL apretado y copia `build/serial_y_led.uf2` a la unidad `RP2350` (o `RPI-RP2`).

## Uso

En Arduino:

```cpp
#include <SiguesAhi.h>

SiguesAhi sigues;
SalidaSerial salidaSerial(Serial);
SalidaLed salidaLed(LED_BUILTIN);

void setup() {
  Serial.begin(115200);

  sigues.configurarRed("nombre de tu red", "clave de tu red");
  sigues.configurarEntidad("Q15180"); // unión soviética
  sigues.agregarSalida(salidaSerial);
  sigues.agregarSalida(salidaLed);
  sigues.comenzar();
}

void loop() {
  sigues.actualizar();
}
```

En el Pico SDK es igual, con `main()` en vez de `setup()` y `loop()`:

```cpp
#include "pico/stdlib.h"
#include <SiguesAhi.h>

SiguesAhi sigues;
SalidaSerial salidaSerial;  // salida estándar, USB o UART
SalidaLed salidaLed;        // LED de la placa

int main() {
  stdio_init_all();

  sigues.configurarRed("nombre de tu red", "clave de tu red");
  sigues.configurarEntidad("Q15180");
  sigues.agregarSalida(salidaSerial);
  sigues.agregarSalida(salidaLed);
  sigues.comenzar();

  while (true) {
    sigues.actualizar();
    sleep_ms(1);
  }
}
```

`actualizar()` debe llamarse en cada `loop()`. Se encarga de conectarse al wifi, reconectarse si se pierde la conexión, consultar a Wikidata en el intervalo configurado, reintentar con espera creciente si algo falla, y avisar a las salidas.

### Datos de la red

Los ejemplos, de Arduino y del Pico SDK, leen los datos de la red de un archivo `secret.h` opcional en la carpeta del ejemplo, que git ignora:

```cpp
#define NOMBRE_RED "nombre de tu red"
#define CLAVE_RED "clave de tu red"
```

### Elegir la institución

Con el id de Wikidata, que aparece en la página de cada entidad, por ejemplo `https://www.wikidata.org/wiki/Q15180`:

```cpp
sigues.configurarEntidad("Q15180");
```

O con el título de un artículo de Wikipedia, tal como aparece en su URL. La biblioteca busca la entidad de Wikidata en la primera consulta:

```cpp
sigues.configurarArticulo("es", "Unión_Soviética");
```

### Configuración

| Función | Descripción | Por omisión |
| --- | --- | --- |
| `configurarRed(nombre, clave)` | red wifi, clave `""` para redes abiertas | — |
| `configurarEntidad(id)` | id de Wikidata | — |
| `configurarArticulo(idioma, titulo)` | artículo de Wikipedia | — |
| `configurarIntervalo(segundos)` | tiempo entre consultas, mínimo 60 | 3600 |
| `configurarConfirmaciones(cantidad)` | consultas seguidas para confirmar un cambio | 3 |
| `configurarAgenteUsuario(texto)` | `User-Agent` enviado a Wikidata, idealmente con un contacto según la [política de Wikimedia](https://meta.wikimedia.org/wiki/User-Agent_policy) | `SiguesAhi/0.1.0 (url del proyecto)` |
| `activarVigilante()` | watchdog: reinicia la placa si el programa se cuelga, ver abajo | desactivado |
| `configurarRegistro(Serial)` | mensajes detallados de diagnóstico; en el Pico SDK, `configurarRegistro()` usa la salida estándar | desactivado |
| `configurarCertificados(pem)` | certificados raíz propios, solo Pico W y Pico 2 W | ISRG Root X1, X2 y DigiCert Global Root G2 |
| `permitirConexionInsegura()` | no verificar el certificado de Wikidata, solo para pruebas, solo Pico W y Pico 2 W | desactivado |

### Lectura del estado

```cpp
sigues.existe();           // true si existe
sigues.yaNoExiste();       // true si ya no existe
sigues.fechaDisolucion();  // "1991-12-26", "1991-12", "1991" o ""
sigues.informe();          // fase, estado, último error, contadores
```

### Salidas

- `SalidaSerial(Serial)` en Arduino, `SalidaSerial()` en el Pico SDK: escribe cada cambio en texto.
- `SalidaLed(pin, activoEnAlto)` en Arduino; en el Pico SDK, `SalidaLed()` para el LED de la placa o `SalidaLed(gpio, activoEnAlto)`: parpadeo rápido al conectarse, un destello corto cada 3 segundos si existe, encendido fijo si ya no existe, doble destello si hay un error.

Se pueden agregar hasta 4 salidas. Para crear una salida propia, hereda de `siguesahi::Salida` (ver el ejemplo `SalidaPropia`):

```cpp
class MiSalida : public siguesahi::Salida {
public:
  void comenzar() override {}                 // una vez, en comenzar()
  void actualizar(uint32_t ahoraMs) override {} // en cada actualizar()
  void notificar(const siguesahi::Informe &informe) override {} // en cada cambio
};
```

## Watchdog

Para un instrumento que funciona solo durante meses, `activarVigilante()` activa el watchdog de hardware de la placa, que la reinicia si el programa se cuelga:

```cpp
sigues.activarVigilante();
sigues.comenzar();
```

- Fuera de las operaciones de red, `loop()` debe llamar a `actualizar()` al menos cada 8 segundos. Evita `delay()` largos en tu código.
- Las operaciones de red que bloquean (conectarse al wifi, cada consulta a Wikidata) tienen un tiempo máximo. Si lo exceden, por ejemplo porque el módulo wifi dejó de responder, la placa se reinicia.
- Después de un reinicio por watchdog, `informe().reinicioPorVigilante` es `true` y `SalidaSerial` lo avisa.

En placas SAMD21 (Nano 33 IoT, MKR WiFi 1010), el watchdog usa la función `sysTickHook()` del núcleo, así que no se puede combinar con otra biblioteca que también la defina.

## Seguridad

La conexión con Wikidata siempre usa HTTPS y verifica el certificado del servidor, para que nadie en la red pueda falsificar la respuesta:

- En placas WiFiNINA, el módulo verifica con los certificados de su firmware.
- En la Pico W y la Pico 2 W, con arduino-pico o con el Pico SDK, la biblioteca incluye certificados raíz y sincroniza la hora por NTP para verificar la cadena, el nombre del servidor y la vigencia. Si Wikimedia cambia a una autoridad que no esté incluida, se pueden entregar otros con `configurarCertificados()`.

## Desarrollo

El núcleo de SiguesAhi ([src/siguesahi/](src/siguesahi/)) no depende de ninguna plataforma. Cada plataforma implementa la red y el reloj definidos en [Plataforma.h](src/siguesahi/Plataforma.h): [arduino/](src/siguesahi/arduino/) y [pico/](src/siguesahi/pico/).

Las pruebas corren en el computador, con una red y un reloj simulados:

```sh
make -C extras/pruebas
```

## Materiales

- Arduino Nano 33 IoT, [store.arduino.cc](https://store.arduino.cc/products/arduino-nano-33-iot), o Raspberry Pi Pico W, [raspberrypi.com](https://www.raspberrypi.com/products/raspberry-pi-pico/)
- Opcional: LED y resistencia de 220 Ω, si no se usa el LED de la placa

## Agradecimientos

Este proyecto fue inspirado por estas obras:

- [@grow_slow](https://github.com/nicolehe/grow_slow), [Nicole He](http://nicole.pizza/), 2016
- [Take a Bullet for This City](http://sites.bxmc.poly.edu/~lukedubois/projects/index.html?id=gun), [Luke Dubois](http://lukedubois.com/), 2014

Este proyecto no habría sido posible sin Wikidata, Wikipedia y Arduino.

## Licencia

MIT
