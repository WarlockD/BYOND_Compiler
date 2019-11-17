#pragma once


#include "queue.h"

#include <type_traits>
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
	struct entry {
		std::pair<KEY, VALUE> value;
		// The top is designed for a que while next_hash is designed for the linked
		// list on the has lookup
		struct { entry* next;  entry** prev; } next_list;
		struct { entry* next;  entry* prev; } next_hash;
		template<typename ARGS...>
		entry(const KEY& key, ARGS ... args) : 
			value(key, std::forward<ARGS>(args)...), 
			next_list{ nullptr, &next_list.next }, 
			next_hash{ nullptr, nullptr }{}
	};
	TAILQ_HEAD(entry_list, list);
	LIST_HEAD(entry_head, entry); 
	HASHER _hasher;
	std::vector<entry_list> _buckets;
	entry_list _list;

	std::pair<size_t,entry*> _findentry(const KEY& key) {
		size_t h = _hasher(key) % _buckets.size();
		entry* e=nullptr;
		LIST_FOREACH(e, &_buckets[h], next) {
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
		auto nnode = new entry{ std::make_pair(key, std::move(v)) };
		LIST_INSERT_HEAD(&_buckets[h], nnode, next);
		TAILQ_INSERT_TAIL(&_list, nnode, list);
	}
	entry* _find_or_add(const KEY& key, const VALUE& v) {
		auto fnode = _findentry(key);
		if (fnode.second) {
			fnode.second->value.second = v;
			return node;
		}
		auto nnode = new entry{ std::make_pair(key, v };
		LIST_INSERT_HEAD(&_buckets[h], nnode, next);
		TAILQ_INSERT_TAIL(&_list, nnode, list);
	}
public:


	class const_iterator {
	protected:
		entry* _entry;
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = ptrdiff_t;
		using value_type = VALUE;
		using pointer = const VALUE*;
		using reference = const VALUE&;
		using const_pointer = const VALUE*;
		using const_reference = const VALUE&;
		
		const_iterator(entry* e) : _entry(e) {}
		const_iterator operator++()  {
			_entry = TAILQ_NEXT(_entry, list);
			return *this;
		}
		const_iterator operator++() {
			const_iterator ret(*this);
			_entry = TAILQ_NEXT(_entry, list);
			return ret;
		}
		const_iterator operator--() {
			_entry = TAILQ_PREV(_entry, entry_head, list);
			return *this;
		}
		const_iterator operator--() {
			const_iterator ret(*this);
			_entry = TAILQ_PREV(_entry, entry_head, list);
			return ret;
		}
		reference operator*() const { return _entry->value; }
		pointer operator->() const { return &_entry->value; }
		void swap(const_iterator& r) { std::swap(r._entry, _entry); }
		bool operator==(const const_iterator& r) const { return _entry == r._entry; }
		bool operator!=(const const_iterator& r) const { return _entry != r._entry; }
	};
	class iterator : public const_iterator {
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using difference_type = ptrdiff_t;
		using value_type = VALUE;
		using pointer = VALUE*;
		using reference = VALUE&;
		using const_pointer = const VALUE*;
		using const_reference = const VALUE&;

		iterator(entry* e) : const_iterator(e) {}
		reference operator*()  { return _entry->value; }
		pointer operator->()  { return &_entry->value; }
		bool operator==(const iterator& r) const { return _entry == r._entry; }
		bool operator!=(const iterator& r) const { return _entry != r._entry; }
	};

	bool empty() const { return TAILQ_EMPTY(list); }
	const_iterator begin() const { return const_iterator(TAILQ_FIRST(&_list)); }
	const_iterator end() const { return const_iterator(nullptr); }
	iterator begin()  { return const_iterator(TAILQ_FIRST(&_list)); }
	iterator end()  { return const_iterator(nullptr); }
	iterator find(const KEY& key) { return iterator(_findentry(key).second); }
	const_iterator find(const KEY& key) const { return const_iterator(_findentry(key).second); }
	// size() is stuipd as we have to ileterate though it, so we don't use it
	void push_back()

};