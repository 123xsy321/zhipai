#pragma once
#ifndef __GAME_TYPES_H__
#define __GAME_TYPES_H__

#include "cocos2d.h"

namespace Solitaire {

	// 1. 花色定义
	enum class CardSuit {
		None = -1,
		Clubs = 0,      // 梅花 (黑色)
		Diamonds,       // 方块 (红色)
		Hearts,         // 红桃 (红色)
		Spades          // 黑桃 (黑色)
	};

	// 2. 点数定义 (A=1 ... K=13)
	enum class CardFace {
		None = -1,
		Ace = 1, Two, Three, Four, Five, Six, Seven,
		Eight, Nine, Ten, Jack, Queen, King
	};

	// 3. 区域定义 (用于撤销功能记录位置)
	enum class CardZone {
		None,
		Playfield,  // 桌面主牌区
		HandStack,  // 手牌堆 (下方翻开的)
		DrawStack   // 抽牌堆 (下方盖着的)
	};

	// 4. 屏幕设计分辨率 (参考截图: 1080*2080)
	static const float DESIGN_WIDTH = 1080.0f;
	static const float DESIGN_HEIGHT = 2080.0f;

	// 5. 卡片尺寸 (参考截图: 1080*1500 是区域，卡片本身大概 200左右)
	// 这里预设一个值，具体根据你的图片调整
	static const float CARD_WIDTH = 140.0f;
	static const float CARD_HEIGHT = 190.0f;
}

#endif // __GAME_TYPES_H__