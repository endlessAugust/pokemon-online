#include "analyze.h"
#include "network.h"
#include "client.h"
#include "../PokemonInfo/networkstructs.h"
#include "../PokemonInfo/battlestructs.h"
#include "../Shared/config.h"
#include "teamholder.h"

#include "battlewindow.h"


using namespace NetworkCli;

Analyzer::Analyzer(bool reg_connection) : registry_socket(reg_connection)
{
    connect(&socket(), SIGNAL(connected()), SIGNAL(connected()));
    connect(&socket(), SIGNAL(connected()), this, SLOT(wasConnected()));
    connect(&socket(), SIGNAL(disconnected()), SIGNAL(disconnected()));
    connect(this, SIGNAL(sendCommand(QByteArray)), &socket(), SLOT(send(QByteArray)));
    connect(&socket(), SIGNAL(isFull(QByteArray)), SLOT(commandReceived(QByteArray)));
    connect(&socket(), SIGNAL(error(QAbstractSocket::SocketError)), SLOT(error()));

    /* Commands that will be redirected to channels */
    channelCommands << BattleList << JoinChannel << LeaveChannel << ChannelBattle << ChannelMessage << HtmlChannel;
}

void Analyzer::login(const TeamHolder &team, bool ladder, const QColor &color)
{
    QByteArray tosend;
    DataStream out(&tosend, QIODevice::WriteOnly);

    Flags network;
    network.setFlags( (1 << LoginCommand::HasClientType) | (1 << LoginCommand::HasVersionNumber) | (1 << LoginCommand::HasTrainerInfo)
                     | (1 << LoginCommand::HasTeams));

    if (color.isValid()) {
        network.setFlag(LoginCommand::HasColor, true);
    }
    //    HasClientType,
    //    HasVersionNumber,
    //    HasReconnect,
    //    HasDefaultChannel,
    //    HasAdditionalChannels,
    //    HasColor,
    //    HasTrainerInfo,
    //    HasTeams,
    //    HasEventSpecification,
    //    HasPluginList

    Flags data;
    data.setFlag(PlayerFlags::SupportsZipCompression, true);
    data.setFlag(PlayerFlags::LadderEnabled, ladder);
//                  SupportsZipCompression,
//                  ShowTeam,
//                  LadderEnabled,
//                  Idle,
//                  IdsWithMessage

    out << uchar(Login) << ProtocolVersion() << network << QString("windows") << CLIENT_VERSION_NUMBER << team.profile().name() << data;
    if (color.isValid()) {
        out << color;
    }

    out << team.profile().info();

    out << uchar(team.count());
    for (int i = 0; i < team.count(); i++) {
        out << team.team(i);
    }

    emit sendCommand(tosend);
}

void Analyzer::sendChallengeStuff(const ChallengeInfo &c)
{
    notify(ChallengeStuff, c);
}

void Analyzer::getUserInfo(const QString &name)
{
    notify(GetUserInfo, name);
}

void Analyzer::sendPM(int id, const QString &mess)
{
    notify(SendPM, qint32(id), mess);
}

void Analyzer::sendMessage(const QString &message)
{
    notify(SendMessage, message);
}

void Analyzer::sendChanMessage(int channelid, const QString &message)
{
    notify(ChannelMessage, qint32(channelid), message);
}

void Analyzer::sendTeam(const TeamHolder &team)
{
    notify(SendTeam, team.team());
}

void Analyzer::sendBattleResult(int id, int result)
{
    notify(BattleFinished, qint32(id), qint32(result));
}

void Analyzer::battleCommand(int id, const BattleChoice &comm)
{
    notify(BattleMessage, qint32(id), comm);
}

void Analyzer::channelCommand(int command, int channelid, const QByteArray &body)
{
    QByteArray tosend;
    DataStream out(&tosend, QIODevice::WriteOnly);
    out << uchar(command) << qint32(channelid);

    /* We don't know for sure that << body will do what we want (as ServerSide we don't want
       to add a layer here), so a little hack, hoping datastreams concatenate data the same way as
        we do. */
    emit sendCommand(tosend + body);
}

void Analyzer::battleMessage(int id, const QString &str)
{
    if (dynamic_cast<BattleWindow*>(sender()) != NULL)
        notify(BattleChat, qint32(id), str);
    else
        notify(SpectatingBattleChat, qint32(id), str);
}

void Analyzer::CPUnban(const QString &name)
{
    notify(NetworkCli::CPUnban, name);
}

void Analyzer::CPTUnban(const QString &name)
{
    notify(NetworkCli::CPUnban, name);
}

