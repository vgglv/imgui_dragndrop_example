#include "Node.hpp"
#include <algorithm>
#include <ranges>

namespace nodes {
	static NodeId last_idx = 0;

	Node::Node(std::string name) : _name(std::move(name)) {
		_uid = ++last_idx;
	}

	int Node::getIndexOfChild(NodeId uid) const {
		auto it = std::ranges::find_if(_children, [uid](const NodePtr& n) {
			return uid == n->uid();
		});
		if (it != _children.end()) {
			return std::distance(_children.begin(), it);
		}
		return -1;
	}

	void Node::removeChild(NodeId uid) {
		auto it = std::ranges::find_if(_children, [uid](const NodePtr& n) {
			return uid == n->_uid;
		});
		if (it != _children.end()) {
			_children.erase(it);
		}
	}

	NodePtr Node::release() {
		if (!_parent) {
			return nullptr;
		}
		auto it = std::ranges::find_if(_parent->_children, [uid = _uid](const NodePtr& n) {
			return uid == n->_uid;
		});
		if (it == _parent->_children.end()) {
			return nullptr;
		}
		NodePtr result = NodePtr(it->release());
		_parent->_children.erase(it);
		return result;
	}

	void Node::insertChild(int index, NodePtr node) {
		if (auto p = node->parent()) {
			p->removeChild(node->uid());
		}

		node->_parent = this;

		if (_children.empty()) {
			_children.push_back(std::move(node));
		} else if (index >= 0) {
			if (static_cast<size_t>(index) < _children.size()) {
				_children.emplace(std::next(_children.begin(), index), std::move(node));
			} else {
				_children.push_back(std::move(node));
			}
		} else {
			if (static_cast<size_t>(-index - 1) < size()) {
				_children.emplace(std::prev(_children.end(), -index - 1), std::move(node));
			} else {
				_children.push_front(std::move(node));
			}
		}
	}

	void Node::addChild(NodePtr node) {
		node->_parent = this;
		_children.emplace_back(std::move(node));
	}
}
