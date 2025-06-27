#ifndef ICON_MENU_H
#define ICON_MENU_H

#include "menu/menu.h"
class Game;
class Player;

class IconMenu : public Menu {
public:
    IconMenu(Game* game, MenuID id);
    ~IconMenu() override;
    
    void update() override;

private:
    H2DE_BasicObject* mainIcon = nullptr;
    std::vector<H2DE_ButtonObject*> iconButtons = {};
    std::vector<H2DE_ButtonObject*> col1Buttons = {};
    std::vector<H2DE_ButtonObject*> col2Buttons = {};

    PlayerIcons icons = PlayerIcons();

    void initCamera() override;
    void loadPlayerIcons();

    void createButtons();
    H2DE_ButtonObject* createButton(std::vector<H2DE_ButtonObject*>& buttons, int index, bool unlocked, const H2DE_Translate& firstButtonTranslate, const H2DE_Translate& buttonScale, const std::function<void(int, bool)>& onMouseUp);
    void applyHitboxToButton(H2DE_ButtonObject* button);

    void createIconButtons();
    void createIconButtons_applyTextures(H2DE_ButtonObject* icon, int index, bool unlocked);

    void createColorButtons();
    void createColorButtons_applyTextures(H2DE_ButtonObject* button, int index, bool unlocked);
    
    void destroyMainIcon();
    void destroyButtons();
    void saveIcons();

    void refreshMainIcon();

    void selectButton(const std::vector<H2DE_ButtonObject*>& buttons, int index) const;
    void selectLoadedPlayerIcons();
};

#endif
