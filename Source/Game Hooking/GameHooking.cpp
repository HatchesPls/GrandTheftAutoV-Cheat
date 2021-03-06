#include "../Header/Cheat Functions/FiberMain.h"
#include "../../ThirdParty/kiero/kiero.h"

HANDLE MainFiber;
IsDLCPresent														GameHooking::is_dlc_present;
GetEventData														GameHooking::get_event_data;
GetLabelText														GameHooking::get_label_text;
GetScriptHandlerIfNetworked											GameHooking::get_script_handler_if_networked;
GetScriptHandler													GameHooking::get_script_handler;
GetPlayerAddress													GameHooking::get_player_address;
GetChatData														    GameHooking::get_chat_data;
static eGameState* 													m_gameState;
static uint64_t														m_worldPtr;
static GameHooking::NativeRegistrationNew**							m_registrationTable;
static std::unordered_map<uint64_t, GameHooking::NativeHandler>		m_handlerCache;
static __int64**													m_globalPtr;
std::vector<LPVOID> HookedFunctions;
ScriptThread* (*GetActiveThread)()									= nullptr;
const int EventCountInteger											= 85;
static char EventRestore[EventCountInteger]							= {};
static std::vector<void*> EventPtr;
uint64_t CMetaData::m_begin											= 0;
uint64_t CMetaData::m_end											= 0;
DWORD CMetaData::m_size												= 0;


IsDLCPresent IsDLCPresentOriginal = nullptr;
bool IsDLCPresentHooked(std::uint32_t DLCHash)
{
	static uint64_t	Last = 0;
	uint64_t Current = MISC::GET_FRAME_COUNT();
	if (Last != Current)
	{
		Last = Current;
		GameHooking::OnTickInit();
	}
	if (DLCHash == 2532323046 && Cheat::CheatFeatures::GTAODeveloperMode) { return true; }
	return IsDLCPresentOriginal(DLCHash);
}

GetScriptHandlerIfNetworked GetScriptHandlerIfNetworkedOriginal = nullptr;
void* GetScriptHandlerIfNetworkedHooked()
{
	return GameHooking::get_script_handler();
}

GetLabelText GetLabelTextOriginal = nullptr;
std::string Cheat::GameFunctions::InGameHelpTextMessage;
const char* GetLabelTextHooked(void* this_, const char* label)
{
	if (std::strcmp(label, "HUD_MPREENTER") == 0)	{ return "Joining a New GTA Online Session with GTAV Cheat"; }
	if (std::strcmp(label, "HUD_JOINING") == 0)		{ return "Loading GTA Online with GTAV Cheat"; }
	if (std::strcmp(label, "HUD_QUITTING") == 0)	{ return "Leaving GTA Online with GTAV Cheat"; }
	if (std::strcmp(label, "PM_QUIT_MP") == 0)		{ return "Leave GTA Online with GTAV Cheat"; }
	if (std::strcmp(label, "PM_ENTER_MP") == 0)		{ return "Join GTA Online with GTAV Cheat"; }
	if (std::strcmp(label, "PM_EXIT_GAME") == 0)	{ return "Exit Game with GTAV Cheat"; }
	if (std::strcmp(label, "PM_GO") == 0)			{ return "Go Online with GTAV Cheat"; }
	if (std::strcmp(label, "PM_FRIEND_FM") == 0)	{ return "Join Friend with GTAV Cheat"; }
	if (std::strcmp(label, "PM_FIND_SESS") == 0)	{ return "Find New Session with GTAV Cheat"; }
	if (!Cheat::GameFunctions::InGameKeyboardWindowTitle.empty())
	{
		if (std::strcmp(label, "FMMC_KEY_TIP8") == 0) { return Cheat::CheatFunctions::StringToChar(Cheat::GameFunctions::InGameKeyboardWindowTitle); }
	}
	if (!Cheat::GameFunctions::InGameHelpTextMessage.empty())
	{
		if (std::strcmp(label, "LETTERS_HELP2") == 0) { return Cheat::CheatFunctions::StringToChar(Cheat::GameFunctions::InGameHelpTextMessage); }
	}
	return GetLabelTextOriginal(this_, label);
}

