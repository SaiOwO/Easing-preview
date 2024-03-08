#include <Geode/Geode.hpp>
#include <Geode/modify/SetupTriggerPopup.hpp>
#include <Geode/modify/ConfigureValuePopup.hpp>

using namespace geode::prelude;

EffectGameObject* m_trigger;
SetupTriggerPopup* m_triggerPopup;
float m_scale;


const char* getEasingName(int id) {
    switch (id) {
        case 1: return "Ease In Out";
        case 2: return "Ease In";
        case 3: return "Ease Out";
        case 4: return "Elastic In Out";
        case 5: return "Elastic In";
        case 6: return "Elastic Out";
        case 7: return "Bounce In Out";
        case 8: return "Bounce In";
        case 9: return "Bounce Out";
		case 10: return "Exponential In Out";
        case 11: return "Exponential In";
        case 12: return "Exponential Out";
		case 13: return "Sine In Out";
        case 14: return "Sine In";
        case 15: return "Sine Out";
		case 16: return "Back In Out";
        case 17: return "Back In";
        case 18: return "Back Out";
			// customs
        case 19: return "Ease In Out Quart";
        case 20: return "Ease In Quart";
        case 21: return "Ease Out Quart";
		case 22: return "Ease In Out Cubic";
        case 23: return "Ease In Cubic";
        case 24: return "Ease Out Cubic";
		case 25: return "Ease In Out Linear";
        case 26: return "Ease In Linear";
        case 27: return "Ease Out Linear";
			// customs 2
		case 28: return "Elastic In Out Quart";
        case 29: return "Elastic In Quart";
        case 30: return "Elastic Out Quart";
		case 31: return "Elastic In Out Cubic";
        case 32: return "Elastic In Cubic";
        case 33: return "Elastic Out Cubic";
		case 34: return "Elastic In Out Linear";
        case 35: return "Elastic In Linear";
        case 36: return "Elastic Out Linear";
        default: return "None";
    }
}

class $modify(alt, ConfigureValuePopup) {
	bool init(ConfigureValuePopupDelegate* p0, float p1, float p2, float p3, gd::string p4, gd::string p5) {
		ConfigureValuePopup::init(p0, p1, p2, p3, p4, p5);

		m_mainLayer->setID("main-layer");
		m_buttonMenu->setID("button-menu");
		reinterpret_cast<CCScale9Sprite*>(m_mainLayer->getChildren()->objectAtIndex(0))->setID("background");
		reinterpret_cast<CCLabelBMFont*>(m_mainLayer->getChildren()->objectAtIndex(1))->setID("title");

		auto bgInput = reinterpret_cast<CCScale9Sprite*>(m_mainLayer->getChildren()->objectAtIndex(3));
		bgInput->setID("input-background");
		bgInput->setPositionY(bgInput->getPositionY() - 15);

		auto input = reinterpret_cast<CCTextInputNode*>(m_mainLayer->getChildren()->objectAtIndex(4));
		input->setID("text-input");
		input->setPositionY(input->getPositionY() - 15);
		
		auto slider = reinterpret_cast<Slider*>(m_mainLayer->getChildren()->objectAtIndex(5));
		slider->setID("slider");
		slider->setPositionY(slider->getPositionY() - 15);


		auto s = CCDirector::sharedDirector()->getWinSize();

		float r = m_trigger->m_easingRate;
		std::string rateName = "Quart";
		if (r >= 0.f && r <= 1.f) rateName = "Linear";
		else if (r >= 1.f && r <= 2.f) rateName = "Quad";
		else if (r >= 2.f && r <= 3.f) rateName = "Cubic";

		for (int i = 0; i < 4; i++) {
			auto tog = CCMenuItemToggler::create(this->getButtonSprite(i, false), this->getButtonSprite(i, true), this, menu_selector(alt::onToggle));
            tog->setPosition({ - 75.f + 50.f * i, 118 });
            tog->setTag(i);
            tog->setClickable(false);
			float value = i == 0 ? 1 : i == 1 ? 2 : i == 2 ? 3 : 4;
			tog->setUserObject(CCFloat::create(value));
            m_buttonMenu->addChild(tog);

            if (i == 0 && rateName == "Linear" || i == 1 && rateName == "Quad" || i == 2 && rateName == "Cubic" || i == 3 && rateName == "Quart")
                tog->toggle(true);
		}

		return true;
	}
	void onToggle(CCObject* s) {
		auto btn = reinterpret_cast<CCMenuItemToggler*>(s);
        for (int i = 0; i < 4; i++)
            reinterpret_cast<CCMenuItemToggler*>(m_buttonMenu->getChildByTag(i))->toggle(false);

        reinterpret_cast<CCMenuItemToggler*>(m_buttonMenu->getChildByTag(btn->getTag()))->toggle(true);

		float value = reinterpret_cast<CCFloat*>(btn->getUserObject())->getValue();
		m_trigger->m_easingRate = value;

        if (auto slider = reinterpret_cast<Slider*>(m_mainLayer->getChildByID("slider"))) {
			slider->setValue(value);
			slider->updateBar();
		}

		if (auto input = reinterpret_cast<CCTextInputNode*>(m_mainLayer->getChildByID("text-input")))
			input->setString(CCString::createWithFormat("%.02f", value)->getCString());
	}
	ButtonSprite* getButtonSprite(int id, bool activated, bool alt = false) {
        const char* text = "Quart";
        const char* buttonSprite = "GJ_button_04.png";
        if (activated) buttonSprite = "GJ_button_02.png";

        switch (id) {
            case 0: text = "Linear"; break;
            case 1: text = "Quad"; break;
            case 2: text = "Cubic"; break;
        }
		auto spr = ButtonSprite::create(text, 50, 0, "bigFont.fnt", buttonSprite, 30, 0.5);
		spr->setScale(.65f);

        return spr;
    }
};

