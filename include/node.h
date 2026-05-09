#pragma once

#include "meta.h"
#include "option.h"

#include <memory>
#include <stdexcept>
#include <vector>
#include <functional>

namespace kinetic {

enum class NodeArenaCellState {
  Ready,
  Taken,
};

template <typename T>
class NodeArenaCell {
private:
  NodeArenaCellState _state;

  std::shared_ptr<T> _value;

  Option<size_t> _node_parent;

  std::vector<size_t> _node_child_list;

public:
  NodeArenaCell()
    : _state(NodeArenaCellState::Ready)
    , _value(nullptr)
    , _node_parent(Option<size_t>::none())
    , _node_child_list()
  {}

  KINETIC_GETTER(_state, state)

  Option<const T &> get() const {
    using ResultT = Option<const T &>;

    switch (get_state()) {
      case NodeArenaCellState::Ready:
        return ResultT::none();
      case NodeArenaCellState::Taken:
        if (_value == nullptr) {
          return ResultT::none();
        }

        return ResultT::some(*_value);
      default:
        throw std::runtime_error("unhandled NodeArenaCellState");
    }
  }

  bool clear() {
    switch (get_state()) {
      case NodeArenaCellState::Ready:
        return false;
      case NodeArenaCellState::Taken: {
        _value = nullptr;
        _state = NodeArenaCellState::Ready;
        return true;
      }
      default:
        throw std::runtime_error("unhandled NodeArenaCellState");
    }
  }

  bool set(std::shared_ptr<T> value) {
    switch (get_state()) {
      case NodeArenaCellState::Ready: {
        _value = value;
        _state = NodeArenaCellState::Taken;
        return true;
      }
      case NodeArenaCellState::Taken:
        return false;
      default:
        throw std::runtime_error("unhandled NodeArenaCellState");
    }
  }

  KINETIC_GETTER(_node_parent, parent)

  KINETIC_SETTER(Option<size_t>, _node_parent, parent)

  bool has_parent() const {
    return _node_parent.is_some();
  }

  std::vector<size_t> list_child() const {
    return _node_child_list;
  }

  void add_child(const size_t node_child) {
    if (has_child(node_child)) {
      return;
    }

    _node_child_list.emplace_back(node_child);
  }

  bool has_child(const size_t node_child) const {
    for (size_t x = 0; x < _node_child_list.size(); x++) {
      if (_node_child_list[x] == node_child) {
        return true;
      }
    }

    return false;
  }

  Option<size_t> get_child(const size_t index) const {
    if (index >= _node_child_list.size()) {
      return Option<size_t>::none();
    }

    return Option<size_t>::some(_node_child_list[index]);
  }
};

template <typename T>
class NodeArena {
private:
  std::vector<NodeArenaCell<T>> _cells;

public:
  NodeArena()
    : _cells()
  {}

  bool check_cell_index(const size_t index) const {
    return index < _cells.size();
  }

  kinetic::Option<size_t> find_cell_with_state(const NodeArenaCellState state) {
    using ResultT = kinetic::Option<size_t>;

    for (size_t x = 0; x < _cells.size(); x++) {
      if (_cells[x].get_state() == state) {
        return ResultT::some(x);
      }
    }

    return ResultT::none();
  }

  size_t create_cell(std::shared_ptr<T> value) {
    const kinetic::Option<size_t> cell_index_ready = find_cell_with_state(NodeArenaCellState::Ready);
    if (cell_index_ready.is_none()) {
      auto cell = NodeArenaCell<T>();
      cell.set(value);

      _cells.emplace_back(cell);

      return _cells.size() - 1;
    } else {
      const size_t cell_index = cell_index_ready.unwrap();

      auto & cell = _cells[cell_index];
      cell.clear();
      cell.set(value);

      return cell_index;
    }
  }

  bool apply_cell(const size_t index, const std::function<void(NodeArenaCell<T> & mut_ref)> & fn) {
    if (!check_cell_index(index)) {
      return false;
    }

    if (_cells[index].get_state() != NodeArenaCellState::Taken) {
      return false;
    }

    fn(_cells[index]);

    return true;
  }

