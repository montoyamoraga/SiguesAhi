// include library for OutputPrinter
#include "OutputPrinter.h"

OutputPrinter::OutputPrinter() {}

OutputPrinter::~OutputPrinter() {
  if (_printer != nullptr) {
    delete _printer;
    _printer = nullptr;
  }
}
