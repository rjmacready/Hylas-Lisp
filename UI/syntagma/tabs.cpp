#include "tabs.hpp"

Tabs::Tabs(QWidget *parent) : QTabWidget(parent)
{
  setupUi(this);
  //After initializing, lets push the prompt and the logo to the console
  console->setHtml(QString(("<img src='./docs/res/img/logo.svg'>" + master.prompt).c_str()));
  //Connect the buttons on the console tab
  connect(bRestart, SIGNAL(clicked()),
		  this, SLOT(restart()));
  connect(bHistory, SIGNAL(clicked()),
		  this, SLOT(history()));
  connect(bSend, SIGNAL(clicked()),
		  this, SLOT(send()));
  //Connect the buttons on the documentation tab
  connect(bBack, SIGNAL(clicked()),
		  docview, SLOT(back()));
  connect(bForward, SIGNAL(clicked()),
		  docview, SLOT(forward()));
}

void Tabs::restart()
{
  console->setHtml(QString(master.prompt.c_str()));
  init();
}

void Tabs::history()
{

}

void Tabs::send()
{
  string code = input->toPlainText().toStdString();
  //cerr << "Code:\n" << code << endl;
  string collect;
  try
  {
	JIT(Compile(readToplevelString(print(readString(code)))));
	QString output = Run().c_str();
	puts("*************\n");
	puts(output.toStdString().c_str());
	puts("*************\n");
	//collect += "Output:\n" + output.toStdString();
	console->setHtml(console->page()->currentFrame()->toHtml() + input->toPlainText() + QString("<br>") + output);
  }
  catch(exception except)
  {
	QString err = getError().c_str();
	//collect += "Error:\n" + err.toStdString();
	console->setHtml(console->page()->currentFrame()->toHtml() + input->toPlainText() + QString("<br>") + err);
  }
  //cerr << collect << endl;
  console->setHtml(QString(((console->page()->currentFrame()->toHtml()).toStdString() + master.prompt).c_str()));
  input->clear();
  cerr << console->page()->currentFrame()->toHtml().toStdString() << endl;
}
