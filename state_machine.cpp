#include <algorithm>
#include <functional>
#include <set>
#include <queue>

#include "state_machine.h"

namespace fsm {
    StateMachine::StateMachine(uint64_t num_states) : num_states_(num_states), transfers_(num_states) {
    }

    uint64_t StateMachine::GetNumStates() const {
        return num_states_;
    }

    void StateMachine::AddTransfer(State from, State to, int8_t transfer_symbol) {
        transfers_[from][transfer_symbol].insert(to);
    }

    void StateMachine::DeleteTransfer(State from, State to, int8_t transfer_symbol) {
        transfers_[from][transfer_symbol].erase(to);
    }

    void StateMachine::DeleteTransfers(State from, State to) {
        for (auto iter = transfers_[from].begin(); iter != transfers_[from].end(); ++iter) {
            iter->second.erase(to);
        }
    }

    void StateMachine::DeleteTransfers(State from, int8_t transfer_symbol) {
        transfers_[from].erase(transfer_symbol);
    }

    void StateMachine::DeleteTransfers(State from) {
        transfers_[from].clear();
    }

    std::unordered_map<int8_t, std::unordered_set<StateMachine::State>> StateMachine::GetTransfers(State from) const {
        return transfers_[from];
    }

    std::unordered_set<StateMachine::State> StateMachine::GetDestinations(State from, int8_t transfer_symbol) const {
        if (transfers_[from].count(transfer_symbol) == 0) {
            return {};
        }
        return transfers_[from].at(transfer_symbol);
    }

    void StateMachine::AddFinalState(State new_final_state) {
        final_states_.insert(new_final_state);
    }

    std::unordered_set<StateMachine::State> StateMachine::GetFinalStates() const {
        return final_states_;
    }

    std::vector<std::unordered_map<int8_t, std::unordered_set<StateMachine::State>>>
            StateMachine::GetReversedTransfers() const {
        std::vector<std::unordered_map<int8_t, std::unordered_set<State>>> res(num_states_);
        for (uint64_t i = 0; i < num_states_; ++i) {
            for (const auto &trans : transfers_[i]) {
                for (State dest : trans.second) {
                    res[dest][trans.first].insert(i);
                }
            }
        }
        return res;
    }

    StateMachine RemoveEmptyTransfers(const StateMachine& machine) {
        StateMachine res(machine);
        auto final_states = machine.GetFinalStates();
        std::function<void(StateMachine::State, StateMachine::State, std::vector<bool>&)> dfs =
                [&res, &dfs, &final_states](StateMachine::State origin, StateMachine::State current,
                        std::vector<bool>& visited) -> void {
                    visited[current] = true;
                    if (final_states.count(current) > 0) {
                        res.AddFinalState(origin);
                    }
                    auto transfers = res.GetTransfers(current);
                    for (auto iter = transfers.begin(); iter != transfers.end(); ++iter) {
                        if (iter->first != StateMachine::empty_transfer_symbol) {
                            for (auto st : iter->second) {
                                res.AddTransfer(origin, st, iter->first);
                            }
                        } else {
                            for (auto st : iter->second) {
                                if (!visited[st]) {
                                    dfs(origin, st, visited);
                                }
                            }
                        }
                    }
                };
        uint64_t num_states = res.GetNumStates();
        for (StateMachine::State st = 0; st < num_states; ++st) {
            std::vector<bool> visited(num_states, false);
            dfs(st, st, visited);
        }
        for (StateMachine::State st = 0; st < num_states; ++st) {
            res.DeleteTransfers(st, StateMachine::empty_transfer_symbol);
        }
        return res;
    }

    StateMachine BuildDeterminedMachine(const StateMachine& machine) {
        std::unordered_set<int8_t> alphabet;
        for (StateMachine::State st = 0; st < machine.GetNumStates(); ++st) {
            auto transfers = machine.GetTransfers(st);
            for (const auto &tr : transfers) {
                alphabet.insert(tr.first);
            }
        }
        return BuildDeterminedMachine(machine, alphabet);
    }

