#pragma once
#include <string>
#include <list>
#include <memory>
#include <cstdint>
#include <utility>

namespace nodes {
	struct Node;
	using NodePtr = std::unique_ptr<Node>;
	using NodeId = uint64_t;

	struct Node {
		explicit Node(std::string name);
		~Node() = default;

		static NodePtr create(const std::string& name) {
			return std::make_unique<Node>(name);
		}

		int getIndexOfChild(NodeId uid) const;
		void removeChild(NodeId uid);
		void insertChild(int index, NodePtr node);
		void addChild(NodePtr node);
		NodePtr release();

		NodeId uid() const {
			return _uid;
		}
		const std::string& name() const {
			return _name;
		}
		const char* name_c() const {
			return _name.c_str();
		}
		Node* parent() const {
			return _parent;
		}
		const std::list<NodePtr>& children() const {
			return _children;
		}
		bool expanded() const {
			return _expanded;
		}
		size_t size() const {
			return _children.size();
		}
		bool empty() const {
			return _children.empty();
		}

		NodeId _uid = 0;
		std::string _name;
		Node* _parent = nullptr;
		std::list<NodePtr> _children;
		bool _expanded = false;
	};
}
