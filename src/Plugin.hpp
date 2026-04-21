// This file is included separately for each engine version
#include <cmath>

namespace Gothic_II_Addon 
{
	const char* INI_SECTION = "ZSIMPLE_ACCELERATION";

	// Настройки (тип / дефолт 1.2)
	float speedMultiplier = 1.2f;
	int speedHotkey = KEY_U;
	
	// Состояние
	static bool isFastMode = false;
	static int messageTimer = 0;
	static char currentMessage[128] = "";

	// -------------------------------------------------------
	// Таблица: имя клавиши (как в INI) -> DirectInput скан-код
	// -------------------------------------------------------
	struct KeyEntry { const char* name; int code; };
	static const KeyEntry KEY_TABLE[] = {
		{ "KEY_ESCAPE",     0x01 }, { "KEY_1",          0x02 },
		{ "KEY_2",          0x03 }, { "KEY_3",          0x04 },
		{ "KEY_4",          0x05 }, { "KEY_5",          0x06 },
		{ "KEY_6",          0x07 }, { "KEY_7",          0x08 },
		{ "KEY_8",          0x09 }, { "KEY_9",          0x0A },
		{ "KEY_0",          0x0B }, { "KEY_MINUS",      0x0C },
		{ "KEY_EQUALS",     0x0D }, { "KEY_BACK",       0x0E },
		{ "KEY_TAB",        0x0F }, { "KEY_Q",          0x10 },
		{ "KEY_W",          0x11 }, { "KEY_E",          0x12 },
		{ "KEY_R",          0x13 }, { "KEY_T",          0x14 },
		{ "KEY_Y",          0x15 }, { "KEY_U",          0x16 },
		{ "KEY_I",          0x17 }, { "KEY_O",          0x18 },
		{ "KEY_P",          0x19 }, { "KEY_LBRACKET",   0x1A },
		{ "KEY_RBRACKET",   0x1B }, { "KEY_RETURN",     0x1C },
		{ "KEY_LCONTROL",   0x1D }, { "KEY_A",          0x1E },
		{ "KEY_S",          0x1F }, { "KEY_D",          0x20 },
		{ "KEY_F",          0x21 }, { "KEY_G",          0x22 },
		{ "KEY_H",          0x23 }, { "KEY_J",          0x24 },
		{ "KEY_K",          0x25 }, { "KEY_L",          0x26 },
		{ "KEY_SEMICOLON",  0x27 }, { "KEY_APOSTROPHE", 0x28 },
		{ "KEY_GRAVE",      0x29 }, { "KEY_LSHIFT",     0x2A },
		{ "KEY_BACKSLASH",  0x2B }, { "KEY_Z",          0x2C },
		{ "KEY_X",          0x2D }, { "KEY_C",          0x2E },
		{ "KEY_V",          0x2F }, { "KEY_B",          0x30 },
		{ "KEY_N",          0x31 }, { "KEY_M",          0x32 },
		{ "KEY_COMMA",      0x33 }, { "KEY_PERIOD",     0x34 },
		{ "KEY_SLASH",      0x35 }, { "KEY_RSHIFT",     0x36 },
		{ "KEY_MULTIPLY",   0x37 }, { "KEY_LMENU",      0x38 },
		{ "KEY_SPACE",      0x39 }, { "KEY_CAPITAL",    0x3A },
		{ "KEY_F1",         0x3B }, { "KEY_F2",         0x3C },
		{ "KEY_F3",         0x3D }, { "KEY_F4",         0x3E },
		{ "KEY_F5",         0x3F }, { "KEY_F6",         0x40 },
		{ "KEY_F7",         0x41 }, { "KEY_F8",         0x42 },
		{ "KEY_F9",         0x43 }, { "KEY_F10",        0x44 },
		{ "KEY_NUMLOCK",    0x45 }, { "KEY_SCROLL",     0x46 },
		{ "KEY_NUMPAD7",    0x47 }, { "KEY_NUMPAD8",    0x48 },
		{ "KEY_NUMPAD9",    0x49 }, { "KEY_SUBTRACT",   0x4A },
		{ "KEY_NUMPAD4",    0x4B }, { "KEY_NUMPAD5",    0x4C },
		{ "KEY_NUMPAD6",    0x4D }, { "KEY_ADD",        0x4E },
		{ "KEY_NUMPAD1",    0x4F }, { "KEY_NUMPAD2",    0x50 },
		{ "KEY_NUMPAD3",    0x51 }, { "KEY_NUMPAD0",    0x52 },
		{ "KEY_DECIMAL",    0x53 }, { "KEY_F11",        0x57 },
		{ "KEY_F12",        0x58 }, { "KEY_NUMPADENTER",0x9C },
		{ "KEY_RCONTROL",   0x9D }, { "KEY_DIVIDE",     0xB5 },
		{ "KEY_RMENU",      0xB8 }, { "KEY_HOME",       0xC7 },
		{ "KEY_UP",         0xC8 }, { "KEY_PRIOR",      0xC9 },
		{ "KEY_LEFT",       0xCB }, { "KEY_RIGHT",      0xCD },
		{ "KEY_END",        0xCF }, { "KEY_DOWN",       0xD0 },
		{ "KEY_NEXT",       0xD1 }, { "KEY_INSERT",     0xD2 },
		{ "KEY_DELETE",     0xD3 },
		// └ышрё√
		{ "KEY_BACKSPACE",  0x0E }, { "KEY_ENTER",      0x1C },
		{ "KEY_LALT",       0x38 }, { "KEY_RALT",       0xB8 },
		{ "KEY_CAPSLOCK",   0x3A }, { "KEY_PGUP",       0xC9 },
		{ "KEY_PGDN",       0xD1 }, { "KEY_UPARROW",    0xC8 },
		{ "KEY_DOWNARROW",  0xD0 }, { "KEY_LEFTARROW",  0xCB },
		{ "KEY_RIGHTARROW", 0xCD },
	};
	static const int KEY_TABLE_SIZE = sizeof(KEY_TABLE) / sizeof(KEY_TABLE[0]);