const std::vector <int> MaliciousScriptsArray = { TSE_KICK_TO_SP, TSE_CEO_KICK, TSE_CEO_BAN, TSE_PROPERTY_TELEPORT, TSE_CAYO_PERICO_TELEPORT,
												  TSE_FORCE_INTO_MISSION };
void* GetEventDataOriginal = nullptr;
bool GetEventDataHooked(int eventGroup, int eventIndex, int* argStruct, int argStructSize)
{
	auto result = static_cast<decltype(&GetEventDataHooked)>(GetEventDataOriginal)(eventGroup, eventIndex, argStruct, argStructSize);
	if (result)
	{
		bool IsBlackListedEvent = std::find(std::begin(MaliciousScriptsArray), std::end(MaliciousScriptsArray), argStruct[0]) != std::end(MaliciousScriptsArray);
		if (Cheat::CheatFeatures::BlockAllScriptEvents)
		{
			return false;
		}
		else if (Cheat::CheatFeatures::BlockMaliciousScriptEvents && IsBlackListedEvent)
		{
			std::string PlayerName = PLAYER::GET_PLAYER_NAME(argStruct[1]);
			std::string MessageString = "Script Event Blocked~n~ID: " + std::to_string(argStruct[0]);
			if (PlayerName != "**Invalid**")
			{
				MessageString.append(" ~n~Sender: " + (std::string)PLAYER::GET_PLAYER_NAME(argStruct[1]));
			}
						
			Cheat::GameFunctions::AdvancedMinimapNotification(MessageString.data(), "Textures", "AdvancedNotificationImage", false, 4, "Protection", "", 0.8f, "");
			return false;
		}
	}
	return result;
}

GetChatData GetChatDataOriginal = nullptr;
__int64 GetChatDataHooked(__int64 a1, __int64 a2, __int64 a3, const char* origText, BOOL isTeam)
{
	if (Cheat::CheatFeatures::LogChatMessages)
	{
		Cheat::CheatFunctions::WriteToFile(Cheat::CheatFunctions::ReturnChatLogFilePath(), Cheat::CheatFunctions::ReturnDateTimeFormatAsString("[%H:%M:%S] Message: ") + (std::string)origText + "\n", true);
		Cheat::Logger::SendMessageToGameChatLogWindow("Message: " + (std::string)origText);
	}
	return GetChatDataOriginal(a1, a2, a3, origText, isTeam);
}

void ScriptFunction(LPVOID lpParameter)
{
	srand(GetTickCount64());
	Cheat::FiberMain();
}

DWORD WakeTime;
void GameHooking::OnTickInit()
{
	static HANDLE scriptFiber;
	if (MainFiber == nullptr) { MainFiber = ConvertThreadToFiber(nullptr); }
	if (timeGetTime() < WakeTime) { return; }
	if (scriptFiber) { SwitchToFiber(scriptFiber); } else  { scriptFiber = CreateFiber(NULL, ScriptFunction, nullptr); }
}

void GameHooking::PauseMainFiber(DWORD ms, bool ShowMessage)
{
	if (ShowMessage) { Cheat::GUI::DrawTextInGame("One Moment Please", { 255, 255, 255, 255 }, { 0.525f, 0.400f }, { 1.5f, 1.5f }, true, true); }
	WakeTime = timeGetTime() + ms;
	SwitchToFiber(MainFiber);
}

uint64_t CMetaData::begin() { return m_begin; }
uint64_t CMetaData::end()	{ return m_end; }
DWORD CMetaData::size()		{ return m_size; }
void CMetaData::init()
{
	if (m_begin && m_size) { return; }
	m_begin = (uint64_t)GetModuleHandleA(nullptr);
	const IMAGE_DOS_HEADER* headerDos = (const IMAGE_DOS_HEADER*)m_begin;
	const IMAGE_NT_HEADERS* headerNt = (const IMAGE_NT_HEADERS64*)((const BYTE*)headerDos + headerDos->e_lfanew);
	m_size = headerNt->OptionalHeader.SizeOfCode;
	m_end = m_begin + m_size;
	return;
}

//CPatternResult
CPatternResult::CPatternResult(void* pVoid) :
	m_pVoid(pVoid)
{}
CPatternResult::CPatternResult(void* pVoid, void* pBegin, void* pEnd) :
	m_pVoid(pVoid),
	m_pBegin(pBegin),
	m_pEnd(pEnd)
{}
CPatternResult::~CPatternResult() {}


