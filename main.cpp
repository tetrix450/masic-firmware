#include <iostream>
#include <bitset>
#include <vector>
#include <string>

// Señales de control
// (Señalan el nº de bit dentro de la palabra de control, de 0 a 39)

#define SIG_IFETCH          39
#define SIG_RCF_CLR         38
#define SIG_PC_OE           37
#define SIG_MEM_IO          36
#define SIG_PC_LOAD         35
#define SIG_SP_OE           34
#define SIG_SPL_LOAD        33
#define SIG_SPH_LOAD        32
#define SIG_D_OE            31
#define SIG_DL_LOAD         30
#define SIG_DH_LOAD         29
#define SIG_MEM_WE          28
#define SIG_MEM_OE          27
#define SIG_D_CLR           26
#define SIG_FILL_BIT        25
#define SIG_MUX_ZOS         24
#define SIG_MUX_C_1         23
#define SIG_MUX_C_0         22
#define SIG_S_DAT_2         21
#define SIG_S_DAT_1         20
#define SIG_S_DAT_0         19
#define SIG_S_AC_2          18
#define SIG_S_AC_1          17
#define SIG_S_AC_0          16
#define SIG_BUS_EN          15
#define SIG_MUX_CI_2        14
#define SIG_MUX_CI_1        13
#define SIG_MUX_CI_0        12
#define SIG_LOAD_I          11
#define SIG_LOAD_ZOS        10
#define SIG_LOAD_C          9
#define SIG_IACK            8
#define SIG_BACK            7
#define SIG_PC_UP           6
#define SIG_SP_UP           5
#define SIG_D_UP            4
#define SIG_RI_LOAD         3
#define SIG_AUX_LOAD        2
#define SIG_AC_LOAD         1
#define SIG_SP_DOWN         0

// Máscara de señales activas a nivel bajo

#define ALL_INACTIVE        0b0010111111111111000000001000111001111111

// Códigos de operación de cada instrucción

#define OP_INST_JMP_ABS        0x00
#define OP_INST_JMP_IND        0x2A
#define OP_INST_CLC            0x01
#define OP_INST_STC            0x02
#define OP_INST_CLI            0x03
#define OP_INST_STI            0x04
#define OP_INST_HLT            0x05
#define OP_INST_INC            0x06
#define OP_INST_DEC            0x07
#define OP_INST_LOAD_ABS       0x08
#define OP_INST_LOAD_IMM       0x09
#define OP_INST_LOAD_IND       0x0A
#define OP_INST_STORE_ABS      0x0B
#define OP_INST_STORE_IND      0x0C
#define OP_INST_ADD_ABS        0x0D
#define OP_INST_ADD_IMM        0x0E
#define OP_INST_ADC_ABS        0x0F
#define OP_INST_ADC_IMM        0x10
#define OP_INST_SUB_ABS        0x11
#define OP_INST_SUB_IMM        0x12
#define OP_INST_AND_ABS        0x13
#define OP_INST_AND_IMM        0x14
#define OP_INST_OR_ABS         0x15
#define OP_INST_OR_IMM         0x16
#define OP_INST_NOT            0x17
#define OP_INST_NEG            0x18
#define OP_INST_CMP_ABS        0x19
#define OP_INST_CMP_IMM        0x1A
#define OP_INST_NOP            0x1B
#define OP_INST_JO             0x1C
#define OP_INST_JNO            0x1D

#define OP_INST_JZ             0x1E
#define OP_INST_JE             0x1E

#define OP_INST_JNZ            0x1F
#define OP_INST_JNE            0x1F

#define OP_INST_JNAE_SIGNED    0x20
#define OP_INST_JB_SIGNED      0x20

#define OP_INST_JAE_SIGNED     0x21
#define OP_INST_JNB_SIGNED     0x21

#define OP_INST_JBE_SIGNED     0x22
#define OP_INST_JNA_SIGNED     0x22

#define OP_INST_JA_SIGNED      0x23
#define OP_INST_JNBE_SIGNED    0x23

#define OP_INST_JC             0x24
#define OP_INST_JNAE_UNSIGNED  0x24
#define OP_INST_JB_UNSIGNED    0x24

