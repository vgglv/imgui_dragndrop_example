#include "dragndrop_example.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include "tasks.hpp"
#include <algorithm>
#include <vector>
#include <list>
#include "icons.hpp"
#include <string_view>

class Node {
public:
	Node() = default;
	~Node() {
		for (auto node : children) {
			delete node;
		}
		children.clear();
	}

	int getIndexOfChild(Node* nd) const {
		auto it = std::find(children.begin(), children.end(), nd);
		if (it != children.end()) {
			return std::distance(children.begin(), it);
		}
		return -1;
	}

	void removeChild(Node* node) {
		auto it = std::ranges::find(children, node);
		if (it != children.end()) {
			children.erase(it);
		}
	}

	void insertChild(int index, Node* nd) {
		if (auto p = nd->parent) {
			p->removeChild(nd);
		}

		nd->parent = this;

		if (children.empty()) {
			children.push_back(nd);
		} else if (index >= 0) {
			if (static_cast<size_t>(index) < children.size()) {
				children.emplace(std::next(children.begin(), index), nd);
			} else {
				children.push_back(nd);
			}
		} else {
			if (static_cast<size_t>(-index - 1) < children.size()) {
				children.emplace(std::prev(children.end(), -index - 1), nd);
			} else {
				children.push_front(nd);
			}
		}
	}
	void addChild(Node* node) {
		children.emplace_back(node);
		node->parent = this;
	}

	std::string name;
	Node* parent = nullptr;
	std::list<Node*> children;
	bool expanded = false;
};

struct MyWindow::Impl {
	Node* selected = nullptr;
	Node* dragged = nullptr;

	std::string_view getIcon(Node* node) noexcept {
		return ICON_FA_CUBE;
	}

	bool isItemSelected(Node* node) {
		return selected == node;
	}

	bool canDropNode(Node* parent, Node* target) {
		Node* p = parent;
		while (p) {
			if (p == target) {
				return false;
			}
			p = p->parent;
		}
		return true;
	}

	void imguiShowTree(Node* node) {
		if (!node) {
			return;
		}
		const auto id = ImGui::GetID(node);
		ImGui::PushID(id);

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		const bool isSelected = isItemSelected(node);
		const bool hasChildren = !node->children.empty();
		bool isOpened = node->expanded;

		if (ImGui::IsDragDropActive() && node->parent) {
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
			ImGui::InvisibleButton("##drop_area", ImVec2(-1, 6.f));
			ImGui::PopStyleVar();
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SceneExplorer")) {
					if (auto nd = dragged; nd) {
						if (auto parent = node->parent; parent) {
							if (canDropNode(parent, nd)) {
								auto index = parent->getIndexOfChild(node);
								TaskController::addFunc([nd, parent, index]() {
									if (auto p = nd->parent; p)
										p->removeChild(nd);
									parent->insertChild(index, nd);
								});
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
		}

		if (hasChildren) {
			if (ImGui::SmallButton(isOpened ? ICON_FA_CARET_DOWN : ICON_FA_CARET_RIGHT)) {
				isOpened = !isOpened;
				node->expanded = isOpened;
			}
			ImGui::SameLine();
		} else {
			ImGui::Bullet();
		}

		auto icon = getIcon(node);
		ImGui::TextUnformatted(std::data(icon), std::next(std::data(icon), std::size(icon)));
		ImGui::SameLine();

		if (ImGui::Selectable(node->name.c_str(), isSelected, 0)) {
			selected = node;
		}
		if (node->parent) {
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
				dragged = node;
				ImGui::SetDragDropPayload("SceneExplorer", nullptr, 0);
				ImGui::Text("%s", node->name.c_str());
				ImGui::EndDragDropSource();
			}
		}

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SceneExplorer")) {
				if (auto nd = dragged; nd) {
					if (canDropNode(node, nd)) {
						TaskController::addFunc([nd, node]() {
							if (auto p = nd->parent; p)
								p->removeChild(nd);
							node->addChild(nd);
						});
					}
					dragged = nullptr;
				}
			}
			ImGui::EndDragDropTarget();
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
			for (auto& nd : node->children) {
				imguiShowTree(nd);
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
			imguiShowTree(rootNode);
			ImGui::EndTable();
		}
		ImGui::EndChild();
	}

	Node* rootNode = nullptr;
};

MyWindow::MyWindow() : d(new Impl()) {
	d->rootNode = new Node();
	d->rootNode->name = "Root";

	Node* last_item = nullptr;
	for (int i = 0; i < 10; i++) {
		auto child = new Node();
		child->name = std::string{"child"} + std::to_string(i);
		d->rootNode->addChild(child);
		last_item = child;
	}

	for (int i = 0; i < 10; i++) {
		auto child = new Node();
		child->name = std::string{"nested_child"} + std::to_string(i);

		last_item->addChild(child);
		last_item = child;
	}
}

MyWindow::~MyWindow() {
	delete d->rootNode;
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