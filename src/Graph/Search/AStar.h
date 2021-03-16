#pragma once

#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <exception>
#include "HeapVector.h"

namespace Graph
{
    template<class Key, class Value>
    using UnorderedMap = std::unordered_map<Key, Value>;

    template<class T>
    using UnorderedSet = std::unordered_set<T>;

    using THROW_RUNTIME_ERROR = std::runtime_error;

    namespace Search
    {
        template<class T>
        class NodeVisiter;
        
        template<class T>
        class HeuristicCostCalculator;

        template<class T>
        using NodeVisiterPtr = SharedPtr<NodeVisiter<T>>;
        
        template<class T>
        using HeuristicCostCalculatorPtr = SharedPtr<HeuristicCostCalculator<T>>;

        template<class T>
        class NodeVisiter
        {
        public:
            virtual const double getCost(const T& parent, const T& child) = 0;
            virtual const bool reachGoal(const T& visit) = 0;
            virtual const vector<T> getAdjacent(const T& visit) = 0;
        };
        
        template<class T>
        class HeuristicCostCalculator
        {
        public:
            virtual const double calcCost(
                const T& visit
            ) = 0;
        };

        template<class T>
        struct SortInfo
        {
            double hcostToGoal;
            double costFromStart;
            T node;
            T parentNode;

            SortInfo(
                const T& n,
                const T& parent,
                const double cs,
                const double cg
            ):node(n), parentNode(parent), costFromStart(cs), hcostToGoal(cg)
            {}
            const double cost() const
            {
                return hcostToGoal + costFromStart;
            }
            const bool operator>(const SortInfo& node) const
            {
                const double owncost = cost();
                const double anothercost = node.cost();
                if (owncost > anothercost) {
                    return true;
                }
                else if (owncost < anothercost) {
                    return false;
                }
                return costFromStart < node.costFromStart;
            }
            SortInfo(const SortInfo& obj)
                :SortInfo(obj.node, obj.parentNode, obj.costFromStart, obj.hcostToGoal)
            {}
            SortInfo operator=(const SortInfo& obj)
            {
                node = obj.node;
                parentNode = obj.parentNode;
                costFromStart = obj.costFromStart;
                hcostToGoal = obj.hcostToGoal;
                return *this;
            }
        };

        template<class T>
        class Astar
        {
        public:
            class Debugger
            {
            public:
                virtual void dump(
                    const UnorderedMap<T, SortInfo<T>>& parentMap
                ) = 0;
            };
            using DebuggerPtr = SharedPtr<Debugger>;

        private:
            const NodeVisiterPtr<T> m_visiter_;
            const HeuristicCostCalculatorPtr<T> m_hcostCalculator_;
            const DebuggerPtr m_debugger_;

            const vector<T> follow(
                const T& target,
                const UnorderedMap<T, SortInfo<T>>& map
            ) const;

        public:
            Astar(
                const NodeVisiterPtr<T>& visiter,
                const HeuristicCostCalculatorPtr<T>& hcostCalculator,
                const DebuggerPtr& debugger=nullptr
            )
                :m_visiter_(visiter)
                ,m_hcostCalculator_(hcostCalculator)
                ,m_debugger_(debugger)
            {}

            const vector<T> searchPath(
                const T& start,
                const T& goal
            );
        };

        template<class T>
        inline const vector<T> Astar<T>::follow(
            const T& target,
            const UnorderedMap<T, SortInfo<T>>& map
        ) const
        {
            vector<T> out;

            T current = target;
            while (current != nullptr) {
                out.push_back(current);
                const auto info = map.find(current);
                if (info == map.end()) {
                    THROW_RUNTIME_ERROR("not found key");
                }
                current = info->second.parentNode;
            }

            std::reverse(out.begin(), out.end());

            return out;
        }

        template<class T>
        inline const vector<T> Astar<T>::searchPath(const T& start, const T& goal)
        {
            // child, {child, parent, costFromStart, costToGoal}}
            std::unordered_map<T, SortInfo<T>> parentMemo;
            UnorderedSet<T> openNodes;
            T visitNode;

            HeapVector<SortInfo<T>> sortList;
            SortInfo<T> startSortInfo(start, nullptr, 0, m_hcostCalculator_->calcCost(start));
            sortList.push(startSortInfo);
            parentMemo.insert({ start, startSortInfo });

            while (sortList.size()){
                visitNode = sortList.pop().node;

                if (openNodes.count(visitNode) > 0) {
                    continue;
                }

                if (m_visiter_->reachGoal(visitNode)) {
                    break;
                }

                const auto nextNodes = m_visiter_->getAdjacent(visitNode);
                const auto& pairInfo = parentMemo.at(visitNode);
                const auto& currentParent = pairInfo.parentNode;
                const SortInfo<T>& currentCost = pairInfo;
                openNodes.insert(visitNode);

                for (const auto nextNode : nextNodes) {
                    if (currentParent == nextNode) {
                        continue;
                    }

                    const double newCostFromStart = 
                        currentCost.costFromStart
                        + m_visiter_->getCost(nextNode, visitNode);

                    // 既に親を持っているか？
                    if (parentMemo.count(nextNode) > 0) {
                        const SortInfo<T>& costInfo = parentMemo.at(nextNode);
                        if (costInfo.costFromStart <= newCostFromStart) {
                            continue;
                        }

                        // 開いていたノードのコストが変わるため
                        if (openNodes.count(nextNode) > 0) {
                            openNodes.erase(nextNode);
                        }
                        parentMemo.erase(nextNode);
                    }

                    SortInfo<T> sortInfo(nextNode, visitNode, newCostFromStart, m_hcostCalculator_->calcCost(nextNode));
                    parentMemo.insert({ nextNode, sortInfo });
                    sortList.push(sortInfo);
                    if (m_debugger_) {
                        m_debugger_->dump(parentMemo);
                    }
                }
            }


            const vector<T> out = follow(visitNode, parentMemo);

            if (out.front() != start) {
                THROW_RUNTIME_ERROR("start node isn't included out");
            }
            if (m_visiter_->reachGoal(out.back())==false) {
                THROW_RUNTIME_ERROR("last node isn't included out");
            }

            return out;
        }
}
}