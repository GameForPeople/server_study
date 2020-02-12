#pragma once

#pragma region [macro & Attributes & Using section]

// macro
#define DISABLED_COPY(type) public: type(const type&) = delete; type& operator=(const type&) = delete;
#define DISABLED_MOVE(type) public: type(type&&) = delete; type& operator=(type&&) = delete;

// Attributes
#define	_NORETURN		[[noreturn]]
#define	_DEPRECATED		[[deprecated]]
#define	_MAYBE_UNUSED	[[maybe_unused]]
#define	_FALLTHROUGH	[[fallthrough]]
#define	_NODISCARD		[[nodiscard]]

namespace USING
{
	using _Key = short;
	using _Pos = short;
}; using namespace USING;

enum class DIRECTION : char
{
	LEFT = 0,
	RIGHT = 1
};
#pragma endregion

#pragma region [const]
constexpr unsigned short SERVER_LISTEN_PORT_NUMBER = 9000;
constexpr _Pos MAX_MAP_SIZE = 15;
constexpr _Key MAX_USER = 10000;
#pragma endregion

#pragma region [protocol]

namespace PACKET_TYPE
{
	enum class SERVER_TO_CLIENT : char
	{
		ADD_OBJECT,
		MOVE_OBJECT,
		DELETE_OBJECT
	};

	enum class CLIENT_TO_SERVER : char
	{
		MOVE_OBJECT
	};
}


#pragma pack(push, 1)

namespace PACKET_DATA
{
	struct BasePacket
	{
		unsigned char size;
		char packetType;
	};

	namespace SERVER_TO_CLIENT
	{
		struct ADD_OBJECT
			: public BasePacket
		{
			_Key key;
			_Pos xPos;
			_Pos yPos;
		};

		struct MOVE_OBJECT
			: public BasePacket
		{
			_Key key;
			_Pos xPos;
			_Pos yPos;
		};

		struct DELETE_OBJECT
			: public BasePacket
		{
			_Key key;
		};
	}

	namespace CLIENT_TO_SERVER
	{
		struct MOVE_OBJECT
			: public BasePacket
		{
			DIRECTION dir;
		};
	}
}

#pragma pack(pop)
#pragma endregion
