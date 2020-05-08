#pragma once

#include "../../../Memento.h"
#include "../../../../../Share/LogicObject/Room/Option.h"

namespace Application {		namespace Packets {
struct CPacket;
} /* Packets */				} /* Application */

namespace GameServer {		namespace Player {			namespace State {
namespace Buff {			namespace BuffRoomProcess {

class OnChangeOption
{
public:
	typedef					::Application::Packets::CPacket
							PACKET;

public:
	static void				Execute( Memento * i_pMemento, PACKET * i_pPacket );

private:
	typedef					::LogicObject::Room::Option::E_MODE
							E_MODE;

private:
	static CBOOL			checkTeamCount( Memento * i_pMemento, CINT i_nCount );	//队伍记数
	static CBOOL			checkMode( E_MODE::ENUM i_eMode );	//检查状态

};

} /* BuffRoomProcess */		} /* Buff */
} /* State */				} /* Player */				} /* GameServer */