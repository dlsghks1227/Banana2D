#pragma once

// Singleton Pattern
// http://bitboom.github.io/singleton-with-cpp11
class GameManage final
{
public:
	~GameManage() = default;

	// �̵� �� ���� ������
	// https://link2me.tistory.com/1756

	// �̵� ������ �� �̵� ���� ������ ����
	GameManage(GameManage&&) = delete;
	GameManage& operator = (GameManage&&) = delete;

	// ���� ������ �� ���� �̵� ������ ����
	GameManage(GameManage const&) = delete;
	GameManage& operator = (GameManage const&) = delete;

	static GameManage& GetInstance();

private:
	GameManage() = default;

	static std::unique_ptr<GameManage>	instance;
	static std::once_flag				flag;
};