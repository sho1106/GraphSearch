#pragma once

#include "TypeDef.h"
#include <algorithm>

namespace Graph
{

    template<class T = size_t>
    class HeapVector
    {
    private:

        const Comp<T> m_comp;
        vector<T> m_heapVec;

        void make()
        {
            std::make_heap(m_heapVec.begin(), m_heapVec.end(), m_comp);
        }

    public:
        HeapVector(const vector<T>& vec, const Comp<T>& comp=std::greater<T>());
        HeapVector(const Comp<T>& comp)
            :m_comp(comp)
        {}
        HeapVector()
            :m_comp(std::greater<T>())
        {}
        
        HeapVector(const HeapVector& obj) = delete;
        virtual ~HeapVector() {};

        const T pop();
        void push(const T& obj);
        const size_t size() const { return m_heapVec.size(); }
        void clear() { m_heapVec.clear(); }
    };

    template<class T>
    inline HeapVector<T>::HeapVector(const vector<T>& vec, const Comp<T>& comp)
        :m_comp(comp)
        ,m_heapVec(vec)
    {
        if (vec.size() > 0) {
            make();
        }
    }

    template<class T>
    inline const T HeapVector<T>::pop()
    {
        if (m_heapVec.empty()) {
            throw RUNTIME_ERROR("it has no elements.");
        }
        std::pop_heap(m_heapVec.begin(), m_heapVec.end(), m_comp);
        const T out = m_heapVec.back();
        m_heapVec.pop_back();
        return out;
    }

    template<class T>
    inline void HeapVector<T>::push(const T& obj)
    {
        m_heapVec.push_back(obj);
        std::push_heap(m_heapVec.begin(), m_heapVec.end(), m_comp);
    }

}

