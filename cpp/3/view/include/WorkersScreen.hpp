#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "../../model/include/Brigade.hpp"

class WorkersScreen {
private:
    Brigade& model_;
    int selected_worker_ = 0;
    bool show_details_ = false;

    ftxui::Element renderWorkersList();
    ftxui::Element renderWorkerDetails();

public:
    WorkersScreen(Brigade& model);
    ftxui::Component create();
};