class Easing : public CCLayer {
	CCLayer* m_pagesLayer;
	int m_currentPage = 0;
	CCLabelBMFont* m_pagesText;
	CCLabelBMFont* m_easingSelectedTxt;
public:
	bool init() {
		if (!CCLayer::init()) return false;

		auto dark = CCLayerColor::create({0,0,0,150});
		this->addChild(dark, -10);

		this->setKeypadEnabled(true);
		this->setTouchEnabled(true);
		this->setTouchMode(ccTouchesMode::kCCTouchesOneByOne);
		this->setPreviousPriority(-504);
		this->setTouchPriority(-504);

		auto s = CCDirector::sharedDirector()->getWinSize();

		auto bg = extension::CCScale9Sprite::create("GJ_square01.png");
		bg->setContentSize({440, 310});
		bg->setPosition(s / 2);
		this->addChild(bg, -1);

		auto menu = CCMenu::create();
		menu->setTouchPriority(-504);
		this->addChild(menu);

		auto closeBtn = CCMenuItemSpriteExtra::create(ButtonSprite::create("Ok", 40.0f, true, "goldFont.fnt", "GJ_button_01.png", 30.0f, 0.8f), this, menu_selector(Easing::onClose));
        closeBtn->setPosition({ 0, -bg->getContentSize().height / 2 + 25 });
        menu->addChild(closeBtn);

		auto title = CCLabelBMFont::create("Easing Preview", "goldFont.fnt");
		title->setPosition({ bg->getPositionX(), bg->getPositionY() + bg->getContentSize().height / 2 - 14 });
		title->setScale(.8f);
		this->addChild(title);

		auto arrow = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
		arrow->setScale(.7f);
		auto leftArrow = CCMenuItemSpriteExtra::create(arrow, this, menu_selector(Easing::onBack));
		leftArrow->setPosition({- bg->getContentSize().width / 2 - 15, 0});
		menu->addChild(leftArrow);

		auto arrow_ = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
		arrow_->setScale(.7f);
		arrow_->setFlipX(true);
		auto rightArrow = CCMenuItemSpriteExtra::create(arrow_, this, menu_selector(Easing::onNext));
		rightArrow->setPosition({bg->getContentSize().width / 2 + 15, 0});
		menu->addChild(rightArrow);

		auto infoBtn = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"), this, menu_selector(Easing::onInfo));
		infoBtn->setPosition({ -bg->getContentSize().width / 2 + 18, bg->getContentSize().height / 2 - 18 });
		menu->addChild(infoBtn);


		m_pagesText = CCLabelBMFont::create("(0 / 0)", "chatFont.fnt");
		m_pagesText->setColor({0, 0, 0});
		m_pagesText->setOpacity(100);
		m_pagesText->setScale(.7f);
		m_pagesText->setPosition({ bg->getPositionX() + bg->getContentSize().width / 2 - 8, bg->getPositionY() + bg->getContentSize().height / 2 - 7 });
		m_pagesText->setAnchorPoint({ 1, 1 });
		this->addChild(m_pagesText);

		m_easingSelectedTxt = CCLabelBMFont::create(CCString::createWithFormat("Selected: %s", getEasingName((int)m_trigger->m_easingType))->getCString(), "bigFont.fnt");
		m_easingSelectedTxt->setScale(.35f);
		m_easingSelectedTxt->setColor({0, 255, 0});
		m_easingSelectedTxt->setPosition({ bg->getPositionX(), title->getPositionY() - 17 });
		this->addChild(m_easingSelectedTxt);

		auto line = CCSprite::createWithSpriteFrameName("floorLine_001.png");
        line->setBlendFunc({GL_SRC_ALPHA, GL_ONE});
        line->setScaleX(.9f);
        line->setOpacity(100);
        line->setPosition({ m_easingSelectedTxt->getPositionX(), m_easingSelectedTxt->getPositionY() - 10 });
        this->addChild(line);

		m_pagesLayer = CCLayer::create();
		this->createPage(1, 9);
		this->createPage(10, 18);
		this->createPage(0, 0);
		this->addChild(m_pagesLayer);

		this->updatePages();

		return true;
	}
	void onInfo(CCObject*) {
		const char* message = "You can select the type of easing by tapping on the preview";
		#ifdef GEODE_IS_WINDOWS
		message = "You can select the type of easing by clicking on the preview";
		#endif
		FLAlertLayer::create("Info", message, "OK")->show();
	}
	void onNext(CCObject*) {
		m_currentPage++;
		if (m_currentPage >= m_pagesLayer->getChildrenCount()) m_currentPage = 0;
		this->updatePages();
	}
	void onBack(CCObject*) {
		m_currentPage--;
		if (m_currentPage < 0) m_currentPage = m_pagesLayer->getChildrenCount() - 1;
		this->updatePages();
	}
	void updatePages() {
        for (int i = 0; i < m_pagesLayer->getChildrenCount(); i++) {
			if (auto menu = reinterpret_cast<CCMenu*>(m_pagesLayer->getChildren()->objectAtIndex(i)))
				menu->setVisible(false);
		}
		reinterpret_cast<CCLayer*>(m_pagesLayer->getChildren()->objectAtIndex(m_currentPage))->setVisible(true);
		m_pagesText->setString(CCString::createWithFormat("(%i / %i)", m_currentPage + 1, m_pagesLayer->getChildrenCount())->getCString());
	}
	void onEasing(CCObject* s) {
		auto toggle = reinterpret_cast<CCMenuItemToggler*>(s);

		for (int i = 0; i < m_pagesLayer->getChildrenCount(); i++) {
			if (auto menu = reinterpret_cast<CCMenu*>(m_pagesLayer->getChildren()->objectAtIndex(i))) {
                for (int i = 0; i < menu->getChildrenCount(); i++) {
					if (auto button = reinterpret_cast<CCMenuItemToggler*>(menu->getChildren()->objectAtIndex(i))) {
						button->toggle(false);
					}
				}
			}
		}
		toggle->toggle(true);

		m_trigger->m_easingType = (EasingType)toggle->getTag();
		m_easingSelectedTxt->setString(CCString::createWithFormat("Selected: %s", getEasingName((int)m_trigger->m_easingType))->getCString());

		//m_triggerPopup->valueChanged(30, 2.0f);
		auto m_triggerEasing = reinterpret_cast<CCLabelBMFont*>(m_triggerPopup->m_mainLayer->getChildByID("easing-type"));
		m_triggerEasing->setString(getEasingName(toggle->getTag()));
		m_triggerEasing->limitLabelWidth(150.f * m_scale, 0.7f * m_scale, 0);

		auto m_triggerEasingRate = reinterpret_cast<CCMenuItemSpriteExtra*>(m_triggerPopup->m_buttonMenu->getChildByID("easing-rate-button"));
		m_triggerEasingRate->setVisible(toggle->getTag() >= 1 && toggle->getTag() <= 6);

		//m_triggerPopup->updateEaseRateLabel();
		//return m_triggerPopup->toggleEaseRateVisibility();
	}
	void createPage(int start, int end) {
		auto s = CCDirector::sharedDirector()->getWinSize();

		auto menu = CCMenu::create();
		menu->setTouchPriority(-504);
		menu->setPosition({0,0});

		float padX = 0, padY = 0;
		for (int i = start; i <= end; i++) {
			auto button = CCMenuItemToggler::create(this->createToggle(i, false), this->createToggle(i, true), this, menu_selector(Easing::onEasing));
			button->setPosition({ s.width / 2 - 130 + padX, s.height / 2 + 75 - padY });
			button->setClickable(false);
			button->setTag(i);
			button->m_fSizeMult = 0.5f;
			if (i == (int)m_trigger->m_easingType)
				button->toggle(true);
			menu->addChild(button);

			padX += 130;
			if (i % 3 == 0) {
				padX = 0;
				padY += 70;
			}
		}
		m_pagesLayer->addChild(menu);
	}
	extension::CCScale9Sprite* createToggle(int i, bool enable) {
		auto blackBG = extension::CCScale9Sprite::create("GJ_square06.png");
		if (!enable) 
			blackBG->setColor({25, 25, 25});
		blackBG->setOpacity(75);
		blackBG->setContentSize({ 130, 70 });

