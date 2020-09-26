#include <iostream>

#include "state_machine.h"

void PrintStateMachine(const fsm::StateMachine& machine) {
    std::cout << "Machine size: " << machine.GetNumStates() << std::endl;
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

void OptimizeMachine(const fsm::StateMachine& source) {
    auto single_transfers = fsm::RemoveEmptyTransfers(source);
    auto determined = fsm::BuildDeterminedMachine(single_transfers);
    auto minimized = fsm::Minimize(determined);

    std::cout << "Source\n";
    PrintStateMachine(source);

    std::cout << "\nSingle-letter transfers\n";
    PrintStateMachine(single_transfers);

    std::cout << "\nDetermined\n";
    PrintStateMachine(determined);

    std::cout << "\nResult\n";
    PrintStateMachine(minimized);
}

void TestSample() {
    // sample from seminar
    fsm::StateMachine source(6);
    uint8_t a = 'a', b = 'b', e = fsm::StateMachine::empty_transfer_symbol;
    source.AddTransfer(0, 1, a);
    source.AddTransfer(1, 2, a);
    source.AddTransfer(1, 3, b);
    source.AddTransfer(1, 4, e);
    source.AddTransfer(2, 1, b);
    source.AddTransfer(3, 1, a);
    source.AddTransfer(4, 4, a);
    source.AddTransfer(4, 5, b);
    source.AddTransfer(4, 0, e);
    source.AddTransfer(5, 4, a);
    source.AddFinalState(4);
    OptimizeMachine(source);
}

void HomeWork1() {
    fsm::StateMachine source(8);
    uint8_t a = 'a', b = 'b', e = fsm::StateMachine::empty_transfer_symbol;
    source.AddTransfer(0, 0, a);
    source.AddTransfer(0, 0, b);
    source.AddTransfer(0, 1, a);
    source.AddTransfer(0, 7, a);
    source.AddTransfer(1, 2, a);
    source.AddTransfer(1, 6, b);
    source.AddTransfer(2, 2, a);
    source.AddTransfer(2, 2, b);
    source.AddTransfer(2, 3, b);
    source.AddTransfer(3, 4, b);
    source.AddTransfer(4, 4, b);
    source.AddTransfer(4, 5, a);
    source.AddTransfer(5, 5, b);
    source.AddTransfer(6, 4, a);
    source.AddTransfer(7, 6, b);
    source.AddFinalState(5);
    OptimizeMachine(source);
}

void HomeWork2() {
    fsm::StateMachine source(5);
    uint8_t a = 'a', b = 'b', e = fsm::StateMachine::empty_transfer_symbol;
    source.AddTransfer(0, 1, a);
    source.AddTransfer(1, 0, e);
    source.AddTransfer(1, 2, a);
    source.AddTransfer(1, 3, b);
    source.AddTransfer(2, 1, b);
    source.AddTransfer(3, 1, a);
    source.AddTransfer(3, 4, b);
    source.AddTransfer(4, 3, a);
    source.AddFinalState(1);
    OptimizeMachine(source);
}


int main() {
    HomeWork2();
    return 0;
}