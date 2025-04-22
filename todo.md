


class Menu {
private:
    Game* game;

protected:

public:
    Menu();
    ~Menu();
}



class MainMenu : public Menu {
private:

public:
    MainMenu();
    ~MainMenu();

    void openCloseGameModal();
    void openAchievementsModal();
    void openSettingsModal();
    void openStatsModal();
}













class Modal {
private:
    Game* game;

protected:

public:
    Modal();
    ~Modal();
};




class CloseGameModal : public Modal {
private:

public:
    CloseGameModal();
    ~CloseGameModal();
}













