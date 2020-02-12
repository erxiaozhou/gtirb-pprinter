// TODO: copyright

#include "AArch64PrettyPrinter.hpp"

#include <capstone/capstone.h>

namespace gtirb_pprint {

AArch64PrettyPrinter::AArch64PrettyPrinter(gtirb::Context& context_,
    gtirb::Module& module_, const ElfSyntax& syntax_,
    const PrintingPolicy& policy_)
    : ElfPrettyPrinter(context_, module_, syntax_, policy_, CS_ARCH_ARM64, CS_MODE_ARM) {}

void AArch64PrettyPrinter::printHeader(std::ostream& os) {
    this->printBar(os);
    // TODO hmm?
    os << "[HEADER]" << '\n';
    this->printBar(os);
    os << '\n';

    for (int i = 0; i < 8; i++) {
        // TODO why is this here?
        os << syntax.nop() << '\n';
    }
}

std::string AArch64PrettyPrinter::getRegisterName(unsigned int reg) const {
    (void) reg;
    return "[REGNAME]";
}

void AArch64PrettyPrinter::printOperandList(std::ostream& os,
        const cs_insn& inst) {
    cs_arm64& detail = inst.detail->arm64;
    uint8_t opCount = detail.op_count;

    for (int i = 0; i < opCount; i++) {
        if (i != 0) {
            os << ',';
        }
        printOperand(os, inst, i);
    }
}

void AArch64PrettyPrinter::printOperand(std::ostream& os,
        const cs_insn& inst, uint64_t index) {
    gtirb::Addr ea(inst.address);
    const cs_arm64_op& op = inst.detail->arm64.operands[index];

    switch (op.type) {
    case ARM64_OP_INVALID:
        std::cerr << "invalid operand\n";
        exit(1);
    default:
        os << "[?OPERAND]";
        return;
    }
}

void AArch64PrettyPrinter::printOpRegdirect(std::ostream& os,
        const cs_insn& inst, unsigned int reg) {
    (void) inst;
    (void) reg;
    os << "[REGDIRECT]";
}

void AArch64PrettyPrinter::printOpImmediate(std::ostream& os,
        const gtirb::SymbolicExpression* symbolic,
        const cs_insn& inst, uint64_t index) {
    (void) symbolic;
    (void) inst;
    (void) index;
    os << "[IMMEDIATE]";
}

void AArch64PrettyPrinter::printOpIndirect(std::ostream& os,
        const gtirb::SymbolicExpression* symbolic,
        const cs_insn& inst, uint64_t index) {
    (void) symbolic;
    (void) inst;
    (void) index;
    os << "[INDIRECT]";
}

const PrintingPolicy& AArch64PrettyPrinterFactory::defaultPrintingPolicy() const {
    return ElfPrettyPrinter::defaultPrintingPolicy();
}

std::unique_ptr<PrettyPrinterBase>
AArch64PrettyPrinterFactory::create(gtirb::Context& gtirb_context,
        gtirb::Module& module, const PrintingPolicy& policy) {
    static const ElfSyntax syntax{};
    return std::make_unique<AArch64PrettyPrinter>(gtirb_context,
            module, syntax, policy);
}

volatile bool AArch64PrettyPrinter::registered = registerPrinter(
        {"elf"}, {"aarch64"},
        std::make_shared<AArch64PrettyPrinterFactory>(), true);

}