		auto easingName = CCLabelBMFont::create(getEasingName(i), "bigFont.fnt");
		easingName->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
		easingName->setPosition({ blackBG->getContentSize().width / 2, blackBG->getContentSize().height - 13 });
		easingName->limitLabelWidth(blackBG->getContentSize().width - 15, 0.45f, 0);
		blackBG->addChild(easingName);

		if (i >= 1 && i <= 6) {
			float r = m_trigger->m_easingRate;
			const char* rateName = "Quart";
			if (r >= 0.f && r <= 1.f) rateName = "Linear";
			else if (r >= 1.f && r <= 2.f) rateName = "Quad";
			else if (r >= 2.f && r <= 3.f) rateName = "Cubic";

			auto easingRate = CCLabelBMFont::create(CCString::createWithFormat("Rate: %.02f | %s", r, rateName)->getCString(), "chatFont.fnt");
			easingRate->setOpacity(100);
			easingRate->setPosition({ blackBG->getContentSize().width / 2, blackBG->getContentSize().height - 25 });
			easingRate->limitLabelWidth(blackBG->getContentSize().width - 15, 0.6f, 0);
			blackBG->addChild(easingRate, 1);
		}

		auto spr = CCSprite::createWithSpriteFrameName("square_01_001.png");
		spr->runAction(this->getEasingAction(i));
		spr->setScale(.8f);

