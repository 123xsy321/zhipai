#pragma once
#ifndef __CARD_VIEW_H__
#define __CARD_VIEW_H__

#include "cocos2d.h"
#include "models/CardModel.h"
#include <functional> // 必须引用这个，用于回调

namespace Solitaire {

	class CardView : public cocos2d::Node {
	public:
		static CardView* createWithModel(CardModel* model);
		void updateView(); // 刷新显示

		// 获取数据模型 (给裁判看)
		CardModel* getModel() const { return _model; }

		// 辅助判断 (给裁判看)
		bool isRedSuit();

		// 外部控制移动
		void moveBackToOriginal(); // 移动失败，弹回
		void moveToNewPosition(cocos2d::Vec2 newPos); // 移动成功，去新家

		// 拖拽结束的回调函数 (打电话给 GameView)
		std::function<void(CardView*)> onDragEndedCallback;

		// 触摸事件回调
		bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
		void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
		void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

	private:
		bool initWithModel(CardModel* model);

		CardModel* _model;

		// 显示组件
		cocos2d::Sprite* _frontBase;
		cocos2d::LayerColor* _backBase;

		// 拼装组件
		cocos2d::Sprite* _smallNum;
		cocos2d::Sprite* _smallSuit;
		cocos2d::Sprite* _bigContent;

		// 路径辅助
		std::string getSmallNumPath();
		std::string getBigNumPath();
		std::string getSuitPath();

		// 记录拖拽前的状态
		cocos2d::Vec2 _originalPos;
		int _originalZOrder;
	};
}
#endif