//CPattern Public
CPattern::CPattern(char* szByte, char* szMask) :
	m_szByte(szByte),
	m_szMask(szMask),
	m_bSet(false)
{}
CPattern::~CPattern() {}

CPattern& CPattern::find(int i, uint64_t startAddress)
{
	match(i, startAddress, false);
	if (m_result.size() <= i)
		m_result.push_back(nullptr);
	return *this;
}

CPattern& CPattern::virtual_find(int i, uint64_t startAddress)
{
	match(i, startAddress, true);
	if (m_result.size() <= i)
		m_result.push_back(nullptr);
	return *this;
}

CPatternResult CPattern::get(int i)
{
	if (m_result.size() > i)
		return m_result[i];
	return nullptr;
}

//CPattern Private
bool CPattern::match(int i, uint64_t startAddress, bool virt)
{
	if (m_bSet)
		return false;
	uint64_t	begin = 0;
	uint64_t	end = 0;
	if (!virt)
	{
		CMetaData::init();
		begin = CMetaData::begin() + startAddress;
		end = CMetaData::end();
		if (begin >= end)
			return false;
	}
	int		j = 0;
	do
	{
		if (virt)
			begin = virtual_find_pattern(startAddress, (BYTE*)m_szByte, m_szMask) + 1;
		else
			begin = find_pattern(begin, end, (BYTE*)m_szByte, m_szMask) + 1;
		if (begin == NULL)
			break;
		j++;
	} while (j < i);

	m_bSet = true;
	return true;
}

bool CPattern::byte_compare(const BYTE* pData, const BYTE* btMask, const char* szMask)
{
	for (; *szMask; ++szMask, ++pData, ++btMask)
		if (*szMask == 'x' && *pData != *btMask)
			break;
	if ((*szMask) != 0)
		return false;
	return true;
}


uint64_t CPattern::find_pattern(uint64_t address, uint64_t end, BYTE *btMask, char *szMask)
{
	size_t len = strlen(szMask) + 1;
	for (uint64_t i = 0; i < (end - address - len); i++)
	{
		BYTE*	ptr = (BYTE*)(address + i);
		if (byte_compare(ptr, btMask, szMask))
		{
			m_result.push_back(CPatternResult((void*)(address + i)));
			return address + i;
		}
	}
	return NULL;
}

uint64_t CPattern::virtual_find_pattern(uint64_t address, BYTE *btMask, char *szMask)
{
	MEMORY_BASIC_INFORMATION mbi;
	char*	pStart = nullptr;
	char*	pEnd = nullptr;
	char*	res = nullptr;
	size_t	maskLen = strlen(szMask);

	while (res == nullptr && sizeof(mbi) == VirtualQuery(pEnd, &mbi, sizeof(mbi)))
	{
		pStart = pEnd;
		pEnd += mbi.RegionSize;
		if (mbi.Protect != PAGE_READWRITE || mbi.State != MEM_COMMIT)
			continue;

		for (int i = 0; pStart < pEnd - maskLen && res == nullptr; ++pStart)
		{
			if (byte_compare((BYTE*)pStart, btMask, szMask))
			{
				m_result.push_back(CPatternResult((void*)pStart, mbi.BaseAddress, pEnd));
				res = pStart;
			}
		}

		mbi = {};
	}
	return (uint64_t)res;
}


template <typename T>
void setPat(std::string	name, char*	pat, char* mask, T** out, bool rel, int offset = 0, int deref = 0, int skip = 0)
{
	T*	ptr = nullptr;

	CPattern pattern(pat, mask);
	pattern.find(1 + skip);
	if (rel)
		ptr = pattern.get(skip).get_rel<T>(offset);
	else
		ptr = pattern.get(skip).get<T>(offset);

	while (true)
	{
		if (ptr == nullptr)
		{
			Cheat::Logger::Error("Failed to find '" + name + "' pattern", false);
			std::exit(EXIT_SUCCESS);
		}

		if (deref <= 0)
		{
			break;
		}
		ptr = *(T**)ptr;
		--deref;
	}
	
	*out = ptr;
	return;
}

