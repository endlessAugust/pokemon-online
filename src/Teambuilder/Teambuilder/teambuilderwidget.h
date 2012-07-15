#ifndef TEAMBUILDERWIDGET_H
#define TEAMBUILDERWIDGET_H

#include <QFrame>
#include <QHash>

class QMainWindow;
class QDockWidget;
class QMenuBar;

class TeamBuilderWidget  : public QFrame
{
    Q_OBJECT
public:
    TeamBuilderWidget(QWidget *parent=0) : QFrame(parent){}
    virtual void updateAll() {updateTeam();}
    virtual void updateTeam(){}
    virtual void addMenus(QMenuBar *){}
    void addDock(int id, Qt::DockWidgetArea area, QDockWidget *widget);
    QDockWidget *getDock(int id);
    void hideAll();
    void showAll();
    void setMainWindow(QMainWindow *main) {window = main;}
signals:
    void teamChanged();
protected:
    QMainWindow *window;
    QHash<int, QDockWidget*> widgets;
};

#endif // TEAMBUILDERWIDGET_H
