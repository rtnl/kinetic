#pragma once

#include "meta.h"
#include "option.h"

#include <memory>
#include <vector>

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
  NodeArenaCell(Option<size_t> node_parent)
    : _state(NodeArenaCellState::Ready)
    , _value(nullptr)
    , _node_parent(node_parent)
    , _node_child_list()
  {}

  KINETIC_GETTER(_state, state)

  Option<const T &> get() const {
    return *_value;
  }

  void clear() {
    switch (get_state()) {
      case NodeArenaCellState::Ready: {
        _value = nullptr;
        break;
      }
      default: {
        break;
      }
    }
  }

  void set(std::shared_ptr<T> value) {
    clear();
    _value = value;
  }

  KINETIC_SETTER(Option<size_t>, _node_parent, parent)

  bool has_parent() const {
    return _node_parent.is_some();
  }

  std::vector<size_t> list_child() const {
    return _node_child_list;
  }

  void add_child(const size_t node_child) {
    bool flag_child_present = false;

    for (size_t x = 0; x < _node_child_list.size(); x++) {
      if (_node_child_list[x] == node_child) {
        flag_child_present = true;
        break;
      }
    }
    if (flag_child_present) {
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
      auto cell = NodeArenaCell<T> (value);

      _cells.emplace_back(cell);

      return cell.size() - 1;
    } else {
      const size_t cell_index = cell_index_ready.unwrap();

      auto & cell = _cells[cell_index];

      cell.set(value);

      return cell_index;
    }
  }

  bool apply_cell(const size_t index, const void (*fn) (NodeArenaCell<T> & mut_ref)) {
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
  Option<R> read(const size_t index, const R (*fn) (const NodeArenaCell<T> & ref)) {
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

  bool apply(const void (*fn) (NodeArenaCell<T> & cell)) {
    _arena->apply_cell(_index, fn);
  }

  template <typename R>
  Option<R> read(const R (*fn) (const NodeArenaCell<T> & cell)) {
    return _arena.read_cell(_index, fn);
  }

public:
  Node(std::shared_ptr<NodeArena<T>> arena, size_t index)
    : _arena(std::move(arena))
    , _index(index)
  {}

  KINETIC_GETTER(_index, index)

  static Node<T> create(std::shared_ptr<NodeArena<T>> arena, std::shared_ptr<T> value) {
    const auto index = arena->create_cell(value);

    auto node = Node<T>(arena, index);

    return node;
  }

  const T & get() const {
    return read<const T &>([](const NodeArenaCell<T> & cell) -> const T & {
      return cell.get();
    });
  }

  void set_parent(const Node<T> & node_parent) {
    apply([node_parent](NodeArenaCell<T> & cell) -> void {
      cell->set_parent(Option<size_t>::some(node_parent.get_index()));
    });
  }

  std::vector<size_t> list_child() const {
    return read([](const NodeArenaCell<T> & cell) -> std::vector<size_t> {
      return cell.list_child();
    });
  }

  Node<T> add_child(std::shared_ptr<T> value) {
    auto node = create(_arena, value);

    node.set_parent(this);

    const auto node_index = node.get_index();

    apply([node_index](NodeArenaCell<T> & cell) -> void {
      cell->add_child(node_index);
    });
  }

  Option<Node<T>> get_child_abs(const size_t index) {
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

  Option<Node<T>> get_child(const size_t index) {
    using ResultT = Option<Node<T>>;

    auto child = Option<size_t>::none();

    apply([index, child](NodeArenaCell<T> & cell) -> void {
      child = cell.get_child(index);
    });

    if (child.is_none()) {
      return ResultT::none();
    }

    const auto child_index = child.unwrap();

    return ResultT::some(Node<T>(_arena, child_index));
  }

  void walk(const void (*fn)(const T & value)) {
    fn(get());

    for (const size_t child_index : list_child()) {
      walk(get_child_abs(child_index).unwrap());
    }
  }
};

}
