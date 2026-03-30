#include "controllers/GameController.h"
#include <algorithm> // 用于 std::shuffle
#include <random>    // 用于随机数生成器

namespace Solitaire {

	GameController* GameController::create() {
		GameController* ret = new (std::nothrow) GameController();
		if (ret && ret->init()) {
			ret->autorelease();
			return ret;
		}
		CC_SAFE_DELETE(ret);
		return nullptr;
	}

	bool GameController::init() {
		return true;
	}

	void GameController::startNewGame() {
		_cards.clear();

		// 1. 生成 52 张牌
		// 4种花色 (0-3)
		for (int suit = 0; suit <= 3; ++suit) {
			// 13个点数 (1-13)
			for (int face = 1; face <= 13; ++face) {
				auto card = CardModel::create(
					suit * 13 + face,       // 简单的唯一ID算法
					(CardFace)face,
					(CardSuit)suit
				);
				_cards.pushBack(card);
			}
		}

		// 2. 洗牌 (使用 C++ 标准库的洗牌算法)
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(_cards.begin(), _cards.end(), g);
	}
}