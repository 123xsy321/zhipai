#ifndef __GAME_VIEW_H__
#define __GAME_VIEW_H__

#include "cocos2d.h"
#include "controllers/GameController.h"
#include "views/CardView.h"
#include <vector>

namespace Solitaire {

	/**
	 * @brief 移动记录结构体
	 * * 功能描述：
	 * 用于记录玩家每一次有效操作的具体状态，作为“历史快照”。
	 * 在执行“回退”(Undo) 功能时，系统通过读取此记录来恢复卡牌的位置、层级和状态。
	 */
	struct MoveRecord {
		CardView* movedCard;// 被移动的那张卡牌视图对象
		cocos2d::Vec2 startPos;// 卡牌移动前在屏幕上的原始坐标位置
		CardView* coveredCard;// 移动发生前，原本位于底部Active位置的卡牌（被新牌覆盖的旧牌）
		bool isFromDrawPile;// 标记该移动是否来自于备用牌堆 (true: 翻牌操作; false: 桌面消除操作)
		int originalZOrder;// 卡牌移动前的原始渲染层级 (Z-Order)，用于回退时恢复遮挡关系 
	};

	/**
	 * @brief 游戏主场景视图类 (GameView)
	 * * 功能与职责：
	 * 1. UI 展示：负责渲染游戏背景、金字塔卡牌布局、备用牌堆、功能按钮及结算文字。
	 * 2. 交互处理：监听并分发用户的触摸事件（点击卡牌、点击按钮）。
	 * 3. 游戏逻辑协调：虽然是 View 层，但在此版本中也承担了消除判定、胜负检查和历史记录管理的职责。
	 * * 使用场景：
	 * 游戏启动后进入的核心玩法界面。
	 */
	class GameView : public cocos2d::Scene {
	public:
		/**
		 * @brief 创建并返回游戏场景实例
		 * * @return cocos2d::Scene* 指向新创建场景的指针
		 */
		static cocos2d::Scene* createScene();

		/**
		 * @brief 场景初始化函数
		 * * 负责加载背景、初始化 UI 控件、请求 Controller 生成数据并完成发牌布局。
		 * * @return bool 初始化成功返回 true，否则返回 false
		 */
		virtual bool init();
		CREATE_FUNC(GameView);// Cocos2d-x 标准宏，实现 create() 静态工厂方法

	private:
		GameController* _controller;// 游戏控制器指针，用于获取初始的卡牌数据模型

		// 核心变量
		CardView* _activeCard;// 当前位于底部中央的“目标牌” (Active Card)，玩家需寻找与其点数差 1 的牌进行匹配
		std::vector<CardView*> _drawPile;// 备用牌堆列表，存储位于屏幕左下角、尚未翻开的卡牌视图
		std::vector<MoveRecord> _history;// 历史操作记录栈，存储 MoveRecord 对象，用于支持无限回退功能
		std::vector<CardView*> _boardCards;// 桌面（金字塔区域）当前剩余的卡牌列表，当此列表为空时判定游戏胜利
		cocos2d::Label* _resultLabel;// 游戏结果显示标签，用于显示 "YOU WIN!" 或 "GAME OVER"

		// 核心函数
		/**
		 * @brief 处理桌面金字塔卡牌的点击事件
		 * * 检查被点击的牌是否与 ActiveCard 匹配（点数差 1）。
		 * 如果匹配，执行消除动画并记录历史；如果不匹配，执行抖动动画。
		 * * @param card 被点击的卡牌视图对象
		 */
		void onBoardCardClicked(CardView* card);

		/**
		 * @brief 处理备用牌堆的点击事件
		 * * 将牌堆顶部的卡牌翻开并移动到 ActiveCard 位置，同时记录历史。
		 * * @param card 被点击的牌堆卡牌视图对象
		 */
		void onDrawPileClicked(CardView* card);
		
		/**
		 * @brief 处理“回退”按钮的点击事件
		 * * 从历史栈中弹出最近一次操作记录，执行逆向动画，将卡牌恢复到操作前的状态和位置。
		 */
		void onUndoClicked();
		
		/**
		 * @brief 检查当前游戏状态（胜利或失败）
		 * * 每次有效操作后调用。
		 * 1. 胜利条件：_boardCards 为空。
		 * 2. 失败条件：_drawPile 为空 且 桌面无牌可消。
		 */
		void checkGameState();
	};
}

#endif // __GAME_VIEW_H__