template <typename T>
void setFn(std::string name, char* pat, char* mask, T* out, int skip = 0)
{
	char* ptr = nullptr;

	CPattern pattern(pat, mask);
	pattern.find(1 + skip);
	ptr = pattern.get(skip).get<char>(0);

	if (ptr == nullptr)
	{
		Cheat::Logger::Error(Cheat::CheatFunctions::StringToChar("Failed to find '" + name + "' pattern"), true);
		std::exit(EXIT_SUCCESS);
	}

	*out = (T)ptr;
	return;
}

void GameHooking::Initialize()
{
	GameHooking::get_label_text					 = static_cast<GetLabelText>(Memory::pattern("48 89 5C 24 ? 57 48 83 EC 20 48 8B DA 48 8B F9 48 85 D2 75 44 E8").count(1).get(0).get<void>(0));
	GameHooking::get_script_handler_if_networked = static_cast<GetScriptHandlerIfNetworked>(Memory::pattern("40 53 48 83 EC 20 E8 ? ? ? ? 48 8B D8 48 85 C0 74 12 48 8B 10 48 8B C8").count(1).get(0).get<void>(0));
	GameHooking::get_script_handler				 = static_cast<GetScriptHandler>(Memory::pattern("48 83 EC 28 E8 ? ? ? ? 33 C9 48 85 C0 74 0C E8 ? ? ? ? 48 8B 88 ? ? ? ?").count(1).get(0).get<void>(0));

	// Set Patterns
	setFn<IsDLCPresent>("is_dlc_present", "\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x20\x81\xF9\x00\x00\x00\x00", "xxxx?xxxxxxx????", &GameHooking::is_dlc_present);
	setFn<GetEventData>("get_event_data", "\x48\x89\x5C\x24\x00\x57\x48\x83\xEC\x20\x49\x8B\xF8\x4C\x8D\x05\x00\x00\x00\x00\x41\x8B\xD9\xE8\x00\x00\x00\x00\x48\x85\xC0\x74\x14\x4C\x8B\x10\x44\x8B\xC3\x48\x8B\xD7\x41\xC1\xE0\x03\x48\x8B\xC8\x41\xFF\x52\x30\x48\x8B\x5C\x24\x00", "xxxx?xxxxxxxxxxx????xxxx????xxxxxxxxxxxxxxxxxxxxxxxxxxxxx?", &GameHooking::get_event_data);
	setFn<GetPlayerAddress>("get_player_address", "\x40\x53\x48\x83\xEC\x20\x33\xDB\x38\x1D\x00\x00\x00\x00\x74\x1C", "xxxxxxxxxx????xx", &GameHooking::get_player_address);
	setFn<GetChatData>("get_chat_data", "\x4D\x85\xC9\x0F\x84\x00\x00\x00\x00\x48\x8B\xC4\x48\x89\x58\x08\x48\x89\x70\x10\x48\x89\x78\x18\x4C\x89\x48\x20\x55\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xA8", "xxxxx????xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx", &get_chat_data);

	char* c_location = nullptr;
	void* v_location = nullptr;

	// Load GameState
	Cheat::Logger::DebugMessage("Load 'GameState'");
	c_location = Memory::pattern("83 3D ? ? ? ? ? 75 17 8B 43 20 25").count(1).get(0).get<char>(2);
	c_location == nullptr ? Cheat::Logger::Error("Failed to load GameState", true) : m_gameState = reinterpret_cast<decltype(m_gameState)>(c_location + *(int32_t*)c_location + 5);
	
	// Load Vector3 Result Fix
	Cheat::Logger::DebugMessage("Load 'Vector3 Result Fix'");
	v_location = Memory::pattern("83 79 18 00 48 8B D1 74 4A FF 4A 18").count(1).get(0).get<void>(0);
	if (v_location != nullptr) scrNativeCallContext::SetVectorResults = (void(*)(scrNativeCallContext*))(v_location);

	// Load Native Registration Table
	Cheat::Logger::DebugMessage("Load 'Native Registration Table'");
	c_location = Memory::pattern("76 32 48 8B 53 40 48 8D 0D").count(1).get(0).get<char>(9);
	c_location == nullptr ? Cheat::Logger::Error("Failed to load Native Registration Table", true) : m_registrationTable = reinterpret_cast<decltype(m_registrationTable)>(c_location + *(int32_t*)c_location + 4);

	// Load Game World Pointer
	Cheat::Logger::DebugMessage("Load 'World Pointer'");
	c_location = Memory::pattern("48 8B 05 ? ? ? ? 45 ? ? ? ? 48 8B 48 08 48 85 C9 74 07").count(1).get(0).get<char>(0);
	c_location == nullptr ? Cheat::Logger::Error("Failed to load World Pointer", true) : m_worldPtr = reinterpret_cast<uint64_t>(c_location) + *reinterpret_cast<int*>(reinterpret_cast<uint64_t>(c_location) + 3) + 7;

	// Load Active Game Thread
	Cheat::Logger::DebugMessage("Load 'Active Game Thread'");
	c_location = Memory::pattern("E8 ? ? ? ? 48 8B 88 ? 01 00 00 ? 02").count(1).get(0).get<char>(1);
	c_location == nullptr ? Cheat::Logger::Error("Failed to load Active Game Thread", true) : GetActiveThread = reinterpret_cast<decltype(GetActiveThread)>(c_location + *(int32_t*)c_location + 4);

	// Get Global Pointer
	Cheat::Logger::DebugMessage("Load 'Global Pointer'");
	c_location = Memory::pattern("4C 8D 05 ? ? ? ? 4D 8B 08 4D 85 C9 74 11").count(1).get(0).get<char>(0);
	__int64 patternAddr = NULL;
	c_location == nullptr ? Cheat::Logger::Error("Failed to load Global Pointer", true) : patternAddr = reinterpret_cast<decltype(patternAddr)>(c_location);
	m_globalPtr = (__int64**)(patternAddr + *(int*)(patternAddr + 3) + 7);

	// Get Event Hook -> Used by defuseEvent
	Cheat::Logger::DebugMessage("Load 'Event Hook'");
	if ((c_location = Memory::pattern("48 83 EC 28 E8 ? ? ? ? 48 8B 0D ? ? ? ? 4C 8D 0D ? ? ? ? 4C 8D 05 ? ? ? ? BA 03").count(1).get(0).get<char>(0)))
	{
		int i = 0, j = 0, matches = 0, found = 0;
		char* pattern = "\x4C\x8D\x05";
		while (found != EventCountInteger)
		{
			if (c_location[i] == pattern[j])
			{
				if (++matches == 3)
				{
					EventPtr.push_back((void*)(reinterpret_cast<uint64_t>(c_location + i - j) + *reinterpret_cast<int*>(c_location + i + 1) + 7));
					found++;
					j = matches = 0;
				}
				j++;
			}
			else
			{
				matches = j = 0;
			}
			i++;
		}
	}

	// Initialize Natives
	Cheat::Logger::DebugMessage("Initialized game natives");
	CrossMapping::initNativeMap();

	bool WaitingGameLoadLogPrinted = false;
	while (*m_gameState != GameStatePlaying)
	{
		if (!WaitingGameLoadLogPrinted)
		{
			Cheat::Logger::Message("Waiting for the game to have finished loading");
			WaitingGameLoadLogPrinted = true;
		}
		Sleep(100);
	}
	if (WaitingGameLoadLogPrinted)
	{
		Cheat::Logger::Message("Game completed loading");
	}

	// Hook Game Functions
	Cheat::Logger::DebugMessage("Hook 'GET_EVENT_DATA'");
	auto status = MH_CreateHook(GameHooking::get_event_data, GetEventDataHooked, (void**)&GetEventDataOriginal);
	if ((status != MH_OK && status != MH_ERROR_ALREADY_CREATED) || MH_EnableHook(GameHooking::get_event_data) != MH_OK) { Cheat::Logger::Error("Failed to hook GET_EVENT_DATA", true);  std::exit(EXIT_FAILURE); }
	HookedFunctions.push_back(GameHooking::get_event_data);

	Cheat::Logger::DebugMessage("Hook 'GET_SCRIPT_HANDLER_IF_NETWORKED'");
	status = MH_CreateHook(GameHooking::get_script_handler_if_networked, GetScriptHandlerIfNetworkedHooked, (void**)&GetScriptHandlerIfNetworkedOriginal);
	if (status != MH_OK || MH_EnableHook(GameHooking::get_script_handler_if_networked) != MH_OK) { Cheat::Logger::Error("Failed to hook GET_SCRIPT_HANDLER_IF_NETWORKED", true);  std::exit(EXIT_FAILURE); }
	HookedFunctions.push_back(GameHooking::get_script_handler_if_networked);

	Cheat::Logger::DebugMessage("Hook 'GET_LABEL_TEXT'");
	status = MH_CreateHook(GameHooking::get_label_text, GetLabelTextHooked, (void**)&GetLabelTextOriginal);
	if (status != MH_OK || MH_EnableHook(GameHooking::get_label_text) != MH_OK) { Cheat::Logger::Error("Failed to hook GET_LABEL_TEXT", true);  std::exit(EXIT_FAILURE); }
	HookedFunctions.push_back(GameHooking::get_label_text);

	Cheat::Logger::DebugMessage("Hook 'GET_CHAT_DATA'");
	status = MH_CreateHook(GameHooking::get_chat_data, GetChatDataHooked, (void**)&GetChatDataOriginal);
	if (status != MH_OK || MH_EnableHook(GameHooking::get_chat_data) != MH_OK) { Cheat::Logger::Error("Failed to hook GET_CHAT_DATA", true);  std::exit(EXIT_FAILURE); }
	HookedFunctions.push_back(GameHooking::get_chat_data);

	Cheat::Logger::DebugMessage("Hook 'IS_DLC_PRESENT'");
	status = MH_CreateHook(GameHooking::is_dlc_present, IsDLCPresentHooked, (void**)&IsDLCPresentOriginal);
	if ((status != MH_OK && status != MH_ERROR_ALREADY_CREATED) || MH_EnableHook(GameHooking::is_dlc_present) != MH_OK) { Cheat::Logger::Error("Failed to hook IS_DLC_PRESENT", true);  std::exit(EXIT_FAILURE); }
	HookedFunctions.push_back(GameHooking::is_dlc_present);
}

