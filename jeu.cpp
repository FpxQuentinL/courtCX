#include "jeu.h"
#include <iostream>
#include <stdlib.h>
#include <QDebug>
#include <QCoreApplication>
#include <QJsonObject>
#include <audiocontroller.h>
#include <wiringPi.h>
#include <QTime>
// Gestion JSON ajout QTIME valid !
int G_InteruptButton=0;
int G_OutputTazer;
int G_OutputDiode;
int G_OutputAiment;
int G_Delay;
int G_DelayFinal;
bool G_DeclenchementInterupt;
QTime G_Action;
QTime G_ActionFinal;
QTime G_Reset;
Jeu::Jeu(IPluginsReport *report) : IPlugins(report)
{

}
void Jeu::loadConfigGameTech(QString config_file)
{
    QJsonDocument docGameTech;
    QDir dirGameTech;
    qDebug() << dirGameTech.path();
    QFile fileGameTech(config_file);
    if (!fileGameTech.open(QIODevice::ReadOnly | QIODevice::Text))
    {
          qDebug("error FFFIILLLEEEUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU");
          return;
    }
    qDebug() << "Opening config file" << config_file;
    docGameTech = QJsonDocument::fromJson(fileGameTech.readAll());
    QJsonObject object = docGameTech.object();
    qDebug("\n\n###\t start json file\t###");
    G_OutputTazer = object.value("ControlerTazer").toString().toInt();
    G_OutputDiode = object.value("ControlerLed").toString().toInt();
    G_OutputAiment = object.value("ControlerAiment").toString().toInt();
    G_InteruptButton = object.value("Interupt").toString().toInt();
    qDebug("###\t end json file###\n\n");

}
void Jeu::loadConfigGameFonct(QString config_file)
{
    QJsonDocument docGame;
    QDir dirGame;
    qDebug() << dirGame.path();
    QFile fileGame(config_file);
    if (!fileGame.open(QIODevice::ReadOnly | QIODevice::Text))
    {
          qDebug("error FFFIILLLEEEUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU");
          return;
    }
    qDebug() << "Opening config file" << config_file;
    docGame = QJsonDocument::fromJson(fileGame.readAll());
    QJsonObject object = docGame.object();
    qDebug("\n\n###\t start json file\t###");

    G_Delay= object.value("Delay").toString().toInt();
    G_DelayFinal = object.value("DelayFinal").toString().toInt();
    _AudioPathInterupt = object.value("AudioPathI").toString();
    _AudioPathFinal = object.value("AudioPathF").toString();



    qDebug("###\t end json file###\n\n");

}


void Jeu::Setup()
{
    wiringPiSetupGpio();
    loadConfigGameFonct("./GameFonct.json");
    loadConfigGameTech("./GameTech.json");
    pinMode(G_OutputTazer,OUTPUT);
    pinMode(G_OutputDiode,OUTPUT);
    pinMode(G_OutputAiment,OUTPUT);
    digitalWrite(G_OutputAiment,HIGH);
    G_Action=QTime(25,0,0,0);
    G_ActionFinal=QTime(25,0,0,0);
    G_Reset=QTime(25,0,0,0);
    if(wiringPiISR(G_InteruptButton,INT_EDGE_FALLING, &Jeu::Interupt)<0)
        qDebug("ERRRROOOOORRRRRR");
}

void Jeu::Interupt()
{
    QTime test=QTime::currentTime().addMSecs(100);

        while (QTime::currentTime()<test){
        if (digitalRead(G_InteruptButton)){
            qDebug()<<"je suis de l'electromagnetisme";
            return;}
        }
        G_Action=QTime::currentTime();
        G_DeclenchementInterupt=true;



}

