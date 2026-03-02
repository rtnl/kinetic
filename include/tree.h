#pragma once

#include <vector>
#include <memory>

#include "meta.h"
#include "uuid.h"
#include "option.h"

namespace kinetic {

template <typename T>
class TreeNode {
private:
  Uuid _id;

  std::shared_ptr<TreeNode<T>> _parent;

  T _value;

  std::vector<std::shared_ptr<TreeNode<T>>> _children;

public:
  TreeNode(std::shared_ptr<TreeNode<T>> parent, T value)
    : _id(Uuid::new_v4())
    , _parent(parent)
    , _value(value)
    , _children({})
  {}

  static std::shared_ptr<TreeNode<T>> of(std::shared_ptr<TreeNode<T>> parent, T value) {
    auto child = std::make_shared<TreeNode<T>>(parent, value);

    if (parent != nullptr) {
      parent.get()->add_child(child);
    }

    return child;
  }

  static std::shared_ptr<TreeNode<T>> of(T value) {
    return TreeNode<T>::of(nullptr, value);
  }

  KINETIC_GETTER(_id, id)

  kinetic::Option<std::shared_ptr<TreeNode<T>>> get_parent() const {
    if (!has_parent()) {
      return kinetic::Option<std::shared_ptr<TreeNode<T>>>::none();
    }

    return kinetic::Option<std::shared_ptr<TreeNode<T>>>::some(_parent);
  }

  bool has_parent() const {
    return _parent != nullptr;
  }

  KINETIC_GETTER(_value, value)

  size_t get_child_count() const {
    return _children.size();
  }

  kinetic::Option<std::shared_ptr<TreeNode<T>>> get_child(const size_t index) const {
    if (index >= _children.size()) {
      return kinetic::Option<std::shared_ptr<TreeNode<T>>>::none();
    }

    return kinetic::Option<std::shared_ptr<TreeNode<T>>>::some(_children.at(index));
  }

  bool has_child(Uuid uuid) const {
    for (const std::shared_ptr<TreeNode<T>> & it : _children) {
      if (it.get()->get_id() == uuid) {
        return true;
      }
    }

    return false;
  }

  bool add_child(std::shared_ptr<TreeNode<T>> value) {
    if (has_child(value.get()->get_id())) {
      return false;
    }

    _children.emplace_back(value);

    return true;
  }
};

}