#define OP_INST_JNC            0x25
#define OP_INST_JAE_UNSIGNED   0x25
#define OP_INST_JNB_UNSIGNED   0x25

#define OP_INST_JBE_UNSIGNED   0x26
#define OP_INST_JNA_UNSIGNED   0x26

#define OP_INST_JA_UNSIGNED    0x27
#define OP_INST_JNBE_UNSIGNED  0x27

#define OP_INST_JS             0x28
#define OP_INST_JNS            0x29
#define OP_INST_SHL            0x2B
#define OP_INST_SHR_SIGNED     0x2C
#define OP_INST_SHR_UNSIGNED   0x2D
#define OP_INST_ROL            0x2E
#define OP_INST_ROR            0x2F
#define OP_INST_RCL            0x3A
#define OP_INST_RCR            0x3B
#define OP_INST_PUSH_IMM       0x30
#define OP_INST_PUSH_AC        0x31
#define OP_INST_POP            0x32
#define OP_INST_CALL           0x33
#define OP_INST_RET            0x34
#define OP_INST_INT            0x35

#define OP_INST_IRET           0x36
#define OP_INST_RETI           0x36

#define OP_INST_LDSP           0x37
#define OP_INST_LOAZ_ABS       0x38
#define OP_INST_LOAZ_IMM       0x39
#define OP_INST_LOAZ_IND       0x3C
#define OP_INST_IN             0x3D
#define OP_INST_OUT            0x3E

