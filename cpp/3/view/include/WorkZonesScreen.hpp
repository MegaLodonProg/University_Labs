#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "../../model/include/Brigade.hpp"

class WorkZonesScreen {
private:
    Brigade& model_;
    int selected_zone_ = 0;
    bool show_details_ = false;

    ftxui::Element renderZonesList();
    ftxui::Element renderZoneDetails();

public:
    WorkZonesScreen(Brigade& model);
    ftxui::Component create();
};