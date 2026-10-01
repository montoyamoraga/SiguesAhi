# pruebas de la lógica pura de SiguesAhi en el computador
# uso: make -C extras/pruebas
# ArduinoJson se busca junto a esta biblioteca, o se indica con
#   make ARDUINOJSON=/ruta/a/ArduinoJson/src

ARDUINOJSON ?= ../../../ArduinoJson/src
CXXFLAGS ?= -std=c++11 -Wall -Wextra -Werror -g -fsanitize=address,undefined

NUCLEO = ../../src/siguesahi
FUENTES = pruebas.cpp pruebas_instrumento.cpp \
	$(NUCLEO)/Http.cpp $(NUCLEO)/Instrumento.cpp $(NUCLEO)/SalidaParpadeo.cpp \
	$(NUCLEO)/SalidaTexto.cpp $(NUCLEO)/Tipos.cpp $(NUCLEO)/Vigilante.cpp \
	$(NUCLEO)/Wikidata.cpp

pruebas: $(FUENTES) pruebas.h $(NUCLEO)/*.h
	$(CXX) $(CXXFLAGS) -I$(ARDUINOJSON) $(FUENTES) -o $@

.PHONY: correr clean
correr: pruebas
	./pruebas

clean:
	rm -f pruebas
	rm -rf pruebas.dSYM

.DEFAULT_GOAL := correr
