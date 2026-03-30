#include "views/GameView.h"
#include "views/CardView.h"
#include "controllers/GameController.h"
#include <algorithm>
#include <vector>

namespace Solitaire {

	cocos2d::Scene* GameView::createScene() {
		return GameView::create();
	}

	bool GameView::init() {
		if (!Scene::init()) return false;

		auto visibleSize = cocos2d::Director::getInstance()->getVisibleSize();

		// 1. 背景
		auto bg = cocos2d::LayerColor::create(cocos2d::Color4B(0, 100, 0, 255));
		this->addChild(bg);

		// 2. 底部操作栏
		float bottomHeight = 350.0f;
		auto bottomBar = cocos2d::LayerColor::create(cocos2d::Color4B(128, 0, 128, 255), visibleSize.width, bottomHeight);
		bottomBar->setPosition(0, 0);
		this->addChild(bottomBar);

		// 3. 回退按钮
		auto undoLabel = cocos2d::Label::createWithSystemFont("UNDO", "Arial", 40);
		undoLabel->setPosition(visibleSize.width - 150, bottomHeight / 2);
		this->addChild(undoLabel);

		auto undoListener = cocos2d::EventListenerTouchOneByOne::create();
		undoListener->onTouchBegan = [undoLabel](cocos2d::Touch* t, cocos2d::Event* e) {
			cocos2d::Rect box = undoLabel->getBoundingBox();
			box.origin.x -= 20; box.origin.y -= 20;
			box.size.width += 40; box.size.height += 40;
			return box.containsPoint(t->getLocation());
		};
		undoListener->onTouchEnded = [this](cocos2d::Touch* t, cocos2d::Event* e) {
			this->onUndoClicked();
		};
		_eventDispatcher->addEventListenerWithSceneGraphPriority(undoListener, undoLabel);

		// 4. 结果标签
		_resultLabel = cocos2d::Label::createWithSystemFont("", "Arial", 80);
		_resultLabel->setPosition(visibleSize.width / 2, visibleSize.height / 2 + 100);
		_resultLabel->setVisible(false);
		_resultLabel->setLocalZOrder(999);
		this->addChild(_resultLabel);

		// 5. 初始化数据
		_controller = GameController::create();
		_controller->retain();
		_controller->startNewGame();
		auto& allCards = _controller->getAllCards();
		int cardIndex = 0;

		_drawPile.clear();
		_boardCards.clear();
		_history.clear();
		_activeCard = nullptr;

		// =========================================
		// A. 布局桌面牌 (金字塔结构)
		// =========================================
		std::vector<int> rows = { 2, 3, 4 };
		float startY = visibleSize.height - 250.0f;
		float gapY = 60.0f;
		float gapX = CARD_WIDTH + 30.0f;

		for (int row = 0; row < rows.size(); ++row) {
			int cardCountInRow = rows[row];
			float rowTotalWidth = (cardCountInRow - 1) * gapX;
			float startX = (visibleSize.width - rowTotalWidth) / 2.0f;

			for (int col = 0; col < cardCountInRow; ++col) {
				if (cardIndex >= allCards.size()) break;

				auto card = CardView::createWithModel(allCards.at(cardIndex++));
				card->getModel()->setFaceUp(true);
				card->updateView();
				float x = startX + col * gapX;
				float y = startY - row * gapY;
				card->setPosition(x, y);
				card->setLocalZOrder(row); // 上层压下层

				this->addChild(card);
				card->onDragEndedCallback = [this](CardView* c) { this->onBoardCardClicked(c); };
				_boardCards.push_back(card);
			}
		}

		// =========================================
		// B. 布局备用牌堆 (Draw Pile) 
		// =========================================
		float pileX = 150.0f; // 稍微靠左一点，留出位置给展开的牌
		float pileY = bottomHeight / 2;

		// 临时列表，用于计算偏移
		int pileCount = 0;

		while (cardIndex < allCards.size() - 1) {
			auto card = CardView::createWithModel(allCards.at(cardIndex++));

			// 备用牌堆“正面朝上”
			card->getModel()->setFaceUp(true);
			card->updateView();

			// 视觉优化：横向展开一点点，展示出堆叠感
			float offset = pileCount * 15.0f; // 每张牌向右偏移15像素
			// 如果牌太多超出屏幕，可以限制一下最大偏移，或者改成只偏移最后几张
			// 这里为了简单，限制只偏移前10张的视觉距离，后面的叠在一起
			if (offset > 150.0f) offset = 150.0f + (pileCount % 5) * 2.0f;

			card->setPosition(pileX + offset, pileY);

			// 越后面的牌 Z 越高，盖在上面
			card->setLocalZOrder(pileCount);

			this->addChild(card);
			_drawPile.push_back(card);

			card->onDragEndedCallback = [this](CardView* c) { this->onDrawPileClicked(c); };
			pileCount++;
		}

		// =========================================
		// C. 布局当前底部牌
		// =========================================
		if (cardIndex < allCards.size()) {
			_activeCard = CardView::createWithModel(allCards.at(cardIndex));
			_activeCard->getModel()->setFaceUp(true);
			_activeCard->updateView();
			_activeCard->setPosition(visibleSize.width / 2 + 100.0f, pileY); // 放在中间偏右
			_activeCard->setLocalZOrder(1000);
			this->addChild(_activeCard);
		}

		return true;
	}

