#include "../PokemonInfo/pokemonstructs.h"

#include <QStackedWidget>
#include <QTabBar>
#include <QGridLayout>
#include <QStringListModel>
#include <QMenuBar>

#include "../PokemonInfo/pokemoninfo.h"
#include "Teambuilder/teammenu.h"
#include "Teambuilder/pokeedit.h"
#include "Teambuilder/teamholder.h"

TeamMenu::TeamMenu(QAbstractItemModel *pokeModel, TeamHolder *team, int index) :
    ui(new _ui()), m_team(team)
{
    ui->pokemonModel = pokeModel;
    setupUi();
    updateTabs();

    switchToTab(index);
}

void TeamMenu::setupUi()
{
    QGridLayout *grid = new QGridLayout(this);
    grid->addWidget(ui->stack = new QStackedWidget(), 0, 0, 1, 1);
    grid->addWidget(ui->pokemonTabs = new QTabBar(), 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignTop);
    grid->setContentsMargins(-1,0,-1,-1);

    for (int i = 0; i < 6; i++) {
        ui->pokemonTabs->addTab(PokemonInfo::Icon(i+1), tr("Slot #&%1").arg(i+1));
    }
    ui->pokemonTabs->setCurrentIndex(0);
    ui->pokemonTabs->setObjectName("pokemonTabs");

    QSettings s;
    QStringList itemList = s.value("show_all_items").toBool() ? ItemInfo::SortedNames(team().team().gen()) : ItemInfo::SortedUsefulNames(team().team().gen());
    ui->itemsModel = new QStringListModel(itemList, this);

    QStringList natures;
    for (int i = 0; i < NatureInfo::NumberOfNatures(); i++) {
        natures.push_back(NatureInfo::Name(i));
    }
    ui->natureModel = new QStringListModel(natures, this);

    connect(ui->pokemonTabs, SIGNAL(currentChanged(int)), SLOT(switchToTab(int)));
}

void TeamMenu::switchToTab(int index)
{
    if (ui->pokemonTabs->currentIndex() != index) {
        /* The signal/slot connection will call us again, thus we return */
        ui->pokemonTabs->setCurrentIndex(index);
        return;
    }
    if (!ui->pokemons.contains(index)) {
        ui->stack->addWidget(ui->pokemons[index]= new PokeEdit(&team().team().poke(index), ui->pokemonModel, ui->itemsModel, ui->natureModel));
        ui->pokemons[index]->setProperty("slot", index);
        connect(ui->pokemons[index], SIGNAL(switchToTrainer()), SIGNAL(switchToTrainer()));
        connect(ui->pokemons[index], SIGNAL(numChanged()), SLOT(tabIconChanged()));
    }
    ui->stack->setCurrentWidget(ui->pokemons[index]);
}

void TeamMenu::tabIconChanged()
{
    int slot = sender()->property("slot").toInt();

    ui->pokemonTabs->setTabIcon(slot, PokemonInfo::Icon(team().team().poke(slot).num()));
}

void TeamMenu::updateTeam()
{
    updateTabs();

    foreach(PokeEdit *p, ui->pokemons) {
        p->setPoke(&team().team().poke(p->property("slot").toInt()));
    }
}

void TeamMenu::updateTabs()
{
    for (int i = 0; i < 6; i++) {
        ui->pokemonTabs->setTabIcon(i, PokemonInfo::Icon(team().team().poke(i).num()));
    }
}

TeamMenu::~TeamMenu()
{
    delete ui;
}

void TeamMenu::addMenus(QMenuBar *menuBar)
{
    QMenu *gen = menuBar->addMenu(tr("&Gen."));
    QActionGroup *gens = new QActionGroup(gen);

    for (int i = GEN_MIN; i <= GEN_MAX; i++) {
        int n = Gen::nums[i-GEN_MIN];

        gen->addSeparator()->setText(GenInfo::Gen(i));

        for (int j = 0; j < n; j++) {
            Pokemon::gen g(i, j);

            ui->gens[g] = gen->addAction(GenInfo::Version(g), this, SLOT(genChanged()));
            ui->gens[g]->setCheckable(true);
            ui->gens[g]->setProperty("gen", QVariant::fromValue(g));
            gens->addAction(ui->gens[g]);
        }
    }

    ui->gens[team().team().gen().num]->setChecked(true);
}

void TeamMenu::genChanged()
{
    Pokemon::gen gen = sender()->property("gen").value<Pokemon::gen>();

    if (gen == team().team().gen()) {
        return;
    }

    team().team().setGen(gen);

    for (int i = 0; i < 6; i++) {
        team().team().poke(i).load();
        team().team().poke(i).runCheck();
    }

    updateAll();
    emit teamChanged();
}
