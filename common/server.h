#ifndef SERVER_H
#define SERVER_H

#include "pb/commands.pb.h"
#include "pb/serverinfo_ban.pb.h"
#include "pb/serverinfo_chat_message.pb.h"
#include "pb/serverinfo_user.pb.h"
#include "pb/serverinfo_warning.pb.h"
#include "server_player_reference.h"

#include <QMap>
#include <QMultiMap>
#include <QMutex>
#include <QObject>
#include <QReadWriteLock>
#include <QStringList>

class Server_DatabaseInterface;
class Server_Game;
class Server_Room;
class Server_ProtocolHandler;
class Server_AbstractUserInterface;
class GameReplay;
class IslMessage;
class SessionEvent;
class RoomEvent;
class DeckList;
class ServerInfo_Game;
class ServerInfo_Room;
class Response;
class GameEventContainer;
class CommandContainer;
class Command_JoinGame;

enum AuthenticationResult
{
    NotLoggedIn,
    PasswordRight,
    UnknownUser,
    WouldOverwriteOldSession,
    UserIsBanned,
    UsernameInvalid,
    RegistrationRequired,
    UserIsInactive,
    ClientIdRequired
};

class Server : public QObject
{
    Q_OBJECT
signals:
    void pingClockTimeout();
    void sigSendIslMessage(const IslMessage &message, int serverId);
    void endSession(qint64 sessionId);
private slots:
    void broadcastRoomUpdate(const ServerInfo_Room &roomInfo, bool sendToIsl = false);

public:
    mutable QReadWriteLock clientsLock, roomsLock; // locking order: roomsLock before clientsLock
    explicit Server(QObject *parent = nullptr);
    ~Server() override = default;
    AuthenticationResult loginUser(Server_ProtocolHandler *session,
                                   QString &name,
                                   const QString &password,
                                   bool passwordNeedsHash,
                                   QString &reason,
                                   int &secondsLeft,
                                   QString &clientid,
                                   QString &clientVersion,
                                   QString &connectionType);

    const QMap<int, Server_Room *> &getRooms()
    {
        return rooms;
    }

    Server_AbstractUserInterface *findUser(const QString &userName) const;
    const QMap<QString, Server_ProtocolHandler *> &getUsers() const
    {
        return users;
    }
    const QMap<qint64, Server_ProtocolHandler *> &getUsersBySessionId() const
    {
        return usersBySessionId;
    }
    virtual QMap<QString, bool> getServerRequiredFeatureList() const
    {
        return QMap<QString, bool>();
    }
    void addClient(Server_ProtocolHandler *player);
    void removeClient(Server_ProtocolHandler *player);
    QList<QString> getOnlineModeratorList() const;
    virtual QString getLoginMessage() const
    {
        return QString();
    }
    virtual QString getRequiredFeatures() const
    {
        return QString();
    }
    virtual bool permitUnregisteredUsers() const
    {
        return true;
    }
    virtual bool getGameShouldPing() const
    {
        return false;
    }
    virtual bool getClientIDRequiredEnabled() const
    {
        return false;
    }
    virtual bool getRegOnlyServerEnabled() const
    {
        return false;
    }
    virtual bool getMaxUserLimitEnabled() const
    {
        return false;
    }
    virtual bool getEnableLogQuery() const
    {
        return false;
    }
    virtual bool getStoreReplaysEnabled() const
    {
        return true;
    }
    virtual int getIdleClientTimeout() const
    {
        return 0;
    }
    virtual int getClientKeepAlive() const
    {
        return 0;
    }
    virtual int getMaxGameInactivityTime() const
    {
        return 9999999;
    }
    virtual int getMaxPlayerInactivityTime() const
    {
        return 9999999;
    }
    virtual int getMessageCountingInterval() const
    {
        return 0;
    }
    virtual int getMaxMessageCountPerInterval() const
    {
        return 0;
    }
    virtual int getMaxMessageSizePerInterval() const
    {
        return 0;
    }
    virtual int getMaxGamesPerUser() const
    {
        return -1;
    }
    virtual int getCommandCountingInterval() const
    {
        return 0;
    }
    virtual int getMaxCommandCountPerInterval() const
    {
        return 0;
    }
    virtual int getMaxUserTotal() const
    {
        return 9999999;
    }
    virtual int getServerID() const
    {
        return 0;
    }
    virtual bool permitCreateGameAsJudge() const
    {
        return false;
    }

