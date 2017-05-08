#pragma once

#include <vector>
#include <algorithm>

#include <boost/intrusive/list.hpp>
#include <boost/intrusive/unordered_set.hpp>

class LRUCacheWithBoost
{
public:
	explicit LRUCacheWithBoost (size_t maxSize)
		:m_mapBuckets (std::max (maxSize / 2, size_t (100))), //© folly
		m_mapTraits (m_mapBuckets.data(), m_mapBuckets.size()),
		m_map (m_mapTraits)
	{
		m_nodes.reserve (maxSize);
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

	void Set (KeyType key, ValueType value) noexcept
	{
		const auto findRes = m_map.find (key, HasherType(), KeyValueEqual());
		if (findRes == m_map.end())
		{
			if (m_nodes.capacity() == m_nodes.size())
			{
				Node& back = m_list.back();
				m_list.pop_back();
				m_map.erase (m_map.iterator_to (back));

				back.m_key = key;
				back.m_value = value;

				m_list.push_front (back);
				m_map.insert (back);
			}
			else
			{
				m_nodes.emplace_back (key, value);
				m_map.insert (m_nodes.back());
				m_list.push_front (m_nodes.back());
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

		KeyType m_key;
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
	std::vector<Node> m_nodes;

	std::vector<NodeMap::bucket_type> m_mapBuckets;
	NodeMap::bucket_traits m_mapTraits;
	NodeMap  m_map;
	NodeList m_list;
};