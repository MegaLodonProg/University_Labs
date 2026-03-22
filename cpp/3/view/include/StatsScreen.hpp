#pragma once

#include <ftxui/component/component.hpp>
#include "../../model/include/Brigade.hpp"

class StatsScreen {
private:
    Brigade& model_;

public:
    StatsScreen(Brigade& model);
    ftxui::Component create();

private:
    ftxui::Component createWillCompleteForm(std::shared_ptr<int> active_form);
    ftxui::Component createProblematicZoneForm(std::shared_ptr<int> active_form);
    ftxui::Component createSimulateDayForm(std::shared_ptr<int> active_form);
};