    Server_DatabaseInterface *getDatabaseInterface() const;
    int getNextLocalGameId()
    {
        QMutexLocker locker(&nextLocalGameIdMutex);
        return ++nextLocalGameId;
    }

    void sendIsl_Response(const Response &item, int serverId = -1, qint64 sessionId = -1);
    void sendIsl_SessionEvent(const SessionEvent &item, int serverId = -1, qint64 sessionId = -1);
    void sendIsl_GameEventContainer(const GameEventContainer &item, int serverId = -1, qint64 sessionId = -1);
    void sendIsl_RoomEvent(const RoomEvent &item, int serverId = -1, qint64 sessionId = -1);
    void sendIsl_GameCommand(const CommandContainer &item, int serverId, qint64 sessionId, int roomId, int playerId);
    void sendIsl_RoomCommand(const CommandContainer &item, int serverId, qint64 sessionId, int roomId);

    const QMap<QString, Server_AbstractUserInterface *> &getExternalUsers() const
    {
        return externalUsers;
    }

    void addPersistentPlayer(const QString &userName, int roomId, int gameId, int playerId);
    void removePersistentPlayer(const QString &userName, int roomId, int gameId, int playerId);
    QList<PlayerReference> getPersistentPlayerReferences(const QString &userName) const;
    int getUsersCount() const;
    int getGamesCount() const;
    int getTCPUserCount() const
    {
        return tcpUserCount;
    }
    int getWebSocketUserCount() const
    {
        return webSocketUserCount;
    }

private:
    QMultiMap<QString, PlayerReference> persistentPlayers;
    mutable QReadWriteLock persistentPlayersLock;
    int nextLocalGameId, tcpUserCount, webSocketUserCount;
    QMutex nextLocalGameIdMutex;

protected slots:
    void externalUserJoined(const ServerInfo_User &userInfo);
    void externalUserLeft(const QString &userName);
    void externalRoomUserJoined(int roomId, const ServerInfo_User &userInfo);
    void externalRoomUserLeft(int roomId, const QString &userName);
    void externalRoomSay(int roomId, const QString &userName, const QString &message);
    void externalRoomRemoveMessages(int roomId, const QString &userName, int amount);
    void externalRoomGameListChanged(int roomId, const ServerInfo_Game &gameInfo);
    void
    externalJoinGameCommandReceived(const Command_JoinGame &cmd, int cmdId, int roomId, int serverId, qint64 sessionId);
    void
    externalGameCommandContainerReceived(const CommandContainer &cont, int playerId, int serverId, qint64 sessionId);
    void externalGameEventContainerReceived(const GameEventContainer &cont, qint64 sessionId);
    void externalResponseReceived(const Response &resp, qint64 sessionId);

    virtual void doSendIslMessage(const IslMessage & /* msg */, int /* serverId */)
    {
    }

protected:
    void prepareDestroy();
    void setDatabaseInterface(Server_DatabaseInterface *_databaseInterface);
    QList<Server_ProtocolHandler *> clients;
    QMap<qint64, Server_ProtocolHandler *> usersBySessionId;
    QMap<QString, Server_ProtocolHandler *> users;
    QMap<qint64, Server_AbstractUserInterface *> externalUsersBySessionId;
    QMap<QString, Server_AbstractUserInterface *> externalUsers;
    QMap<int, Server_Room *> rooms;
    QMap<QThread *, Server_DatabaseInterface *> databaseInterfaces;
    void addRoom(Server_Room *newRoom);
};

#endif
