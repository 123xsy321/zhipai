#pragma once
#ifndef __CARD_MODEL_H__
#define __CARD_MODEL_H__

#include "cocos2d.h"
#include "configs/GameTypes.h"

namespace Solitaire {

	class CardModel : public cocos2d::Ref {
	public:
		// 静态创建方法 (Cocos 风格)
		static CardModel* create(int id, CardFace face, CardSuit suit);

		// Getters (只读)
		int getId() const { return _id; }
		CardFace getFace() const { return _face; }
		CardSuit getSuit() const { return _suit; }
		bool isFaceUp() const { return _isFaceUp; }

		// 记录逻辑位置 (不是屏幕像素坐标，而是第几行第几列，或者层级)
		void setZOrder(int z) { _zOrder = z; }
		int getZOrder() const { return _zOrder; }

		// 翻牌状态改变
		void setFaceUp(bool isFaceUp);

	private:
		bool init(int id, CardFace face, CardSuit suit);

		int _id;            // 唯一ID
		CardFace _face;     // 点数
		CardSuit _suit;     // 花色
		bool _isFaceUp;     // 正反面
		int _zOrder;        // 堆叠顺序
	};
}
#endif