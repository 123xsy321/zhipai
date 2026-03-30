#include "views/CardView.h"
#include <string>

namespace Solitaire {

	CardView* CardView::createWithModel(CardModel* model) {
		CardView* view = new (std::nothrow) CardView();
		if (view && view->initWithModel(model)) {
			view->autorelease();
			return view;
		}
		CC_SAFE_DELETE(view);
		return nullptr;
	}

	bool CardView::initWithModel(CardModel* model) {
		if (!Node::init()) return false;
		_model = model;

		float targetWidth = CARD_WIDTH;
		float targetHeight = CARD_HEIGHT;

		// 1. 设置容器大小和锚点
		this->setContentSize(cocos2d::Size(targetWidth, targetHeight));
		this->setAnchorPoint(cocos2d::Vec2(0.5f, 0.5f));

		// 2. 正面 (Front)
		_frontBase = cocos2d::Sprite::create("res/card_general.png");
		if (_frontBase) {
			float scale = targetWidth / _frontBase->getContentSize().width;
			_frontBase->setScale(scale);
			_frontBase->setPosition(targetWidth / 2, targetHeight / 2);
			this->addChild(_frontBase);

			// 拼图
			auto size = _frontBase->getContentSize();

			// 小数字
			_smallNum = cocos2d::Sprite::create(getSmallNumPath());
			if (_smallNum) {
				_smallNum->setAnchorPoint(cocos2d::Vec2(0.5, 1.0));
				_smallNum->setPosition(size.width * 0.15f, size.height * 0.95f);
				_frontBase->addChild(_smallNum);
			}

			// 小花色
			_smallSuit = cocos2d::Sprite::create(getSuitPath());
			if (_smallSuit) {
				_smallSuit->setScale(0.5f);
				_smallSuit->setAnchorPoint(cocos2d::Vec2(0.5, 1.0));
				if (_smallNum) _smallSuit->setPosition(_smallNum->getPositionX(), _smallNum->getPositionY() - _smallNum->getContentSize().height);
				_frontBase->addChild(_smallSuit);
			}

			// 大图案
			_bigContent = cocos2d::Sprite::create(getBigNumPath());
			if (_bigContent) {
				_bigContent->setPosition(size.width * 0.6f, size.height * 0.4f);
				_frontBase->addChild(_bigContent);
			}
		}

		// 3. 背面 (Back) - 紫色背景
		_backBase = cocos2d::LayerColor::create(cocos2d::Color4B(100, 50, 200, 255), targetWidth, targetHeight);
		_backBase->ignoreAnchorPointForPosition(false);
		_backBase->setAnchorPoint(cocos2d::Vec2(0.5, 0.5));
		_backBase->setPosition(targetWidth / 2, targetHeight / 2);

		// 加个黑边框方便看清堆叠
		auto border = cocos2d::LayerColor::create(cocos2d::Color4B::BLACK, targetWidth, 2);
		border->setPosition(0, targetHeight - 2);
		_backBase->addChild(border);

		this->addChild(_backBase);

		// 4. 刷新显示 & 监听
		updateView();

		auto listener = cocos2d::EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true); // 吞噬事件，保证点到上面的牌就不会点到下面
		listener->onTouchBegan = CC_CALLBACK_2(CardView::onTouchBegan, this);
		listener->onTouchMoved = CC_CALLBACK_2(CardView::onTouchMoved, this);
		listener->onTouchEnded = CC_CALLBACK_2(CardView::onTouchEnded, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

		return true;
	}

	void CardView::updateView() {
		if (!_model) return;
		bool isFaceUp = _model->isFaceUp();
		if (_frontBase) _frontBase->setVisible(isFaceUp);
		if (_backBase) _backBase->setVisible(!isFaceUp);
	}

	// --- 外部控制 ---
	void CardView::moveBackToOriginal() {
		this->stopAllActions();
		this->runAction(cocos2d::Sequence::create(
			cocos2d::EaseBackOut::create(cocos2d::MoveTo::create(0.2f, _originalPos)),
			cocos2d::CallFunc::create([this]() { this->setLocalZOrder(_originalZOrder); }),
			nullptr
		));
	}

	void CardView::moveToNewPosition(cocos2d::Vec2 newPos) {
		this->stopAllActions();
		this->setPosition(newPos);
		_originalPos = newPos;
	}

	// --- 触摸事件 ---
	bool CardView::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) {
		// 无论正面背面，只要点中了，就允许交互！

		// 坐标转换
		cocos2d::Vec2 p = this->getParent()->convertToNodeSpace(touch->getLocation());

		if (this->getBoundingBox().containsPoint(p)) {
			_originalPos = this->getPosition();
			_originalZOrder = this->getLocalZOrder();
			// 注意：点击时不自动 setLocalZOrder(1000)，交给 GameView 决定是否要飞
			return true;
		}
		return false;
	}

	void CardView::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event) {
		// 不需要拖拽，留空即可
	}

	void CardView::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) {
		// 触发点击回调
		if (onDragEndedCallback) {
			onDragEndedCallback(this);
		}
	}

	// --- 路径辅助 ---
	std::string CardView::getSmallNumPath() {
		std::string color = isRedSuit() ? "red" : "black";
		int val = (int)_model->getFace();
		std::string faceStr = (val == 1) ? "A" : (val == 11) ? "J" : (val == 12) ? "Q" : (val == 13) ? "K" : std::to_string(val);
		return "res/number/small_" + color + "_" + faceStr + ".png";
	}

	std::string CardView::getBigNumPath() {
		std::string color = isRedSuit() ? "red" : "black";
		int val = (int)_model->getFace();
		std::string faceStr = (val == 1) ? "A" : (val == 11) ? "J" : (val == 12) ? "Q" : (val == 13) ? "K" : std::to_string(val);
		return "res/number/big_" + color + "_" + faceStr + ".png";
	}

	std::string CardView::getSuitPath() {
		switch (_model->getSuit()) {
		case CardSuit::Clubs:    return "res/suits/club.png";
		case CardSuit::Diamonds: return "res/suits/diamond.png";
		case CardSuit::Hearts:   return "res/suits/heart.png";
		case CardSuit::Spades:   return "res/suits/spade.png";
		default: return "";
		}
	}

	bool CardView::isRedSuit() {
		return (_model->getSuit() == CardSuit::Diamonds || _model->getSuit() == CardSuit::Hearts);
	}
}