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
  Form* code;
  if(freopen("stdout.buf","w",stdout) == NULL)
	nerror("Could not open stdout buffer.");
  string buf;
  try
  {
	master.output = Plain;
	code = readString(input->toPlainText().toStdString());
	string s = print(code);
	master.output = HTML;
	JIT(Compile(readToplevelString(s)));
	QString output = Run().c_str();
	fclose(stdout); //It's VITAL to close stdout before reading from the buffer file
	ifstream base("stdout.buf");
	if(!base.good())
	  nerror("Could not read from stdout buffer.");
	stringstream file;
	file << base.rdbuf();
	buf = newlinesToTag(file.str());
	console->setHtml(console->page()->currentFrame()->toHtml()
	  + QString(print(code).c_str()) + QString("<br>") + QString(buf.c_str()) + output);
  }
  catch(exception except)
  {
	QString err = getError().c_str();
	console->setHtml(console->page()->currentFrame()->toHtml() + QString(print(code).c_str()) + QString("<br>") + err);
  }
  if(remove("stdout.buf") != 0)
	nerror("Could not remove the stdout buffer.");
  console->setHtml(QString(((console->page()->currentFrame()->toHtml()).toStdString() + master.prompt).c_str()));
  input->clear();
}
