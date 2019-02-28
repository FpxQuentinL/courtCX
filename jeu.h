#ifndef JEU_H
#define JEU_H

#include "iplugins.h"
#include <QJsonObject>

class Jeu : public IPlugins
{
    Q_OBJECT

public:
    Jeu(IPluginsReport* report);

    // QThread interface
    //bool checkVictoryCondition();
    //bool Game_Success();
    //bool Game_Fail();
    //bool Game_Stop();
    //bool Game_State();
    bool Game_Socketio(QJsonObject packet);
    bool Game_Audio(QJsonObject packet);
    void Setup();
    void loadConfigGameFonct(QString config_file);
    void loadConfigGameTech(QString config_file);
    QString _AudioPathInterupt;
    QString _AudioPathFinal;
    bool Game_Reset();
public Q_SLOTS:
    static void Interupt();
    void Slot_RCon(QJsonObject packet);

protected:
    virtual void main_game_callback() override;
    int _score;

Q_SIGNALS:
    void Signal_RCon_AudioControler(QJsonObject);
    void Signal_RCon_NetworkIO(QJsonObject);
    void Signal_RCon_Network(QJsonObject);

};

#endif // JEU_H
