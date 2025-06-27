#ifndef MENU_H
#define MENU_H

#include "data.h"
class Game;
class Scenery;
class Modal;

class Menu {
public:
    Menu(Game* game, MenuID id);
    virtual ~Menu();

    void close(const std::function<void()>& callback);

    virtual void update() = 0;

    void openModal(ModalID id);
    void closeModal();

    constexpr const MenuID getId() const { return id; }
    inline Modal* getModal() const { return modal; }
    inline bool isAModalOpened() const { return (modal != nullptr); }

protected:
    Game* game;

    virtual void initCamera() = 0;
    void setCameraTranslate(const H2DE_Translate& translate);

private:
    MenuID id;

    Modal* modal = nullptr;

    std::vector<H2DE_Object*> objects = {};

    void initObjects();
    void createObject(const Data::ObjectBuffer& objectBuffer);
    H2DE_BasicObject* createBasicObject(const Data::ObjectBuffer& objectBuffer) const;
    H2DE_ButtonObject* createButtonObject(const Data::ObjectBuffer& objectBuffer) const;
    H2DE_TextObject* createTextObject(const Data::ObjectBuffer& objectBuffer) const;

    void destroyObjects();
    void destroyModal();

    void disableButtons();
};

#include "menu/main_menu.h"
#include "menu/level_menu.h"
#include "menu/icon_menu.h"

#endif
