#include "models/CardModel.h"

namespace Solitaire {

	CardModel* CardModel::create(int id, CardFace face, CardSuit suit) {
		CardModel* model = new (std::nothrow) CardModel();
		if (model && model->init(id, face, suit)) {
			model->autorelease();
			return model;
		}
		CC_SAFE_DELETE(model);
		return nullptr;
	}

	bool CardModel::init(int id, CardFace face, CardSuit suit) {
		this->_id = id;
		this->_face = face;
		this->_suit = suit;
		this->_isFaceUp = false; // Ä¬ÈÏ±³Ãæ³¯ÉÏ
		this->_zOrder = 0;
		return true;
	}

	void CardModel::setFaceUp(bool isFaceUp) {
		_isFaceUp = isFaceUp;
	}
}