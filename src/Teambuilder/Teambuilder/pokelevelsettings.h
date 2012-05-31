#ifndef POKELEVELSETTINGS_H
#define POKELEVELSETTINGS_H

#include <QFrame>

namespace Ui {
    class PokeLevelSettings;
}

class PokeTeam;

class PokeLevelSettings : public QFrame
{
    Q_OBJECT

public:
    explicit PokeLevelSettings(QWidget *parent = 0);
    ~PokeLevelSettings();

    void setPoke(PokeTeam *poke);
    void updateAll();

signals:
    void levelUpdated();
    void shinyUpdated();
    void genderUpdated();

public slots:
    void changeLevel(int newLevel);
    void changeShinyness(bool isShiny);
    void changeGender();

    void updateGender();
    void updateShiny();
private:
    Ui::PokeLevelSettings *ui;

    PokeTeam *m_poke;
    PokeTeam &poke() {return *m_poke;}
    const PokeTeam &poke() const {return *m_poke;}

    void setGender();
};

#endif // POKELEVELSETTINGS_H