		switch (m_trigger->m_objectID) {
			case 901:
			case 1914:
				spr->setPosition({ blackBG->getContentSize().width / 2 - 40, blackBG->getContentSize().height / 2 - 11 });
				break;
			default: 
				spr->setPosition({ blackBG->getContentSize().width / 2, blackBG->getContentSize().height / 2 - 11 });
				break;
        }
			
		blackBG->addChild(spr);

		return blackBG;
	}
	CCFiniteTimeAction* getEasingAction(int id) {
		CCSequence* action = CCSequence::createWithTwoActions(CCDelayTime::create(0),CCDelayTime::create(0));
		float value = 80;

		float time = 1.0f;
		float rate = m_trigger->m_easingRate;
		int obj = m_trigger->m_objectID;

		if (obj == 901 || obj == 1914) {
			switch (id) {
				case 1: 
					action = CCSequence::createWithTwoActions(CCEaseInOut::create(CCMoveBy::create(time, {value, 0}), rate), CCEaseInOut::create(CCMoveBy::create(time, {-value, 0}), rate));
					break;
				case 2: 
					action = CCSequence::createWithTwoActions(CCEaseIn::create(CCMoveBy::create(time, {value, 0}), rate), CCEaseIn::create(CCMoveBy::create(time, {-value, 0}), rate));
					break;
				case 3: 
					action = CCSequence::createWithTwoActions(CCEaseOut::create(CCMoveBy::create(time, {value, 0}), rate), CCEaseOut::create(CCMoveBy::create(time, {-value, 0}), rate));
					break;
				case 4: 
					action = CCSequence::createWithTwoActions(CCEaseElasticInOut::create(CCMoveBy::create(time, {value, 0}), rate), CCEaseElasticInOut::create(CCMoveBy::create(time, {-value, 0}), rate));
					break;
				case 5: 
					action = CCSequence::createWithTwoActions(CCEaseElasticIn::create(CCMoveBy::create(time, {value, 0}), rate), CCEaseElasticIn::create(CCMoveBy::create(time, {-value, 0}), rate));
					break;
				case 6: 
					action = CCSequence::createWithTwoActions(CCEaseElasticOut::create(CCMoveBy::create(time, {value, 0}), rate), CCEaseElasticOut::create(CCMoveBy::create(time, {-value, 0}), rate));
					break;
				case 7: 
					action = CCSequence::createWithTwoActions(CCEaseBounceInOut::create(CCMoveBy::create(time, {value, 0})), CCEaseBounceInOut::create(CCMoveBy::create(time, {-value, 0})));
					break;
				case 8: 
					action = CCSequence::createWithTwoActions(CCEaseBounceIn::create(CCMoveBy::create(time, {value, 0})), CCEaseBounceIn::create(CCMoveBy::create(time, {-value, 0})));
					break;
				case 9: 
					action = CCSequence::createWithTwoActions(CCEaseBounceOut::create(CCMoveBy::create(time, {value, 0})), CCEaseBounceOut::create(CCMoveBy::create(time, {-value, 0})));
					break;
				case 10: 
					action = CCSequence::createWithTwoActions(CCEaseExponentialInOut::create(CCMoveBy::create(time, {value, 0})), CCEaseExponentialInOut::create(CCMoveBy::create(time, {-value, 0})));
					break;
				case 11: 
					action = CCSequence::createWithTwoActions(CCEaseExponentialIn::create(CCMoveBy::create(time, {value, 0})), CCEaseExponentialIn::create(CCMoveBy::create(time, {-value, 0})));
					break;
				case 12: 
					action = CCSequence::createWithTwoActions(CCEaseExponentialOut::create(CCMoveBy::create(time, {value, 0})), CCEaseExponentialOut::create(CCMoveBy::create(time, {-value, 0})));
					break;
				case 13: 
					action = CCSequence::createWithTwoActions(CCEaseSineInOut::create(CCMoveBy::create(time, {value, 0})), CCEaseSineInOut::create(CCMoveBy::create(time, {-value, 0})));
					break;
				case 14: 
					action = CCSequence::createWithTwoActions(CCEaseSineIn::create(CCMoveBy::create(time, {value, 0})), CCEaseSineIn::create(CCMoveBy::create(time, {-value, 0})));
					break;
				case 15: 
					action = CCSequence::createWithTwoActions(CCEaseSineOut::create(CCMoveBy::create(time, {value, 0})), CCEaseSineOut::create(CCMoveBy::create(time, {-value, 0})));
					break;
				case 16: 
					action = CCSequence::createWithTwoActions(CCEaseBackInOut::create(CCMoveBy::create(time, {value, 0})), CCEaseBackInOut::create(CCMoveBy::create(time, {-value, 0})));
					break;
				case 17: 
					action = CCSequence::createWithTwoActions(CCEaseBackIn::create(CCMoveBy::create(time, {value, 0})), CCEaseBackIn::create(CCMoveBy::create(time, {-value, 0})));
					break;
				case 18: 
					action = CCSequence::createWithTwoActions(CCEaseBackOut::create(CCMoveBy::create(time, {value, 0})), CCEaseBackOut::create(CCMoveBy::create(time, {-value, 0})));
					break;
				default: 
					action = CCSequence::createWithTwoActions(CCMoveBy::create(time, {value, 0}), CCMoveBy::create(time, {-value, 0}));
					break;
			}
		}
		else if (obj == 2067 || obj == 1913) {
			value = 0.8f;
			switch (id) {
				case 1: 
					action = CCSequence::createWithTwoActions(CCEaseInOut::create(CCScaleTo::create(time, 0.5f + value), rate), CCEaseInOut::create(CCScaleTo::create(time, 0.5f), rate));
					break;
				case 2: 
					action = CCSequence::createWithTwoActions(CCEaseIn::create(CCScaleTo::create(time, 0.5f + value), rate), CCEaseIn::create(CCScaleTo::create(time, 0.5f), rate));
					break;
				case 3: 
					action = CCSequence::createWithTwoActions(CCEaseOut::create(CCScaleTo::create(time, 0.5f + value), rate), CCEaseOut::create(CCScaleTo::create(time, 0.5f), rate));
					break;
				case 4: 
					action = CCSequence::createWithTwoActions(CCEaseElasticInOut::create(CCScaleTo::create(time, 0.5f + value), rate), CCEaseElasticInOut::create(CCScaleTo::create(time, 0.5f), rate));
					break;
				case 5: 
					action = CCSequence::createWithTwoActions(CCEaseElasticIn::create(CCScaleTo::create(time, 0.5f + value), rate), CCEaseElasticIn::create(CCScaleTo::create(time, 0.5f), rate));
					break;
				case 6: 
					action = CCSequence::createWithTwoActions(CCEaseElasticOut::create(CCScaleTo::create(time, 0.5f + value), rate), CCEaseElasticOut::create(CCScaleTo::create(time, 0.5f), rate));
					break;
				case 7: 
					action = CCSequence::createWithTwoActions(CCEaseBounceInOut::create(CCScaleTo::create(time, 0.5f + value)), CCEaseBounceInOut::create(CCScaleTo::create(time, 0.5f)));
					break;
				case 8: 
					action = CCSequence::createWithTwoActions(CCEaseBounceIn::create(CCScaleTo::create(time, 0.5f + value)), CCEaseBounceIn::create(CCScaleTo::create(time, 0.5f)));
					break;
				case 9: 
					action = CCSequence::createWithTwoActions(CCEaseBounceOut::create(CCScaleTo::create(time, 0.5f + value)), CCEaseBounceOut::create(CCScaleTo::create(time, 0.5f)));
					break;
				case 10: 
					action = CCSequence::createWithTwoActions(CCEaseExponentialInOut::create(CCScaleTo::create(time, 0.5f + value)), CCEaseExponentialInOut::create(CCScaleTo::create(time, 0.5f)));
					break;
				case 11: 
					action = CCSequence::createWithTwoActions(CCEaseExponentialIn::create(CCScaleTo::create(time, 0.5f + value)), CCEaseExponentialIn::create(CCScaleTo::create(time, 0.5f)));
					break;
				case 12: 
					action = CCSequence::createWithTwoActions(CCEaseExponentialOut::create(CCScaleTo::create(time, 0.5f + value)), CCEaseExponentialOut::create(CCScaleTo::create(time, 0.5f)));
					break;
				case 13: 
					action = CCSequence::createWithTwoActions(CCEaseSineInOut::create(CCScaleTo::create(time, 0.5f + value)), CCEaseSineInOut::create(CCScaleTo::create(time, 0.5f)));
					break;
				case 14: 
					action = CCSequence::createWithTwoActions(CCEaseSineIn::create(CCScaleTo::create(time, 0.5f + value)), CCEaseSineIn::create(CCScaleTo::create(time, 0.5f)));
					break;
				case 15: 
					action = CCSequence::createWithTwoActions(CCEaseSineOut::create(CCScaleTo::create(time, 0.5f + value)), CCEaseSineOut::create(CCScaleTo::create(time, 0.5f)));
					break;
				case 16: 
					action = CCSequence::createWithTwoActions(CCEaseBackInOut::create(CCScaleTo::create(time, 0.5f + value)), CCEaseBackInOut::create(CCScaleTo::create(time, 0.5f)));
					break;
				case 17: 
					action = CCSequence::createWithTwoActions(CCEaseBackIn::create(CCScaleTo::create(time, 0.5f + value)), CCEaseBackIn::create(CCScaleTo::create(time, 0.5f)));
					break;
				case 18: 
					action = CCSequence::createWithTwoActions(CCEaseBackOut::create(CCScaleTo::create(time, 0.5f + value)), CCEaseBackOut::create(CCScaleTo::create(time, 0.5f)));
					break;
				default: 
					action = CCSequence::createWithTwoActions(CCScaleTo::create(time, 0.5f + value), CCScaleTo::create(time, 0.5f));
					break;
			}
		}
		else {
			value = 180;
			switch (id) {
				case 1: 
					action = CCSequence::createWithTwoActions(CCEaseInOut::create(CCRotateBy::create(time, value), rate), CCEaseInOut::create(CCRotateBy::create(time, -value), rate));
					break;
				case 2: 
					action = CCSequence::createWithTwoActions(CCEaseIn::create(CCRotateBy::create(time, value), rate), CCEaseIn::create(CCRotateBy::create(time, -value), rate));
					break;
				case 3: 
					action = CCSequence::createWithTwoActions(CCEaseOut::create(CCRotateBy::create(time, value), rate), CCEaseOut::create(CCRotateBy::create(time, -value), rate));
					break;
				case 4: 
					action = CCSequence::createWithTwoActions(CCEaseElasticInOut::create(CCRotateBy::create(time, value), rate), CCEaseElasticInOut::create(CCRotateBy::create(time, -value), rate));
					break;
				case 5: 
					action = CCSequence::createWithTwoActions(CCEaseElasticIn::create(CCRotateBy::create(time, value), rate), CCEaseElasticIn::create(CCRotateBy::create(time, -value), rate));
					break;
				case 6: 
					action = CCSequence::createWithTwoActions(CCEaseElasticOut::create(CCRotateBy::create(time, value), rate), CCEaseElasticOut::create(CCRotateBy::create(time, -value), rate));
					break;
				case 7: 
					action = CCSequence::createWithTwoActions(CCEaseBounceInOut::create(CCRotateBy::create(time, value)), CCEaseBounceInOut::create(CCRotateBy::create(time, -value)));
					break;
				case 8: 
					action = CCSequence::createWithTwoActions(CCEaseBounceIn::create(CCRotateBy::create(time, value)), CCEaseBounceIn::create(CCRotateBy::create(time, -value)));
					break;
				case 9: 
					action = CCSequence::createWithTwoActions(CCEaseBounceOut::create(CCRotateBy::create(time, value)), CCEaseBounceOut::create(CCRotateBy::create(time, -value)));
					break;
				case 10: 
					action = CCSequence::createWithTwoActions(CCEaseExponentialInOut::create(CCRotateBy::create(time, value)), CCEaseExponentialInOut::create(CCRotateBy::create(time, -value)));
					break;
				case 11: 
					action = CCSequence::createWithTwoActions(CCEaseExponentialIn::create(CCRotateBy::create(time, value)), CCEaseExponentialIn::create(CCRotateBy::create(time, -value)));
					break;
				case 12: 
					action = CCSequence::createWithTwoActions(CCEaseExponentialOut::create(CCRotateBy::create(time, value)), CCEaseExponentialOut::create(CCRotateBy::create(time, -value)));
					break;
				case 13: 
					action = CCSequence::createWithTwoActions(CCEaseSineInOut::create(CCRotateBy::create(time, value)), CCEaseSineInOut::create(CCRotateBy::create(time, -value)));
					break;
				case 14: 
					action = CCSequence::createWithTwoActions(CCEaseSineIn::create(CCRotateBy::create(time, value)), CCEaseSineIn::create(CCRotateBy::create(time, -value)));
					break;
				case 15: 
					action = CCSequence::createWithTwoActions(CCEaseSineOut::create(CCRotateBy::create(time, value)), CCEaseSineOut::create(CCRotateBy::create(time, -value)));
					break;
				case 16: 
					action = CCSequence::createWithTwoActions(CCEaseBackInOut::create(CCRotateBy::create(time, value)), CCEaseBackInOut::create(CCRotateBy::create(time, -value)));
					break;
				case 17: 
					action = CCSequence::createWithTwoActions(CCEaseBackIn::create(CCRotateBy::create(time, value)), CCEaseBackIn::create(CCRotateBy::create(time, -value)));
					break;
				case 18: 
					action = CCSequence::createWithTwoActions(CCEaseBackOut::create(CCRotateBy::create(time, value)), CCEaseBackOut::create(CCRotateBy::create(time, -value)));
					break;
				default: 
					action = CCSequence::createWithTwoActions(CCRotateBy::create(time, value), CCRotateBy::create(time, -value));
					break;
			}
		}
		
		return CCRepeatForever::create(action);
	}
	void show() {
		CCScene::get()->addChild(this, CCScene::get()->getHighestChildZ());
	}
	void onClose(CCObject*) {
		this->removeMeAndCleanup();
	}
	void keyBackClicked() {
		this->onClose(0);
	}
	void onEasingPopup(CCObject*) {
		Easing::create()->show();
	}
	CREATE_FUNC(Easing);
};

