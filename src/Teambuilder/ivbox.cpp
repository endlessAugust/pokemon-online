#include "../PokemonInfo/pokemonstructs.h"
#include "../PokemonInfo/pokemoninfo.h"
#include "ivbox.h"
#include "ui_ivbox.h"
#include "theme.h"

IvBox::IvBox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IvBox)
{
    ui->setupUi(this);

    QLabel *statsLabels[6] = {ui->hpivlabel, ui->attivlabel, ui->defivlabel, ui->spattivlabel, ui->spdefivlabel, ui->speedivlabel};
    QSpinBox *ivChangers[6] = {ui->hpivspin, ui->attivspin, ui->defivspin, ui->spattivspin, ui->spdefivspin, ui->speedivspin};
    QRadioButton *abilitiesButtons[3] = {ui->ability1, ui->ability2, ui->ability3};

    memcpy(m_statslabel, statsLabels, sizeof(statsLabels));
    memcpy(m_ivchangers, ivChangers, sizeof(ivChangers));
    memcpy(m_abilities, abilitiesButtons, sizeof(abilitiesButtons));

#define setNums(var) for(int i = 0; i < 6; i++) { var[i]->setProperty("ivstat", i); }
    setNums(m_ivchangers);
#undef setNums

    for(int i = 0; i < 6; i++) {
        connect(m_ivchangers[i], SIGNAL(valueChanged(int)), this, SLOT(changeIV(int)));
    }

    for(int i = 1; i < TypeInfo::NumberOfTypes() - 1; i++) {
        ui->hiddenPowerType->addItem(TypeInfo::Name(i));
    }

    connect(ui->hiddenPowerType, SIGNAL(activated(int)), this, SLOT(changeHiddenPower(int)));

}

IvBox::~IvBox()
{
    delete ui;
}

void IvBox::setPoke(PokeTeam *poke)
{
    m_poke = poke;
    updateAll();
}

// We need to initialize in a separate function since setPoke() is obviously called after IvBox::IvBox
// So if we do this things inside that function, a nicely crash.

void IvBox::updateAll()
{
    // Abilities.

    for(int i = 0; i < 3; i++) {
        if(poke().abilities().ab(i) != 0) {
            m_abilities[i]->setVisible(true);
            m_abilities[i]->setText(AbilityInfo::Name(poke().abilities().ab(i)));
            m_abilities[i]->setToolTip(AbilityInfo::Desc(poke().abilities().ab(i)));
            connect(m_abilities[i], SIGNAL(toggled(bool)), this, SLOT(changeAbility()));
        } else {
             m_abilities[i]->setVisible(false);
        }
    }
    updateIVs();
    updateStats();
    updateHiddenPower();
}

void IvBox::updateStats()
{
    for(int i = 0; i < 6; i++) {
        updateStat(i);
    }
    emit statsUpdated();
}

void IvBox::updateStat(int stat)
{
    if(poke().natureBoost(stat) != 0) {
        QColor themeColor;
        switch(poke().natureBoost(stat)) {
        case -1:  themeColor = Theme::Color("Teambuilder/statHindered"); break;
        case 1: themeColor = Theme::Color("Teambuilder/statRaised"); break;
        }
        m_statslabel[stat]->setText(toColor(QString::number(poke().stat(stat)), themeColor));
    } else {
        m_statslabel[stat]->setText(QString::number(poke().stat(stat)));
    }
    emit statsUpdated();
}

void IvBox::changeIV(int newValue)
{
    int stat = sender()->property("ivstat").toInt();
    if(poke().DV(stat) != newValue) {
        poke().setDV(stat, newValue);
        updateStat(stat);
        updateHiddenPower();
    }
}

void IvBox::updateAbility()
{
    for(int i = 0; i < 3; i++) {
        if(poke().ability() == poke().abilities().ab(i)) {
            m_abilities[i]->setChecked(true);
        }
    }
}

void IvBox::changeAbility()
{
    for(int i = 0; i < 3; i++) {
        if(m_abilities[i]->isChecked()) {
            poke().ability() = poke().abilities().ab(i);
            break;
        }
    }
}

void IvBox::updateIVs()
{
    for(int i = 0; i < 6; i++) {
        updateIV(i);
    }
}

void IvBox::updateIV(int stat)
{
    m_ivchangers[stat]->setValue(poke().DV(stat));
    updateStat(stat);
}

void IvBox::updateHiddenPower()
{
    ui->hiddenPowerPower->setText("Power: " + QString::number(calculateHiddenPowerPower()));
    int type = calculateHiddenPowerType();
    ui->hiddenPowerType->setCurrentIndex(type - 1);
}

int IvBox::calculateHiddenPowerPower()
{
    return HiddenPowerInfo::Power(poke().gen().num, poke().DV(0), poke().DV(1), poke().DV(2), poke().DV(3), poke().DV(4), poke().DV(5));
}

int IvBox::calculateHiddenPowerType()
{
    return HiddenPowerInfo::Type(poke().gen().num, poke().DV(0), poke().DV(1), poke().DV(2), poke().DV(3), poke().DV(4), poke().DV(5));
}

void IvBox::changeHiddenPower(int newType)
{
    if (newType == calculateHiddenPowerType()) {
        return;
    }
    QStringList possibility = HiddenPowerInfo::PossibilitiesForType(newType).front();

    for (int i = 0; i < std::max(6, possibility.size()); i++) {
        poke().setDV(i, possibility[i].toInt());
    }

    updateIVs();
}