	// Конвертирует имя типа "KEY_U" в скан-код. Возвращает 0 если не найдено.
	int KeyNameToScanCode(zSTRING const& name)
	{
		// Приводим к верхнему регистру для надежности
		zSTRING upper = name;
		upper.Upper();
		for (int i = 0; i < KEY_TABLE_SIZE; i++) {
			if (upper == KEY_TABLE[i].name) {
				return KEY_TABLE[i].code;
			}
		}
		// Если пользователь написал число напрямую (например "22")
		int direct = upper.ToInt();
		return direct;
	}

	void Game_Init()
	{
		// 1. Записать дефолт в конфиг (если нет записи)
		if (!zoptions->EntryExists(INI_SECTION, "Hotkey")) {
			zoptions->WriteString(INI_SECTION, "Hotkey", "KEY_U", FALSE);
		}
		if (!zoptions->EntryExists(INI_SECTION, "Multiplier")) {
			zoptions->WriteReal(INI_SECTION, "Multiplier", 1.2f, FALSE);
		}

		// 2. Читаем множитель скорости
		speedMultiplier = zoptions->ReadReal(INI_SECTION, "Multiplier", 1.2f);

		// 3. Читаем хоткей по имени: "KEY_U", "KEY_F", "KEY_F5" и т.д.
		zSTRING keyName = zoptions->ReadString(INI_SECTION, "Hotkey", "KEY_U");
		speedHotkey = KeyNameToScanCode(keyName);
		if (speedHotkey <= 0) {
			speedHotkey = KEY_U; // дефолт если имя не найдено
		}
	}
void Game_Loop()
	{
		if (!oCNpc::player) return;

		if (zinput->KeyToggled(speedHotkey)) {
			isFastMode = !isFastMode;

			zCModel* model = oCNpc::player->GetModel();
			if (model) {
				model->timeScale = isFastMode ? speedMultiplier : 1.0f;
			}

			const char* msg = isFastMode ? "Ускорение: ВКЛ" : "Ускорение: ВЫКЛ";
			strncpy(currentMessage, msg, sizeof(currentMessage) - 1);
			currentMessage[sizeof(currentMessage) - 1] = '\0';
			messageTimer = 180; 
		}

		if (messageTimer > 0) {
			messageTimer--;
			screen->PrintCX(7000, zSTRING(currentMessage));
		}
	}

	// ==========================================================
	// ХУК: Основной цикл мира (MainWorld_Render)
	// ==========================================================
	void __fastcall Hook_oCGame_MainWorld_Render(Union::Registers& reg);
	auto Partial_zCWorld_Render = Union::CreatePartialHook(reinterpret_cast<void*>(zSwitch(0x0063DC76, 0x0066498B, 0x0066BA76, 0x006C87EB)), &Hook_oCGame_MainWorld_Render);
	void __fastcall Hook_oCGame_MainWorld_Render(Union::Registers& reg)
	{
		Game_Loop();
	}

	// ==========================================================
	// ХУК: Инициализация игры
	// ==========================================================
	void __fastcall Hook_oCGame_Init(oCGame* self, void* vtable);
	auto Ivk_oCGame_Init = Union::CreateHook(reinterpret_cast<void*>(zSwitch(0x00636F50, 0x0065D480, 0x006646D0, 0x006C1060)), &Hook_oCGame_Init, Union::HookType::Hook_Detours);
	void __fastcall Hook_oCGame_Init(oCGame* self, void* vtable)
	{
		Ivk_oCGame_Init(self, vtable);
		Game_Init();
	}

	// Заглушки Union
	void Game_EntryPoint() {}
	void Game_Exit() {}
	void Game_PreLoop() {}
	void Game_PostLoop() {}
	void Game_MenuLoop() {}
	void Game_SaveBegin() {}
	void Game_SaveEnd() {}
	void LoadBegin() {}
	void LoadEnd() {}
	// Обнуляем состояние ускорения при загрузке (чтобы не было рассинхрона со скоростью модели)
	void ResetState() {
		isFastMode = false;
		messageTimer = 0;
	}

	void Game_LoadBegin_NewGame() { ResetState(); }
	void Game_LoadEnd_NewGame() {}
	void Game_LoadBegin_SaveGame() { ResetState(); }
	void Game_LoadEnd_SaveGame() {}
	void Game_LoadBegin_ChangeLevel() { ResetState(); }
	void Game_LoadEnd_ChangeLevel() {}
	void Game_LoadBegin_TriggerChangeLevel() {}
	void Game_LoadEnd_TriggerChangeLevel() {}
	void Game_Pause() {}
	void Game_Unpause() {}
	void Game_DefineExternals() {}
	void Game_ApplySettings() {}
}

