#include <iostream>
#include <bitset>
#include <vector>
#include <string>

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
#define SIG_BUS_EN              15
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

#define ALL_INACTIVE            0b0010111011111111000000001000111001111111

// Códigos de operación de cada instrucción

#define OP_INST_JMP_ABS         0x00
#define OP_INST_CLC             0x01
#define OP_INST_STC             0x02
#define OP_INST_CLI             0x03
#define OP_INST_STI             0x04
#define OP_INST_HLT             0x05
#define OP_INST_INC             0x06
#define OP_INST_DEC             0x07
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
#define OP_INST_NEG             0x18
#define OP_INST_CMP_ABS         0x19
#define OP_INST_CMP_IMM         0x1A
#define OP_INST_NOP             0x1B
#define OP_INST_JO              0x1C
#define OP_INST_JNO             0x1D

#define OP_INST_JZ              0x1E
#define OP_INST_JE              0x1E

#define OP_INST_JNZ             0x1F
#define OP_INST_JNE             0x1F

#define OP_INST_JNAE_SIGNED     0x20
#define OP_INST_JB_SIGNED       0x20

#define OP_INST_JAE_SIGNED      0x21
#define OP_INST_JNB_SIGNED      0x21

#define OP_INST_JBE_SIGNED      0x22
#define OP_INST_JNA_SIGNED      0x22

#define OP_INST_JA_SIGNED       0x23
#define OP_INST_JNBE_SIGNED     0x23

#define OP_INST_JC              0x24
#define OP_INST_JNAE_UNSIGNED   0x24
#define OP_INST_JB_UNSIGNED     0x24

#define OP_INST_JNC             0x25
#define OP_INST_JAE_UNSIGNED    0x25
#define OP_INST_JNB_UNSIGNED    0x25

#define OP_INST_JBE_UNSIGNED    0x26
#define OP_INST_JNA_UNSIGNED    0x26

#define OP_INST_JA_UNSIGNED     0x27
#define OP_INST_JNBE_UNSIGNED   0x27

#define OP_INST_JS              0x28
#define OP_INST_JNS             0x29
#define OP_INST_JMP_IND         0x2A
#define OP_INST_SHL             0x2B
#define OP_INST_SHR_SIGNED      0x2C
#define OP_INST_SHR_UNSIGNED    0x2D
#define OP_INST_ROL             0x2E
#define OP_INST_ROR             0x2F
#define OP_INST_IN_IND          0x30
#define OP_INST_PUSH_AC         0x31
#define OP_INST_POP             0x32
#define OP_INST_CALL            0x33
#define OP_INST_RET             0x34
#define OP_INST_INT             0x35

#define OP_INST_IRET            0x36
#define OP_INST_RETI            0x36

#define OP_INST_LDSP            0x37
#define OP_INST_OUT_IND         0x38
#define OP_INST_LDSPL           0x39

#define OP_INST_RCL             0x3A
#define OP_INST_RCR             0x3B
#define OP_INST_CMP_IND         0x3C
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
                std::cout << "Se ha intentado asignar valores al paso " << step << "de una instrucción con tamaño " << size << std::endl;
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