static GameHooking::NativeHandler _Handler(uint64_t origHash)
{
	uint64_t newHash = CrossMapping::MapNative(origHash);
	if (newHash == 0)
	{
		return nullptr;
	}

	GameHooking::NativeRegistrationNew * table = m_registrationTable[newHash & 0xFF];

	for (; table; table = table->getNextRegistration())
	{
		for (uint32_t i = 0; i < table->getNumEntries(); i++)
		{
			if (newHash == table->getHash(i))
			{
				return table->handlers[i];
			}
		}
	}
	return nullptr;
}

GameHooking::NativeHandler GameHooking::GetNativeHandler(uint64_t origHash)
{
	auto& handler = m_handlerCache[origHash];
	if (handler == nullptr)
	{
		handler = _Handler(origHash);
	}
	return handler;
}

uint64_t GameHooking::getWorldPtr()
{
	return m_worldPtr;
}

void GameHooking::defuseEvent(GameEvents e, bool toggle)
{
	static const unsigned char retn = 0xC3;
	char* p = (char*)EventPtr[e];
	if (toggle)
	{
		if (EventRestore[e] == 0)
			EventRestore[e] = p[0];
		*p = retn;
	}
	else
	{
		if (EventRestore[e] != 0)
			*p = EventRestore[e];
	}
}

__int64** GameHooking::getGlobalPtr()
{
	return m_globalPtr;
}

DWORD WINAPI UnloadThread(LPVOID lpParam)
{
	// Stop threads
	Cheat::CheatFunctions::SendThreadTerminateSignal = true;

	// Stop DirectX hook - TODO; remove the hooks and unload ImGui
	kiero::shutdown();

	// Disable MinHook hooks
	MH_DisableHook(MH_ALL_HOOKS); // TODO; remove all hooks, don't just disable them
	MH_Uninitialize();

	// Unitializing Logger
	Cheat::Logger::Uninit();

	// Exit
	FreeLibraryAndExitThread(Cheat::CheatModuleHandle, EXIT_SUCCESS);
}

void GameHooking::Unload()
{
	Cheat::Logger::DebugMessage("Unloading");
	CreateThread(NULL, NULL, UnloadThread, NULL, NULL, NULL);
}