class $modify(Test, SetupTriggerPopup) {
	bool init(EffectGameObject* p0, CCArray* p1, float p2, float p3, int p4) {
		SetupTriggerPopup::init(p0, p1, p2, p3, p4);

		m_triggerPopup = this;

		if (LevelEditorLayer::get() && p0 && p1) {
			if (p0 != nullptr)
				m_trigger = p0;
			else
				m_trigger = reinterpret_cast<EffectGameObject*>(p1->objectAtIndex(0));
		}
		return true;
	}
	void createEasingControls(CCPoint p0, float p1, int p2, int p3) {
		SetupTriggerPopup::createEasingControls(p0, p1, p2, p3);

		if (LevelEditorLayer::get()) {
			reinterpret_cast<CCLabelBMFont*>(m_mainLayer->getChildren()->objectAtIndex(m_mainLayer->getChildrenCount() - 1))->setID("easing-type");
			auto title = reinterpret_cast<CCLabelBMFont*>(m_mainLayer->getChildren()->objectAtIndex(m_mainLayer->getChildrenCount() - 2));
			title->setID("easing-text");
			reinterpret_cast<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildren()->objectAtIndex(m_buttonMenu->getChildrenCount() - 1))->setID("easing-left-button");
			reinterpret_cast<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildren()->objectAtIndex(m_buttonMenu->getChildrenCount() - 2))->setID("easing-right-button");
			auto rate = reinterpret_cast<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildren()->objectAtIndex(m_buttonMenu->getChildrenCount() - 3));
			rate->setID("easing-rate-button");
			reinterpret_cast<CCNode*>(rate->getChildren()->objectAtIndex(0))->setID("easing-rate-sprite");
			reinterpret_cast<CCNode*>(reinterpret_cast<CCNode*>(rate->getChildren()->objectAtIndex(0))->getChildren()->objectAtIndex(0))->setID("easing-rate-sprite-label");

			m_scale = p1;

			auto bg = CCSprite::create("GJ_button_05.png");
			auto icon = CCSprite::createWithSpriteFrameName("GJ_starsIcon_001.png");
			icon->setPosition(bg->getContentSize() / 2);
			bg->addChild(icon);
			bg->setScale(0.65f * m_scale);
			auto btn = CCMenuItemSpriteExtra::create(bg, this, menu_selector(Easing::onEasingPopup));
			btn->setID("easing-preview-button"_spr);
			auto pos = m_buttonMenu->convertToNodeSpace(p0);
			btn->setPosition({pos.x - (55 * m_scale), pos.y});
			m_buttonMenu->addChild(btn);

			this->schedule(schedule_selector(Test::checkVisible)); // i dont want to find the class_member (CCArray*)
		}
	}
	void checkVisible(float dt) {
		if (m_buttonMenu->getChildByID("easing-preview-button"_spr) && m_mainLayer->getChildByID("easing-text"))
			m_buttonMenu->getChildByID("easing-preview-button"_spr)->setVisible(m_mainLayer->getChildByID("easing-text")->isVisible());
	}
};
