#include <iostream>

#include "state_machine.h"

void PrintStateMachine(const fsm::StateMachine& machine) {
    std::cout << "Final states: ";
    for (auto s : machine.GetFinalStates()) {
        std::cout << s << " ";
    }
    std::cout << std::endl;
    std::cout << "Transfers:\n";
    for (uint64_t i = 0; i < machine.GetNumStates(); ++i) {
        auto tr_list = machine.GetTransfers(i);
        std::cout << "\t" << i << ":\n";
        for (const auto &t : tr_list) {
            if (t.first != fsm::StateMachine::empty_transfer_symbol) {
                std::cout << "\t\t" << t.first << " -> ";
            } else {
                std::cout << "\t\teps -> ";
            }
            for (auto s : t.second) {
                std::cout << s << " ";
            }
            std::cout << std::endl;
        }
    }
}

int main() {
    fsm::StateMachine m(6);
    char a = 'a', b = 'b', e = '\0';
    m.AddTransfer(0, 1, a);
    m.AddTransfer(1, 2, a);
    m.AddTransfer(1, 3, b);
    m.AddTransfer(1, 4, e);
    m.AddTransfer(2, 1, b);
    m.AddTransfer(3, 1, a);
    m.AddTransfer(4, 4, a);
    m.AddTransfer(4, 5, b);
    m.AddTransfer(4, 0, e);
    m.AddTransfer(5, 4, a);
    m.AddFinalState(4);
    auto single_transfers = fsm::RemoveEmptyTransfers(m);
    auto det = fsm::BuildDeterminedMachine(single_transfers);
    auto min = fsm::Minimize(det);

    std::cout << "Source\n";
    PrintStateMachine(m);
    std::cout << "\nResult\n";
    PrintStateMachine(min);
    return 0;
}