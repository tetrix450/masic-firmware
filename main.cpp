#include <iostream>
#include <bitset>
#include <vector>
#include <string>
#include <iomanip>

// Señales de control
// (Señalan el nº de bit dentro de la palabra de control, de 0 a 39)

#define SIG_IFETCH              39
#define SIG_RCF_CLR             38
#define SIG_PC_OE               37
#define SIG_MEM_IO              36
#define SIG_PC_LOAD             35
#define SIG_SP_OE               34
#define SIG_SP_LOAD             33
#define SIG_MUX_ADD             32
#define SIG_D_OE                31
#define SIG_DL_LOAD             30
#define SIG_DH_LOAD             29
#define SIG_MEM_WE              28
#define SIG_MEM_OE              27
#define SIG_D_CLR               26
#define SIG_FILL_BIT            25
#define SIG_MUX_ZOS             24
#define SIG_MUX_C_1             23
#define SIG_MUX_C_0             22
#define SIG_S_DAT_2             21
#define SIG_S_DAT_1             20
#define SIG_S_DAT_0             19
#define SIG_S_AC_2              18
#define SIG_S_AC_1              17
#define SIG_S_AC_0              16
#define SIG_BUS_DIS             15
#define SIG_MUX_CI_2            14
#define SIG_MUX_CI_1            13
#define SIG_MUX_CI_0            12
#define SIG_LOAD_I              11
#define SIG_LOAD_ZOS            10
#define SIG_LOAD_C              9
#define SIG_IACK                8
#define SIG_BACK                7
#define SIG_PC_UP               6
#define SIG_SP_UP               5
#define SIG_D_UP                4
#define SIG_RI_LOAD             3
#define SIG_AUX_LOAD            2
#define SIG_AC_LOAD             1
#define SIG_SP_DOWN             0

// Máscara de señales activas a nivel bajo

#define ALL_INACTIVE            0b0010111011111111000000000000111001111111

// Códigos de operación de cada instrucción

#define OP_INST_JMP_ABS         0x00
#define OP_INST_CLC             0x01
#define OP_INST_STC             0x02
#define OP_INST_CLH             0x03
#define OP_INST_STH             0x04
#define OP_INST_SUBC_ABS        0x05
#define OP_INST_SUBC_IMM        0x06
#define OP_INST_STSP_ABS        0x07
#define OP_INST_LOAD_ABS        0x08
#define OP_INST_LOAD_IMM        0x09
#define OP_INST_LOAD_IND        0x0A
#define OP_INST_STORE_ABS       0x0B
#define OP_INST_STORE_IND       0x0C
#define OP_INST_ADD_ABS         0x0D
#define OP_INST_ADD_IMM         0x0E
#define OP_INST_ADC_ABS         0x0F
#define OP_INST_ADC_IMM         0x10
#define OP_INST_SUB_ABS         0x11
#define OP_INST_SUB_IMM         0x12
#define OP_INST_AND_ABS         0x13
#define OP_INST_AND_IMM         0x14
#define OP_INST_OR_ABS          0x15
#define OP_INST_OR_IMM          0x16
#define OP_INST_NOT             0x17
#define OP_INST_STSP_IMM        0x18
#define OP_INST_CMP_ABS         0x19
#define OP_INST_CMP_IMM         0x1A
#define OP_INST_NOP             0x1B
#define OP_INST_JV              0x1C
#define OP_INST_JNV             0x1D
#define OP_INST_JZ              0x1E
#define OP_INST_JNZ             0x1F
#define OP_INST_JC              0x20
#define OP_INST_JNC             0x21
#define OP_INST_JS              0x22
#define OP_INST_JNS             0x23
//#define OP_INST               0x24
//#define OP_INST               0x25
//#define OP_INST               0x26
//#define OP_INST               0x27
//#define OP_INST               0x28
//#define OP_INST               0x29
#define OP_INST_JMP_IND         0x2A
#define OP_INST_SHL             0x2B
#define OP_INST_SHRA            0x2C
#define OP_INST_SHR             0x2D
#define OP_INST_ROL             0x2E
#define OP_INST_ROR             0x2F
#define OP_INST_IN_IND          0x30
#define OP_INST_PUSH            0x31
#define OP_INST_POP             0x32
#define OP_INST_CALL            0x33
#define OP_INST_RET             0x34
#define OP_INST_INT             0x35
#define OP_INST_RETI            0x36
#define OP_INST_SCL             0x37
#define OP_INST_SCR             0x38
#define OP_INST_LDSPL           0x39
#define OP_INST_RCL             0x3A
#define OP_INST_RCR             0x3B
#define OP_INST_OUT_IND         0x3C
#define OP_INST_IN_ABS          0x3D
#define OP_INST_OUT_ABS         0x3E
#define OP_INST_LDSPH           0x3F

/*

El objetivo de este programa es el de generar un fichero con todas las señales de control
definidas para cada instrucción y cada entrada a la unidad de control, para usarlo a la
hora de programar los integrados (5 x FLASH 256Kx8)

*/

// Palabra de control de 40 bits -> usaré un bitset<40>
typedef std::bitset<40> c_word;

class instruction{
    private:
        int size;
        std::vector<c_word> steps;
    public:
        instruction(int n){
            size = n;
            steps = std::vector<c_word>(n, c_word(ALL_INACTIVE));
        };

        int get_size(){
            return size;
        }

        c_word get(int pos){
            return steps[pos];
        }

        void flip(int step, int ctrl_signal){
            if(step >= size){
                std::cout << "Se ha intentado asignar valores al paso " << step << " de una instrucción con tamaño " << size << std::endl;
            }else{
                steps[step].flip(ctrl_signal);
            }
        };

        friend std::ostream& operator<<(std::ostream& out, const instruction& inst) {
            // Mostrar una a una cada palabra de control
            for(long unsigned int i = 0; i < inst.steps.size(); i++){
                // Concatenar cada microinstrucción
                out << i << ": " << inst.steps[i] << std::endl; 
            }
            return out;
        }
};

std::vector<std::string> inst_names(64);
std::vector<std::string> signal_names(40);

void check_instructions(c_word* salida){
    // Para comprobar las instrucciones
    for(int flags = 0; flags < 256; flags++){

        // Extraigo cada bit por separado
        std::bitset<1> ifetch   ((flags&0b10000000)>>7);
        std::bitset<1> irq      ((flags&0b01000000)>>6);
        std::bitset<1> brq      ((flags&0b00100000)>>5);
        std::bitset<1> i        ((flags&0b00010000)>>4);
        std::bitset<1> z        ((flags&0b00001000)>>3);
        std::bitset<1> o        ((flags&0b00000100)>>2);
        std::bitset<1> s        ((flags&0b00000010)>>1);
        std::bitset<1> c        (flags&0b00000001);

        std::cout << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl << std::endl;
        std::cout << "╔═══════════════════════════════════════════════╗" << std::endl;
        std::cout << "║    ifetch=" << ifetch << " irq=" << irq << " brq=" << brq << " I=" << i << " Z=" << z << " O=" << o << " S=" << s << " C=" << c << "   ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════╝" << std::endl;

        std::cout << std::hex;
        for(int ri = 0; ri < 64; ri++){
            std::cout << "┌───────────────────────────────────────────────┐" << std::endl;
            std::cout << "│ 0x" << std::setw(2) << std::setfill('0') << ri << ": " << inst_names[ri];

            for(int j = 0; j < 40 - (int)inst_names[ri].length(); j++){
                std::cout << " ";
            }
            std::cout << "│" << std::endl;

            for(int step = 0; step < 16; step++){
                std::cout << "│" << step << ": ";

                // Mostrar la palabra de control
                uint64_t posicion = (ifetch.to_ulong()<<17) + (irq.to_ulong()<<16) + (brq.to_ulong()<<15) + (i.to_ulong()<<14) + (z.to_ulong()<<13) + (o.to_ulong()<<12) + (s.to_ulong()<<11) + (c.to_ulong()<<10) + (step<<6) + ri;
                uint64_t palabra = salida[posicion].to_ulong();
                
                // Construyo un string con las señales activadas
                std::string activated_signals;

                uint64_t signals = ALL_INACTIVE ^ palabra; // Operación XOR para comprobar diferencias
                for(int i = 0; i < 40; i++){
                    // Comprobar si la señal i-ésima está activa
                    if((signals>>i) & 1){
                        activated_signals += " " + signal_names[i];
                    }
                }

                std::bitset<8> byte_0(palabra&0xFF);
                std::bitset<8> byte_1((palabra>>8)&0xFF);
                std::bitset<8> byte_2((palabra>>16)&0xFF);
                std::bitset<8> byte_3((palabra>>24)&0xFF);
                std::bitset<8> byte_4((palabra>>32)&0xFF);

                std::cout << byte_4 << " " << byte_3 << " " << byte_2 << " " << byte_1 << " "<< byte_0 << "│" << " [" << posicion << "] " << activated_signals << std::endl;

                // Dejar de mostrar la instrucción en el final (RCF_CLR activo)
                if((palabra&0b0100000000000000000000000000000000000000) == 0b0100000000000000000000000000000000000000){
                    break;
                }
            }
            std::cout << "└───────────────────────────────────────────────┘" << std::endl;
            //std::cin.get();
        }
    }
}

