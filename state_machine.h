#ifndef FSM_STATE_MACHINE_H
#define FSM_STATE_MACHINE_H

#include <cstdint>

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fsm {
    class StateMachine {
    public:
        using State = uint64_t;
        static constexpr int8_t empty_transfer_symbol = '\0';

        explicit StateMachine(uint64_t number_states);

        uint64_t GetNumStates() const;

        void AddTransfer(State from, State to, int8_t transfer_symbol);
        void DeleteTransfer(State from, State to, int8_t transfer_symbol);
        void DeleteTransfers(State from, State to);
        void DeleteTransfers(State from, int8_t transfer_symbol);
        void DeleteTransfers(State from);
        std::unordered_map<int8_t, std::unordered_set<State>> GetTransfers(State from) const;

        std::unordered_set<State> GetDestinations(State from, int8_t transfer_symbol) const;

        void AddFinalState(State new_final_state);
        std::unordered_set<State> GetFinalStates() const;

        std::vector<std::unordered_map<int8_t, std::unordered_set<State>>> GetReversedTransfers() const;

    private:
        uint64_t num_states_;
        std::unordered_set<State> final_states_;
        std::vector<std::unordered_map<int8_t, std::unordered_set<State>>> transfers_;
    };

    StateMachine RemoveEmptyTransfers(const StateMachine& machine);
    StateMachine BuildDeterminedMachine(const StateMachine& machine, const std::unordered_set<int8_t>& alphabet);
    StateMachine BuildDeterminedMachine(const StateMachine& machine);
    StateMachine Minimize(const StateMachine& machine, const std::unordered_set<int8_t>& alphabet);
    StateMachine Minimize(const StateMachine& machine);
}

#endif //FSM_STATE_MACHINE_H