bool Jeu::Game_Reset()
{
    QJsonObject packetReset{
        {"io_socket","reset"},
        {"command","{\"color\":\"0x000000\",\"delay\":1,\"nbr_led\":144,\"scale\":1}"}
    };
    QJsonObject stop_sound
        {
            {"command", "stop"},
            {"id", "all"},
            {"force", true}
        };

        std::cout << "YOU RESET !!!" << std::endl;
        Game_Audio(stop_sound);
        Game_Socketio(packetReset);
        digitalWrite(G_OutputAiment,HIGH);
        digitalWrite(G_OutputTazer,LOW);
        digitalWrite(G_OutputDiode,LOW);
        G_Reset=QTime(25,0,0,0);
}


bool Jeu::Game_Audio(QJsonObject packet)
{
    Signal_RCon_AudioControler(packet);
    return (true);
}

bool Jeu::Game_Socketio(QJsonObject packet)
{

    //std::cout << "YOU Socket iO !!!" << std::endl;
    Signal_RCon_NetworkIO(packet);
    return (true);
}

void Jeu::Slot_RCon( QJsonObject packet )
{
    qDebug()<<">IPlugins::Slot_RCon<"<< "\n" << packet;
    if (packet.contains("command"))
    {
        if (packet.value("command")=="reset")
        {
            Game_Reset();
        }
    }
}


//GameFlow
void Jeu::main_game_callback()
{
    if(G_DeclenchementInterupt)
    {
    QJsonObject Audiopacket;

       Audiopacket.insert ("command", "play");
       Audiopacket.insert ("path",_AudioPathInterupt);


    //{"command", "{  \"color\":0xF00000,\"delay\":1,\"brightness_max\":255,\"brightness_min\":32,\"brightness_scale\":1,\"pos_start\":0,\"pos_end\":42,\"scale\":10}"}
    QJsonObject Interuptpacket
    {
        {"io_socket", "iterate_and_pulse"},
        {"command", "{  \"color\":\"0xFF0000\",\"delay\":50,\"brightness_max\":255,\"brightness_min\":32,\"brightness_scale\":100,\"pos_start\":0,\"pos_end\":7,\"scale\":1}"}

    };
//    message::ptr const& test; //thibaud you can delete this line

    //std::cout << "YOU WIN !!!" << std::endl;
    G_DeclenchementInterupt=false;

    Game_Audio(Audiopacket);
    QTime T =QTime::currentTime().addMSecs(2400);
    while (QTime::currentTime()<T)
    {

    }
    Game_Socketio(Interuptpacket);
     digitalWrite(G_OutputDiode,HIGH);
    }

    if (G_Action.isValid())
    {
        //qDebug()<<G_Action.isValid
    if (QTime::currentTime()>G_Action.addSecs(G_Delay))
    {
        //qDebug()<<"bonjour";
        QJsonObject packet
        {
            {"io_socket", "iterate_and_pulse"},
            {"command", "{\"color\":\"0xFF0000\",\"delay\":1,\"brightness_max\":255,\"brightness_min\":32,\"brightness_scale\":50,\"pos_start\":7,\"pos_end\":50,\"scale\":1}"}
        };
        Game_Socketio(packet);
        G_Action= QTime(25,0,0,0);
        G_ActionFinal =QTime::currentTime();
        qDebug("lancementAction");
    }
    }

    if (G_ActionFinal.isValid())
    {
        qDebug("yo");
        if (QTime::currentTime()>G_ActionFinal.addMSecs(G_DelayFinal))
        {
            qDebug("lancementActionFinal");
            G_ActionFinal=QTime(25,0,0,0);
            G_Reset=QTime::currentTime();
            QJsonObject Audiopacket;
            {
               Audiopacket.insert ("command", "play");
               Audiopacket.insert ("path",_AudioPathFinal);
            };


            Game_Audio(Audiopacket);
            digitalWrite(G_OutputTazer,HIGH);
            QTime T2=QTime::currentTime().addMSecs(6000);
            while (QTime::currentTime()<T2)
            {

            }
            digitalWrite(G_OutputTazer,LOW);
            digitalWrite(G_OutputAiment,LOW);

        }
    }
    if (G_Reset.isValid())
    {
        if(QTime::currentTime()>G_Reset.addSecs(10))
        {
            Game_Reset();

        }
    }


}

