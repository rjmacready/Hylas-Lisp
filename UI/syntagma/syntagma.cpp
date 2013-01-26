#include <string>

#include <QApplication>
#include <QtGui>
#include <QWebFrame>

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include "../../src/hylas.hpp"
#include "../../src/core.hpp"
#include "../../src/errors.hpp"
#include "../../src/reader.hpp"
#include "../../src/fndef.hpp"
#include "../../src/external.hpp"
#include "../../src/types.hpp"
#include "../../src/utils.hpp"
#include "../../src/docs.hpp"

#include "../../src/hylas.cpp"
#include "../../src/core.cpp"
#include "../../src/errors.cpp"
#include "../../src/reader.cpp"
#include "../../src/fndef.cpp"
#include "../../src/external.cpp"
#include "../../src/types.cpp"
#include "../../src/tests.cpp"
#include "../../src/utils.cpp"
#include "../../src/docs.cpp"

using namespace Hylas;

#include "tabs.cpp"

int main(int argc, char *argv[])
{
  init();
  master.output = HTML;
  master.debug = true;
  master.prompt = "<br><strong>&gt;</strong>";
  QApplication Syntagma(argc,argv);
  Tabs* tabs = new Tabs;
  tabs->show();
  return Syntagma.exec();
}