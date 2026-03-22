#pragma once

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "../../model/include/Brigade.hpp"
#include "Workman.hpp"
#include "Master.hpp"

class ActionsScreen {
private:
    Brigade& model_;

public:
    ActionsScreen(Brigade& model);
    ftxui::Component create();

private:
    // Действия с работниками
    ftxui::Component createHireForm(std::shared_ptr<int> active_form);
    ftxui::Component createFireForm(std::shared_ptr<int> active_form);
    ftxui::Component createModifyForm(std::shared_ptr<int> active_form);
    ftxui::Component createFriendshipForm(std::shared_ptr<int> active_form);
    ftxui::Component createEnmityForm(std::shared_ptr<int> active_form);
    ftxui::Component createPromoteForm(std::shared_ptr<int> active_form);

    // Действия с участками
    ftxui::Component createCreateZoneForm(std::shared_ptr<int> active_form);
    ftxui::Component createModifyZoneForm(std::shared_ptr<int> active_form);
    ftxui::Component createAssignWorkerForm(std::shared_ptr<int> active_form);
    ftxui::Component createZoneProductivityForm(std::shared_ptr<int> active_form);
};