/*

El objetivo de este programa es el de generar un fichero con todas las señales de control
definidas para cada instrucción y cada entrada a la unidad de control, para usarlo a la
hora de programar los integrados (5 x FLASH 512Kx8)

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

        void flip(int step, int ctrl_signal){
            steps[step].flip(ctrl_signal);
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

    inst_jmp_abs.flip(0, SIG_MEM_OE);   // M(PC++) -> DL
    inst_jmp_abs.flip(0, SIG_BUS_EN);
    inst_jmp_abs.flip(0, SIG_PC_OE);
    inst_jmp_abs.flip(0, SIG_PC_UP);
    inst_jmp_abs.flip(0, SIG_DL_LOAD);
    inst_jmp_abs.flip(0, SIG_S_DAT_2);

    inst_jmp_abs.flip(1, SIG_MEM_OE);   // M(PC++) -> DH
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

    inst_clc.flip(0, SIG_S_DAT_1);
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

    inst_stc.flip(0, SIG_FILL_BIT);
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

    inst_cli.flip(0, SIG_S_DAT_1);
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

    inst_sti.flip(0, SIG_FILL_BIT);
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

    inst_hlt.flip(0, SIG_RCF_CLR);

    microcode[OP_INST_HLT] = inst_hlt;

    // INC (0x06)
    instruction inst_inc(1);

    inst_inc.flip(0, SIG_S_DAT_1);
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

    inst_dec.flip(0, SIG_S_DAT_1);
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

    inst_load_abs.flip(0, SIG_MEM_OE);
    inst_load_abs.flip(0, SIG_PC_OE);
    inst_load_abs.flip(0, SIG_PC_UP);
    inst_load_abs.flip(0, SIG_DL_LOAD);
    inst_load_abs.flip(0, SIG_S_DAT_2);
    inst_load_abs.flip(0, SIG_BUS_EN);
    
    inst_load_abs.flip(1, SIG_MEM_OE);
    inst_load_abs.flip(1, SIG_PC_OE);
    inst_load_abs.flip(1, SIG_PC_UP);
    inst_load_abs.flip(1, SIG_DH_LOAD);
    inst_load_abs.flip(1, SIG_S_DAT_2);
    inst_load_abs.flip(1, SIG_BUS_EN);

    inst_load_abs.flip(2, SIG_MEM_OE);
    inst_load_abs.flip(2, SIG_BUS_EN);
    inst_load_abs.flip(2, SIG_D_OE);
    inst_load_abs.flip(2, SIG_S_AC_2);
    inst_load_abs.flip(2, SIG_S_AC_0);
    inst_load_abs.flip(2, SIG_AC_LOAD);
    inst_load_abs.flip(2, SIG_S_DAT_2);

    inst_load_abs.flip(3, SIG_MEM_OE);
    inst_load_abs.flip(3, SIG_BUS_EN);
    inst_load_abs.flip(3, SIG_PC_OE);
    inst_load_abs.flip(3, SIG_PC_UP);
    inst_load_abs.flip(3, SIG_RI_LOAD);
    inst_load_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_LOAD_ABS] = inst_load_abs;

    // LOAD (imm) (0x09)

    instruction inst_load_imm(2);

    inst_load_imm.flip(0, SIG_MEM_OE);
    inst_load_imm.flip(0, SIG_PC_OE);
    inst_load_imm.flip(0, SIG_PC_UP);
    inst_load_imm.flip(0, SIG_S_AC_0);
    inst_load_imm.flip(0, SIG_S_AC_2);
    inst_load_imm.flip(0, SIG_AC_LOAD);
    inst_load_imm.flip(0, SIG_S_DAT_2);
    inst_load_imm.flip(0, SIG_BUS_EN);

    inst_load_imm.flip(1, SIG_MEM_OE);
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
    inst_load_ind.flip(2, SIG_S_DAT_2); // BUS <- MEM
    inst_load_ind.flip(2, SIG_D_OE);
    inst_load_ind.flip(2, SIG_D_UP);
    inst_load_ind.flip(2, SIG_S_AC_2); // AC <- BUS
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

    inst_store_abs.flip(0, SIG_MEM_OE);
    inst_store_abs.flip(0, SIG_BUS_EN);
    inst_store_abs.flip(0, SIG_PC_OE);
    inst_store_abs.flip(0, SIG_PC_UP);
    inst_store_abs.flip(0, SIG_DL_LOAD);

    inst_store_abs.flip(1, SIG_MEM_OE);
    inst_store_abs.flip(1, SIG_BUS_EN);
    inst_store_abs.flip(1, SIG_PC_OE);
    inst_store_abs.flip(1, SIG_PC_UP);
    inst_store_abs.flip(1, SIG_DH_LOAD);

    inst_store_abs.flip(2, SIG_MEM_WE);
    inst_store_abs.flip(2, SIG_S_DAT_0);
    inst_store_abs.flip(2, SIG_D_OE);

    inst_store_abs.flip(3, SIG_MEM_OE);
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

    inst_add_abs.flip(0, SIG_MEM_OE);
    inst_add_abs.flip(0, SIG_BUS_EN);
    inst_add_abs.flip(0, SIG_S_DAT_2);
    inst_add_abs.flip(0, SIG_PC_OE);
    inst_add_abs.flip(0, SIG_PC_UP);
    inst_add_abs.flip(0, SIG_DL_LOAD);

    inst_add_abs.flip(1, SIG_MEM_OE);
    inst_add_abs.flip(1, SIG_BUS_EN);
    inst_add_abs.flip(1, SIG_S_DAT_2);
    inst_add_abs.flip(1, SIG_PC_OE);
    inst_add_abs.flip(1, SIG_PC_UP);
    inst_add_abs.flip(1, SIG_DH_LOAD);

    inst_add_abs.flip(2, SIG_MEM_OE);
    inst_add_abs.flip(2, SIG_BUS_EN);
    inst_add_abs.flip(2, SIG_S_DAT_2);
    inst_add_abs.flip(2, SIG_D_OE);
    inst_add_abs.flip(2, SIG_MUX_ZOS);
    inst_add_abs.flip(2, SIG_LOAD_ZOS);
    inst_add_abs.flip(2, SIG_AC_LOAD);
    inst_add_abs.flip(2, SIG_LOAD_C);
    inst_add_abs.flip(2, SIG_MUX_C_0);
    inst_add_abs.flip(2, SIG_MUX_C_1);

    inst_add_abs.flip(3, SIG_MEM_OE);
    inst_add_abs.flip(3, SIG_BUS_EN);
    inst_add_abs.flip(3, SIG_RI_LOAD);
    inst_add_abs.flip(3, SIG_PC_OE);
    inst_add_abs.flip(3, SIG_PC_UP);
    inst_add_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_ADD_ABS] = inst_add_abs;

    // ADD (imm) (0x0E)

    instruction inst_add_imm(2);

    inst_add_imm.flip(0, SIG_MEM_OE);
    inst_add_imm.flip(0, SIG_BUS_EN);
    inst_add_imm.flip(0, SIG_S_DAT_2);
    inst_add_imm.flip(0, SIG_PC_OE);
    inst_add_imm.flip(0, SIG_PC_UP);
    inst_add_imm.flip(0, SIG_MUX_ZOS);
    inst_add_imm.flip(0, SIG_LOAD_ZOS);
    inst_add_imm.flip(0, SIG_AC_LOAD);
    inst_add_imm.flip(0, SIG_LOAD_C);
    inst_add_imm.flip(0, SIG_MUX_C_0);
    inst_add_imm.flip(0, SIG_MUX_C_1);

    inst_add_imm.flip(1, SIG_MEM_OE);
    inst_add_imm.flip(1, SIG_BUS_EN);
    inst_add_imm.flip(1, SIG_RI_LOAD);
    inst_add_imm.flip(1, SIG_PC_OE);
    inst_add_imm.flip(1, SIG_PC_UP);
    inst_add_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_ADD_IMM] = inst_add_imm;

    // ADC (abs) (0x0F)

    instruction inst_adc_abs(4);

    inst_adc_abs.flip(0, SIG_MEM_OE);
    inst_adc_abs.flip(0, SIG_BUS_EN);
    inst_adc_abs.flip(0, SIG_S_DAT_2);
    inst_adc_abs.flip(0, SIG_PC_OE);
    inst_adc_abs.flip(0, SIG_PC_UP);
    inst_adc_abs.flip(0, SIG_DL_LOAD);

    inst_adc_abs.flip(1, SIG_MEM_OE);
    inst_adc_abs.flip(1, SIG_BUS_EN);
    inst_adc_abs.flip(1, SIG_S_DAT_2);
    inst_adc_abs.flip(1, SIG_PC_OE);
    inst_adc_abs.flip(1, SIG_PC_UP);
    inst_adc_abs.flip(1, SIG_DH_LOAD);

    inst_adc_abs.flip(2, SIG_MEM_OE);
    inst_adc_abs.flip(2, SIG_BUS_EN);
    inst_adc_abs.flip(2, SIG_S_DAT_2);
    inst_adc_abs.flip(2, SIG_D_OE);
    inst_adc_abs.flip(2, SIG_MUX_ZOS);
    inst_adc_abs.flip(2, SIG_LOAD_ZOS);
    inst_adc_abs.flip(2, SIG_AC_LOAD);
    inst_adc_abs.flip(2, SIG_LOAD_C);
    inst_adc_abs.flip(2, SIG_MUX_CI_2);
    inst_adc_abs.flip(2, SIG_MUX_C_0);
    inst_adc_abs.flip(2, SIG_MUX_C_1);

    inst_adc_abs.flip(3, SIG_MEM_OE);
    inst_adc_abs.flip(3, SIG_BUS_EN);
    inst_adc_abs.flip(3, SIG_RI_LOAD);
    inst_adc_abs.flip(3, SIG_PC_OE);
    inst_adc_abs.flip(3, SIG_PC_UP);
    inst_adc_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_ADC_ABS] = inst_adc_abs;

    // ADC (imm) (0x10)

    instruction inst_adc_imm(2);

    inst_adc_imm.flip(0, SIG_MEM_OE);
    inst_adc_imm.flip(0, SIG_BUS_EN);
    inst_adc_imm.flip(0, SIG_S_DAT_2);
    inst_adc_imm.flip(0, SIG_PC_OE);
    inst_adc_imm.flip(0, SIG_PC_UP);
    inst_adc_imm.flip(0, SIG_MUX_ZOS);
    inst_adc_imm.flip(0, SIG_LOAD_ZOS);
    inst_adc_imm.flip(0, SIG_AC_LOAD);
    inst_adc_imm.flip(0, SIG_LOAD_C);
    inst_adc_imm.flip(0, SIG_MUX_CI_2);
    inst_adc_imm.flip(0, SIG_MUX_C_0);
    inst_adc_imm.flip(0, SIG_MUX_C_1);

    inst_adc_imm.flip(1, SIG_MEM_OE);
    inst_adc_imm.flip(1, SIG_BUS_EN);
    inst_adc_imm.flip(1, SIG_RI_LOAD);
    inst_adc_imm.flip(1, SIG_PC_OE);
    inst_adc_imm.flip(1, SIG_PC_UP);
    inst_adc_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_ADC_IMM] = inst_add_imm;

    // SUB (abs) (0x11)
    // Las restas afectan al flag de overflow al revés que las sumas

    instruction inst_sub_abs(4);

    inst_sub_abs.flip(0, SIG_MEM_OE); // DL <- M(PC++)
    inst_sub_abs.flip(0, SIG_BUS_EN);
    inst_sub_abs.flip(0, SIG_S_DAT_2);
    inst_sub_abs.flip(0, SIG_PC_OE);
    inst_sub_abs.flip(0, SIG_PC_UP);
    inst_sub_abs.flip(0, SIG_DL_LOAD);

    inst_sub_abs.flip(1, SIG_MEM_OE); // DH <- M(PC++), AUX <- AC
    inst_sub_abs.flip(1, SIG_BUS_EN);
    inst_sub_abs.flip(1, SIG_S_DAT_2);
    inst_sub_abs.flip(1, SIG_PC_OE);
    inst_sub_abs.flip(1, SIG_PC_UP);
    inst_sub_abs.flip(1, SIG_DH_LOAD);
    inst_sub_abs.flip(1, SIG_AUX_LOAD);

    inst_sub_abs.flip(2, SIG_MEM_OE); // AC <- NOT(M(D))
    inst_sub_abs.flip(2, SIG_BUS_EN);
    inst_sub_abs.flip(2, SIG_S_DAT_2);
    inst_sub_abs.flip(2, SIG_D_OE);
    inst_sub_abs.flip(2, SIG_S_AC_2);
    inst_sub_abs.flip(2, SIG_AC_LOAD);

    inst_sub_abs.flip(3, SIG_MEM_OE); // AC <- AC + AUX + 1, RI <- M(PC++)
    inst_sub_abs.flip(3, SIG_AC_LOAD);
    inst_sub_abs.flip(3, SIG_S_AC_2);
    inst_sub_abs.flip(3, SIG_MUX_CI_0);
    inst_sub_abs.flip(3, SIG_MUX_ZOS);
    inst_sub_abs.flip(3, SIG_LOAD_ZOS);
    inst_sub_abs.flip(3, SIG_MUX_C_1);
    inst_sub_abs.flip(3, SIG_LOAD_C);
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
    inst_sub_imm.flip(0, S_AC_2);
    inst_sub_imm.flip(0, SIG_AC_LOAD);
    inst_sub_imm.flip(0, SIG_AUX_LOAD);
    
    inst_sub_imm.flip(1, SIG_MEM_OE); // AC <- AC + AUX + 1, RI <- M(PC++), RCF_CLR
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

    inst_and_abs.flip(0, SIG_MEM_OE);
    inst_and_abs.flip(0, SIG_BUS_EN);
    inst_and_abs.flip(0, SIG_PC_OE);
    inst_and_abs.flip(0, SIG_PC_UP);
    inst_and_abs.flip(0, SIG_DL_LOAD);

    inst_and_abs.flip(1, SIG_MEM_OE);
    inst_and_abs.flip(1, SIG_BUS_EN);
    inst_and_abs.flip(1, SIG_PC_OE);
    inst_and_abs.flip(1, SIG_PC_UP);
    inst_and_abs.flip(1, SIG_DH_LOAD);

    inst_and_abs.flip(2, SIG_MEM_OE);
    inst_and_abs.flip(2, SIG_BUS_EN);
    inst_and_abs.flip(2, SIG_D_OE);
    inst_and_abs.flip(2, SIG_S_AC_1);
    inst_and_abs.flip(2, SIG_AC_LOAD);
    inst_and_abs.flip(2, SIG_MUX_ZOS);
    inst_and_abs.flip(2, SIG_LOAD_ZOS);

    inst_and_abs.flip(3, SIG_MEM_OE);
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

    inst_and_imm.flip(1, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_and_imm.flip(1, SIG_BUS_EN);
    inst_and_imm.flip(1, SIG_RI_LOAD);
    inst_and_imm.flip(1, SIG_PC_OE);
    inst_and_imm.flip(1, SIG_PC_UP);
    inst_and_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_AND_IMM] = inst_and_imm;

    // OR (abs) (0x15)

    instruction inst_or_abs(4);

    inst_or_abs.flip(0, SIG_MEM_OE);
    inst_or_abs.flip(0, SIG_BUS_EN);
    inst_or_abs.flip(0, SIG_PC_OE);
    inst_or_abs.flip(0, SIG_PC_UP);
    inst_or_abs.flip(0, SIG_DL_LOAD);

    inst_or_abs.flip(1, SIG_MEM_OE);
    inst_or_abs.flip(1, SIG_BUS_EN);
    inst_or_abs.flip(1, SIG_PC_OE);
    inst_or_abs.flip(1, SIG_PC_UP);
    inst_or_abs.flip(1, SIG_DH_LOAD);

    inst_or_abs.flip(2, SIG_MEM_OE);
    inst_or_abs.flip(2, SIG_BUS_EN);
    inst_or_abs.flip(2, SIG_D_OE);
    inst_or_abs.flip(2, SIG_S_AC_1);
    inst_or_abs.flip(2, SIG_S_AC_0);
    inst_or_abs.flip(2, SIG_AC_LOAD);
    inst_or_abs.flip(2, SIG_MUX_ZOS);
    inst_or_abs.flip(2, SIG_LOAD_ZOS);
    inst_or_abs.flip(2, SIG_LOAD_C);
    inst_or_abs.flip(2, SIG_MUX_C_1);

    inst_or_abs.flip(3, SIG_MEM_OE);
    inst_or_abs.flip(3, SIG_BUS_EN);
    inst_or_abs.flip(3, SIG_RI_LOAD);
    inst_or_abs.flip(3, SIG_PC_OE);
    inst_or_abs.flip(3, SIG_PC_UP);
    inst_or_abs.flip(3, SIG_RCF_CLR);

    microcode[OP_INST_OR_ABS] = inst_or_abs;

    // OR (0x16) (imm)

    instruction inst_or_imm(2);

    inst_or_imm.flip(0, SIG_AC_LOAD); // AC <- AC | M(PC++)
    inst_or_imm.flip(0, SIG_S_AC_2);
    inst_or_imm.flip(0, SIG_PC_OE);
    inst_or_imm.flip(0, SIG_PC_UP);
    inst_or_imm.flip(0, SIG_MEM_OE);
    inst_or_imm.flip(0, SIG_BUS_EN);
    inst_or_imm.flip(0, SIG_S_DAT_2);

    inst_or_imm.flip(1, SIG_MEM_OE); // RI <- M(PC++), RCF_CLR
    inst_or_imm.flip(1, SIG_BUS_EN);
    inst_or_imm.flip(1, SIG_RI_LOAD);
    inst_or_imm.flip(1, SIG_PC_OE);
    inst_or_imm.flip(1, SIG_PC_UP);
    inst_or_imm.flip(1, SIG_RCF_CLR);

    microcode[OP_INST_OR_IMM] = inst_or_imm;

    // NOT (0x17)

    instruction inst_not(1);

    inst_not.flip(0, SIG_AC_LOAD); // AC <- NOT(AC), RI <- M(PC++), RCF_CLR
    inst_not.flip(0, SIG_S_AC_2);
    inst_not.flip(0, SIG_S_DAT_0);
    inst_not.flip(0, SIG_RI_LOAD);
    inst_not.flip(0, SIG_BUS_EN);
    inst_not.flip(0, SIG_MEM_OE);
    inst_not.flip(0, SIG_PC_OE);
    inst_not.flip(0, SIG_PC_UP);
    inst_not.flip(0, SIG_RCF_CLR);

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

    return 0;
}