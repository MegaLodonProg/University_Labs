#pragma once

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "../../model/include/Brigade.hpp"
#include "WorkersScreen.hpp"
#include "ActionsScreen.hpp"
#include "StatsScreen.hpp"
#include "WorkZonesScreen.hpp"
#include "ImportExportScreen.hpp"

class MainScreen {
private:
    Brigade& model_; // Ссылка на бизнес-логику (из модели)
    int selected_tab = 0;

    // Экземпляры экранов - каждый отвечает за свою вкладку
    WorkersScreen workers_screen_;
    WorkZonesScreen work_zones_screen_;
    ActionsScreen actions_screen_;
    StatsScreen stats_screen_;
    ImportExportScreen import_export_screen_;

public:
    MainScreen(Brigade& model);
    ftxui::Component createUI(); // Создает весь интерфейс
};