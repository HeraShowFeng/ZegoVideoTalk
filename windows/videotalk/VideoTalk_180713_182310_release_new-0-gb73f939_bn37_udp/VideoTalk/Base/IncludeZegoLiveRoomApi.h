#ifndef IncludeZegoLiveRoomApi_h
#define IncludeZegoLiveRoomApi_h

#include <QDebug>

#if defined(Q_OS_WIN)

#include "LiveRoom.h"
#include "LiveRoom-Publisher.h"
#include "LiveRoom-Player.h"
#include "LiveRoomDefines.h"
#include "RoomDefines.h"
#include "AVDefines.h"

extern ZEGOAVKIT_API void ZegoExternalLogWithNotice(const char* content);

inline void log_string_notice(const char* content) { ZegoExternalLogWithNotice(content); qDebug() << content; }

#define qtoc(content) content.toStdString().c_str()

#elif defined(Q_OS_MAC)

#include "LiveRoom.h"
#include "LiveRoom-IM.h"
#include "LiveRoom-Player.h"
#include "LiveRoom-Publisher.h"

#else
static_assert(false, "Unkown Platform");
#endif


#endif /* IncludeZegoLiveRoomApi_h */