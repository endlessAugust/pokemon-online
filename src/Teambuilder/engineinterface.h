#ifndef PLUGINDELEGATE_H
#define PLUGINDELEGATE_H

class TeamHolderInterface;
class ThemeAccessor;

class MainEngineInterface {
public:
    virtual TeamHolderInterface* trainerTeam() = 0;
    virtual ThemeAccessor* theme() = 0;
};

#endif // PLUGINDELEGATE_H