int main(){

    // Vector con todas las instrucciones
    std::vector<instruction> microcode(64, instruction(0));

    // JMP (abs) (0x00)
    instruction inst_jmp_abs(4);

    inst_jmp_abs.flip(0, SIG_MEM_OE);   // DL <- M(PC++)
    inst_jmp_abs.flip(0, SIG_BUS_EN);
    inst_jmp_abs.flip(0, SIG_PC_OE);
    inst_jmp_abs.flip(0, SIG_PC_UP);
    inst_jmp_abs.flip(0, SIG_DL_LOAD);
    inst_jmp_abs.flip(0, SIG_S_DAT_2);

    inst_jmp_abs.flip(1, SIG_MEM_OE);   // DH <- M(PC++)
    inst_jmp_abs.flip(1, SIG_BUS_EN);
    inst_jmp_abs.flip(1, SIG_PC_OE);
    inst_jmp_abs.flip(1, SIG_PC_UP);
    inst_jmp_abs.flip(1, SIG_DH_LOAD);
    inst_jmp_abs.flip(1, SIG_S_DAT_2);

    inst_jmp_abs.flip(2, SIG_PC_LOAD);  // PC <- D

    inst_jmp_abs.flip(3, SIG_PC_UP);    // PC++, RI <- M(D), RCF_CLR
    inst_jmp_abs.flip(3, SIG_MEM_OE);
    inst_jmp_abs.flip(3, SIG_BUS_EN);
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
    inst_clc.flip(0, SIG_BUS_EN);
    inst_clc.flip(0, SIG_PC_UP);
    inst_clc.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_CLC] = inst_clc;

    // STC (0x02)
    instruction inst_stc(1);

    inst_stc.flip(0, SIG_FILL_BIT); // C <- 1, RI <- M(PC++), RCF_CLR0
    inst_stc.flip(0, SIG_S_DAT_1);
    inst_stc.flip(0, SIG_S_DAT_2);
    inst_stc.flip(0, SIG_LOAD_C);
    inst_stc.flip(0, SIG_MEM_OE);
    inst_stc.flip(0, SIG_PC_OE);
    inst_stc.flip(0, SIG_RI_LOAD);
    inst_stc.flip(0, SIG_PC_UP);
    inst_stc.flip(0, SIG_BUS_EN);
    inst_stc.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_STC] = inst_stc;

    // CLI (0x03)
    instruction inst_cli(1);

    inst_cli.flip(0, SIG_S_DAT_1); // I <- 0, RI <- M(PC++), RCF_CLR
    inst_cli.flip(0, SIG_S_DAT_2);
    inst_cli.flip(0, SIG_LOAD_I);
    inst_cli.flip(0, SIG_MEM_OE);
    inst_cli.flip(0, SIG_PC_OE);
    inst_cli.flip(0, SIG_RI_LOAD);
    inst_cli.flip(0, SIG_PC_UP);
    inst_cli.flip(0, SIG_BUS_EN);
    inst_cli.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_CLI] = inst_cli;

    // STI (0x04)
    instruction inst_sti(1);

    inst_sti.flip(0, SIG_FILL_BIT); // I <- 1, RI <- M(PC++), RCF_CLR
    inst_sti.flip(0, SIG_S_DAT_1);
    inst_sti.flip(0, SIG_S_DAT_2);
    inst_sti.flip(0, SIG_LOAD_I);
    inst_sti.flip(0, SIG_MEM_OE);
    inst_sti.flip(0, SIG_BUS_EN);
    inst_sti.flip(0, SIG_PC_OE);
    inst_sti.flip(0, SIG_RI_LOAD);
    inst_sti.flip(0, SIG_PC_UP);
    inst_sti.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_STI] = inst_sti;

    // HLT (0x05)
    instruction inst_hlt(1);

    inst_hlt.flip(0, SIG_RCF_CLR); // La única manera de salir es con RESET

    microcode[OP_INST_HLT] = inst_hlt;

    // INC (0x06)
    instruction inst_inc(1);

    inst_inc.flip(0, SIG_S_DAT_1); // AC <- AC + 0 + 1, C <- ALU_C, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_inc.flip(0, SIG_S_DAT_2);
    inst_inc.flip(0, SIG_MUX_CI_0);
    inst_inc.flip(0, SIG_MUX_C_1);
    inst_inc.flip(0, SIG_LOAD_C);
    inst_inc.flip(0, SIG_MUX_ZOS);
    inst_inc.flip(0, SIG_LOAD_ZOS);
    inst_inc.flip(0, SIG_AC_LOAD);
    inst_inc.flip(0, SIG_MEM_OE);
    inst_sti.flip(0, SIG_BUS_EN);
    inst_inc.flip(0, SIG_RI_LOAD);
    inst_inc.flip(0, SIG_PC_OE);
    inst_inc.flip(0, SIG_PC_UP);
    inst_inc.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_INC] = inst_inc;

    // DEC (0x07)
    instruction inst_dec(1);

    inst_dec.flip(0, SIG_S_DAT_1); // AC <- AC - 1, C <- ALU_C, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_dec.flip(0, SIG_S_DAT_2);
    inst_dec.flip(0, SIG_FILL_BIT);
    inst_dec.flip(0, SIG_MUX_C_1);
    inst_dec.flip(0, SIG_LOAD_C);
    inst_dec.flip(0, SIG_MUX_ZOS);
    inst_dec.flip(0, SIG_LOAD_ZOS);
    inst_dec.flip(0, SIG_AC_LOAD);
    inst_dec.flip(0, SIG_MEM_OE);
    inst_dec.flip(0, SIG_RI_LOAD);
    inst_dec.flip(0, SIG_PC_OE);
    inst_dec.flip(0, SIG_PC_UP);
    inst_dec.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_DEC] = inst_dec;

    // LOAD (abs) (0x08)

    instruction inst_load_abs(4);

    inst_load_abs.flip(0, SIG_MEM_OE);  // DL <- M(PC++)
    inst_load_abs.flip(0, SIG_PC_OE);
    inst_load_abs.flip(0, SIG_PC_UP);
    inst_load_abs.flip(0, SIG_DL_LOAD);
    inst_load_abs.flip(0, SIG_S_DAT_2);
    inst_load_abs.flip(0, SIG_BUS_EN);
    
    inst_load_abs.flip(1, SIG_MEM_OE);  // DH <- M(PC++)
    inst_load_abs.flip(1, SIG_PC_OE);
    inst_load_abs.flip(1, SIG_PC_UP);
    inst_load_abs.flip(1, SIG_DH_LOAD);
    inst_load_abs.flip(1, SIG_S_DAT_2);
    inst_load_abs.flip(1, SIG_BUS_EN);

    inst_load_abs.flip(2, SIG_MEM_OE);  // AC <- M(D)
    inst_load_abs.flip(2, SIG_BUS_EN);
    inst_load_abs.flip(2, SIG_D_OE);
    inst_load_abs.flip(2, SIG_S_AC_2);
    inst_load_abs.flip(2, SIG_S_AC_0);
    inst_load_abs.flip(2, SIG_AC_LOAD);
    inst_load_abs.flip(2, SIG_S_DAT_2);

    inst_load_abs.flip(3, SIG_MEM_OE);  // RI <- M(PC++), RCF_CLR
    inst_load_abs.flip(3, SIG_BUS_EN);
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
    inst_load_imm.flip(0, SIG_BUS_EN);

    inst_load_imm.flip(1, SIG_MEM_OE);  // RI <- M(PC++), RCF_CLR
    inst_load_imm.flip(1, SIG_BUS_EN);
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
    inst_load_ind.flip(0, SIG_BUS_EN);

    inst_load_ind.flip(1, SIG_MEM_OE); // DH <- M(PC++)
    inst_load_ind.flip(1, SIG_PC_OE);
    inst_load_ind.flip(1, SIG_PC_UP);
    inst_load_ind.flip(1, SIG_DH_LOAD);
    inst_load_ind.flip(1, SIG_S_DAT_2);
    inst_load_ind.flip(1, SIG_BUS_EN);

    inst_load_ind.flip(2, SIG_MEM_OE); // AC <- M(D++)
    inst_load_ind.flip(2, SIG_BUS_EN);
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
    inst_load_ind.flip(3, SIG_BUS_EN);

    inst_load_ind.flip(4, SIG_S_DAT_0); // DL <- AC
    inst_load_ind.flip(4, SIG_DL_LOAD);

    inst_load_ind.flip(5, SIG_MEM_OE); // AC <- M(D)
    inst_load_ind.flip(5, SIG_BUS_EN);
    inst_load_ind.flip(5, SIG_D_OE);
    inst_load_ind.flip(5, SIG_S_DAT_2);
    inst_load_ind.flip(5, SIG_S_AC_0);
    inst_load_ind.flip(5, SIG_S_AC_2);
    inst_load_ind.flip(5, SIG_AC_LOAD);
    
    inst_load_ind.flip(6, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_load_ind.flip(6, SIG_BUS_EN);
    inst_load_ind.flip(6, SIG_RI_LOAD);
    inst_load_ind.flip(6, SIG_PC_OE);
    inst_load_ind.flip(6, SIG_PC_UP);
    inst_load_ind.flip(6, SIG_RCF_CLR);

    microcode[OP_INST_LOAD_IND] = inst_load_ind;

    // STORE (abs) (0x0B)

    instruction inst_store_abs(4);

    inst_store_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_store_abs.flip(0, SIG_BUS_EN);
    inst_store_abs.flip(0, SIG_PC_OE);
    inst_store_abs.flip(0, SIG_PC_UP);
    inst_store_abs.flip(0, SIG_DL_LOAD);

    inst_store_abs.flip(1, SIG_MEM_OE); // DH <- M(PC++)
    inst_store_abs.flip(1, SIG_BUS_EN);
    inst_store_abs.flip(1, SIG_PC_OE);
    inst_store_abs.flip(1, SIG_PC_UP);
    inst_store_abs.flip(1, SIG_DH_LOAD);

    inst_store_abs.flip(2, SIG_MEM_WE); // M(D) <- AC
    inst_store_abs.flip(2, SIG_BUS_EN);
    inst_store_abs.flip(2, SIG_S_DAT_0);
    inst_store_abs.flip(2, SIG_D_OE);

    inst_store_abs.flip(3, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_store_abs.flip(3, SIG_BUS_EN);
    inst_store_abs.flip(3, SIG_PC_OE);
    inst_store_abs.flip(3, SIG_PC_UP);
    inst_store_abs.flip(3, SIG_RI_LOAD);
    inst_store_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_STORE_ABS] = inst_store_abs;

    // STORE (ind) (0x0C)
    instruction inst_store_ind(7);

    inst_store_ind.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_store_ind.flip(0, SIG_BUS_EN);
    inst_store_ind.flip(0, SIG_S_DAT_2);
    inst_store_ind.flip(0, SIG_PC_OE);
    inst_store_ind.flip(0, SIG_PC_UP);
    inst_store_ind.flip(0, SIG_DL_LOAD);

    inst_store_ind.flip(1, SIG_MEM_OE); // DH <- M(PC++), AUX <- AC
    inst_store_ind.flip(1, SIG_BUS_EN);
    inst_store_ind.flip(1, SIG_S_DAT_2);
    inst_store_ind.flip(1, SIG_PC_OE);
    inst_store_ind.flip(1, SIG_PC_UP);
    inst_store_ind.flip(1, SIG_DH_LOAD);
    inst_store_ind.flip(1, SIG_AUX_LOAD);

    inst_store_ind.flip(2, SIG_MEM_OE); // AC <- M(D++)
    inst_store_ind.flip(2, SIG_BUS_EN);
    inst_store_ind.flip(2, SIG_S_DAT_2);
    inst_store_ind.flip(2, SIG_D_OE);
    inst_store_ind.flip(2, SIG_D_UP);
    inst_store_ind.flip(2, SIG_S_AC_2);
    inst_store_ind.flip(2, SIG_S_AC_0);
    inst_store_ind.flip(2, SIG_AC_LOAD);

    inst_store_ind.flip(3, SIG_MEM_OE); // DH <- M(D)
    inst_store_ind.flip(3, SIG_BUS_EN);
    inst_store_ind.flip(3, SIG_S_DAT_2);
    inst_store_ind.flip(3, SIG_D_OE);
    inst_store_ind.flip(3, SIG_DH_LOAD);
    
    inst_store_ind.flip(4, SIG_S_DAT_0); // DL <- AC
    inst_store_ind.flip(4, SIG_DL_LOAD);

    inst_store_ind.flip(5, SIG_MEM_WE); // M(D) <- AUX, AC <- AUX
    inst_store_ind.flip(5, SIG_BUS_EN);
    inst_store_ind.flip(5, SIG_D_OE);
    inst_store_ind.flip(5, SIG_S_AC_2);
    inst_store_ind.flip(5, SIG_S_AC_0);
    inst_store_ind.flip(5, SIG_AC_LOAD);

    inst_store_ind.flip(6, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_store_ind.flip(6, SIG_PC_OE);
    inst_store_ind.flip(6, SIG_PC_UP);
    inst_store_ind.flip(6, SIG_MEM_OE);
    inst_store_ind.flip(6, SIG_BUS_EN);
    inst_store_ind.flip(6, SIG_RCF_CLR);

    microcode[OP_INST_STORE_IND] = inst_store_ind;
    
    // ADD (abs) (0x0D)
    
    instruction inst_add_abs(4);

    inst_add_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_add_abs.flip(0, SIG_BUS_EN);
    inst_add_abs.flip(0, SIG_S_DAT_2);
    inst_add_abs.flip(0, SIG_PC_OE);
    inst_add_abs.flip(0, SIG_PC_UP);
    inst_add_abs.flip(0, SIG_DL_LOAD);

    inst_add_abs.flip(1, SIG_MEM_OE); // DH <- M(PC++)
    inst_add_abs.flip(1, SIG_BUS_EN);
    inst_add_abs.flip(1, SIG_S_DAT_2);
    inst_add_abs.flip(1, SIG_PC_OE);
    inst_add_abs.flip(1, SIG_PC_UP);
    inst_add_abs.flip(1, SIG_DH_LOAD);

    inst_add_abs.flip(2, SIG_MEM_OE); // AC <- AC + M(D), C <- ALU_C, ZOS <- ALU_ZOS
    inst_add_abs.flip(2, SIG_BUS_EN);
    inst_add_abs.flip(2, SIG_S_DAT_2);
    inst_add_abs.flip(2, SIG_D_OE);
    inst_add_abs.flip(2, SIG_MUX_ZOS);
    inst_add_abs.flip(2, SIG_LOAD_ZOS);
    inst_add_abs.flip(2, SIG_AC_LOAD);
    inst_add_abs.flip(2, SIG_LOAD_C);
    inst_add_abs.flip(2, SIG_MUX_C_1);

    inst_add_abs.flip(3, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_add_abs.flip(3, SIG_BUS_EN);
    inst_add_abs.flip(3, SIG_RI_LOAD);
    inst_add_abs.flip(3, SIG_PC_OE);
    inst_add_abs.flip(3, SIG_PC_UP);
    inst_add_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_ADD_ABS] = inst_add_abs;

    // ADD (imm) (0x0E)

    instruction inst_add_imm(2);

    inst_add_imm.flip(0, SIG_MEM_OE); // AC <- AC + M(PC++), C <- ALU_C, ZOS <- ALU_ZOS
    inst_add_imm.flip(0, SIG_BUS_EN);
    inst_add_imm.flip(0, SIG_S_DAT_2);
    inst_add_imm.flip(0, SIG_PC_OE);
    inst_add_imm.flip(0, SIG_PC_UP);
    inst_add_imm.flip(0, SIG_MUX_ZOS);
    inst_add_imm.flip(0, SIG_LOAD_ZOS);
    inst_add_imm.flip(0, SIG_AC_LOAD);
    inst_add_imm.flip(0, SIG_LOAD_C);
    inst_add_imm.flip(0, SIG_MUX_C_1);

    inst_add_imm.flip(1, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_add_imm.flip(1, SIG_BUS_EN);
    inst_add_imm.flip(1, SIG_RI_LOAD);
    inst_add_imm.flip(1, SIG_PC_OE);
    inst_add_imm.flip(1, SIG_PC_UP);
    inst_add_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_ADD_IMM] = inst_add_imm;

    // ADC (abs) (0x0F)

    instruction inst_adc_abs(4);

    inst_adc_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_adc_abs.flip(0, SIG_BUS_EN);
    inst_adc_abs.flip(0, SIG_S_DAT_2);
    inst_adc_abs.flip(0, SIG_PC_OE);
    inst_adc_abs.flip(0, SIG_PC_UP);
    inst_adc_abs.flip(0, SIG_DL_LOAD);

    inst_adc_abs.flip(1, SIG_MEM_OE); // DH <- (M(PC++))
    inst_adc_abs.flip(1, SIG_BUS_EN);
    inst_adc_abs.flip(1, SIG_S_DAT_2);
    inst_adc_abs.flip(1, SIG_PC_OE);
    inst_adc_abs.flip(1, SIG_PC_UP);
    inst_adc_abs.flip(1, SIG_DH_LOAD);

    inst_adc_abs.flip(2, SIG_MEM_OE); // AC <- AC + M(D) + C, C <- ALU_C, ZOS <- ALU_ZOS
    inst_adc_abs.flip(2, SIG_BUS_EN);
    inst_adc_abs.flip(2, SIG_S_DAT_2);
    inst_adc_abs.flip(2, SIG_D_OE);
    inst_adc_abs.flip(2, SIG_MUX_ZOS);
    inst_adc_abs.flip(2, SIG_LOAD_ZOS);
    inst_adc_abs.flip(2, SIG_AC_LOAD);
    inst_adc_abs.flip(2, SIG_LOAD_C);
    inst_adc_abs.flip(2, SIG_MUX_CI_2);
    inst_adc_abs.flip(2, SIG_MUX_C_1);

    inst_adc_abs.flip(3, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_adc_abs.flip(3, SIG_BUS_EN);
    inst_adc_abs.flip(3, SIG_RI_LOAD);
    inst_adc_abs.flip(3, SIG_PC_OE);
    inst_adc_abs.flip(3, SIG_PC_UP);
    inst_adc_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_ADC_ABS] = inst_adc_abs;

    // ADC (imm) (0x10)

    instruction inst_adc_imm(2);

    inst_adc_imm.flip(0, SIG_MEM_OE); // AC <- AC + M(PC++) + C, C <- ALU_C, ZOS <- ALU_ZOS
    inst_adc_imm.flip(0, SIG_BUS_EN);
    inst_adc_imm.flip(0, SIG_S_DAT_2);
    inst_adc_imm.flip(0, SIG_PC_OE);
    inst_adc_imm.flip(0, SIG_PC_UP);
    inst_adc_imm.flip(0, SIG_MUX_ZOS);
    inst_adc_imm.flip(0, SIG_LOAD_ZOS);
    inst_adc_imm.flip(0, SIG_AC_LOAD);
    inst_adc_imm.flip(0, SIG_LOAD_C);
    inst_adc_imm.flip(0, SIG_MUX_CI_2);
    inst_adc_imm.flip(0, SIG_MUX_C_1);

    inst_adc_imm.flip(1, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_adc_imm.flip(1, SIG_BUS_EN);
    inst_adc_imm.flip(1, SIG_RI_LOAD);
    inst_adc_imm.flip(1, SIG_PC_OE);
    inst_adc_imm.flip(1, SIG_PC_UP);
    inst_adc_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_ADC_IMM] = inst_add_imm;

    // SUB (abs) (0x11)

    instruction inst_sub_abs(4);

    inst_sub_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_sub_abs.flip(0, SIG_BUS_EN);
    inst_sub_abs.flip(0, SIG_S_DAT_2);
    inst_sub_abs.flip(0, SIG_PC_OE);
    inst_sub_abs.flip(0, SIG_PC_UP);
    inst_sub_abs.flip(0, SIG_DL_LOAD);

    inst_sub_abs.flip(1, SIG_MEM_OE); // DH <- M(PC++)
    inst_sub_abs.flip(1, SIG_BUS_EN);
    inst_sub_abs.flip(1, SIG_S_DAT_2);
    inst_sub_abs.flip(1, SIG_PC_OE);
    inst_sub_abs.flip(1, SIG_PC_UP);
    inst_sub_abs.flip(1, SIG_DH_LOAD);

    inst_sub_abs.flip(2, SIG_MEM_OE); // AC <- AC + NOT(M(D)) + 1, C <- ALU_C, ZOS <- ALU_ZOS
    inst_sub_abs.flip(2, SIG_BUS_EN);
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
    inst_sub_abs.flip(3, SIG_BUS_EN);
    inst_sub_abs.flip(3, SIG_RI_LOAD);
    inst_sub_abs.flip(3, SIG_PC_OE);
    inst_sub_abs.flip(3, SIG_PC_UP);
    inst_sub_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_SUB_ABS] = inst_sub_abs;

    // SUB (imm) (0x12)

    instruction inst_sub_imm(2);

    inst_sub_imm.flip(0, SIG_MEM_OE); // AUX <- AC, AC <- NOT(M(PC++))
    inst_sub_imm.flip(0, SIG_BUS_EN);
    inst_sub_imm.flip(0, SIG_PC_OE);
    inst_sub_imm.flip(0, SIG_PC_UP);
    inst_sub_imm.flip(0, SIG_S_AC_2);
    inst_sub_imm.flip(0, SIG_AC_LOAD);
    inst_sub_imm.flip(0, SIG_AUX_LOAD);
    
    inst_sub_imm.flip(1, SIG_MEM_OE); // AC <- AC + AUX + 1, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_sub_imm.flip(1, SIG_BUS_EN);
    inst_sub_imm.flip(1, SIG_AC_LOAD);
    inst_sub_imm.flip(1, SIG_MUX_CI_0);
    inst_sub_imm.flip(1, SIG_MUX_ZOS);
    inst_sub_imm.flip(1, SIG_LOAD_ZOS);
    inst_sub_imm.flip(1, SIG_MUX_C_1);
    inst_sub_imm.flip(1, SIG_LOAD_C);
    inst_sub_imm.flip(1, SIG_S_AC_2);
    inst_sub_imm.flip(1, SIG_RI_LOAD);
    inst_sub_imm.flip(1, SIG_PC_OE);
    inst_sub_imm.flip(1, SIG_PC_UP);
    inst_sub_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_SUB_IMM] = inst_sub_imm;

    // AND (abs) (0x13)
    instruction inst_and_abs(4);

    inst_and_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_and_abs.flip(0, SIG_BUS_EN);
    inst_and_abs.flip(0, SIG_PC_OE);
    inst_and_abs.flip(0, SIG_PC_UP);
    inst_and_abs.flip(0, SIG_DL_LOAD);

    inst_and_abs.flip(1, SIG_MEM_OE); // DH <- M(PC++)
    inst_and_abs.flip(1, SIG_BUS_EN);
    inst_and_abs.flip(1, SIG_PC_OE);
    inst_and_abs.flip(1, SIG_PC_UP);
    inst_and_abs.flip(1, SIG_DH_LOAD);

    inst_and_abs.flip(2, SIG_MEM_OE); // AC <- AC & M(D), ZOS <- ALU_ZOS
    inst_and_abs.flip(2, SIG_BUS_EN);
    inst_and_abs.flip(2, SIG_S_DAT_2);
    inst_and_abs.flip(2, SIG_D_OE);
    inst_and_abs.flip(2, SIG_S_AC_1);
    inst_and_abs.flip(2, SIG_AC_LOAD);
    inst_and_abs.flip(2, SIG_MUX_ZOS);
    inst_and_abs.flip(2, SIG_LOAD_ZOS);

    inst_and_abs.flip(3, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_and_abs.flip(3, SIG_BUS_EN);
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
    inst_and_imm.flip(0, SIG_BUS_EN);
    inst_and_imm.flip(0, SIG_S_DAT_2);
    inst_and_imm.flip(0, SIG_MUX_ZOS);
    inst_and_imm.flip(0, SIG_LOAD_ZOS);

    inst_and_imm.flip(1, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_and_imm.flip(1, SIG_BUS_EN);
    inst_and_imm.flip(1, SIG_RI_LOAD);
    inst_and_imm.flip(1, SIG_PC_OE);
    inst_and_imm.flip(1, SIG_PC_UP);
    inst_and_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_AND_IMM] = inst_and_imm;

    // OR (abs) (0x15)

    instruction inst_or_abs(4);

    inst_or_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_or_abs.flip(0, SIG_BUS_EN);
    inst_or_abs.flip(0, SIG_PC_OE);
    inst_or_abs.flip(0, SIG_PC_UP);
    inst_or_abs.flip(0, SIG_DL_LOAD);

    inst_or_abs.flip(1, SIG_MEM_OE); // DH <- M(PC++)
    inst_or_abs.flip(1, SIG_BUS_EN);
    inst_or_abs.flip(1, SIG_PC_OE);
    inst_or_abs.flip(1, SIG_PC_UP);
    inst_or_abs.flip(1, SIG_DH_LOAD);

    inst_or_abs.flip(2, SIG_MEM_OE); // AC <- AC | M(D), ZOS <- ALU_ZOS
    inst_or_abs.flip(2, SIG_BUS_EN);
    inst_or_abs.flip(2, SIG_D_OE);
    inst_or_abs.flip(2, SIG_S_AC_1);
    inst_or_abs.flip(2, SIG_S_AC_0);
    inst_or_abs.flip(2, SIG_AC_LOAD);
    inst_or_abs.flip(2, SIG_MUX_ZOS);
    inst_or_abs.flip(2, SIG_LOAD_ZOS);

    inst_or_abs.flip(3, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_or_abs.flip(3, SIG_BUS_EN);
    inst_or_abs.flip(3, SIG_RI_LOAD);
    inst_or_abs.flip(3, SIG_PC_OE);
    inst_or_abs.flip(3, SIG_PC_UP);
    inst_or_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_OR_ABS] = inst_or_abs;

    // OR (0x16) (imm)

    instruction inst_or_imm(2);

    inst_or_imm.flip(0, SIG_AC_LOAD); // AC <- AC | M(PC++), ZOS <- ALU_ZOS
    inst_or_imm.flip(0, SIG_S_AC_2);
    inst_or_imm.flip(0, SIG_PC_OE);
    inst_or_imm.flip(0, SIG_PC_UP);
    inst_or_imm.flip(0, SIG_MEM_OE);
    inst_or_imm.flip(0, SIG_BUS_EN);
    inst_or_imm.flip(0, SIG_S_DAT_2);
    inst_or_imm.flip(0, SIG_MUX_ZOS);
    inst_or_imm.flip(0, SIG_LOAD_ZOS);

    inst_or_imm.flip(1, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_or_imm.flip(1, SIG_BUS_EN);
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
    inst_not.flip(0, SIG_BUS_EN);
    inst_not.flip(0, SIG_MEM_OE);
    inst_not.flip(0, SIG_PC_OE);
    inst_not.flip(0, SIG_PC_UP);
    inst_not.flip(0, SIG_RCF_CLR);
    inst_not.flip(0, SIG_MUX_ZOS);
    inst_not.flip(0, SIG_LOAD_ZOS);

    microcode[OP_INST_NOT] = inst_not;

    // NEG (0x18)

    instruction inst_neg(1);

    inst_neg.flip(0, SIG_AC_LOAD); // AC <- NOT(AC)
    inst_neg.flip(0, SIG_S_AC_2);

    inst_neg.flip(1, SIG_AC_LOAD); // AC <- AC + 1, RI <- M(PC++), RCF_CLR
    inst_neg.flip(1, SIG_S_DAT_2);
    inst_neg.flip(1, SIG_S_DAT_1);
    inst_neg.flip(1, SIG_RI_LOAD);
    inst_neg.flip(1, SIG_BUS_EN);
    inst_neg.flip(1, SIG_MEM_OE);
    inst_neg.flip(1, SIG_PC_OE);
    inst_neg.flip(1, SIG_PC_UP);
    inst_neg.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_NEG] = inst_neg;

    // CMP (abs) (0x19)

    instruction inst_cmp_abs(4);

    inst_cmp_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_cmp_abs.flip(0, SIG_BUS_EN);
    inst_cmp_abs.flip(0, SIG_S_DAT_2);
    inst_cmp_abs.flip(0, SIG_PC_OE);
    inst_cmp_abs.flip(0, SIG_PC_UP);
    inst_cmp_abs.flip(0, SIG_DL_LOAD);

    inst_cmp_abs.flip(1, SIG_MEM_OE); // DH <- M(PC++)
    inst_cmp_abs.flip(1, SIG_BUS_EN);
    inst_cmp_abs.flip(1, SIG_S_DAT_2);
    inst_cmp_abs.flip(1, SIG_PC_OE);
    inst_cmp_abs.flip(1, SIG_PC_UP);
    inst_cmp_abs.flip(1, SIG_DH_LOAD);

    inst_cmp_abs.flip(2, SIG_MEM_OE); // (nada) <- AC + NOT(M(D)) + 1, C <- ALU_C, ZOS <- ALU_ZOS
    inst_cmp_abs.flip(2, SIG_BUS_EN);
    inst_cmp_abs.flip(2, SIG_D_OE);
    inst_cmp_abs.flip(2, SIG_MUX_ADD);
    inst_cmp_abs.flip(2, SIG_MUX_CI_0);
    inst_cmp_abs.flip(2, SIG_LOAD_C);
    inst_cmp_abs.flip(2, SIG_MUX_C_1);
    inst_cmp_abs.flip(2, SIG_LOAD_ZOS);
    inst_cmp_abs.flip(2, SIG_MUX_ZOS);
    inst_cmp_abs.flip(2, SIG_S_DAT_2);

    inst_cmp_abs.flip(3, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_cmp_abs.flip(3, SIG_BUS_EN);
    inst_cmp_abs.flip(3, SIG_RI_LOAD);
    inst_cmp_abs.flip(3, SIG_PC_OE);
    inst_cmp_abs.flip(3, SIG_PC_UP);
    inst_cmp_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_CMP_ABS] = inst_cmp_abs;

    // CMP (imm) (0x1A)

    instruction inst_cmp_imm(2);

    inst_cmp_imm.flip(0, SIG_MEM_OE); // AUX <- AC, AC <- NOT(M(PC++))
    inst_cmp_imm.flip(0, SIG_BUS_EN);
    inst_cmp_imm.flip(0, SIG_PC_OE);
    inst_cmp_imm.flip(0, SIG_PC_UP);
    inst_cmp_imm.flip(0, SIG_S_AC_2);
    inst_cmp_imm.flip(0, SIG_AC_LOAD);
    inst_cmp_imm.flip(0, SIG_AUX_LOAD);
    
    inst_cmp_imm.flip(1, SIG_MEM_OE); // (nada) <- AC + AUX + 1, C <- ALU_C, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_cmp_imm.flip(1, SIG_BUS_EN);
    inst_cmp_imm.flip(1, SIG_MUX_CI_0);
    inst_cmp_imm.flip(1, SIG_MUX_ZOS);
    inst_cmp_imm.flip(1, SIG_LOAD_ZOS);
    inst_cmp_imm.flip(1, SIG_MUX_C_1);
    inst_cmp_imm.flip(1, SIG_LOAD_C);
    inst_cmp_imm.flip(1, SIG_S_AC_2);
    inst_cmp_imm.flip(1, SIG_RI_LOAD);
    inst_cmp_imm.flip(1, SIG_PC_OE);
    inst_cmp_imm.flip(1, SIG_PC_UP);
    inst_cmp_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_CMP_IMM] = inst_cmp_imm;

    // NOP (0x1B)

    instruction inst_nop(1);
    
    inst_nop.flip(0, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_nop.flip(0, SIG_BUS_EN);
    inst_nop.flip(0, SIG_PC_OE);
    inst_nop.flip(0, SIG_PC_UP);
    inst_nop.flip(0, SIG_MEM_OE);
    inst_nop.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_NOP] = inst_nop;

    // SALTOS CONDICIONALES (abs) (0x1C -> 0x29)
    // Estas instrucciones tienen el mismo microprograma que JMP pero
    // teniendo en cuenta las flags
    for(int i = 0x1C; i <= 0x29; i++){
        microcode[i] = inst_jmp_abs;
    }

    // JMP (ind) (0x2A)

    instruction inst_jmp_ind(4);

    inst_jmp_ind.flip(0, SIG_MEM_OE);   // DL <- M(PC++)
    inst_jmp_ind.flip(0, SIG_BUS_EN);
    inst_jmp_ind.flip(0, SIG_PC_OE);
    inst_jmp_ind.flip(0, SIG_PC_UP);
    inst_jmp_ind.flip(0, SIG_DL_LOAD);
    inst_jmp_ind.flip(0, SIG_S_DAT_2);

    inst_jmp_ind.flip(1, SIG_MEM_OE);   // DH <- M(PC++), AUX <- AC
    inst_jmp_ind.flip(1, SIG_BUS_EN);
    inst_jmp_ind.flip(1, SIG_PC_OE);
    inst_jmp_ind.flip(1, SIG_PC_UP);
    inst_jmp_ind.flip(1, SIG_DH_LOAD);
    inst_jmp_ind.flip(1, SIG_S_DAT_2);
    inst_jmp_ind.flip(1, SIG_AUX_LOAD);

    inst_jmp_ind.flip(2, SIG_AC_LOAD); // AC <- M(D++)
    inst_jmp_ind.flip(2, SIG_BUS_EN);
    inst_jmp_ind.flip(2, SIG_MEM_OE);
    inst_jmp_ind.flip(2, SIG_D_OE);
    inst_jmp_ind.flip(2, SIG_D_UP);

    inst_jmp_ind.flip(3, SIG_DH_LOAD); // DH <- M(D++)
    inst_jmp_ind.flip(3, SIG_MEM_OE);
    inst_jmp_ind.flip(3, SIG_BUS_EN);
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
    inst_jmp_ind.flip(6, SIG_BUS_EN);
    inst_jmp_ind.flip(6, SIG_D_OE);
    inst_jmp_ind.flip(6, SIG_RI_LOAD);
    inst_jmp_ind.flip(6, SIG_RCF_CLR);

    microcode[OP_INST_JMP_IND] = inst_jmp_ind;

    // SHL (0x2B)

    instruction inst_shl(1);

    inst_shl.flip(0, SIG_AC_LOAD); // AC <- AC + AC, C <- AC.7, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_shl.flip(0, SIG_S_DAT_0);
    inst_shl.flip(0, SIG_MUX_ZOS);
    inst_shl.flip(0, SIG_LOAD_ZOS);
    inst_shl.flip(0, SIG_MUX_C_0);
    inst_shl.flip(0, SIG_LOAD_C);
    inst_shl.flip(0, SIG_RI_LOAD);
    inst_shl.flip(0, SIG_BUS_EN);
    inst_shl.flip(0, SIG_PC_OE);
    inst_shl.flip(0, SIG_PC_UP);
    inst_shl.flip(0, SIG_MEM_OE);
    inst_shl.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_SHL] = inst_shl;

    // SHR SIGNED (0x2C)

    instruction inst_shr_signed(1);

    inst_shr_signed.flip(0, SIG_AC_LOAD); // AC <- S & AC >> 1, C <- AC.0, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_shr_signed.flip(0, SIG_S_AC_0);
    inst_shr_signed.flip(0, SIG_MUX_CI_0);
    inst_shr_signed.flip(0, SIG_MUX_CI_1);
    inst_shr_signed.flip(0, SIG_MUX_ZOS);
    inst_shr_signed.flip(0, SIG_LOAD_ZOS);
    inst_shr_signed.flip(0, SIG_LOAD_C);
    inst_shr_signed.flip(0, SIG_RI_LOAD);
    inst_shr_signed.flip(0, SIG_BUS_EN);
    inst_shr_signed.flip(0, SIG_PC_OE);
    inst_shr_signed.flip(0, SIG_PC_UP);
    inst_shr_signed.flip(0, SIG_MEM_OE);
    inst_shr_signed.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_SHR_SIGNED] = inst_shr_signed;

    // SHR UNSIGNED (0x2D)

    instruction inst_shr_unsigned(1);

    inst_shr_unsigned.flip(0, SIG_AC_LOAD); // AC <- S & AC7..AC1, C <- AC.0, ZOS <- ALU_ZOS, RI <- M(PC++), RCF_CLR
    inst_shr_unsigned.flip(0, SIG_S_AC_0);
    inst_shr_unsigned.flip(0, SIG_MUX_ZOS);
    inst_shr_unsigned.flip(0, SIG_LOAD_ZOS);
    inst_shr_unsigned.flip(0, SIG_LOAD_C);
    inst_shr_unsigned.flip(0, SIG_RI_LOAD);
    inst_shr_unsigned.flip(0, SIG_BUS_EN);
    inst_shr_unsigned.flip(0, SIG_PC_OE);
    inst_shr_unsigned.flip(0, SIG_PC_UP);
    inst_shr_unsigned.flip(0, SIG_MEM_OE);
    inst_shr_unsigned.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_SHR_UNSIGNED] = inst_shr_unsigned;

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
    inst_rol.flip(0, SIG_BUS_EN);
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
    inst_ror.flip(0, SIG_BUS_EN);
    inst_ror.flip(0, SIG_PC_OE);
    inst_ror.flip(0, SIG_PC_UP);
    inst_ror.flip(0, SIG_MEM_OE);
    inst_ror.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_ROR] = inst_ror;

    // IN (ind) (0x30) (puntero de E/S almacenado en memoria)

    instruction inst_in_ind(7);

    inst_in_ind.flip(0, SIG_DL_LOAD); // DL <- M(PC++)
    inst_in_ind.flip(0, SIG_BUS_EN);
    inst_in_ind.flip(0, SIG_MEM_OE);
    inst_in_ind.flip(0, SIG_PC_OE);
    inst_in_ind.flip(0, SIG_PC_UP);
    inst_in_ind.flip(0, SIG_S_DAT_2);

    inst_in_ind.flip(1, SIG_DH_LOAD); // DH <- M(PC++)
    inst_in_ind.flip(1, SIG_BUS_EN);
    inst_in_ind.flip(1, SIG_MEM_OE);
    inst_in_ind.flip(1, SIG_PC_OE);
    inst_in_ind.flip(1, SIG_PC_UP);
    inst_in_ind.flip(1, SIG_S_DAT_2);

    inst_in_ind.flip(2, SIG_AC_LOAD); // AC <- M(D++)
    inst_in_ind.flip(2, SIG_BUS_EN);
    inst_in_ind.flip(2, SIG_MEM_OE);
    inst_in_ind.flip(2, SIG_D_OE);
    inst_in_ind.flip(2, SIG_D_UP);
    inst_in_ind.flip(2, SIG_S_DAT_2);
    inst_in_ind.flip(2, SIG_S_AC_0);
    inst_in_ind.flip(2, SIG_S_AC_2);

    inst_in_ind.flip(3, SIG_DH_LOAD); // DH <- M(D)
    inst_in_ind.flip(3, SIG_BUS_EN);
    inst_in_ind.flip(3, SIG_MEM_OE);
    inst_in_ind.flip(3, SIG_D_OE);
    inst_in_ind.flip(3, SIG_S_DAT_2);

    inst_in_ind.flip(4, SIG_DL_LOAD); // DL <- AC
    inst_in_ind.flip(4, SIG_S_DAT_0);

    inst_in_ind.flip(5, SIG_AC_LOAD); // AC <- IO(D)
    inst_in_ind.flip(5, SIG_BUS_EN);
    inst_in_ind.flip(5, SIG_MEM_IO);
    inst_in_ind.flip(5, SIG_MEM_OE);
    inst_in_ind.flip(5, SIG_D_OE);
    inst_in_ind.flip(5, SIG_S_DAT_2);

    inst_in_ind.flip(6, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_in_ind.flip(6, SIG_BUS_EN);
    inst_in_ind.flip(6, SIG_PC_OE);
    inst_in_ind.flip(6, SIG_PC_UP);
    inst_in_ind.flip(6, SIG_MEM_OE);
    inst_in_ind.flip(6, SIG_RCF_CLR);

    microcode[OP_INST_IN_IND] = inst_in_ind;

    // PUSH (0x31)

    instruction inst_push_ac(2);

    inst_push_ac.flip(0, SIG_SP_OE); // M(SP--) <- AC
    inst_push_ac.flip(0, SIG_SP_DOWN);
    inst_push_ac.flip(0, SIG_S_DAT_0);

    inst_push_ac.flip(1, SIG_BUS_EN); // RI <- M(PC++), RCF_CLR
    inst_push_ac.flip(1, SIG_MEM_OE);
    inst_push_ac.flip(1, SIG_PC_OE);
    inst_push_ac.flip(1, SIG_PC_UP);
    inst_push_ac.flip(1, SIG_RI_LOAD);
    inst_push_ac.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_PUSH_AC] = inst_push_ac;

    // POP (0x32)

    instruction inst_pop(2);

    inst_pop.flip(0, SIG_AC_LOAD); // AC <- M(SP++)
    inst_pop.flip(0, SIG_S_AC_0);
    inst_pop.flip(0, SIG_S_AC_2);
    inst_pop.flip(0, SIG_S_DAT_2);
    inst_pop.flip(0, SIG_BUS_EN);
    inst_pop.flip(0, SIG_MEM_OE);
    inst_pop.flip(0, SIG_SP_OE);
    inst_pop.flip(0, SIG_SP_UP);

    inst_pop.flip(1, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_pop.flip(1, SIG_BUS_EN);
    inst_pop.flip(1, SIG_MEM_OE);
    inst_pop.flip(1, SIG_PC_OE);
    inst_pop.flip(1, SIG_PC_UP);
    inst_pop.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_POP] = inst_pop;

    // CALL (abs) (0X33)

    instruction inst_call(7);

    inst_call.flip(0, SIG_DL_LOAD); // DL <- M(PC++)
    inst_call.flip(0, SIG_BUS_EN);
    inst_call.flip(0, SIG_PC_OE);
    inst_call.flip(0, SIG_PC_UP);
    inst_call.flip(0, SIG_MEM_OE);
    inst_call.flip(0, SIG_S_DAT_2);

    inst_call.flip(1, SIG_DH_LOAD); // DH <- M(PC++), AUX <- AC
    inst_call.flip(1, SIG_BUS_EN);
    inst_call.flip(1, SIG_PC_OE);
    inst_call.flip(1, SIG_PC_UP);
    inst_call.flip(1, SIG_MEM_OE);
    inst_call.flip(1, SIG_S_DAT_2);

    inst_call.flip(2, SIG_AC_LOAD); // AC <- PCL
    inst_call.flip(2, SIG_PC_OE);
    inst_call.flip(2, SIG_S_DAT_1);
    inst_call.flip(2, SIG_S_AC_0);
    inst_call.flip(2, SIG_S_AC_2);

    inst_call.flip(3, SIG_MEM_WE); // M(SP--) <- AC
    inst_call.flip(3, SIG_BUS_EN);
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
    inst_call.flip(5, SIG_BUS_EN);
    inst_call.flip(5, SIG_SP_OE);
    inst_call.flip(5, SIG_SP_DOWN);
    inst_call.flip(5, SIG_S_DAT_0);
    inst_call.flip(5, SIG_PC_LOAD);

    inst_call.flip(6, SIG_AC_LOAD); // RI <- M(PC++), AC <- AUX, RCF_CLR
    inst_call.flip(6, SIG_S_AC_0);
    inst_call.flip(6, SIG_S_AC_2);
    inst_call.flip(6, SIG_RI_LOAD);
    inst_call.flip(6, SIG_BUS_EN);
    inst_call.flip(6, SIG_MEM_OE);
    inst_call.flip(6, SIG_PC_OE);
    inst_call.flip(6, SIG_PC_UP);
    inst_call.flip(6, SIG_RCF_CLR);

    microcode[OP_INST_CALL] = inst_call;

    // RET (0x34)

    instruction inst_ret(4);

    inst_ret.flip(0, SIG_DH_LOAD); // DH <- M(SP++)
    inst_ret.flip(0, SIG_BUS_EN);
    inst_ret.flip(0, SIG_MEM_OE);
    inst_ret.flip(0, SIG_SP_OE);
    inst_ret.flip(0, SIG_SP_UP);
    inst_ret.flip(0, SIG_S_DAT_2);

    inst_ret.flip(1, SIG_DL_LOAD); // DL <- M(SP++)
    inst_ret.flip(1, SIG_BUS_EN);
    inst_ret.flip(1, SIG_MEM_OE);
    inst_ret.flip(1, SIG_SP_OE);
    inst_ret.flip(1, SIG_SP_UP);
    inst_ret.flip(1, SIG_S_DAT_2);

    inst_ret.flip(2, SIG_PC_LOAD); // PC <- D

    inst_ret.flip(3, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_ret.flip(3, SIG_BUS_EN);
    inst_ret.flip(3, SIG_MEM_OE);
    inst_ret.flip(3, SIG_PC_OE);
    inst_ret.flip(3, SIG_PC_UP);
    inst_ret.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_RET] = inst_ret;

    // INT (0x35)

    instruction inst_int(13); 

    inst_int.flip(0, SIG_AUX_LOAD); // AUX <- AC, AC <- M(PC++)
    inst_int.flip(0, SIG_AC_LOAD);
    inst_int.flip(0, SIG_BUS_EN);
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
    inst_int.flip(3, SIG_BUS_EN);
    inst_int.flip(3, SIG_MEM_OE);
    inst_int.flip(3, SIG_D_OE);
    inst_int.flip(3, SIG_D_UP);
    inst_int.flip(3, SIG_S_DAT_2);
    
    inst_int.flip(4, SIG_DH_LOAD); // DH <- M(D++)
    inst_int.flip(4, SIG_BUS_EN);
    inst_int.flip(4, SIG_MEM_OE);
    inst_int.flip(4, SIG_D_OE);
    inst_int.flip(4, SIG_D_UP);
    inst_int.flip(4, SIG_S_DAT_2);
    
    inst_int.flip(5, SIG_DL_LOAD); // DL <- AC
    inst_int.flip(5, SIG_S_DAT_0);

    inst_int.flip(6, SIG_AC_LOAD); // AC <- PCL
    inst_int.flip(6, SIG_PC_OE);
    inst_int.flip(6, SIG_S_DAT_1);
    inst_int.flip(6, SIG_S_AC_0);
    inst_int.flip(6, SIG_S_AC_2);

    inst_int.flip(7, SIG_S_DAT_0); // M(SP--) <- AC
    inst_int.flip(7, SIG_BUS_EN);
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
    inst_int.flip(9, SIG_BUS_EN);
    inst_int.flip(9, SIG_SP_OE);
    inst_int.flip(9, SIG_SP_DOWN);
    inst_int.flip(9, SIG_S_DAT_0);

    inst_int.flip(10, SIG_MEM_WE); // M(SP--) <- EST
    inst_int.flip(10, SIG_BUS_EN);
    inst_int.flip(10, SIG_SP_OE);
    inst_int.flip(10, SIG_SP_DOWN);
    inst_int.flip(10, SIG_S_DAT_0);
    inst_int.flip(10, SIG_S_DAT_2);
    
    inst_int.flip(11, SIG_PC_LOAD); // PC <- D, AC <- AUX
    inst_int.flip(11, SIG_AC_LOAD);
    inst_int.flip(11, SIG_S_AC_0);
    inst_int.flip(11, SIG_S_AC_2);
    
    inst_int.flip(12, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_int.flip(12, SIG_BUS_EN);
    inst_int.flip(12, SIG_MEM_OE);
    inst_int.flip(12, SIG_PC_OE);
    inst_int.flip(12, SIG_PC_UP);
    inst_int.flip(12, SIG_RCF_CLR);

    microcode[OP_INST_INT] = inst_int;

    // IRET, RETI (0x36)

    instruction inst_iret(5);

    inst_iret.flip(0, SIG_LOAD_I); // EST <- M(SP++)
    inst_iret.flip(0, SIG_LOAD_C);
    inst_iret.flip(0, SIG_LOAD_ZOS);
    inst_iret.flip(0, SIG_BUS_EN);
    inst_iret.flip(0, SIG_SP_OE);
    inst_iret.flip(0, SIG_SP_UP);
    inst_iret.flip(0, SIG_MEM_OE);
    inst_iret.flip(0, SIG_S_DAT_2);

    inst_iret.flip(1, SIG_BUS_EN); // DH <- M(SP++)
    inst_iret.flip(1, SIG_MEM_OE);
    inst_iret.flip(1, SIG_DH_LOAD);
    inst_iret.flip(1, SIG_SP_OE);
    inst_iret.flip(1, SIG_SP_UP);
    inst_iret.flip(1, SIG_S_DAT_2);

    inst_iret.flip(2, SIG_BUS_EN); // DL <- M(SP++)
    inst_iret.flip(2, SIG_MEM_OE);
    inst_iret.flip(2, SIG_DL_LOAD);
    inst_iret.flip(2, SIG_SP_OE);
    inst_iret.flip(2, SIG_SP_UP);
    inst_iret.flip(2, SIG_S_DAT_2);

    inst_iret.flip(3, SIG_PC_LOAD); // PC <- D

    inst_iret.flip(4, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_iret.flip(4, SIG_BUS_EN);
    inst_iret.flip(4, SIG_MEM_OE);
    inst_iret.flip(4, SIG_PC_OE);
    inst_iret.flip(4, SIG_PC_UP);
    inst_iret.flip(4, SIG_RCF_CLR);

    microcode[OP_INST_IRET] = inst_iret;

    // STSP (imm) (0x37)

    instruction inst_stsp(4);

    inst_stsp.flip(0, SIG_DL_LOAD); // DL <- M(PC++)
    inst_stsp.flip(0, SIG_BUS_EN);
    inst_stsp.flip(0, SIG_MEM_OE);
    inst_stsp.flip(0, SIG_PC_OE);
    inst_stsp.flip(0, SIG_PC_UP);
    inst_stsp.flip(0, SIG_S_DAT_2);

    inst_stsp.flip(1, SIG_DH_LOAD); // DH <- M(PC++)
    inst_stsp.flip(1, SIG_BUS_EN);
    inst_stsp.flip(1, SIG_MEM_OE);
    inst_stsp.flip(1, SIG_PC_OE);
    inst_stsp.flip(1, SIG_PC_UP);
    inst_stsp.flip(1, SIG_S_DAT_2);

    inst_stsp.flip(2, SIG_SP_LOAD); // SP <- D

    inst_stsp.flip(3, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_stsp.flip(3, SIG_BUS_EN);
    inst_stsp.flip(3, SIG_MEM_OE);
    inst_stsp.flip(3, SIG_PC_OE);
    inst_stsp.flip(3, SIG_PC_UP);
    inst_stsp.flip(3, SIG_RCF_CLR);

    // OUT (ind) (0x38) (puntero de E/S almacenado en memoria)

    instruction inst_out_ind(7);

    inst_out_ind.flip(0, SIG_DL_LOAD); // DL <- M(PC++)
    inst_out_ind.flip(0, SIG_BUS_EN);
    inst_out_ind.flip(0, SIG_MEM_OE);
    inst_out_ind.flip(0, SIG_PC_OE);
    inst_out_ind.flip(0, SIG_PC_UP);
    inst_out_ind.flip(0, SIG_S_DAT_2);

    inst_out_ind.flip(1, SIG_DH_LOAD); // DH <- M(PC++)
    inst_out_ind.flip(1, SIG_BUS_EN);
    inst_out_ind.flip(1, SIG_MEM_OE);
    inst_out_ind.flip(1, SIG_PC_OE);
    inst_out_ind.flip(1, SIG_PC_UP);
    inst_out_ind.flip(1, SIG_S_DAT_2);

    inst_out_ind.flip(2, SIG_AC_LOAD); // AC <- M(D++)
    inst_out_ind.flip(2, SIG_BUS_EN);
    inst_out_ind.flip(2, SIG_MEM_OE);
    inst_out_ind.flip(2, SIG_D_OE);
    inst_out_ind.flip(2, SIG_D_UP);
    inst_out_ind.flip(2, SIG_S_DAT_2);
    inst_out_ind.flip(2, SIG_S_AC_0);
    inst_out_ind.flip(2, SIG_S_AC_2);

    inst_out_ind.flip(3, SIG_DH_LOAD); // DH <- M(D)
    inst_out_ind.flip(3, SIG_BUS_EN);
    inst_out_ind.flip(3, SIG_MEM_OE);
    inst_out_ind.flip(3, SIG_D_OE);
    inst_out_ind.flip(3, SIG_S_DAT_2);

    inst_out_ind.flip(4, SIG_DL_LOAD); // DL <- AC
    inst_out_ind.flip(4, SIG_S_DAT_0);

    inst_out_ind.flip(5, SIG_BUS_EN); // IO(D) <- AC
    inst_out_ind.flip(5, SIG_MEM_IO);
    inst_out_ind.flip(5, SIG_MEM_WE);
    inst_out_ind.flip(5, SIG_D_OE);
    inst_out_ind.flip(5, SIG_S_DAT_0);

    inst_out_ind.flip(6, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_out_ind.flip(6, SIG_BUS_EN);
    inst_out_ind.flip(6, SIG_PC_OE);
    inst_out_ind.flip(6, SIG_PC_UP);
    inst_out_ind.flip(6, SIG_MEM_OE);
    inst_out_ind.flip(6, SIG_RCF_CLR);

    microcode[OP_INST_OUT_IND] = inst_out_ind;

    // LDSPL (0x39)

    instruction inst_ldspl(2);

    inst_ldspl.flip(0, SIG_AC_LOAD); // AC <- SPL
    inst_ldspl.flip(0, SIG_SP_OE);
    inst_ldspl.flip(0, SIG_S_DAT_1);
    inst_ldspl.flip(0, SIG_S_AC_0);
    inst_ldspl.flip(0, SIG_S_AC_2);

    inst_ldspl.flip(1, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_ldspl.flip(1, SIG_BUS_EN);
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
    inst_rcl.flip(0, SIG_BUS_EN);
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
    inst_rcr.flip(0, SIG_BUS_EN);
    inst_rcr.flip(0, SIG_PC_OE);
    inst_rcr.flip(0, SIG_PC_UP);
    inst_rcr.flip(0, SIG_MEM_OE);
    inst_rcr.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_RCR] = inst_rcr;

    // CMP (ind) (0X3C)

    instruction inst_cmp_ind(7);

    inst_cmp_ind.flip(0, SIG_DL_LOAD); // DL <- M(PC++)
    inst_cmp_ind.flip(0, SIG_MEM_OE);
    inst_cmp_ind.flip(0, SIG_BUS_EN);
    inst_cmp_ind.flip(0, SIG_PC_OE);
    inst_cmp_ind.flip(0, SIG_PC_UP);
    inst_cmp_ind.flip(0, SIG_S_DAT_2);

    inst_cmp_ind.flip(1, SIG_AUX_LOAD); // AUX <- AC, DH <- M(PC++)
    inst_cmp_ind.flip(1, SIG_DH_LOAD);
    inst_cmp_ind.flip(1, SIG_MEM_OE);
    inst_cmp_ind.flip(1, SIG_BUS_EN);
    inst_cmp_ind.flip(1, SIG_PC_OE);
    inst_cmp_ind.flip(1, SIG_PC_UP);
    inst_cmp_ind.flip(1, SIG_S_DAT_2);

    inst_cmp_ind.flip(2, SIG_MEM_OE); // AC <- M(D++)
    inst_cmp_ind.flip(2, SIG_BUS_EN);
    inst_cmp_ind.flip(2, SIG_S_DAT_2);
    inst_cmp_ind.flip(2, SIG_D_OE);
    inst_cmp_ind.flip(2, SIG_D_UP);
    inst_cmp_ind.flip(2, SIG_S_AC_0);
    inst_cmp_ind.flip(2, SIG_AC_LOAD);

    inst_cmp_ind.flip(3, SIG_MEM_OE); // DH <- M(D)
    inst_cmp_ind.flip(3, SIG_BUS_EN);
    inst_cmp_ind.flip(3, SIG_S_DAT_2);
    inst_cmp_ind.flip(3, SIG_D_OE);
    inst_cmp_ind.flip(3, SIG_DH_LOAD);

    inst_cmp_ind.flip(4, SIG_S_DAT_0); // DL <- AC
    inst_cmp_ind.flip(4, SIG_DL_LOAD);

    inst_cmp_ind.flip(5, SIG_MEM_OE); // (nada) <- AC + NOT(M(D)) + 1, C <- ALU_C, ZOS <- ALU_ZOS
    inst_cmp_ind.flip(5, SIG_BUS_EN);
    inst_cmp_ind.flip(5, SIG_D_OE);
    inst_cmp_ind.flip(5, SIG_MUX_ADD);
    inst_cmp_ind.flip(5, SIG_MUX_CI_0);
    inst_cmp_ind.flip(5, SIG_LOAD_C);
    inst_cmp_ind.flip(5, SIG_MUX_C_1);
    inst_cmp_ind.flip(5, SIG_LOAD_ZOS);
    inst_cmp_ind.flip(5, SIG_MUX_ZOS);
    inst_cmp_ind.flip(5, SIG_S_DAT_2);

    inst_cmp_ind.flip(6, SIG_MEM_OE); // AC <- AUX, RI <- M(PC++), RCF_CLR
    inst_cmp_ind.flip(6, SIG_S_AC_0);
    inst_cmp_ind.flip(6, SIG_S_AC_2);
    inst_cmp_ind.flip(6, SIG_BUS_EN);
    inst_cmp_ind.flip(6, SIG_RI_LOAD);
    inst_cmp_ind.flip(6, SIG_PC_OE);
    inst_cmp_ind.flip(6, SIG_PC_UP);
    inst_cmp_ind.flip(6, SIG_RCF_CLR);

    microcode[OP_INST_CMP_IND] = inst_cmp_abs;

    // IN (abs) (0x3D)

    instruction inst_in_abs(4);

    inst_in_abs.flip(0, SIG_DL_LOAD); // DL <- M(PC++)
    inst_in_abs.flip(0, SIG_MEM_OE);
    inst_in_abs.flip(0, SIG_BUS_EN);
    inst_in_abs.flip(0, SIG_PC_OE);
    inst_in_abs.flip(0, SIG_PC_UP);

    inst_in_abs.flip(1, SIG_DH_LOAD); // DH <- M(PC++)
    inst_in_abs.flip(1, SIG_MEM_OE);
    inst_in_abs.flip(1, SIG_BUS_EN);
    inst_in_abs.flip(1, SIG_PC_OE);
    inst_in_abs.flip(1, SIG_PC_UP);

    inst_in_abs.flip(2, SIG_AC_LOAD); // AC <- IO(D)
    inst_in_abs.flip(2, SIG_BUS_EN);
    inst_in_abs.flip(2, SIG_MEM_IO);
    inst_in_abs.flip(2, SIG_MEM_OE);
    inst_in_abs.flip(2, SIG_D_OE);
    inst_in_abs.flip(2, SIG_S_DAT_2);

    inst_in_abs.flip(3, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_in_abs.flip(3, SIG_BUS_EN);
    inst_in_abs.flip(3, SIG_PC_OE);
    inst_in_abs.flip(3, SIG_PC_UP);
    inst_in_abs.flip(3, SIG_MEM_OE);
    inst_in_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_IN_ABS] = inst_in_abs;

    // OUT (abs) (0x3E)

    instruction inst_out_abs(7);

    inst_out_abs.flip(0, SIG_DL_LOAD); // DL <- M(PC++)
    inst_out_abs.flip(0, SIG_BUS_EN);
    inst_out_abs.flip(0, SIG_MEM_OE);
    inst_out_abs.flip(0, SIG_PC_OE);
    inst_out_abs.flip(0, SIG_PC_UP);
    inst_out_abs.flip(0, SIG_S_DAT_2);

    inst_out_abs.flip(1, SIG_DH_LOAD); // DH <- M(PC++)
    inst_out_abs.flip(1, SIG_BUS_EN);
    inst_out_abs.flip(1, SIG_MEM_OE);
    inst_out_abs.flip(1, SIG_PC_OE);
    inst_out_abs.flip(1, SIG_PC_UP);
    inst_out_abs.flip(1, SIG_S_DAT_2);

    inst_out_abs.flip(2, SIG_BUS_EN); // IO(D) <- AC
    inst_out_abs.flip(2, SIG_MEM_IO);
    inst_out_abs.flip(2, SIG_MEM_WE);
    inst_out_abs.flip(2, SIG_D_OE);
    inst_out_abs.flip(2, SIG_S_DAT_0);

    inst_out_abs.flip(3, SIG_RI_LOAD); // RI <- M(PC++), RCF_CLR
    inst_out_abs.flip(3, SIG_BUS_EN);
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
    inst_ldsph.flip(1, SIG_BUS_EN);
    inst_ldsph.flip(1, SIG_MEM_OE);
    inst_ldsph.flip(1, SIG_PC_OE);
    inst_ldsph.flip(1, SIG_PC_UP);
    inst_ldsph.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_LDSPH] = inst_ldsph;

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

    std::cout << "Instrucciones microprogramadas. Creando el resto...\n";
    c_word salida[262144];

    // Primero copio todas las instrucciones:
    
    // Hay 64 códigos de operación
    for(int ins = 0; ins < 64; ins++){
        // Que tienen 16 pasos como máximo (los pasos inutilizados sólo activan SIG_RCF_CLR)
        for(int step = 0; step < 16; step++){
            // Que a su vez tienen 256 combinaciones de IFETCH, IRQ, BRQ, I, Z, O, S, C
            for(int comb = 0; comb < 256; comb++){
                int posicion = ins + step*64 + comb*1024;
                
                // Comprobar si la microinstrucción existe
                if(step < microcode[ins].get_size()){
                    salida[posicion] = microcode[ins].get(step);
                }else{
                    // Si no existe, tomamos la microinstrucción de HLT (RCF_CLR)
                    salida[posicion] = inst_hlt.get(0);
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

        // DEBUG
        // std::cout << input << " # ifetch: " << ifetch << " irq: " << irq << " brq: " << brq << " i: " << i << " z: " << z << " o: " << o << " s: " << s << " c: " << c << " # rcf: " << rcf << " ri: " << ri << std::endl; 

        // Si se está capturando una interrupción (IFETCH activa)
        if(ifetch){
            if(rcf == 0){
                salida[palabra] = c_word(ALL_INACTIVE); // AUX <- AC, AC <- I/O(INT), IACK
                salida[palabra].flip(SIG_AC_LOAD);
                salida[palabra].flip(SIG_AUX_LOAD);
                salida[palabra].flip(SIG_S_DAT_2);
                salida[palabra].flip(SIG_IACK);
            }else{
                salida[palabra] = inst_int.get(rcf);
            }
        }

        // Último paso de la instrucción
        int last_step = microcode[ri].get_size() - 1;

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
            }
        }

        if(
            (ri == OP_INST_JZ && z)                         ||
            (ri == OP_INST_JNZ && !z)                       ||
            (ri == OP_INST_JC && c)                         ||
            (ri == OP_INST_JNC && !c)                       ||
            (ri == OP_INST_JS && s)                         ||
            (ri == OP_INST_JNS && !s)                       ||
            (ri == OP_INST_JO && o)                         ||
            (ri == OP_INST_JNO && !o)                       ||
            (ri == OP_INST_JB_SIGNED && s != o)             ||
            (ri == OP_INST_JNB_SIGNED && s == o)            ||
            (ri == OP_INST_JBE_SIGNED && (z || (s != o)))   ||
            (ri == OP_INST_JNBE_SIGNED && !(z || (s != o))) ||
            (ri == OP_INST_JBE_UNSIGNED && (c || z))        ||
            (ri == OP_INST_JNBE_UNSIGNED && !(c || z))
        ){
            /*
            Si se cumple la condición significa que se trata de una instrucción de salto condicional
            con los flags correspondientes así que no se hace nada (la instrucción de salto se ejecuta)

            Si no se cumple, se reemplaza por la instrucción que no hace nada más que avanzar a la siguiente
            (como si fuera un NOP de 3 bytes)
            */
        }else{
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

    return 0;
}