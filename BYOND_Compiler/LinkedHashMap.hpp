#pragma once


#include "queue.h"

#include <forward_list>
#include <vector>

// ok, so, while there are many ways to do this, the most space efficent way is to control the buckets themselves.
// Hence, we do the buckets ourseves and do two forward lists in each buckete.


template<typename KEY, typename VALUE,typename HASHER=std::hash<KEY>>
class LinkedHashMap {
public:
	using iterator_category = typename std::bidirectional_iterator_tag;
	using difference_type = typename ptrdiff_t;
	using value_type = typename std::pair<KEY,VALUE>;
	using pointer = const VALUE*;
	using reference = const VALUE&;
	using const_pointer = const VALUE*;
	using const_reference = const VALUE&;
private:
	static constexpr size_t lmod(size_t s, size_t, size) { return  s & (size - 1); }
	struct node {
		std::pair<KEY, VALUE> value;
		LIST_ENTRY(entry) hash;
		TAILQ_ENTRY(entry) list;
	};
	TAILQ_HEAD(list_t, node) _list;
	LIST_HEAD(hash_t, node);
	HASHER _hasher;
	std::vector<hash_t> _buckets;
	entry_list ;
	void _insert_node(hash_t& bucket, node* n) {
		LIST_INSERT_HEAD(&bucket, n, hash);
		TAILQ_INSERT_TAIL(&_list, n, list);
	}
	void _remove_node(node* n) {
		LIST_REMOVE(n, hash);
		TAILQ_REMOVE(&_list, n, list);
	}
	std::pair<size_t,entry*> _findentry(const KEY& key) {
		size_t h = _hasher(key) % _buckets.size();
		entry* e=nullptr;
		LIST_FOREACH(e, &_buckets[h], hash) {
			if (e->value.first == key)
				return std::make_pair(h, e);
		}
		return std::pair<h, nullptr>;
	}
	entry* _find_or_add(const KEY& key, VALUE&& v) {
		auto fnode = _findentry(key);
		if (fnode.second) {
			fnode.second->value.second = std::move(v);
			return node;
		}
		_insert_node(_buckets[h], new entry{ std::make_pair(key, std::move(v)) });
	}
	entry* _find_or_add(const KEY& key, const VALUE& v) {
		auto fnode = _findentry(key);
		if (fnode.second) {
			fnode.second->value.second = v;
			return node;
		}
		_insert_node(_buckets[h], new entry{ std::make_pair(key, v) });
	}
public:


	class const_iterator {
	protected:
		const entry* _node;
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = ptrdiff_t;
		using value_type = VALUE;
		using pointer = const VALUE*;
		using reference = const VALUE&;
		using const_pointer = const VALUE*;
		using const_reference = const VALUE&;
		const_iterator() : _node(nullptr) {}
		const_iterator(const entry* e) : _node(e) {}
		const_iterator& operator++()  {
			_node = TAILQ_NEXT(_node, list);
			return *this;
		}
		const_iterator operator++(int) {
			const_iterator ret = *this;
			_node = TAILQ_NEXT(_node, list);
			return ret;
		}
		& operator--() {
			_node = TAILQ_PREV(_node, list_t, list);
			return *this;
		}
		const_iterator operator--(int) {
			const_iterator ret(*this);
			_node = TAILQ_PREV(_node, list_t, list);
			return ret;
		}
		reference operator*() const { return _node->value; }
		pointer operator->() const { return  std::addressof(_node->value); }
		bool operator==(const const_iterator& r) const { return _node == r._node; }
		bool operator!=(const const_iterator& r) const { return _node != r._node; }
		friend class const_iterator;
	};
	class const_iterator {
	protected:
		entry* _node;
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = ptrdiff_t;
		using value_type = VALUE;
		using pointer = VALUE*;
		using reference = VALUE&;
		using const_pointer = const VALUE*;
		using const_reference = const VALUE&;
		iterator() : _node(nullptr) {}
		iterator(entry* e) : _node(e) {}
		iterator& operator++() {
			_node = TAILQ_NEXT(_node, list);
			return *this;
		}
		iterator operator++(int) {
			const_iterator ret = *this;
			_node = TAILQ_NEXT(_node, list);
			return ret;
		}
		iterator& operator--() {
			_node = TAILQ_PREV(_node, list_t, list);
			return *this;
		}
		iterator operator--(int) {
			const_iterator ret(*this);
			_node = TAILQ_PREV(_node, list_t, list);
			return ret;
		}
		reference operator*()  { return _node->value; }
		pointer operator->()  { return  std::addressof(_node->value); }
		const_reference operator*() const { return _node->value; }
		const_pointer operator->() const { return  std::addressof(_node->value); }
		bool operator==(const iterator& r) const { return _node == r._node; }
		bool operator!=(const iterator& r) const { return _node != r._node; }
		friend class iterator;
	};

	bool empty() const { return TAILQ_EMPTY(list); }
	const_iterator begin() const { return const_iterator(TAILQ_FIRST(&_list)); }
	const_iterator end() const { return const_iterator(nullptr); }
	iterator begin()  { return const_iterator(TAILQ_FIRST(&_list)); }
	iterator end()  { return const_iterator(nullptr); }
	iterator find(const KEY& key) { return iterator(_findentry(key).second); }
	const_iterator find(const KEY& key) const { return const_iterator(_findentry(key).second); }
	// size() is stuipd as we have to ileterate though it, so we don't use it


};

template<typename KEY, typename VALUE, typename HASHER>
inline bool operator==(const LinkedHashMap<KEY, VALUE, HASHER>::iterator& l, const LinkedHashMap<KEY, VALUE, HASHER>::const_iterator& r)  { return l._node == r._node; }
inline bool operator!=(const LinkedHashMap<KEY, VALUE, HASHER>::iterator& l, const LinkedHashMap<KEY, VALUE, HASHER>::const_iterator& r)  { return l._node != r._node; }