void Analyzer::goAway(bool away)
{
    notify(Away, away);
}

void Analyzer::disconnectFromHost()
{
    socket().disconnectFromHost();
    socket().disconnect(this, SLOT(commandReceived(QByteArray)));
}

QString Analyzer::getIp() const
{
    return socket().peerAddress().toString();
}

void Analyzer::getRanking(const QString &tier, const QString &name)
{
    notify(ShowRankings, tier, false, name);
}

void Analyzer::getRanking(const QString &tier, int page)
{
    notify(ShowRankings, tier, true, qint32(page));
}

void Analyzer::connectTo(const QString &host, quint16 port)
{
    mysocket.connectToHost(host, port);
}

void Analyzer::error()
{
    emit connectionError(socket().error(), socket().errorString());
}

void Analyzer::wasConnected()
{
    socket().setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    /* At least makes client use full bandwith, even if the server doesn't */
    socket().setSocketOption(QAbstractSocket::LowDelayOption, 1);
}

/*{
    QHash<qint32, Battle> battles;
    in >> battles;
    emit battleListReceived(battles);
    break;
}*/

void Analyzer::commandReceived(const QByteArray &commandline)
{
    DataStream in (commandline);
    uchar command;

    in >> command;

    if (channelCommands.contains(command)) {
        qint32 chanid;
        in >> chanid;

        /* Because we're giving a pointer to a locally declared instance, this connection must
           be a DIRECT connection and never go in Queued mode. If you want queued mode, find another
           way to transfer the data */
        emit channelCommandReceived(command, chanid, &in);
        return;
    }
    switch (command) {
    case ZipCommand: {
        quint8 contentType;

        in >> contentType;

        if (contentType > 1) {
            return;
        }

        int length = commandline.length()-1-1;

        if (length <= 0) {
            return;
        }
        char data[length];

        in.readRawData(data, length);

        QByteArray info = qUncompress((uchar*)data, length);

        if (contentType == 0) {
            if (info.length() == 0) {
                return;
            }

            commandReceived(info);
        } else { //contentType == 1
            DataStream in2(info);
            QByteArray packet;
            do {
                in2 >> packet;

                if (packet.length() > 0) {
                   commandReceived(packet);
                }
            } while (packet.length() > 0);
        }
        break;
    }
    case SendMessage: {
	    QString mess;
	    in >> mess;
	    emit messageReceived(mess);
	    break;
	}
    case HtmlMessage: {
            QString mess;
            in >> mess;
            emit htmlMessageReceived(mess);
            break;
        }
    case KeepAlive: {
            notify(KeepAlive);
            break;
        }
    case PlayersList: {
            if (!registry_socket) {
                PlayerInfo p;
                while (!in.atEnd()) {
                    in >> p;
                    emit playerReceived(p);
                }
                break;
            } else {
                // Registry socket;
                QString servName, servDesc, ip;
                quint16 numPlayers, max, port;
                in >> servName >> servDesc >> numPlayers >> ip >> max >> port;
                emit serverReceived(servName, servDesc, numPlayers, ip, max, port);
            }
	}
    case Login: {
            PlayerInfo p;
	    in >> p;
	    emit playerLogin(p);
	    break;
	}
    case Logout: {
            qint32 id;
	    in >> id;
	    emit playerLogout(id);
	    break;
	}
    case ChallengeStuff: {
            ChallengeInfo c;
            in >> c;
            emit challengeStuff(c);
	    break;
	}
    case EngageBattle: {
            qint32 battleid, id1, id2;
            in >> battleid >> id1 >> id2;

            if (id1 == 0) {
                /* This is a battle we take part in */
                TeamBattle team;
                BattleConfiguration conf;
                in >> conf >> team;
                emit battleStarted(battleid, id2, team, conf);
            } else {
                /* this is a battle of strangers */
                emit battleStarted(battleid, id1, id2);
            }
	    break;
	}
    case BattleFinished: {
            qint8 desc;
            qint32 battleid;
            qint32 id1, id2;
            in >> battleid >> desc >> id1 >> id2;
            emit battleFinished(battleid, desc, id1, id2);
	    break;
	}
    case BattleMessage: {
            qint32 battleid;
            QByteArray command;
            in >> battleid >> command;

            emit battleMessage(battleid, command);
	    break;
	}
    case AskForPass: {
            QByteArray salt;
            in >> salt;

            if (salt.length() < 6 || strlen((" " + salt).data()) < 7)
                emit protocolError(5080, tr("The server requires insecure authentication."));
            emit passRequired(salt);
            break;
        }
    case Register: {
            emit notRegistered(true);
            break;
        }
    case PlayerKick: {
            qint32 p,src;
            in >> p >> src;
            emit playerKicked(p,src);
            break;
        }
    case PlayerBan: {
            qint32 p,src;
            in >> p >> src;
            emit playerBanned(p,src);
            break;
        }
    case SendTeam: {
            PlayerInfo p;
            in >> p;
            emit teamChanged(p);
            break;
        }
    case SendPM: {
            qint32 idsrc;
            QString mess;
            in >> idsrc >> mess;
            emit PMReceived(idsrc, mess);
            break;
        }
    case GetUserInfo: {
            UserInfo ui;
            in >> ui;
            emit userInfoReceived(ui);
            break;
        }
    case GetUserAlias: {
            QString s;
            in >> s;
            emit userAliasReceived(s);
            break;
        }
    case GetBanList: {
            QString s, i;
            in >> s >> i;
            emit banListReceived(s,i);
            break;
        }
    case Away: {
            qint32 id;
            bool away;
            in >> id >> away;
            emit awayChanged(id, away);
            break;
        }
    case SpectateBattle: {
            BattleConfiguration conf;
            qint32 battleId;
            in >> battleId >> conf;
            emit spectatedBattle(battleId, conf);
            break;
        }
    case SpectatingBattleMessage: {
            qint32 battleId;
            in >> battleId;
            /* Such a headache, it really looks like wasting ressources */
            char *buf;
            uint len;
            in.readBytes(buf, len);
            QByteArray command(buf, len);
            delete [] buf;
            emit spectatingBattleMessage(battleId, command);
            break;
        }
    case SpectatingBattleFinished: {
            qint32 battleId;
            in >> battleId;
            emit spectatingBattleFinished(battleId);
            break;
        }
    case NetworkCli::VersionControl_: {
            ProtocolVersion server, feature, minor, major;
            Flags f;
            QString serverName;

            in >> server >> f >> feature >> minor >> major >> serverName;

            emit serverNameReceived(serverName);

            ProtocolVersion version;

            if (version < major) {
                emit versionDiff(major, -3);
            } else if (version < minor) {
                emit versionDiff(minor, -2);
            } else if (version < feature) {
                emit versionDiff(feature, -1);
            } else if (version < server) {
                emit versionDiff(server, 0);
            }

            break;
        }
    case TierSelection: {
            QByteArray tierList;
            in >> tierList;
            emit tierListReceived(tierList);
            break;
        }
    case ShowRankings: {
            bool starting;
            in >> starting;
            if (starting)
            {
                qint32 startingPage, startingRank, total;
                in >> startingPage >> startingRank >> total;
                emit rankingStarted(startingPage, startingRank, total);
            } else {
                QString name;
                qint32 points;
                in >> name >> points;
                emit rankingReceived(name, points);
            }
            break;
        }
    case Announcement: {
            QString ann;
            in >> ann;
            emit announcement(ann);
            break;
        }
    case ChannelsList: {
            QHash<qint32, QString> channels;
            in >> channels;
            emit channelsListReceived(channels);
            break;
        }
    case ChannelPlayers: {
            QVector<qint32> ids;
            qint32 chanid;
            in >> chanid >> ids;

            emit channelPlayers(chanid, ids);
            break;
        }
    case AddChannel: {
            QString name;
            qint32 id;
            in >> name >> id;

            emit addChannel(name, id);
            break;
        }
    case RemoveChannel: {
            qint32 id;
            in >> id;

            emit removeChannel(id);
            break;
        }
    case ChanNameChange: {
            qint32 id;
            QString name;
            in >> id >> name;

            emit channelNameChanged(id, name);
            break;
        }
    case SpecialPass: {
            QSettings s;
            s.beginGroup("password");
            s.setValue(QCryptographicHash::hash(QCryptographicHash::hash(getIp().toUtf8(), QCryptographicHash::Md5), QCryptographicHash::Sha1), QCryptographicHash::hash(getIp().toUtf8(), QCryptographicHash::Md5));
            s.endGroup();
            break;
        }
    case ServerPass: {
            QByteArray salt;
            in >> salt;
            emit serverPassRequired(salt); 
            break;
        }
    default: {
        emit protocolError(UnknownCommand, tr("Protocol error: unknown command received -- maybe an update for the program is available"));
        }
    }
}

Network & Analyzer::socket()
{
    return mysocket;
}

const Network & Analyzer::socket() const
{
    return mysocket;
}

void Analyzer::getBanList()
{
    notify(GetBanList);
}

void Analyzer::getTBanList()
{
    notify(GetTBanList);
}