  template <typename R>
  Option<R> read_cell(const size_t index, const std::function<R(const NodeArenaCell<T> & mut_ref)> & fn) const {
    if (!check_cell_index(index)) {
      return Option<R>::none();
    }

    if (_cells[index].get_state() != NodeArenaCellState::Taken) {
      return Option<R>::none();
    }

    return Option<R>::some(fn(_cells[index]));
  }
};

template <typename T>
class Node {
private:
  std::shared_ptr<NodeArena<T>> _arena;

  size_t _index;

  bool apply(const std::function<void(NodeArenaCell<T> & cell)> & fn) const {
    return _arena->apply_cell(_index, fn);
  }

  template <typename R>
  Option<R> read(const std::function<R(const NodeArenaCell<T> & cell)> & fn) const {
    return _arena->read_cell(_index, fn);
  }

public:
  Node(std::shared_ptr<NodeArena<T>> arena, size_t index)
    : _arena(std::move(arena))
    , _index(index)
  {}

  Node()
    : _arena(nullptr)
    , _index(0)
  {}

  KINETIC_GETTER(_index, index)

  static Node<T> of(std::shared_ptr<NodeArena<T>> arena, size_t index) {
    return Node<T>(arena, index);
  }

  static Node<T> create(std::shared_ptr<NodeArena<T>> arena, std::shared_ptr<T> value) {
    return Node<T>::of(arena, arena->create_cell(value));
  }

  Option<const T &> get() const {
    return read<const T &>([](const NodeArenaCell<T> & cell) -> const T & {
      return cell.get();
    });
  }

  Option<Node<T>> get_parent() const {
    using ResultT = Option<Node<T>>;

    const auto parent_opt = read<Option<size_t>>([](const NodeArenaCell<T> & cell) -> Option<size_t> {
      return cell.get_parent();
    });
    if (parent_opt.is_none()) {
      return ResultT::none();
    }

    const auto parent = parent_opt.unwrap();
    if (parent.is_none()) {
      return ResultT::none();
    }

    return ResultT::some(Node<T>::of(_arena, parent.unwrap()));
  }

  void set_parent(const size_t node_parent_index) const {
    apply([node_parent_index](NodeArenaCell<T> & cell) -> void {
      cell.set_parent(Option<size_t>::some(node_parent_index));
    });
  }

  std::vector<size_t> list_child() const {
    return read([](const NodeArenaCell<T> & cell) -> std::vector<size_t> {
      return cell.list_child();
    });
  }

  Node<T> create_child(std::shared_ptr<T> value) const {
    auto node = create(_arena, value);
    node.set_parent(get_index());

    const auto node_index = node.get_index();

    apply([node_index](NodeArenaCell<T> & cell) -> void {
      cell.add_child(node_index);
      return;
    });

    return node;
  }

  Option<Node<T>> get_child_abs(const size_t index) const {
    using ResultT = Option<Node<T>>;

    bool flag_child_exists = false;
    apply([index, flag_child_exists](NodeArenaCell<T> & cell) -> void {
      flag_child_exists = cell.has_child();
    });

    if (!flag_child_exists) {
      return ResultT::none();
    }

    return ResultT::some(Node<T>(_arena, index));
  }

  Option<Node<T>> get_child(const size_t index) const {
    using ResultT = Option<Node<T>>;

    auto child = Option<size_t>::none();
    apply([index, child](NodeArenaCell<T> & cell) -> void {
      child = cell.get_child(index);
    });

    if (child.is_none()) {
      return ResultT::none();
    }

    return ResultT::some(Node<T>(_arena, child.unwrap()));
  }

  void walk(const std::function<void (const T &)> & fn) const {
    const Option<const T &> value_opt = get();
    if (value_opt.is_some()) {
      fn(value_opt.unwrap());
    }

    for (const size_t child_index : list_child()) {
      get_child_abs(child_index).unwrap().walk(fn);
    }
  }
};

}
