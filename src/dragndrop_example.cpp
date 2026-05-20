#include "dragndrop_example.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "tasks.hpp"
#include <algorithm>
#include <vector>
#include <list>
#include "icons.hpp"
#include <string_view>
#include "Node.hpp"

struct MyWindow::Impl {
	nodes::Node* selected = nullptr;
	nodes::Node* dragged = nullptr;
	nodes::Node* dropTarget = nullptr;

	enum class DropType {
		None,
		Before,
		Into,
		After
	};
	DropType dropType = DropType::None;

	std::string_view getIcon(nodes::Node* node) noexcept {
		return ICON_FA_CUBE;
	}

	bool isItemSelected(nodes::Node* node) {
		return selected == node;
	}

	bool isChildOf(const nodes::Node* target, const nodes::Node* parent) const {
		bool result = false;
		for (const auto& child : parent->children()) {
			if (target->uid() == child->uid()) {
				result = true;
				break;
			}
			result = isChildOf(target, child.get());
			if (result) {
				break;
			}
		}
		return result;
	}

	bool isChildOfAnySelectedNode(const nodes::Node* child) const {
		if (!child) {
			return false;
		}
		return isChildOf(child, selected);
	}

	bool canDropNode(nodes::Node* parent, nodes::Node* target) {
		nodes::Node* p = parent;
		while (p) {
			if (p == target) {
				return false;
			}
			p = p->parent();
		}
		return true;
	}

	void imguiShowTree(nodes::Node* node) {
		if (!node) {
			return;
		}
		const auto id = ImGui::GetID(node);
		ImGui::PushID(id);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		const bool isSelected = isItemSelected(node);
		const bool hasChildren = !node->empty();
		bool isOpened = node->expanded();

		if (hasChildren) {
			if (ImGui::SmallButton(isOpened ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT)) {
				isOpened = !isOpened;
				node->_expanded = isOpened;
			}
			ImGui::SameLine();
		} else {
			ImGui::Bullet();
		}

		auto icon = getIcon(node);
		ImGui::TextUnformatted(std::data(icon), std::next(std::data(icon), std::size(icon)));
		ImGui::SameLine();

		ImGui::Selectable(node->name_c(), isSelected, 0);

		if (ImGui::IsItemActivated()) {
			dragged = node;
			selected = node;
		}

		ImRect rect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		if (dragged && dragged != node && ImGui::IsMouseDown(0)) {
			ImVec2 mouse = ImGui::GetIO().MousePos;

			if (rect.Contains(mouse)) {
				float h = rect.GetHeight();
				float top = rect.Min.y + h * 0.25f;
				float bottom = rect.Max.y - h * 0.25f;

				dropTarget = node;

				if (mouse.y < top)
					dropType = DropType::Before;
				else if (mouse.y > bottom)
					dropType = DropType::After;
				else
					dropType = DropType::Into;
			}
		}

		if (dropTarget == node) {
			ImDrawList* dl = ImGui::GetWindowDrawList();

			if (dropType == DropType::Before) {
				dl->AddLine(
					{rect.Min.x, rect.Min.y},
					{rect.Max.x, rect.Min.y},
					IM_COL32(80,200,255,255), 2.f);
			}
			else if (dropType == DropType::After) {
				dl->AddLine(
					{rect.Min.x, rect.Max.y},
					{rect.Max.x, rect.Max.y},
					IM_COL32(80,200,255,255), 2.f);
			}
			else if (dropType == DropType::Into) {
				dl->AddRect(
					rect.Min, rect.Max,
					IM_COL32(80,200,255,255), 0.f, 0, 2.f);
			}
		}

		if (ImGui::IsMouseReleased(0)) {
			bool isAnyChildOfSelected = isChildOfAnySelectedNode(dropTarget);
			if (!isAnyChildOfSelected) {
				printf("[Error] cannot put parent into child\n");
			}
			if (dragged && dropTarget && dragged != dropTarget && !isAnyChildOfSelected) {
				TaskController::addFunc([dragged = dragged, dropType = dropType, dropTarget = dropTarget]() {
					if (dropType == DropType::Into) {
						dropTarget->addChild(dragged->release());
					}
					else {
						auto parent = dropTarget->_parent;
						if (!parent) return;

						int index = parent->getIndexOfChild(dropTarget->uid());
						if (dropType == DropType::After)
							index++;

						parent->insertChild(index, dragged->release());
					}
				});
			}
			dragged = nullptr;
			dropTarget = nullptr;
			dropType = DropType::None;
		}

		ImGui::TableNextColumn();
		{
			bool locked = true;
			if (ImGui::SmallButton(locked ? ICON_FA_LOCK : ICON_FA_LOCK_OPEN)) {
				//core::utils::node::setLocked(node, !locked, true);
			}
		}
		ImGui::SameLine();
		{
			bool visible = true;
			if (ImGui::SmallButton(visible ? ICON_FA_EYE : ICON_FA_EYE_LOW_VISION)) {
				//core::utils::node::setVisible(node, !visible, true);
			}
		}

		if (isOpened) {
			ImGui::TreePush(node);
			for (const auto& node : node->children()) {
				imguiShowTree(node.get());
			}
			ImGui::TreePop();
		}

		ImGui::PopID();
	}


	void draw() {
		ImGui::BeginChild("scrolling", ImVec2(-1, -1), true);
		const ImGuiTableFlags flags = ImGuiTableFlags_Borders;
		if (ImGui::BeginTable("##SceneObjects", 2, flags)) {
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed);
			imguiShowTree(rootNode.get());
			ImGui::EndTable();
		}
		ImGui::EndChild();
	}

	nodes::NodePtr rootNode = nullptr;
};

MyWindow::MyWindow() : d(new Impl()) {
	using namespace nodes;
	d->rootNode = Node::create("Root");

	Node* last_item = nullptr;
	for (int i = 0; i < 10; i++) {
		auto child = Node::create(std::string{"child"} + std::to_string(i));
		last_item = child.get();
		d->rootNode->addChild(std::move(child));
	}

	for (int i = 0; i < 10; i++) {
		auto child = Node::create(std::string{"nested_child"} + std::to_string(i));
		auto temp = child.get();
		last_item->addChild(std::move(child));
		last_item = temp;
	}
}

MyWindow::~MyWindow() {
	delete d;
}

void MyWindow::draw() {
	bool isOpen = ImGui::Begin("MyWindow");
	if (!isOpen) {
		return;
	}
	d->draw();

	ImGui::End();
}