#pragma once
#ifndef __GAME_CONTROLLER_H__
#define __GAME_CONTROLLER_H__

#include "cocos2d.h"
#include "models/CardModel.h"
#include <vector>

namespace Solitaire {

	class GameController : public cocos2d::Ref {
	public:
		static GameController* create();
		bool init();

		// 核心功能：初始化一副新牌
		void startNewGame();

		// 获取当前所有的牌数据
		const cocos2d::Vector<CardModel*>& getAllCards() const { return _cards; }

	private:
		// 用 Cocos 的 Vector 来存牌，会自动管理内存引用计数
		cocos2d::Vector<CardModel*> _cards;
	};
}
#endif