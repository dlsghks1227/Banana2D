#include "framework.h"
#include "GameManage.h"

std::unique_ptr<GameManage> GameManage::instance = nullptr;
std::once_flag GameManage::flag;

GameManage& GameManage::GetInstance()
{
	std::call_once(GameManage::flag, []() {
#ifdef _DEBUG
		std::wcout << L"Create GameManage instance\n";
#endif
		GameManage::instance.reset(new GameManage);
		});

#ifdef _DEBUG
	std::wcout << L"Get GameManage instance\n";
#endif

	return (*GameManage::instance);
}