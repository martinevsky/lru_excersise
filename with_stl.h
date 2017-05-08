#pragma once

#include <list>
#include <unordered_map>

class LRUCacheWithStl 
{
public:
	explicit LRUCacheWithStl (size_t capacity):m_maxSize (capacity)
    {
    }
    
    int Get (int key) noexcept
    {
        const auto findRes = m_map.find (key);
        if (findRes == m_map.end())
            return -1;
            
        touch (findRes->second.second);
        return findRes->second.first;
    }
    
    void Set (int key, int value) 
    {
        const auto findRes = m_map.find (key);
        if (findRes == m_map.end())
        {
            m_list.push_front (key);
            try
            {
                m_map.insert (std::make_pair(key, std::make_pair (value, m_list.begin())));
            }
            catch (...)
            {
                m_list.pop_front();
                throw;
            }
            
            if (m_map.size() > m_maxSize)
            {
                m_map.erase (m_list.back());
                m_list.pop_back();
            }
        }
        else
        {
            findRes->second.first = value;
            touch (findRes->second.second);
        }
    }
    
private:
    void touch (std::list<int>::iterator iter) noexcept
    {
        m_list.splice (m_list.begin(), m_list, iter);
    }
    
private:
    std::list<int> m_list;
    std::unordered_map<int, std::pair<int, decltype (m_list)::iterator>> m_map;

    const size_t m_maxSize;
};
