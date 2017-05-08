#pragma once

#include <vector>
#include <algorithm>

#include <boost/intrusive/list.hpp>
#include <boost/intrusive/unordered_set.hpp>

class LRUCacheWithBoost
{
public:
	explicit LRUCacheWithBoost (size_t maxSize)
		:m_maxSize (maxSize),
		m_mapBuckets (std::max (maxSize / 2, size_t (100))), //© folly
		m_mapTraits (m_mapBuckets.data(), m_mapBuckets.size()),
		m_map (m_mapTraits)
	{
	}

	~LRUCacheWithBoost()
	{
		m_map.clear();
		m_list.clear_and_dispose (std::default_delete<Node>());
	}

public:
	typedef int ValueType;
	typedef int KeyType;

	typedef std::hash<KeyType> HasherType;

	ValueType Get (KeyType key) noexcept
	{
		const auto findRes = m_map.find (key, HasherType(), KeyValueEqual());
		if (findRes == m_map.end())
			return -1;

		Touch (*findRes);
		return findRes->m_value;
	}

	void Set (KeyType key, ValueType value)
	{
		const auto findRes = m_map.find (key, HasherType(), KeyValueEqual());
		if (findRes == m_map.end())
		{
			const auto newNode = new Node (key, value);
			m_map.insert (*newNode);
			m_list.push_front (*newNode);

			if (m_map.size() > m_maxSize)
			{
				m_map.erase (m_map.iterator_to (m_list.back()));
				m_list.pop_back_and_dispose (std::default_delete<Node>());
			}
		}
		else
		{
			findRes->m_value = value;
			Touch (*findRes);
		}
	}

private:
	typedef boost::intrusive::link_mode<boost::intrusive::safe_link> LinkMode;

	struct Node : public boost::intrusive::unordered_set_base_hook<LinkMode>, public boost::intrusive::list_base_hook<LinkMode>
	{
		Node (KeyType key, ValueType value) :m_key (key), m_value (value)
		{
		}

		const KeyType m_key;
		ValueType m_value;

		friend bool operator== (const Node& lhs, const Node& rhs)
		{
			return lhs.m_key == rhs.m_key;
		}

		friend std::size_t hash_value (const Node& node)
		{
			return HasherType()(node.m_key);
		}
	};

	struct KeyValueEqual
	{
		bool operator()(KeyType lhs, const Node& rhs)
		{
			return lhs == rhs.m_key;
		}

		bool operator()(const Node& lhs, const KeyType rhs)
		{
			return lhs.m_key == rhs;
		}
	};

	typedef boost::intrusive::unordered_set<Node> NodeMap;
	typedef boost::intrusive::list<Node> NodeList;

	void Touch (Node& node)
	{
		m_list.erase (m_list.iterator_to (node));
		m_list.push_front (node);
	}

private:
	const size_t m_maxSize;

	std::vector<NodeMap::bucket_type> m_mapBuckets;
	NodeMap::bucket_traits m_mapTraits;
	NodeMap  m_map;
	NodeList m_list;
};