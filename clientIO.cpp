#include <QJsonObject>
#include <QString>
#include <QObject>
#include <QDebug>

#include "clientIO.h"
#include <sio_client.h>
#include <functional>
#include <cstring>
#include <cstdlib>

using namespace sio;
using namespace std;

clientIO::clientIO(QObject *parent) :
    QObject(parent)
    //_io2(new client())
{

}

/*
void Server::acceptNextConnection()
{
    QTcpSocket* socket = nextPendingConnection();
    connect( socket, SIGNAL(readyRead()), this, SLOT(readSocket()) );
    _sockets.push_back( socket );
    qDebug() << "New Client connected : ";
}

void ClientIO::readSocket()
{
    QTcpSocket* socket = dynamic_cast< QTcpSocket* >( sender() );
    if( !socket )
        return;
    qDebug() << "socket received : " << socket->readAll();
    test_signal();
}
*/

void clientIO::Slot_RCon(QJsonObject packet)
{
    qDebug("TU IO SOCKET !!! :D");

    writeSocket(packet);

}


void clientIO::writeSocket(QJsonObject packet)
{
    QString m_data;
 //   QString m_socket;

    qDebug()<< packet.value("io_socket").toString();
    m_data = packet.value("command").toString();
  //  m_socket = packet.value("io_socket").toString();
    QByteArray bytes = m_data.toUtf8();
  //  QByteArray bytes = m_data.toUtf8();
    std::string data(bytes.data(),bytes.length());
   // std::string socket(bytes.data(),bytes.length());


     _io2.socket()->emit(packet.value("io_socket").toString().toStdString().c_str(), data);
   //_io2.socket()->emit("iterate_and_pulse", "{color:0xF00000, delay:1, brightness_max:255, brightness_min:32, brightness_scale:5, pos_start:0, pos_end:144, scale:1}");
}


void clientIO::Initialize(QJsonObject config)
{
    qDebug() << "Conection to :"  << config.value("adress").toString();
   // _io2.connect("http://localhost:4242");
    _io2.connect(config.value("adress").toString().toStdString());

}
