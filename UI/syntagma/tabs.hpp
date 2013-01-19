#include <ui_tabs.h>

class Tabs : public QTabWidget, public Ui::tabs
{
  Q_OBJECT
  public:
	Tabs(QWidget *parent = 0);
  private slots:
	void restart();
	void history();
	void send();
};
