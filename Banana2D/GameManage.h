#pragma once

// Singleton Pattern
// http://bitboom.github.io/singleton-with-cpp11
class GameManage final
{
public:
	~GameManage() = default;

	// 이동 및 복사 생성자
	// https://link2me.tistory.com/1756

	// 이동 생성자 및 이동 복사 연산자 제거
	GameManage(GameManage&&) = delete;
	GameManage& operator = (GameManage&&) = delete;

	// 복사 생성자 및 복사 이동 연산자 제거
	GameManage(GameManage const&) = delete;
	GameManage& operator = (GameManage const&) = delete;

	static GameManage& GetInstance();

private:
	GameManage() = default;

	static std::unique_ptr<GameManage>	instance;
	static std::once_flag				flag;
};