    StateMachine BuildDeterminedMachine(const StateMachine& machine, const std::unordered_set<int8_t>& alphabet) {
        std::vector<std::unordered_set<StateMachine::State>> new_states = {{0}};
        std::queue<uint64_t> states_to_obtain;
        std::vector<std::unordered_map<int8_t, uint64_t>> new_transfers;

        states_to_obtain.push({0});
        while (!states_to_obtain.empty()) {
            auto state = states_to_obtain.front();
            states_to_obtain.pop();

            if (new_transfers.size() <= state) {
                new_transfers.resize(state + 1);
            }
            for (int8_t ch : alphabet) {
                std::unordered_set<StateMachine::State> new_dest;
                for (auto st : new_states[state]) {
                    auto dests = machine.GetDestinations(st, ch);
                    for (auto dest : dests) {
                        new_dest.insert(dest);
                    }
                }
                int new_dest_pos = 0;
                while (new_dest_pos < new_states.size()) {
                    if (new_dest == new_states[new_dest_pos]) {
                        break;
                    }
                    ++new_dest_pos;
                }
                if (new_dest_pos == new_states.size()) {
                    new_states.push_back(new_dest);
                    states_to_obtain.push(new_dest_pos);
                }
                new_transfers[state][ch] = new_dest_pos;
            }
        }
        auto final_states = machine.GetFinalStates();
        StateMachine res(new_states.size());
        for (uint64_t i = 0; i < new_states.size(); ++i) {
            for (const auto &tr : new_transfers[i]) {
                res.AddTransfer(i, tr.second, tr.first);
            }
            for (auto st : new_states[i]) {
                if (final_states.count(st) > 0) {
                    res.AddFinalState(i);
                }
            }
        }
        return res;
    }

    StateMachine Minimize(const StateMachine& machine) {
        std::unordered_set<int8_t> alphabet;
        for (StateMachine::State st = 0; st < machine.GetNumStates(); ++st) {
            auto transfers = machine.GetTransfers(st);
            for (const auto &tr : transfers) {
                alphabet.insert(tr.first);
            }
        }
        return Minimize(machine, alphabet);
    }

    StateMachine Minimize(const StateMachine& machine, const std::unordered_set<int8_t>& alphabet) {
        auto reversed_transfers  = machine.GetReversedTransfers();
        uint64_t num_states = machine.GetNumStates();
        std::vector<std::vector<bool>> equivalency_table(num_states, std::vector<bool>(num_states, true));
        std::queue<std::pair<StateMachine::State, StateMachine::State>> obtain_queue;
        auto final_states = machine.GetFinalStates();
        // Mark final and non-final states as not equivalent
        for (uint64_t i = 0; i < num_states; ++i) {
            for (uint64_t j = i + 1; j < num_states; ++j) {
                if (final_states.count(i) != final_states.count(j)) {
                    equivalency_table[i][j] = equivalency_table[j][i] = false;
                    obtain_queue.emplace(i, j);
                }
            }
        }
        // Mark states, from which we get to not equivalent states, as not equivalent
        while (!obtain_queue.empty()) {
            auto not_equiv_states = obtain_queue.front();
            obtain_queue.pop();
            for (int8_t transfer_symbol : alphabet) {
                for (auto first_original : reversed_transfers[not_equiv_states.first][transfer_symbol]) {
                    for (auto second_original : reversed_transfers[not_equiv_states.second][transfer_symbol]) {
                        if (equivalency_table[first_original][second_original]) {
                            equivalency_table[first_original][second_original] = false;
                            equivalency_table[second_original][first_original] = false;
                            obtain_queue.emplace(first_original, second_original);
                        }
                    }
                }
            }
        }

        // Divide states into equivalency classes
        uint64_t infinity = std::numeric_limits<uint64_t>::max();
        std::vector<uint64_t> class_number(num_states, infinity);
        std::vector<StateMachine::State> class_delegate;
        std::vector<bool> class_is_final;
        uint64_t cur_class_number = 0;
        for (uint64_t i = 0; i < num_states; ++i) {
            if (class_number[i] == infinity) {
                class_delegate.push_back(i);
                class_is_final.push_back(false);
                for (uint64_t j = i; j < num_states; ++j) {
                    if (equivalency_table[i][j]) {
                        class_number[j] = cur_class_number;
                        if (final_states.count(j) != 0) {
                            class_is_final.back() = true;
                        }
                    }
                }
                ++cur_class_number;
            }
        }
        // Make new state machine
        StateMachine res(cur_class_number);
        for (uint64_t i = 0; i < cur_class_number; ++i) {
            auto old_transfers = machine.GetTransfers(class_delegate[i]);
            for (int8_t transfer_symbol : alphabet) {
                uint64_t dest = class_number[*old_transfers[transfer_symbol].begin()];
                res.AddTransfer(i, dest, transfer_symbol);
            }
            if (class_is_final[i]) {
                res.AddFinalState(i);
            }
        }
        return res;
    }
}