int main(){

    inst_names[0x00] = "JMP (abs)";
    inst_names[0x01] = "CLC";
    inst_names[0x02] = "STC";
    inst_names[0x03] = "CLI";
    inst_names[0x04] = "STI";
    inst_names[0x05] = "SUBC (abs)";
    inst_names[0x06] = "SUBC (imm)";
    inst_names[0x07] = "STSP (abs)";
    inst_names[0x08] = "LOAD (abs)";
    inst_names[0x09] = "LOAD (imm)";
    inst_names[0x0a] = "LOAD (ind)";
    inst_names[0x0b] = "STORE (abs)";
    inst_names[0x0c] = "STORE (ind)";
    inst_names[0x0d] = "ADD (abs)";
    inst_names[0x0e] = "ADD (imm)";
    inst_names[0x0f] = "ADC (abs)";
    inst_names[0x10] = "ADC (imm)";
    inst_names[0x11] = "SUB (abs)";
    inst_names[0x12] = "SUB (imm)";
    inst_names[0x13] = "AND (abs)";
    inst_names[0x14] = "AND (imm)";
    inst_names[0x15] = "OR (abs)";
    inst_names[0x16] = "OR (imm)";
    inst_names[0x17] = "NOT";
    inst_names[0x18] = "STSP (imm)";
    inst_names[0x19] = "CMP (abs)";
    inst_names[0x1a] = "CMP (imm)";
    inst_names[0x1b] = "NOP";
    inst_names[0x1c] = "JV";
    inst_names[0x1d] = "JNV";
    inst_names[0x1e] = "JZ";
    inst_names[0x1f] = "JNZ";
    inst_names[0x20] = "JC";
    inst_names[0x21] = "JNC";
    inst_names[0x22] = "JS";
    inst_names[0x23] = "JNS";
    inst_names[0x24] = "???";
    inst_names[0x25] = "???";
    inst_names[0x27] = "???";
    inst_names[0x28] = "???";
    inst_names[0x29] = "???";
    inst_names[0x2a] = "JMP (ind)";
    inst_names[0x2b] = "SHL";
    inst_names[0x2c] = "SHRA";
    inst_names[0x2d] = "SHR";
    inst_names[0x2e] = "ROL";
    inst_names[0x2f] = "ROR";
    inst_names[0x30] = "IN (ind)";
    inst_names[0x31] = "PUSH";
    inst_names[0x32] = "POP";
    inst_names[0x33] = "CALL";
    inst_names[0x34] = "RET";
    inst_names[0x35] = "INT";
    inst_names[0x36] = "RETI";
    inst_names[0x37] = "SCL";
    inst_names[0x38] = "SCR";
    inst_names[0x39] = "LDSPL";
    inst_names[0x3a] = "RCL";
    inst_names[0x3b] = "RCR";
    inst_names[0x3c] = "OUT (imm)";
    inst_names[0x3d] = "IN (abs)";
    inst_names[0x3e] = "OUT (abs)";
    inst_names[0x3f] = "LDSPH";

    signal_names = {"SP_DOWN", "AC_LOAD", "AUX_LOAD", "RI_LOAD", "D_UP",
    "SP_UP", "PC_UP", "BACK", "IACK", "C_LOAD", "ZOS_LOAD", "I_LOAD", "MUX_CI_0",
    "MUX_CI_1", "MUX_CI_2", "BUS_DIS", "S_AC_0", "S_AC_1", "S_AC_2", "S_DAT_0",
    "S_DAT_1", "S_DAT_2", "MUX_C_0", "MUX_C_1", "MUX_ZOS", "FILL_BIT", "D_CLR",
    "MEM_OE", "MEM_WE", "DH_LOAD", "DL_LOAD", "D_OE", "MUX_ADD", "SP_LOAD",
    "SP_OE", "PC_LOAD", "MEM_IO", "PC_OE", "RCF_CLR", "IFETCH"};

    // Vector con todas las instrucciones
    std::vector<instruction> microcode(64, instruction(0));

    // NOP (0x1B)

    instruction inst_nop(1);
    
    inst_nop.flip(0, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_nop.flip(0, SIG_PC_OE);
    inst_nop.flip(0, SIG_PC_UP);
    inst_nop.flip(0, SIG_MEM_OE);
    inst_nop.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_NOP] = inst_nop;

    // JMP (abs) (0x00)
    instruction inst_jmp_abs(4);

    inst_jmp_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_jmp_abs.flip(0, SIG_PC_OE);
    inst_jmp_abs.flip(0, SIG_PC_UP);
    inst_jmp_abs.flip(0, SIG_DL_LOAD);
    inst_jmp_abs.flip(0, SIG_S_DAT_2);

    inst_jmp_abs.flip(1, SIG_MEM_OE);   // DH <- M(PC++)
    inst_jmp_abs.flip(1, SIG_PC_OE);
    inst_jmp_abs.flip(1, SIG_PC_UP);
    inst_jmp_abs.flip(1, SIG_DH_LOAD);
    inst_jmp_abs.flip(1, SIG_S_DAT_2);

    inst_jmp_abs.flip(2, SIG_PC_LOAD);  // PC <- D

    inst_jmp_abs.flip(3, SIG_PC_UP);    // PC++, RI <- M(D), RCF_CLR
    inst_jmp_abs.flip(3, SIG_MEM_OE);
    inst_jmp_abs.flip(3, SIG_D_OE);
    inst_jmp_abs.flip(3, SIG_RI_LOAD);
    inst_jmp_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_JMP_ABS] = inst_jmp_abs;

    // CLC 0x01
    instruction inst_clc(1);

    inst_clc.flip(0, SIG_S_DAT_1); // C <- 0, RI <- M(PC++), RCF_CLR
    inst_clc.flip(0, SIG_S_DAT_2);
    inst_clc.flip(0, SIG_LOAD_C);
    inst_clc.flip(0, SIG_MEM_OE);
    inst_clc.flip(0, SIG_PC_OE);
    inst_clc.flip(0, SIG_RI_LOAD);
    inst_clc.flip(0, SIG_PC_UP);
    inst_clc.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_CLC] = inst_clc;

    // STC (0x02)
    instruction inst_stc(1);

    inst_stc.flip(0, SIG_FILL_BIT); // C <- 1, RI <- M(PC++), RCF_CLR
    inst_stc.flip(0, SIG_S_DAT_1);
    inst_stc.flip(0, SIG_S_DAT_2);
    inst_stc.flip(0, SIG_LOAD_C);
    inst_stc.flip(0, SIG_MEM_OE);
    inst_stc.flip(0, SIG_PC_OE);
    inst_stc.flip(0, SIG_RI_LOAD);
    inst_stc.flip(0, SIG_PC_UP);
    inst_stc.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_STC] = inst_stc;

    // CLH (0x03)
    instruction inst_clh(1);

    inst_clh.flip(0, SIG_S_DAT_1); // I <- 0, RI <- M(PC++), RCF_CLR
    inst_clh.flip(0, SIG_S_DAT_2);
    inst_clh.flip(0, SIG_LOAD_I);
    inst_clh.flip(0, SIG_MEM_OE);
    inst_clh.flip(0, SIG_PC_OE);
    inst_clh.flip(0, SIG_RI_LOAD);
    inst_clh.flip(0, SIG_PC_UP);
    inst_clh.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_CLH] = inst_clh;

    // STH (0x04)
    instruction inst_sth(1);

    inst_sth.flip(0, SIG_FILL_BIT); // I <- 1, RI <- M(PC++), RCF_CLR
    inst_sth.flip(0, SIG_S_DAT_1);
    inst_sth.flip(0, SIG_S_DAT_2);
    inst_sth.flip(0, SIG_LOAD_I);
    inst_sth.flip(0, SIG_MEM_OE);
    inst_sth.flip(0, SIG_PC_OE);
    inst_sth.flip(0, SIG_RI_LOAD);
    inst_sth.flip(0, SIG_PC_UP);
    inst_sth.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_STH] = inst_sth;

    // SUBC (abs) (0x05)

    instruction inst_subc_abs(4);

    inst_subc_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_subc_abs.flip(0, SIG_S_DAT_2);
    inst_subc_abs.flip(0, SIG_PC_OE);
    inst_subc_abs.flip(0, SIG_PC_UP);
    inst_subc_abs.flip(0, SIG_DL_LOAD);

    inst_subc_abs.flip(1, SIG_MEM_OE); // DH <- M(PC++)
    inst_subc_abs.flip(1, SIG_S_DAT_2);
    inst_subc_abs.flip(1, SIG_PC_OE);
    inst_subc_abs.flip(1, SIG_PC_UP);
    inst_subc_abs.flip(1, SIG_DH_LOAD);

    inst_subc_abs.flip(2, SIG_MEM_OE); // AC <- AC + NOT(M(D)) + C, C <- ALU_C, ZOS <- ALU_ZOS
    inst_subc_abs.flip(2, SIG_D_OE);
    inst_subc_abs.flip(2, SIG_MUX_ADD);
    inst_subc_abs.flip(2, SIG_MUX_CI_1);
    inst_subc_abs.flip(2, SIG_LOAD_C);
    inst_subc_abs.flip(2, SIG_MUX_C_1);
    inst_subc_abs.flip(2, SIG_LOAD_ZOS);
    inst_subc_abs.flip(2, SIG_MUX_ZOS);
    inst_subc_abs.flip(2, SIG_S_DAT_2);
    inst_subc_abs.flip(2, SIG_AC_LOAD);

    inst_subc_abs.flip(3, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_subc_abs.flip(3, SIG_RI_LOAD);
    inst_subc_abs.flip(3, SIG_PC_OE);
    inst_subc_abs.flip(3, SIG_PC_UP);
    inst_subc_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_SUBC_ABS] = inst_subc_abs;

    // SUBC (imm) (0x06)

    instruction inst_subc_imm(2);

    inst_subc_imm.flip(0, SIG_MEM_OE); // AC <- AC + NOT(M(PC++)) + C, C <- ALU_C, ZOS <- ALU_ZOS
    inst_subc_imm.flip(0, SIG_S_DAT_2);
    inst_subc_imm.flip(0, SIG_PC_OE);
    inst_subc_imm.flip(0, SIG_PC_UP);
    inst_subc_imm.flip(0, SIG_MUX_ZOS);
    inst_subc_imm.flip(0, SIG_LOAD_ZOS);
    inst_subc_imm.flip(0, SIG_AC_LOAD);
    inst_subc_imm.flip(0, SIG_LOAD_C);
    inst_subc_imm.flip(0, SIG_MUX_C_1);
    inst_subc_imm.flip(0, SIG_MUX_CI_1);
    inst_subc_imm.flip(0, SIG_MUX_ADD);
        
    inst_subc_imm.flip(1, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_subc_imm.flip(1, SIG_RI_LOAD);
    inst_subc_imm.flip(1, SIG_PC_OE);
    inst_subc_imm.flip(1, SIG_PC_UP);
    inst_subc_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_SUBC_IMM] = inst_subc_imm;

    // STSP (abs) (0x07)

    instruction inst_stsp_abs(6);

    inst_stsp_abs.flip(0, SIG_DL_LOAD); // DL <- M(PC++)
    inst_stsp_abs.flip(0, SIG_MEM_OE);
    inst_stsp_abs.flip(0, SIG_PC_OE);
    inst_stsp_abs.flip(0, SIG_PC_UP);
    inst_stsp_abs.flip(0, SIG_S_DAT_2);

    inst_stsp_abs.flip(1, SIG_DH_LOAD); // DH <- M(PC++), AUX <- AC
    inst_stsp_abs.flip(1, SIG_MEM_OE);
    inst_stsp_abs.flip(1, SIG_PC_OE);
    inst_stsp_abs.flip(1, SIG_PC_UP);
    inst_stsp_abs.flip(1, SIG_S_DAT_2);
    inst_stsp_abs.flip(1, SIG_AUX_LOAD);

    inst_stsp_abs.flip(2, SIG_MEM_OE); // AC <- M(D++)
    inst_stsp_abs.flip(2, SIG_S_DAT_2);
    inst_stsp_abs.flip(2, SIG_D_OE);
    inst_stsp_abs.flip(2, SIG_D_UP);
    inst_stsp_abs.flip(2, SIG_S_AC_2);
    inst_stsp_abs.flip(2, SIG_S_AC_0);
    inst_stsp_abs.flip(2, SIG_AC_LOAD);

    inst_stsp_abs.flip(3, SIG_DH_LOAD); // DH <- M(D++)
    inst_stsp_abs.flip(3, SIG_S_DAT_2);
    inst_stsp_abs.flip(3, SIG_D_OE);
    inst_stsp_abs.flip(3, SIG_D_UP);
    inst_stsp_abs.flip(3, SIG_MEM_OE);

    inst_stsp_abs.flip(4, SIG_DL_LOAD); // DL <- AC
    inst_stsp_abs.flip(4, SIG_S_DAT_0);

    inst_stsp_abs.flip(5, SIG_SP_LOAD); // SP <- D, AC <- AUX, RI <- M(PC++), RCF_CLR
    inst_stsp_abs.flip(5, SIG_S_AC_0);
    inst_stsp_abs.flip(5, SIG_S_AC_2);
    inst_stsp_abs.flip(5, SIG_AC_LOAD);
    inst_stsp_abs.flip(5, SIG_RI_LOAD);
    inst_stsp_abs.flip(5, SIG_MEM_OE);
    inst_stsp_abs.flip(5, SIG_PC_OE);
    inst_stsp_abs.flip(5, SIG_PC_UP);
    inst_stsp_abs.flip(5, SIG_RCF_CLR);

    microcode[OP_INST_STSP_ABS] = inst_stsp_abs;

    // LOAD (abs) (0x08)

    instruction inst_load_abs(4);

    inst_load_abs.flip(0, SIG_MEM_OE);  // DL <- M(PC++)
    inst_load_abs.flip(0, SIG_PC_OE);
    inst_load_abs.flip(0, SIG_PC_UP);
    inst_load_abs.flip(0, SIG_DL_LOAD);
    inst_load_abs.flip(0, SIG_S_DAT_2);
    
    inst_load_abs.flip(1, SIG_MEM_OE);  // DH <- M(PC++)
    inst_load_abs.flip(1, SIG_PC_OE);
    inst_load_abs.flip(1, SIG_PC_UP);
    inst_load_abs.flip(1, SIG_DH_LOAD);
    inst_load_abs.flip(1, SIG_S_DAT_2);

    inst_load_abs.flip(2, SIG_MEM_OE);  // AC <- M(D)
    inst_load_abs.flip(2, SIG_D_OE);
    inst_load_abs.flip(2, SIG_S_AC_2);
    inst_load_abs.flip(2, SIG_S_AC_0);
    inst_load_abs.flip(2, SIG_AC_LOAD);
    inst_load_abs.flip(2, SIG_S_DAT_2);

    inst_load_abs.flip(3, SIG_MEM_OE);  // RI <- M(PC++), RCF_CLR
    inst_load_abs.flip(3, SIG_PC_OE);
    inst_load_abs.flip(3, SIG_PC_UP);
    inst_load_abs.flip(3, SIG_RI_LOAD);
    inst_load_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_LOAD_ABS] = inst_load_abs;

    // LOAD (imm) (0x09)

    instruction inst_load_imm(2);

    inst_load_imm.flip(0, SIG_MEM_OE);  // AC <- M(PC++)
    inst_load_imm.flip(0, SIG_PC_OE);
    inst_load_imm.flip(0, SIG_PC_UP);
    inst_load_imm.flip(0, SIG_S_AC_0);
    inst_load_imm.flip(0, SIG_S_AC_2);
    inst_load_imm.flip(0, SIG_AC_LOAD);
    inst_load_imm.flip(0, SIG_S_DAT_2);

    inst_load_imm.flip(1, SIG_MEM_OE);  // RI <- M(PC++), RCF_CLR
    inst_load_imm.flip(1, SIG_PC_OE);
    inst_load_imm.flip(1, SIG_PC_UP);
    inst_load_imm.flip(1, SIG_RI_LOAD);
    inst_load_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_LOAD_IMM] = inst_load_imm;

    // LOAD (ind) (0x0A)

    instruction inst_load_ind(7);

    inst_load_ind.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_load_ind.flip(0, SIG_PC_OE);
    inst_load_ind.flip(0, SIG_PC_UP);
    inst_load_ind.flip(0, SIG_DL_LOAD);
    inst_load_ind.flip(0, SIG_S_DAT_2);

    inst_load_ind.flip(1, SIG_MEM_OE); // DH <- M(PC++)
    inst_load_ind.flip(1, SIG_PC_OE);
    inst_load_ind.flip(1, SIG_PC_UP);
    inst_load_ind.flip(1, SIG_DH_LOAD);
    inst_load_ind.flip(1, SIG_S_DAT_2);

    inst_load_ind.flip(2, SIG_MEM_OE); // AC <- M(D++)
    inst_load_ind.flip(2, SIG_S_DAT_2);
    inst_load_ind.flip(2, SIG_D_OE);
    inst_load_ind.flip(2, SIG_D_UP);
    inst_load_ind.flip(2, SIG_S_AC_2);
    inst_load_ind.flip(2, SIG_S_AC_0);
    inst_load_ind.flip(2, SIG_AC_LOAD);

    inst_load_ind.flip(3, SIG_MEM_OE); // DH <- M(D)
    inst_load_ind.flip(3, SIG_D_OE);
    inst_load_ind.flip(3, SIG_DH_LOAD);
    inst_load_ind.flip(3, SIG_S_DAT_2);

    inst_load_ind.flip(4, SIG_S_DAT_0); // DL <- AC
    inst_load_ind.flip(4, SIG_DL_LOAD);

    inst_load_ind.flip(5, SIG_MEM_OE); // AC <- M(D)
    inst_load_ind.flip(5, SIG_D_OE);
    inst_load_ind.flip(5, SIG_S_DAT_2);
    inst_load_ind.flip(5, SIG_S_AC_0);
    inst_load_ind.flip(5, SIG_S_AC_2);
    inst_load_ind.flip(5, SIG_AC_LOAD);
    
    inst_load_ind.flip(6, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_load_ind.flip(6, SIG_RI_LOAD);
    inst_load_ind.flip(6, SIG_PC_OE);
    inst_load_ind.flip(6, SIG_PC_UP);
    inst_load_ind.flip(6, SIG_RCF_CLR);

    microcode[OP_INST_LOAD_IND] = inst_load_ind;

    // STORE (abs) (0x0B)

    instruction inst_store_abs(4);
    
    inst_store_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_store_abs.flip(0, SIG_S_DAT_2);
    inst_store_abs.flip(0, SIG_PC_OE);
    inst_store_abs.flip(0, SIG_PC_UP);
    inst_store_abs.flip(0, SIG_DL_LOAD);

    inst_store_abs.flip(1, SIG_MEM_OE); // DH <- M(PC++)
    inst_store_abs.flip(1, SIG_S_DAT_2);
    inst_store_abs.flip(1, SIG_PC_OE);
    inst_store_abs.flip(1, SIG_PC_UP);
    inst_store_abs.flip(1, SIG_DH_LOAD);

    inst_store_abs.flip(2, SIG_MEM_WE); // M(D) <- AC
    inst_store_abs.flip(2, SIG_S_DAT_0);
    inst_store_abs.flip(2, SIG_D_OE);

    inst_store_abs.flip(3, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_store_abs.flip(3, SIG_PC_OE);
    inst_store_abs.flip(3, SIG_PC_UP);
    inst_store_abs.flip(3, SIG_RI_LOAD);
    inst_store_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_STORE_ABS] = inst_store_abs;

    // STORE (ind) (0x0C)
    instruction inst_store_ind(7);

    inst_store_ind.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_store_ind.flip(0, SIG_S_DAT_2);
    inst_store_ind.flip(0, SIG_PC_OE);
    inst_store_ind.flip(0, SIG_PC_UP);
    inst_store_ind.flip(0, SIG_DL_LOAD);

    inst_store_ind.flip(1, SIG_MEM_OE); // DH <- M(PC++), AUX <- AC
    inst_store_ind.flip(1, SIG_S_DAT_2);
    inst_store_ind.flip(1, SIG_PC_OE);
    inst_store_ind.flip(1, SIG_PC_UP);
    inst_store_ind.flip(1, SIG_DH_LOAD);
    inst_store_ind.flip(1, SIG_AUX_LOAD);

    inst_store_ind.flip(2, SIG_MEM_OE); // AC <- M(D++)
    inst_store_ind.flip(2, SIG_S_DAT_2);
    inst_store_ind.flip(2, SIG_D_OE);
    inst_store_ind.flip(2, SIG_D_UP);
    inst_store_ind.flip(2, SIG_S_AC_2);
    inst_store_ind.flip(2, SIG_S_AC_0);
    inst_store_ind.flip(2, SIG_AC_LOAD);

    inst_store_ind.flip(3, SIG_MEM_OE); // DH <- M(D)
    inst_store_ind.flip(3, SIG_S_DAT_2);
    inst_store_ind.flip(3, SIG_D_OE);
    inst_store_ind.flip(3, SIG_DH_LOAD);
    
    inst_store_ind.flip(4, SIG_S_DAT_0); // DL <- AC
    inst_store_ind.flip(4, SIG_DL_LOAD);

    inst_store_ind.flip(5, SIG_MEM_WE); // M(D) <- AUX, AC <- AUX
    inst_store_ind.flip(5, SIG_D_OE);
    inst_store_ind.flip(5, SIG_S_AC_2);
    inst_store_ind.flip(5, SIG_S_AC_0);
    inst_store_ind.flip(5, SIG_AC_LOAD);

    inst_store_ind.flip(6, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_store_ind.flip(6, SIG_PC_OE);
    inst_store_ind.flip(6, SIG_PC_UP);
    inst_store_ind.flip(6, SIG_MEM_OE);
    inst_store_ind.flip(6, SIG_RCF_CLR);

    microcode[OP_INST_STORE_IND] = inst_store_ind;
    
    // ADD (abs) (0x0D)
    
    instruction inst_add_abs(4);

    inst_add_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_add_abs.flip(0, SIG_S_DAT_2);
    inst_add_abs.flip(0, SIG_PC_OE);
    inst_add_abs.flip(0, SIG_PC_UP);
    inst_add_abs.flip(0, SIG_DL_LOAD);

    inst_add_abs.flip(1, SIG_MEM_OE); // DH <- M(PC++)
    inst_add_abs.flip(1, SIG_S_DAT_2);
    inst_add_abs.flip(1, SIG_PC_OE);
    inst_add_abs.flip(1, SIG_PC_UP);
    inst_add_abs.flip(1, SIG_DH_LOAD);

    inst_add_abs.flip(2, SIG_MEM_OE); // AC <- AC + M(D), C <- ALU_C, ZOS <- ALU_ZOS
    inst_add_abs.flip(2, SIG_S_DAT_2);
    inst_add_abs.flip(2, SIG_D_OE);
    inst_add_abs.flip(2, SIG_MUX_ZOS);
    inst_add_abs.flip(2, SIG_LOAD_ZOS);
    inst_add_abs.flip(2, SIG_AC_LOAD);
    inst_add_abs.flip(2, SIG_LOAD_C);
    inst_add_abs.flip(2, SIG_MUX_C_1);

    inst_add_abs.flip(3, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_add_abs.flip(3, SIG_RI_LOAD);
    inst_add_abs.flip(3, SIG_PC_OE);
    inst_add_abs.flip(3, SIG_PC_UP);
    inst_add_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_ADD_ABS] = inst_add_abs;

    // ADD (imm) (0x0E)

    instruction inst_add_imm(2);

    inst_add_imm.flip(0, SIG_MEM_OE); // AC <- AC + M(PC++), C <- ALU_C, ZOS <- ALU_ZOS
    inst_add_imm.flip(0, SIG_S_DAT_2);
    inst_add_imm.flip(0, SIG_PC_OE);
    inst_add_imm.flip(0, SIG_PC_UP);
    inst_add_imm.flip(0, SIG_MUX_ZOS);
    inst_add_imm.flip(0, SIG_LOAD_ZOS);
    inst_add_imm.flip(0, SIG_AC_LOAD);
    inst_add_imm.flip(0, SIG_LOAD_C);
    inst_add_imm.flip(0, SIG_MUX_C_1);

    inst_add_imm.flip(1, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_add_imm.flip(1, SIG_RI_LOAD);
    inst_add_imm.flip(1, SIG_PC_OE);
    inst_add_imm.flip(1, SIG_PC_UP);
    inst_add_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_ADD_IMM] = inst_add_imm;

    // ADC (abs) (0x0F)

    instruction inst_adc_abs(4);

    inst_adc_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_adc_abs.flip(0, SIG_S_DAT_2);
    inst_adc_abs.flip(0, SIG_PC_OE);
    inst_adc_abs.flip(0, SIG_PC_UP);
    inst_adc_abs.flip(0, SIG_DL_LOAD);

    inst_adc_abs.flip(1, SIG_MEM_OE); // DH <- (M(PC++))
    inst_adc_abs.flip(1, SIG_S_DAT_2);
    inst_adc_abs.flip(1, SIG_PC_OE);
    inst_adc_abs.flip(1, SIG_PC_UP);
    inst_adc_abs.flip(1, SIG_DH_LOAD);

    inst_adc_abs.flip(2, SIG_MEM_OE); // AC <- AC + M(D) + C, C <- ALU_C, ZOS <- ALU_ZOS
    inst_adc_abs.flip(2, SIG_S_DAT_2);
    inst_adc_abs.flip(2, SIG_D_OE);
    inst_adc_abs.flip(2, SIG_MUX_ZOS);
    inst_adc_abs.flip(2, SIG_LOAD_ZOS);
    inst_adc_abs.flip(2, SIG_AC_LOAD);
    inst_adc_abs.flip(2, SIG_LOAD_C);
    inst_adc_abs.flip(2, SIG_MUX_CI_1);
    inst_adc_abs.flip(2, SIG_MUX_C_1);

    inst_adc_abs.flip(3, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_adc_abs.flip(3, SIG_RI_LOAD);
    inst_adc_abs.flip(3, SIG_PC_OE);
    inst_adc_abs.flip(3, SIG_PC_UP);
    inst_adc_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_ADC_ABS] = inst_adc_abs;

    // ADC (imm) (0x10)

    instruction inst_adc_imm(2);

    inst_adc_imm.flip(0, SIG_MEM_OE); // AC <- AC + M(PC++) + C, C <- ALU_C, ZOS <- ALU_ZOS
    inst_adc_imm.flip(0, SIG_S_DAT_2);
    inst_adc_imm.flip(0, SIG_PC_OE);
    inst_adc_imm.flip(0, SIG_PC_UP);
    inst_adc_imm.flip(0, SIG_MUX_ZOS);
    inst_adc_imm.flip(0, SIG_LOAD_ZOS);
    inst_adc_imm.flip(0, SIG_AC_LOAD);
    inst_adc_imm.flip(0, SIG_LOAD_C);
    inst_adc_imm.flip(0, SIG_MUX_CI_1);
    inst_adc_imm.flip(0, SIG_MUX_C_1);

    inst_adc_imm.flip(1, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_adc_imm.flip(1, SIG_RI_LOAD);
    inst_adc_imm.flip(1, SIG_PC_OE);
    inst_adc_imm.flip(1, SIG_PC_UP);
    inst_adc_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_ADC_IMM] = inst_adc_imm;

    // SUB (abs) (0x11)

    instruction inst_sub_abs(4);

    inst_sub_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_sub_abs.flip(0, SIG_S_DAT_2);
    inst_sub_abs.flip(0, SIG_PC_OE);
    inst_sub_abs.flip(0, SIG_PC_UP);
    inst_sub_abs.flip(0, SIG_DL_LOAD);

    inst_sub_abs.flip(1, SIG_MEM_OE); // DH <- M(PC++)
    inst_sub_abs.flip(1, SIG_S_DAT_2);
    inst_sub_abs.flip(1, SIG_PC_OE);
    inst_sub_abs.flip(1, SIG_PC_UP);
    inst_sub_abs.flip(1, SIG_DH_LOAD);

    inst_sub_abs.flip(2, SIG_MEM_OE); // AC <- AC + NOT(M(D)) + 1, C <- ALU_C, ZOS <- ALU_ZOS
    inst_sub_abs.flip(2, SIG_D_OE);
    inst_sub_abs.flip(2, SIG_MUX_ADD);
    inst_sub_abs.flip(2, SIG_MUX_CI_0);
    inst_sub_abs.flip(2, SIG_LOAD_C);
    inst_sub_abs.flip(2, SIG_MUX_C_1);
    inst_sub_abs.flip(2, SIG_LOAD_ZOS);
    inst_sub_abs.flip(2, SIG_MUX_ZOS);
    inst_sub_abs.flip(2, SIG_S_DAT_2);
    inst_sub_abs.flip(2, SIG_AC_LOAD);

    inst_sub_abs.flip(3, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_sub_abs.flip(3, SIG_RI_LOAD);
    inst_sub_abs.flip(3, SIG_PC_OE);
    inst_sub_abs.flip(3, SIG_PC_UP);
    inst_sub_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_SUB_ABS] = inst_sub_abs;

    // SUB (imm) (0x12)

    instruction inst_sub_imm(2);

    inst_sub_imm.flip(0, SIG_MEM_OE); // AC <- AC + NOT(M(PC++)) + 1, C <- ALU_C, ZOS <- ALU_ZOS
    inst_sub_imm.flip(0, SIG_S_DAT_2);
    inst_sub_imm.flip(0, SIG_PC_OE);
    inst_sub_imm.flip(0, SIG_PC_UP);
    inst_sub_imm.flip(0, SIG_MUX_ZOS);
    inst_sub_imm.flip(0, SIG_LOAD_ZOS);
    inst_sub_imm.flip(0, SIG_AC_LOAD);
    inst_sub_imm.flip(0, SIG_LOAD_C);
    inst_sub_imm.flip(0, SIG_MUX_C_1);
    inst_sub_imm.flip(0, SIG_MUX_CI_0);
    inst_sub_imm.flip(0, SIG_MUX_ADD);
        
    inst_sub_imm.flip(1, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_sub_imm.flip(1, SIG_RI_LOAD);
    inst_sub_imm.flip(1, SIG_PC_OE);
    inst_sub_imm.flip(1, SIG_PC_UP);
    inst_sub_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_SUB_IMM] = inst_sub_imm;

    // AND (abs) (0x13)
    instruction inst_and_abs(4);

    inst_and_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_and_abs.flip(0, SIG_PC_OE);
    inst_and_abs.flip(0, SIG_PC_UP);
    inst_and_abs.flip(0, SIG_DL_LOAD);

    inst_and_abs.flip(1, SIG_MEM_OE); // DH <- M(PC++)
    inst_and_abs.flip(1, SIG_PC_OE);
    inst_and_abs.flip(1, SIG_PC_UP);
    inst_and_abs.flip(1, SIG_DH_LOAD);

    inst_and_abs.flip(2, SIG_MEM_OE); // AC <- AC & M(D), ZOS <- ALU_ZOS
    inst_and_abs.flip(2, SIG_S_DAT_2);
    inst_and_abs.flip(2, SIG_D_OE);
    inst_and_abs.flip(2, SIG_S_AC_1);
    inst_and_abs.flip(2, SIG_AC_LOAD);
    inst_and_abs.flip(2, SIG_MUX_ZOS);
    inst_and_abs.flip(2, SIG_LOAD_ZOS);

    inst_and_abs.flip(3, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_and_abs.flip(3, SIG_RI_LOAD);
    inst_and_abs.flip(3, SIG_PC_OE);
    inst_and_abs.flip(3, SIG_PC_UP);
    inst_and_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_AND_ABS] = inst_and_abs;

    // AND (imm) (0x14)

    instruction inst_and_imm(2);

    inst_and_imm.flip(0, SIG_AC_LOAD); // AC <- AC & M(PC++)
    inst_and_imm.flip(0, SIG_S_AC_2);
    inst_and_imm.flip(0, SIG_S_AC_0);
    inst_and_imm.flip(0, SIG_PC_OE);
    inst_and_imm.flip(0, SIG_PC_UP);
    inst_and_imm.flip(0, SIG_MEM_OE);
    inst_and_imm.flip(0, SIG_S_DAT_2);
    inst_and_imm.flip(0, SIG_MUX_ZOS);
    inst_and_imm.flip(0, SIG_LOAD_ZOS);

    inst_and_imm.flip(1, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_and_imm.flip(1, SIG_RI_LOAD);
    inst_and_imm.flip(1, SIG_PC_OE);
    inst_and_imm.flip(1, SIG_PC_UP);
    inst_and_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_AND_IMM] = inst_and_imm;

    // OR (abs) (0x15)

    instruction inst_or_abs(4);

    inst_or_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_or_abs.flip(0, SIG_PC_OE);
    inst_or_abs.flip(0, SIG_PC_UP);
    inst_or_abs.flip(0, SIG_DL_LOAD);

    inst_or_abs.flip(1, SIG_MEM_OE); // DH <- M(PC++)
    inst_or_abs.flip(1, SIG_PC_OE);
    inst_or_abs.flip(1, SIG_PC_UP);
    inst_or_abs.flip(1, SIG_DH_LOAD);

    inst_or_abs.flip(2, SIG_MEM_OE); // AC <- AC | M(D), ZOS <- ALU_ZOS
    inst_or_abs.flip(2, SIG_D_OE);
    inst_or_abs.flip(2, SIG_S_AC_1);
    inst_or_abs.flip(2, SIG_S_AC_0);
    inst_or_abs.flip(2, SIG_AC_LOAD);
    inst_or_abs.flip(2, SIG_MUX_ZOS);
    inst_or_abs.flip(2, SIG_LOAD_ZOS);

    inst_or_abs.flip(3, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_or_abs.flip(3, SIG_RI_LOAD);
    inst_or_abs.flip(3, SIG_PC_OE);
    inst_or_abs.flip(3, SIG_PC_UP);
    inst_or_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_OR_ABS] = inst_or_abs;

    // OR (imm) (0x16) 

    instruction inst_or_imm(2);

    inst_or_imm.flip(0, SIG_AC_LOAD); // AC <- AC | M(PC++), ZOS <- ALU_ZOS
    inst_or_imm.flip(0, SIG_S_AC_2);
    inst_or_imm.flip(0, SIG_PC_OE);
    inst_or_imm.flip(0, SIG_PC_UP);
    inst_or_imm.flip(0, SIG_MEM_OE);
    inst_or_imm.flip(0, SIG_S_DAT_2);
    inst_or_imm.flip(0, SIG_MUX_ZOS);
    inst_or_imm.flip(0, SIG_LOAD_ZOS);

    inst_or_imm.flip(1, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_or_imm.flip(1, SIG_RI_LOAD);
    inst_or_imm.flip(1, SIG_PC_OE);
    inst_or_imm.flip(1, SIG_PC_UP);
    inst_or_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_OR_IMM] = inst_or_imm;

    // NOT (0x17)

    instruction inst_not(1);

    inst_not.flip(0, SIG_AC_LOAD); // AC <- NOT(AC), ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_not.flip(0, SIG_S_AC_2);
    inst_not.flip(0, SIG_S_DAT_0);
    inst_not.flip(0, SIG_RI_LOAD);
    inst_not.flip(0, SIG_MEM_OE);
    inst_not.flip(0, SIG_PC_OE);
    inst_not.flip(0, SIG_PC_UP);
    inst_not.flip(0, SIG_RCF_CLR);
    inst_not.flip(0, SIG_MUX_ZOS);
    inst_not.flip(0, SIG_LOAD_ZOS);

    microcode[OP_INST_NOT] = inst_not;

    // STSP (imm) (0x18)

    instruction inst_stsp(3);

    inst_stsp.flip(0, SIG_DL_LOAD); // DL <- M(PC++)
    inst_stsp.flip(0, SIG_MEM_OE);
    inst_stsp.flip(0, SIG_PC_OE);
    inst_stsp.flip(0, SIG_PC_UP);
    inst_stsp.flip(0, SIG_S_DAT_2);

    inst_stsp.flip(1, SIG_DH_LOAD); // DH <- M(PC++)
    inst_stsp.flip(1, SIG_MEM_OE);
    inst_stsp.flip(1, SIG_PC_OE);
    inst_stsp.flip(1, SIG_PC_UP);
    inst_stsp.flip(1, SIG_S_DAT_2);

    inst_stsp.flip(2, SIG_SP_LOAD); // SP <- D, RI <- M(PC++), RCF_CLR
    inst_stsp.flip(2, SIG_RI_LOAD);
    inst_stsp.flip(2, SIG_MEM_OE);
    inst_stsp.flip(2, SIG_PC_OE);
    inst_stsp.flip(2, SIG_PC_UP);
    inst_stsp.flip(2, SIG_RCF_CLR);

    microcode[OP_INST_STSP_IMM] = inst_stsp;

    // CMP (abs) (0x19)

    instruction inst_cmp_abs(4);

    inst_cmp_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_cmp_abs.flip(0, SIG_S_DAT_2);
    inst_cmp_abs.flip(0, SIG_PC_OE);
    inst_cmp_abs.flip(0, SIG_PC_UP);
    inst_cmp_abs.flip(0, SIG_DL_LOAD);

    inst_cmp_abs.flip(1, SIG_MEM_OE); // DH <- M(PC++)
    inst_cmp_abs.flip(1, SIG_S_DAT_2);
    inst_cmp_abs.flip(1, SIG_PC_OE);
    inst_cmp_abs.flip(1, SIG_PC_UP);
    inst_cmp_abs.flip(1, SIG_DH_LOAD);

    inst_cmp_abs.flip(2, SIG_MEM_OE); // (nada) <- AC + NOT(M(D)) + 1, C <- ALU_C, ZOS <- ALU_ZOS
    inst_cmp_abs.flip(2, SIG_D_OE);
    inst_cmp_abs.flip(2, SIG_MUX_ADD);
    inst_cmp_abs.flip(2, SIG_MUX_CI_0);
    inst_cmp_abs.flip(2, SIG_LOAD_C);
    inst_cmp_abs.flip(2, SIG_MUX_C_1);
    inst_cmp_abs.flip(2, SIG_LOAD_ZOS);
    inst_cmp_abs.flip(2, SIG_MUX_ZOS);
    inst_cmp_abs.flip(2, SIG_S_DAT_2);

    inst_cmp_abs.flip(3, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_cmp_abs.flip(3, SIG_RI_LOAD);
    inst_cmp_abs.flip(3, SIG_PC_OE);
    inst_cmp_abs.flip(3, SIG_PC_UP);
    inst_cmp_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_CMP_ABS] = inst_cmp_abs;

    // CMP (imm) (0x1A)

    instruction inst_cmp_imm(2);

    inst_cmp_imm.flip(0, SIG_MEM_OE); // (nada) <- AC + NOT(M(PC++)) + 1, C <- ALU_C, ZOS <- ALU_ZOS
    inst_cmp_imm.flip(0, SIG_S_DAT_2);
    inst_cmp_imm.flip(0, SIG_PC_OE);
    inst_cmp_imm.flip(0, SIG_PC_UP);
    inst_cmp_imm.flip(0, SIG_MUX_ZOS);
    inst_cmp_imm.flip(0, SIG_LOAD_ZOS);
    inst_cmp_imm.flip(0, SIG_LOAD_C);
    inst_cmp_imm.flip(0, SIG_MUX_C_1);
    inst_cmp_imm.flip(0, SIG_MUX_CI_0);
    inst_cmp_imm.flip(0, SIG_MUX_ADD);
        
    inst_cmp_imm.flip(1, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_cmp_imm.flip(1, SIG_RI_LOAD);
    inst_cmp_imm.flip(1, SIG_PC_OE);
    inst_cmp_imm.flip(1, SIG_PC_UP);
    inst_cmp_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_CMP_IMM] = inst_cmp_imm;

    // SALTOS CONDICIONALES (abs) (0x1C -> 0x23)
    // Estas instrucciones tienen el mismo microprograma que JMP
    // (Después de procesarse más adelante se asociará otro microprograma
    // en función de las respectivas flags)
    for(uint8_t i = 0x1C; i <= 0x23; i++){
        microcode[i] = inst_jmp_abs;
    }

    // CÓDIGOS DE OPERACIÓN SIN UTILIZAR (0x24 -> 0x29)
    for(int i = 0x24; i <= 0x29; i++){
        microcode[i] = inst_nop;
    }

    // ...

    // JMP (ind) (0x2A)

    instruction inst_jmp_ind(7);

    inst_jmp_ind.flip(0, SIG_MEM_OE);   // DL <- M(PC++)
    inst_jmp_ind.flip(0, SIG_PC_OE);
    inst_jmp_ind.flip(0, SIG_PC_UP);
    inst_jmp_ind.flip(0, SIG_DL_LOAD);
    inst_jmp_ind.flip(0, SIG_S_DAT_2);

    inst_jmp_ind.flip(1, SIG_MEM_OE);   // DH <- M(PC++), AUX <- AC
    inst_jmp_ind.flip(1, SIG_PC_OE);
    inst_jmp_ind.flip(1, SIG_PC_UP);
    inst_jmp_ind.flip(1, SIG_DH_LOAD);
    inst_jmp_ind.flip(1, SIG_S_DAT_2);
    inst_jmp_ind.flip(1, SIG_AUX_LOAD);

    inst_jmp_ind.flip(2, SIG_AC_LOAD); // AC <- M(D++)
    inst_jmp_ind.flip(2, SIG_MEM_OE);
    inst_jmp_ind.flip(2, SIG_D_OE);
    inst_jmp_ind.flip(2, SIG_D_UP);

    inst_jmp_ind.flip(3, SIG_DH_LOAD); // DH <- M(D++)
    inst_jmp_ind.flip(3, SIG_MEM_OE);
    inst_jmp_ind.flip(3, SIG_D_OE);
    inst_jmp_ind.flip(3, SIG_D_UP);

    inst_jmp_ind.flip(4, SIG_DL_LOAD); // DL <- AC
    inst_jmp_ind.flip(4, SIG_S_DAT_0);

    inst_jmp_ind.flip(5, SIG_PC_LOAD);  // PC <- D, AC <- AUX
    inst_jmp_ind.flip(5, SIG_S_AC_0);
    inst_jmp_ind.flip(5, SIG_S_AC_2);
    inst_jmp_ind.flip(5, SIG_AC_LOAD);

    inst_jmp_ind.flip(6, SIG_PC_UP);    // PC++, RI <- M(D), RCF_CLR
    inst_jmp_ind.flip(6, SIG_MEM_OE);
    inst_jmp_ind.flip(6, SIG_D_OE);
    inst_jmp_ind.flip(6, SIG_RI_LOAD);
    inst_jmp_ind.flip(6, SIG_RCF_CLR);

    microcode[OP_INST_JMP_IND] = inst_jmp_ind;

    // SHL (0x2B)

    instruction inst_shl(1);

    inst_shl.flip(0, SIG_AC_LOAD); // AC <- AC + AC, C <- ALU_C, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_shl.flip(0, SIG_S_DAT_0);
    inst_shl.flip(0, SIG_MUX_ZOS);
    inst_shl.flip(0, SIG_LOAD_ZOS);
    inst_shl.flip(0, SIG_MUX_C_1);
    inst_shl.flip(0, SIG_LOAD_C);
    inst_shl.flip(0, SIG_RI_LOAD);
    inst_shl.flip(0, SIG_PC_OE);
    inst_shl.flip(0, SIG_PC_UP);
    inst_shl.flip(0, SIG_MEM_OE);
    inst_shl.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_SHL] = inst_shl;

    // SHRA (0x2C)

    instruction inst_shra(1);

    inst_shra.flip(0, SIG_AC_LOAD); // AC <- S & AC >> 1, C <- AC.0, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_shra.flip(0, SIG_S_AC_0);
    inst_shra.flip(0, SIG_MUX_CI_0);
    inst_shra.flip(0, SIG_MUX_CI_1);
    inst_shra.flip(0, SIG_MUX_ZOS);
    inst_shra.flip(0, SIG_LOAD_ZOS);
    inst_shra.flip(0, SIG_LOAD_C);
    inst_shra.flip(0, SIG_RI_LOAD);
    inst_shra.flip(0, SIG_PC_OE);
    inst_shra.flip(0, SIG_PC_UP);
    inst_shra.flip(0, SIG_MEM_OE);
    inst_shra.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_SHRA] = inst_shra;

    // SHR (0x2D)

    instruction inst_shr(1);

    inst_shr.flip(0, SIG_AC_LOAD); // AC <- S & AC7..AC1, C <- AC.0, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_shr.flip(0, SIG_S_AC_0);
    inst_shr.flip(0, SIG_MUX_ZOS);
    inst_shr.flip(0, SIG_LOAD_ZOS);
    inst_shr.flip(0, SIG_LOAD_C);
    inst_shr.flip(0, SIG_RI_LOAD);
    inst_shr.flip(0, SIG_PC_OE);
    inst_shr.flip(0, SIG_PC_UP);
    inst_shr.flip(0, SIG_MEM_OE);
    inst_shr.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_SHR] = inst_shr;

    // ROL (0x2E)

    instruction inst_rol(1);

    inst_rol.flip(0, SIG_AC_LOAD); // AC <- AC6..AC0 & AC7, C <- AC7, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_rol.flip(0, SIG_MUX_CI_1);
    inst_rol.flip(0, SIG_MUX_CI_0);
    inst_rol.flip(0, SIG_S_DAT_0);
    inst_rol.flip(0, SIG_MUX_ZOS);
    inst_rol.flip(0, SIG_LOAD_ZOS);
    inst_rol.flip(0, SIG_MUX_C_0);
    inst_rol.flip(0, SIG_LOAD_C);
    inst_rol.flip(0, SIG_RI_LOAD);
    inst_rol.flip(0, SIG_PC_OE);
    inst_rol.flip(0, SIG_PC_UP);
    inst_rol.flip(0, SIG_MEM_OE);
    inst_rol.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_ROL] = inst_rol;

    // ROR (0x2F)

    instruction inst_ror(1);

    inst_ror.flip(0, SIG_AC_LOAD); // AC <- AC0 & AC7..AC1, C <- AC0, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_ror.flip(0, SIG_S_AC_0);
    inst_ror.flip(0, SIG_MUX_CI_2);
    inst_ror.flip(0, SIG_MUX_ZOS);
    inst_ror.flip(0, SIG_LOAD_ZOS);
    inst_ror.flip(0, SIG_LOAD_C);
    inst_ror.flip(0, SIG_RI_LOAD);
    inst_ror.flip(0, SIG_PC_OE);
    inst_ror.flip(0, SIG_PC_UP);
    inst_ror.flip(0, SIG_MEM_OE);
    inst_ror.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_ROR] = inst_ror;

    // IN (ind) (0x30) (puntero de E/S almacenado en memoria)

    instruction inst_in_ind(7);

    inst_in_ind.flip(0, SIG_DL_LOAD); // DL <- M(PC++)
    inst_in_ind.flip(0, SIG_MEM_OE);
    inst_in_ind.flip(0, SIG_PC_OE);
    inst_in_ind.flip(0, SIG_PC_UP);
    inst_in_ind.flip(0, SIG_S_DAT_2);

    inst_in_ind.flip(1, SIG_DH_LOAD); // DH <- M(PC++)
    inst_in_ind.flip(1, SIG_MEM_OE);
    inst_in_ind.flip(1, SIG_PC_OE);
    inst_in_ind.flip(1, SIG_PC_UP);
    inst_in_ind.flip(1, SIG_S_DAT_2);

    inst_in_ind.flip(2, SIG_AC_LOAD); // AC <- M(D++)
    inst_in_ind.flip(2, SIG_MEM_OE);
    inst_in_ind.flip(2, SIG_D_OE);
    inst_in_ind.flip(2, SIG_D_UP);
    inst_in_ind.flip(2, SIG_S_DAT_2);
    inst_in_ind.flip(2, SIG_S_AC_0);
    inst_in_ind.flip(2, SIG_S_AC_2);

    inst_in_ind.flip(3, SIG_DH_LOAD); // DH <- M(D)
    inst_in_ind.flip(3, SIG_MEM_OE);
    inst_in_ind.flip(3, SIG_D_OE);
    inst_in_ind.flip(3, SIG_S_DAT_2);

    inst_in_ind.flip(4, SIG_DL_LOAD); // DL <- AC
    inst_in_ind.flip(4, SIG_S_DAT_0);

    inst_in_ind.flip(5, SIG_AC_LOAD); // AC <- IO(D)
    inst_in_ind.flip(5, SIG_MEM_IO);
    inst_in_ind.flip(5, SIG_MEM_OE);
    inst_in_ind.flip(5, SIG_D_OE);
    inst_in_ind.flip(5, SIG_S_DAT_2);

    inst_in_ind.flip(6, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_in_ind.flip(6, SIG_PC_OE);
    inst_in_ind.flip(6, SIG_PC_UP);
    inst_in_ind.flip(6, SIG_MEM_OE);
    inst_in_ind.flip(6, SIG_RCF_CLR);

    microcode[OP_INST_IN_IND] = inst_in_ind;

    // PUSH (0x31)

    instruction inst_push(2);

    inst_push.flip(0, SIG_SP_OE); // M(SP--) <- AC
    inst_push.flip(0, SIG_SP_DOWN);
    inst_push.flip(0, SIG_S_DAT_0);

    inst_push.flip(1, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_push.flip(1, SIG_PC_OE);
    inst_push.flip(1, SIG_PC_UP);
    inst_push.flip(1, SIG_RI_LOAD);
    inst_push.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_PUSH] = inst_push;

    // POP (0x32)

    instruction inst_pop(2);

    inst_pop.flip(0, SIG_AC_LOAD); // AC <- M(SP++)
    inst_pop.flip(0, SIG_S_AC_0);
    inst_pop.flip(0, SIG_S_AC_2);
    inst_pop.flip(0, SIG_S_DAT_2);
    inst_pop.flip(0, SIG_MEM_OE);
    inst_pop.flip(0, SIG_SP_OE);
    inst_pop.flip(0, SIG_SP_UP);

    inst_pop.flip(1, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_pop.flip(1, SIG_MEM_OE);
    inst_pop.flip(1, SIG_PC_OE);
    inst_pop.flip(1, SIG_PC_UP);
    inst_pop.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_POP] = inst_pop;

    // CALL (0X33)

    instruction inst_call(7);

    inst_call.flip(0, SIG_DL_LOAD); // DL <- M(PC++)
    inst_call.flip(0, SIG_PC_OE);
    inst_call.flip(0, SIG_PC_UP);
    inst_call.flip(0, SIG_MEM_OE);
    inst_call.flip(0, SIG_S_DAT_2);

    inst_call.flip(1, SIG_DH_LOAD); // DH <- M(PC++), AUX <- AC
    inst_call.flip(1, SIG_PC_OE);
    inst_call.flip(1, SIG_PC_UP);
    inst_call.flip(1, SIG_MEM_OE);
    inst_call.flip(1, SIG_S_DAT_2);
    inst_call.flip(1, SIG_AUX_LOAD);

    inst_call.flip(2, SIG_AC_LOAD); // AC <- PCL
    inst_call.flip(2, SIG_PC_OE);
    inst_call.flip(2, SIG_S_DAT_1);
    inst_call.flip(2, SIG_S_AC_0);
    inst_call.flip(2, SIG_S_AC_2);

    inst_call.flip(3, SIG_MEM_WE); // M(SP--) <- AC
    inst_call.flip(3, SIG_SP_OE);
    inst_call.flip(3, SIG_SP_DOWN);
    inst_call.flip(3, SIG_S_DAT_0);

    inst_call.flip(4, SIG_AC_LOAD); // AC <- PCH
    inst_call.flip(4, SIG_PC_OE);
    inst_call.flip(4, SIG_S_DAT_1);
    inst_call.flip(4, SIG_S_DAT_0);
    inst_call.flip(4, SIG_S_AC_0);
    inst_call.flip(4, SIG_S_AC_2);

    inst_call.flip(5, SIG_MEM_WE); // M(SP--) <- AC, PC <- D
    inst_call.flip(5, SIG_SP_OE);
    inst_call.flip(5, SIG_SP_DOWN);
    inst_call.flip(5, SIG_S_DAT_0);
    inst_call.flip(5, SIG_PC_LOAD);

    inst_call.flip(6, SIG_AC_LOAD); // RI <- M(PC++), AC <- AUX, RCF_CLR
    inst_call.flip(6, SIG_S_AC_0);
    inst_call.flip(6, SIG_S_AC_2);
    inst_call.flip(6, SIG_RI_LOAD);
    inst_call.flip(6, SIG_MEM_OE);
    inst_call.flip(6, SIG_PC_OE);
    inst_call.flip(6, SIG_PC_UP);
    inst_call.flip(6, SIG_RCF_CLR);

    microcode[OP_INST_CALL] = inst_call;

    // RET (0x34)

    instruction inst_ret(5);

    inst_ret.flip(0, SIG_SP_UP); // SP++

    inst_ret.flip(1, SIG_DH_LOAD); // DH <- M(SP++)
    inst_ret.flip(1, SIG_MEM_OE);
    inst_ret.flip(1, SIG_SP_OE);
    inst_ret.flip(1, SIG_SP_UP);
    inst_ret.flip(1, SIG_S_DAT_2);

    inst_ret.flip(2, SIG_DL_LOAD); // DL <- M(SP)
    inst_ret.flip(2, SIG_MEM_OE);
    inst_ret.flip(2, SIG_SP_OE);
    inst_ret.flip(2, SIG_S_DAT_2);

    inst_ret.flip(3, SIG_PC_LOAD); // PC <- D

    inst_ret.flip(4, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_ret.flip(4, SIG_MEM_OE);
    inst_ret.flip(4, SIG_PC_OE);
    inst_ret.flip(4, SIG_PC_UP);
    inst_ret.flip(4, SIG_RCF_CLR);

    microcode[OP_INST_RET] = inst_ret;

    // INT (0x35)

    instruction inst_int(13); 

    inst_int.flip(0, SIG_AUX_LOAD); // AUX <- AC, AC <- M(PC++)
    inst_int.flip(0, SIG_S_AC_0);
    inst_int.flip(0, SIG_S_AC_2);
    inst_int.flip(0, SIG_AC_LOAD);
    inst_int.flip(0, SIG_MEM_OE);
    inst_int.flip(0, SIG_PC_OE);
    inst_int.flip(0, SIG_PC_UP);
    inst_int.flip(0, SIG_S_DAT_2);

    inst_int.flip(1, SIG_AC_LOAD); // AC <- AC + AC, D_CLR
    inst_int.flip(1, SIG_S_DAT_0);
    inst_int.flip(1, SIG_D_CLR);

    inst_int.flip(2, SIG_DL_LOAD); // DL <- AC
    inst_int.flip(2, SIG_S_DAT_0);
    
    inst_int.flip(3, SIG_AC_LOAD); // AC <- M(D++)
    inst_int.flip(3, SIG_S_AC_0);
    inst_int.flip(3, SIG_S_AC_2);
    inst_int.flip(3, SIG_MEM_OE);
    inst_int.flip(3, SIG_D_OE);
    inst_int.flip(3, SIG_D_UP);
    inst_int.flip(3, SIG_S_DAT_2);
    
    inst_int.flip(4, SIG_DH_LOAD); // DH <- M(D)
    inst_int.flip(4, SIG_MEM_OE);
    inst_int.flip(4, SIG_D_OE);
    inst_int.flip(4, SIG_S_DAT_2);
    
    inst_int.flip(5, SIG_DL_LOAD); // DL <- AC
    inst_int.flip(5, SIG_S_DAT_0);

    inst_int.flip(6, SIG_AC_LOAD); // AC <- PCL
    inst_int.flip(6, SIG_PC_OE);
    inst_int.flip(6, SIG_S_DAT_1);
    inst_int.flip(6, SIG_S_AC_0);
    inst_int.flip(6, SIG_S_AC_2);

    inst_int.flip(7, SIG_S_DAT_0); // M(SP--) <- AC
    inst_int.flip(7, SIG_MEM_WE);
    inst_int.flip(7, SIG_SP_OE);
    inst_int.flip(7, SIG_SP_DOWN);

    inst_int.flip(8, SIG_AC_LOAD); // AC <- PCH
    inst_int.flip(8, SIG_PC_OE);
    inst_int.flip(8, SIG_S_DAT_1);
    inst_int.flip(8, SIG_S_DAT_0);
    inst_int.flip(8, SIG_S_AC_0);
    inst_int.flip(8, SIG_S_AC_2);

    inst_int.flip(9, SIG_MEM_WE); // M(SP--) <- AC
    inst_int.flip(9, SIG_SP_OE);
    inst_int.flip(9, SIG_SP_DOWN);
    inst_int.flip(9, SIG_S_DAT_0);

    inst_int.flip(10, SIG_MEM_WE); // M(SP--) <- EST
    inst_int.flip(10, SIG_SP_OE);
    inst_int.flip(10, SIG_SP_DOWN);
    inst_int.flip(10, SIG_S_DAT_0);
    inst_int.flip(10, SIG_S_DAT_2);
    
    inst_int.flip(11, SIG_PC_LOAD); // PC <- D, AC <- AUX
    inst_int.flip(11, SIG_AC_LOAD);
    inst_int.flip(11, SIG_S_AC_0);
    inst_int.flip(11, SIG_S_AC_2);
    
    inst_int.flip(12, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_int.flip(12, SIG_MEM_OE);
    inst_int.flip(12, SIG_PC_OE);
    inst_int.flip(12, SIG_PC_UP);
    inst_int.flip(12, SIG_RCF_CLR);

    microcode[OP_INST_INT] = inst_int;

    // RETI (0x36)

    instruction inst_reti(6);

    inst_reti.flip(0, SIG_SP_UP); // SP++

    inst_reti.flip(1, SIG_LOAD_I); // EST <- M(SP++)
    inst_reti.flip(1, SIG_LOAD_C);
    inst_reti.flip(1, SIG_LOAD_ZOS);
    inst_reti.flip(1, SIG_SP_OE);
    inst_reti.flip(1, SIG_SP_UP);
    inst_reti.flip(1, SIG_MEM_OE);
    inst_reti.flip(1, SIG_S_DAT_2);

    inst_reti.flip(2, SIG_MEM_OE); // DH <- M(SP++)
    inst_reti.flip(2, SIG_DH_LOAD);
    inst_reti.flip(2, SIG_SP_OE);
    inst_reti.flip(2, SIG_SP_UP);
    inst_reti.flip(2, SIG_S_DAT_2);

    inst_reti.flip(3, SIG_MEM_OE); // DL <- M(SP)
    inst_reti.flip(3, SIG_DL_LOAD);
    inst_reti.flip(3, SIG_SP_OE);
    inst_reti.flip(3, SIG_S_DAT_2);

    inst_reti.flip(4, SIG_PC_LOAD); // PC <- D

    inst_reti.flip(5, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_reti.flip(5, SIG_MEM_OE);
    inst_reti.flip(5, SIG_PC_OE);
    inst_reti.flip(5, SIG_PC_UP);
    inst_reti.flip(5, SIG_RCF_CLR);

    microcode[OP_INST_RETI] = inst_reti;

    // SCL (0x37)

    instruction inst_scl(1);

    inst_scl.flip(0, SIG_AC_LOAD); // AC <- AC + AC + C, C <- AC.7, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_scl.flip(0, SIG_S_DAT_0);
    inst_scl.flip(0, SIG_MUX_ZOS);
    inst_scl.flip(0, SIG_LOAD_ZOS);
    inst_scl.flip(0, SIG_MUX_C_0);
    inst_scl.flip(0, SIG_MUX_CI_1);
    inst_scl.flip(0, SIG_LOAD_C);
    inst_scl.flip(0, SIG_RI_LOAD);
    inst_scl.flip(0, SIG_PC_OE);
    inst_scl.flip(0, SIG_PC_UP);
    inst_scl.flip(0, SIG_MEM_OE);
    inst_scl.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_SCL] = inst_scl;

    // SCR (0x38)

    instruction inst_scr(1);

    inst_scr.flip(0, SIG_AC_LOAD); // AC <- C & AC7..AC1, C <- AC.0, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_scr.flip(0, SIG_S_AC_0);
    inst_scr.flip(0, SIG_MUX_CI_0);
    inst_scr.flip(0, SIG_MUX_CI_1);
    inst_scr.flip(0, SIG_MUX_ZOS);
    inst_scr.flip(0, SIG_LOAD_ZOS);
    inst_scr.flip(0, SIG_LOAD_C);
    inst_scr.flip(0, SIG_RI_LOAD);
    inst_scr.flip(0, SIG_PC_OE);
    inst_scr.flip(0, SIG_PC_UP);
    inst_scr.flip(0, SIG_MEM_OE);
    inst_scr.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_SCR] = inst_scr;

    // LDSPL (0x39)

    instruction inst_ldspl(2);

    inst_ldspl.flip(0, SIG_AC_LOAD); // AC <- SPL
    inst_ldspl.flip(0, SIG_SP_OE);
    inst_ldspl.flip(0, SIG_S_DAT_1);
    inst_ldspl.flip(0, SIG_S_AC_0);
    inst_ldspl.flip(0, SIG_S_AC_2);

    inst_ldspl.flip(1, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_ldspl.flip(1, SIG_MEM_OE);
    inst_ldspl.flip(1, SIG_PC_OE);
    inst_ldspl.flip(1, SIG_PC_UP);
    inst_ldspl.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_LDSPL] = inst_ldspl;

    // RCL (0x3A)

    instruction inst_rcl(1);

    inst_rcl.flip(0, SIG_AC_LOAD); // AC <- AC6..AC0 & C, C <- AC7, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_rcl.flip(0, SIG_MUX_CI_1);
    inst_rcl.flip(0, SIG_S_DAT_0);
    inst_rcl.flip(0, SIG_MUX_ZOS);
    inst_rcl.flip(0, SIG_LOAD_ZOS);
    inst_rcl.flip(0, SIG_MUX_C_0);
    inst_rcl.flip(0, SIG_LOAD_C);
    inst_rcl.flip(0, SIG_RI_LOAD);
    inst_rcl.flip(0, SIG_PC_OE);
    inst_rcl.flip(0, SIG_PC_UP);
    inst_rcl.flip(0, SIG_MEM_OE);
    inst_rcl.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_RCL] = inst_rcl;

    // RCR (0x3B)

    instruction inst_rcr(1);

    inst_rcr.flip(0, SIG_AC_LOAD); // AC <- C & AC7..AC1, C <- AC.0, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_rcr.flip(0, SIG_S_AC_0);
    inst_rcr.flip(0, SIG_MUX_CI_1);
    inst_rcr.flip(0, SIG_MUX_ZOS);
    inst_rcr.flip(0, SIG_LOAD_ZOS);
    inst_rcr.flip(0, SIG_LOAD_C);
    inst_rcr.flip(0, SIG_RI_LOAD);
    inst_rcr.flip(0, SIG_PC_OE);
    inst_rcr.flip(0, SIG_PC_UP);
    inst_rcr.flip(0, SIG_MEM_OE);
    inst_rcr.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_RCR] = inst_rcr;

    // OUT (ind) (0x3C) (puntero de E/S almacenado en memoria)

    instruction inst_out_ind(7);

    inst_out_ind.flip(0, SIG_DL_LOAD); // DL <- M(PC++)
    inst_out_ind.flip(0, SIG_MEM_OE);
    inst_out_ind.flip(0, SIG_PC_OE);
    inst_out_ind.flip(0, SIG_PC_UP);
    inst_out_ind.flip(0, SIG_S_DAT_2);

    inst_out_ind.flip(1, SIG_DH_LOAD); // DH <- M(PC++)
    inst_out_ind.flip(1, SIG_MEM_OE);
    inst_out_ind.flip(1, SIG_PC_OE);
    inst_out_ind.flip(1, SIG_PC_UP);
    inst_out_ind.flip(1, SIG_S_DAT_2);

    inst_out_ind.flip(2, SIG_AC_LOAD); // AC <- M(D++), AUX <- AC
    inst_out_ind.flip(2, SIG_MEM_OE);
    inst_out_ind.flip(2, SIG_D_OE);
    inst_out_ind.flip(2, SIG_D_UP);
    inst_out_ind.flip(2, SIG_S_DAT_2);
    inst_out_ind.flip(2, SIG_S_AC_0);
    inst_out_ind.flip(2, SIG_S_AC_2);
    inst_out_ind.flip(2, SIG_AUX_LOAD);

    inst_out_ind.flip(3, SIG_DH_LOAD); // DH <- M(D)
    inst_out_ind.flip(3, SIG_MEM_OE);
    inst_out_ind.flip(3, SIG_D_OE);
    inst_out_ind.flip(3, SIG_S_DAT_2);

    inst_out_ind.flip(4, SIG_DL_LOAD); // DL <- AC
    inst_out_ind.flip(4, SIG_S_DAT_0);

    inst_out_ind.flip(5, SIG_S_AC_0);// AC <- AUX, IO(D) <- AUX
    inst_out_ind.flip(5, SIG_S_AC_2);
    inst_out_ind.flip(5, SIG_AC_LOAD);
    inst_out_ind.flip(5, SIG_MEM_IO);
    inst_out_ind.flip(5, SIG_MEM_WE);
    inst_out_ind.flip(5, SIG_D_OE);

    inst_out_ind.flip(6, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_out_ind.flip(6, SIG_PC_OE);
    inst_out_ind.flip(6, SIG_PC_UP);
    inst_out_ind.flip(6, SIG_MEM_OE);
    inst_out_ind.flip(6, SIG_RCF_CLR);

    microcode[OP_INST_OUT_IND] = inst_out_ind;

    // IN (abs) (0x3D)

    instruction inst_in_abs(4);

    inst_in_abs.flip(0, SIG_DL_LOAD); // DL <- M(PC++)
    inst_in_abs.flip(0, SIG_MEM_OE);
    inst_in_abs.flip(0, SIG_PC_OE);
    inst_in_abs.flip(0, SIG_PC_UP);

    inst_in_abs.flip(1, SIG_DH_LOAD); // DH <- M(PC++)
    inst_in_abs.flip(1, SIG_MEM_OE);
    inst_in_abs.flip(1, SIG_PC_OE);
    inst_in_abs.flip(1, SIG_PC_UP);

    inst_in_abs.flip(2, SIG_AC_LOAD); // AC <- IO(D)
    inst_in_abs.flip(2, SIG_MEM_IO);
    inst_in_abs.flip(2, SIG_MEM_OE);
    inst_in_abs.flip(2, SIG_D_OE);
    inst_in_abs.flip(2, SIG_S_DAT_2);

    inst_in_abs.flip(3, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_in_abs.flip(3, SIG_PC_OE);
    inst_in_abs.flip(3, SIG_PC_UP);
    inst_in_abs.flip(3, SIG_MEM_OE);
    inst_in_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_IN_ABS] = inst_in_abs;

    // OUT (abs) (0x3E)

    instruction inst_out_abs(7);

    inst_out_abs.flip(0, SIG_DL_LOAD); // DL <- M(PC++)
    inst_out_abs.flip(0, SIG_MEM_OE);
    inst_out_abs.flip(0, SIG_PC_OE);
    inst_out_abs.flip(0, SIG_PC_UP);
    inst_out_abs.flip(0, SIG_S_DAT_2);

    inst_out_abs.flip(1, SIG_DH_LOAD); // DH <- M(PC++)
    inst_out_abs.flip(1, SIG_MEM_OE);
    inst_out_abs.flip(1, SIG_PC_OE);
    inst_out_abs.flip(1, SIG_PC_UP);
    inst_out_abs.flip(1, SIG_S_DAT_2);

    inst_out_abs.flip(2, SIG_MEM_IO); // IO(D) <- AC
    inst_out_abs.flip(2, SIG_MEM_WE);
    inst_out_abs.flip(2, SIG_D_OE);
    inst_out_abs.flip(2, SIG_S_DAT_0);

    inst_out_abs.flip(3, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_out_abs.flip(3, SIG_PC_OE);
    inst_out_abs.flip(3, SIG_PC_UP);
    inst_out_abs.flip(3, SIG_MEM_OE);
    inst_out_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_OUT_ABS] = inst_out_abs;

    // LDSPH (0x3F)

    instruction inst_ldsph(2);

    inst_ldsph.flip(0, SIG_AC_LOAD); // AC <- SPH
    inst_ldsph.flip(0, SIG_SP_OE);
    inst_ldsph.flip(0, SIG_S_DAT_0);
    inst_ldsph.flip(0, SIG_S_DAT_1);
    inst_ldsph.flip(0, SIG_S_AC_0);
    inst_ldsph.flip(0, SIG_S_AC_2);

    inst_ldsph.flip(1, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_ldsph.flip(1, SIG_MEM_OE);
    inst_ldsph.flip(1, SIG_PC_OE);
    inst_ldsph.flip(1, SIG_PC_UP);
    inst_ldsph.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_LDSPH] = inst_ldsph;

    /*
        // Muestro todas las instrucciones
        std::cout << std::hex;
        for(int i = 0; i < 64; i++){
            std::cout << "[0x" << i << "]" << std::endl;
            std::cout << microcode[i];
        }
        std::cout << std::dec;
    */

    //############################################################//
    //             GENERACIÓN COMPLETA DEL MICROCÓDIGO            //
    //############################################################//

    /*
        I
        F
        E               R R R R
        T I B           C C C C R R R R R R
        C R R           F F F F I I I I I I
        H Q Q I Z O S C 3 2 1 0 5 4 3 2 1 0

        x x x x x x x x x x x x x x x x x x  <- Palabra de entrada
    
        Archivo completo = 256Kx40 (18 bits de entrada y 40 de salida)
    */

    std::cout << "Instrucciones microprogramadas. Creando las palabras de control...\n";
    c_word salida[262144];

    // Primero genero palabras vacías con RCF_CLR activo
    instruction inst_hlt(1);
    inst_hlt.flip(0, SIG_RCF_CLR);
    for(int i = 0; i < 262144; i++){
        salida[i] = inst_hlt.get(0);
    }
    
    // Hay 256 combinaciones de flags
    for(int comb = 0; comb < 256; comb++){
        // 64 instrucciones
        for(int ins = 0; ins < 64; ins++){
            // Con 16 pasos como máximo
            for(int step = 0; step < 16; step++){
                int posicion = ins + (step<<6) + (comb<<10);
                
                if(step < microcode[ins].get_size()){
                    salida[posicion] = microcode[ins].get(step);
                }else{
                    break;
                }
            }
        }
    }
    
    typedef std::bitset<18> i_word;

    // Hay 262144 palabras de control en total
    for(int palabra = 0; palabra < 262144; palabra++){
        i_word input(palabra);

        bool ifetch = input[17];
        bool irq = input[16];
        bool brq = input[15];
        bool i = input[14];
        bool z = input[13];
        bool o = input[12];
        bool s = input[11];
        bool c = input[10];
        int rcf = ((input.to_ulong() >> 6) & 0xF);
        int ri = input.to_ulong() & 0x3F;

        // Si se trata de un salto condicional
        if(ri >= 0x1C && ri <= 0x23){
            if(// Comprobar si se cumplen las condiciones
                (ri == OP_INST_JZ && z)                         ||
                (ri == OP_INST_JNZ && !z)                       ||
                (ri == OP_INST_JC && c)                         ||
                (ri == OP_INST_JNC && !c)                       ||
                (ri == OP_INST_JS && s)                         ||
                (ri == OP_INST_JNS && !s)                       ||
                (ri == OP_INST_JV && o)                         ||
                (ri == OP_INST_JNV && !o)
            ){
                //std::cout << "salto condicional correcto."<< std::endl;
                /*
                Si se cumple la condición significa que se trata de una instrucción de salto condicional
                con los flags correspondientes así que no se hace nada (la instrucción de salto se ejecuta)
                
                Si no se cumple, se reemplaza por la instrucción que no hace nada más que avanzar a la siguiente
                (como si fuera un NOP de 3 bytes)
                */
            }else{
                //std::cout << "salto condicional nop."<< std::endl;
                switch(rcf){
                    case 0:
                        salida[palabra] = c_word(ALL_INACTIVE);
                        salida[palabra].flip(SIG_PC_UP);
                    break;
                    case 1:
                        salida[palabra] = c_word(ALL_INACTIVE);
                        salida[palabra].flip(SIG_PC_UP);
                    break;
                    case 2:
                        salida[palabra] = inst_nop.get(0);
                    break;
                    default:
                        salida[palabra] = c_word(ALL_INACTIVE);
                        salida[palabra].flip(SIG_RCF_CLR);
                    break;
                }
            }
        }

        // Si se está capturando una interrupción (IFETCH activa)
        if(ifetch){
            if(rcf == 0){
                salida[palabra] = c_word(ALL_INACTIVE); // AUX <- AC, AC <- I/O(INT), IACK
                salida[palabra].flip(SIG_BUS_DIS);
                salida[palabra].flip(SIG_AC_LOAD);
                salida[palabra].flip(SIG_AUX_LOAD);
                salida[palabra].flip(SIG_S_DAT_2);
                salida[palabra].flip(SIG_IACK);
                salida[palabra].flip(SIG_IFETCH);
            }else{
                salida[palabra] = inst_int.get(rcf);
                salida[palabra].flip(SIG_IFETCH); // IFETCH debe mantenerse activa mientras se procesa la interrupción
            }
        }

        // Último paso de la instrucción (Puede cambiar en función de ifetch)
        int last_step;
        if(!ifetch){
            last_step = microcode[ri].get_size() - 1;
        }else{
            last_step = microcode[OP_INST_INT].get_size() - 1;
        }

        // Sólo se atiende a la interrupción si BRQ está inactiva e I está activo
        if(irq && !brq && i){
            // Si es el último paso de la instrucción, activar ifetch
            if(rcf == last_step){
                salida[palabra].flip(SIG_IFETCH);
            }
        }

        if((!irq && brq) || (irq && brq)){
            // Si es el primer paso de la instrucción, activar BACK
            if(rcf == 0){
                salida[palabra] = c_word(ALL_INACTIVE);
                salida[palabra].flip(SIG_BACK);
                salida[palabra].flip(SIG_RCF_CLR);
                salida[palabra].flip(SIG_BUS_DIS);
            }
        }
    }

    // Exportar el archivo binario a 5 archivos .bin (1 por cada EEPROM)

    FILE *archivo0 = fopen("firmware_0.bin", "wb");  // modo binario
    if (archivo0 == NULL) {
        perror("No se pudo abrir el archivo0");
        return 1;
    }
    FILE *archivo1 = fopen("firmware_1.bin", "wb");  // modo binario
    if (archivo1 == NULL) {
        perror("No se pudo abrir el archivo1");
        return 1;
    }
    FILE *archivo2 = fopen("firmware_2.bin", "wb");  // modo binario
    if (archivo2 == NULL) {
        perror("No se pudo abrir el archivo2");
        return 1;
    }
    FILE *archivo3 = fopen("firmware_3.bin", "wb");  // modo binario
    if (archivo3 == NULL) {
        perror("No se pudo abrir el archivo3");
        return 1;
    }
    FILE *archivo4 = fopen("firmware_4.bin", "wb");  // modo binario
    if (archivo4 == NULL) {
        perror("No se pudo abrir el archivo4");
        return 1;
    }

    // Se itera por cada palabra de control
    for(int i = 0; i < 262144; i++){
        // Convierto la palabra a uint64_t
        uint64_t palabra = salida[i].to_ulong();

        // Extraigo 5 palabras de 8 bits cada una
        uint8_t byte_0 = palabra&0xFF;
        uint8_t byte_1 = (palabra>>8)&0xFF;
        uint8_t byte_2 = (palabra>>16)&0xFF;
        uint8_t byte_3 = (palabra>>24)&0xFF;
        uint8_t byte_4 = (palabra>>32)&0xFF;

        // Escribo en el correspondiente archivo
        fwrite(&byte_0, sizeof(uint8_t), 1, archivo0);
        fwrite(&byte_1, sizeof(uint8_t), 1, archivo1);
        fwrite(&byte_2, sizeof(uint8_t), 1, archivo2);
        fwrite(&byte_3, sizeof(uint8_t), 1, archivo3);
        fwrite(&byte_4, sizeof(uint8_t), 1, archivo4);
    }

    fclose(archivo0);
    fclose(archivo1);
    fclose(archivo2);
    fclose(archivo3);
    fclose(archivo4);

    std::cout << "Archivos exportados" << std::endl;

    check_instructions(salida);

    return 0;
}