	// --- 点击桌面牌 ---
	void GameView::onBoardCardClicked(CardView* clickedCard) {
		if (!_activeCard || _resultLabel->isVisible()) return;

		int boardVal = (int)clickedCard->getModel()->getFace();
		int activeVal = (int)_activeCard->getModel()->getFace();
		int diff = abs(boardVal - activeVal);
		bool isMatch = (diff == 1 || diff == 12);

		if (isMatch) {
			MoveRecord record;
			record.movedCard = clickedCard;
			record.startPos = clickedCard->getPosition();
			record.coveredCard = _activeCard;
			record.isFromDrawPile = false;
			record.originalZOrder = clickedCard->getLocalZOrder(); // 记录 Z
			_history.push_back(record);

			auto it = std::find(_boardCards.begin(), _boardCards.end(), clickedCard);
			if (it != _boardCards.end()) _boardCards.erase(it);

			clickedCard->setLocalZOrder(2000); // 飞行动画最高层
			clickedCard->runAction(cocos2d::MoveTo::create(0.2f, _activeCard->getPosition()));

			_activeCard->setLocalZOrder(0);
			_activeCard->setVisible(false);

			_activeCard = clickedCard;
			_activeCard->onDragEndedCallback = nullptr;

			checkGameState();
		}
		else {
			// 失败抖动
			clickedCard->runAction(cocos2d::Sequence::create(
				cocos2d::MoveBy::create(0.05f, cocos2d::Vec2(-10, 0)),
				cocos2d::MoveBy::create(0.05f, cocos2d::Vec2(20, 0)),
				cocos2d::MoveBy::create(0.05f, cocos2d::Vec2(-10, 0)),
				nullptr
			));
		}
	}

	// --- 点击备用牌堆 ---
	void GameView::onDrawPileClicked(CardView* clickedCard) {
		if (_drawPile.empty() || _resultLabel->isVisible()) return;

		// 不管点谁，永远只动最上面那张 (back)
		CardView* topCard = _drawPile.back();

		MoveRecord record;
		record.movedCard = topCard;
		record.startPos = topCard->getPosition();
		record.coveredCard = _activeCard;
		record.isFromDrawPile = true;
		record.originalZOrder = topCard->getLocalZOrder(); // 记录 Z
		_history.push_back(record);

		_drawPile.pop_back();

		// 既然本来就是正面，就不需要 setFaceUp(true) 了，但写着也无妨
		topCard->setLocalZOrder(2000);
		topCard->runAction(cocos2d::MoveTo::create(0.2f, _activeCard->getPosition()));

		_activeCard->setVisible(false);
		_activeCard = topCard;
		_activeCard->onDragEndedCallback = nullptr;

		checkGameState();
	}

	// --- 回退 ---
	void GameView::onUndoClicked() {
		if (_history.empty()) return;
		if (_resultLabel->isVisible()) _resultLabel->setVisible(false);

		MoveRecord lastMove = _history.back();
		_history.pop_back();

		CardView* currentActive = lastMove.movedCard;
		currentActive->stopAllActions();
		currentActive->runAction(cocos2d::Sequence::create(
			cocos2d::MoveTo::create(0.2f, lastMove.startPos),
			cocos2d::CallFunc::create([currentActive, lastMove, this]() {
			if (lastMove.isFromDrawPile) {
				// 退回去的时候保持正面
				currentActive->getModel()->setFaceUp(true);
				currentActive->updateView();
				this->_drawPile.push_back(currentActive);
				currentActive->onDragEndedCallback = [this](CardView* c) { this->onDrawPileClicked(c); };
			}
			else {
				this->_boardCards.push_back(currentActive);
				currentActive->onDragEndedCallback = [this](CardView* c) { this->onBoardCardClicked(c); };
			}
			// 恢复层级
			currentActive->setLocalZOrder(lastMove.originalZOrder);
		}),
			nullptr
			));

		CardView* oldActive = lastMove.coveredCard;
		if (oldActive) {
			oldActive->setVisible(true);
			oldActive->setLocalZOrder(1000);
			_activeCard = oldActive;
		}
	}

	// --- 胜负 ---
	void GameView::checkGameState() {
		if (_boardCards.empty()) {
			_resultLabel->setString("YOU WIN!");
			_resultLabel->setColor(cocos2d::Color3B::YELLOW);
			_resultLabel->setVisible(true);
			return;
		}

		if (_drawPile.empty()) {
			bool hasMove = false;
			int activeVal = (int)_activeCard->getModel()->getFace();

			for (auto card : _boardCards) {
				int val = (int)card->getModel()->getFace();
				int diff = abs(val - activeVal);
				if (diff == 1 || diff == 12) {
					hasMove = true;
					break;
				}
			}

			if (!hasMove) {
				_resultLabel->setString("GAME OVER");
				_resultLabel->setColor(cocos2d::Color3B::RED);
				_resultLabel->setVisible(true);
			}
		}
	}
}