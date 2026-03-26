# Instruction_Custom_RISCV_Ibex_PR2026
Ce dépôt comprend un tuto pour créer des instructions custom sur le processeur Ibex Risc-V en modifiant la Toolchain. Il contient des exemples pour des instructions inspirées du type R, I, U, B.

# Installation

## Dépôt Toolchain RISCV

Installer la Toolchain Risc-V et ses dépendances en suivant les instructions ci-dessous ou ce référer directement au dépôt github: [GitHub - riscv-collab/riscv-gnu-toolchain: GNU toolchain for RISC-V, including GCC · GitHub](https://github.com/riscv/riscv-gnu-toolchain.git)

Installer les prérequis:

```
sudo apt-get install autoconf automake autotools-dev curl python3 libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev device-tree-compiler
```

Cloner le noyeau Linux et ses submodules:

```
git clone --recurse-submodules https://github.com/riscv/riscv-gnu-toolchain.git
```

## Dépôt Opcodes RISCV

Installer le processeur ibex et son environnement de simulation en suivant les instructions de ce lien [GitHub - riscv/riscv-opcodes: RISC-V Opcodes · GitHub](https://github.com/riscv/riscv-opcodes.git)
Commande pour cloner le dépôt:

```
git clone https://github.com/riscv/riscv-opcodes.git
```

# Custom Instructions

Nous vous proposons un tuto pour 4 instructions custom inspirées des types d'instructions déjà existants dans l' ISA Risc-V.

## Instructions :

### Instruction "mod"

Cette instruction permet de calculer le reste d'une division euclidienne entre deux registres sources et d'enregistrer le résultat dans un registre destination. Son opcode est `CUSTOM0`, elle est inspirée des instructions de **type-R**.

###### Syntaxe Assembleur :

```
mod rd rs1 rs2
```

###### Exemple :

```
int a, b, c;
a = 15;
b = 4;
asm volatile(
    "mod   %0, %1, %2\n\t"
    : "=r" (c)
    : "r" (a), "r" (b)
);    // c contient la valeur 3
```

###### Mask et Match :

| **Mask** | **Match** |
| --- | --- |
| `0xfe00707f` | `0xb` |

## Instruction "mroli"

Cette instruction permet de réaliser une rotation vers la droite du registre source du nombre de la valeur immédiate et d'enregistrer le résultat dans le registre destination. Son opcode est `CUSTOM1`, elle est inspirée des instructions de **type-I**.

###### Syntaxe Assembleur :

```
mroli rd rs1 imm12
```

###### Exemple :

```
int a, b;
b = 305419896;    // 305419896 = 0x1234_5678
asm volatile(
    "mroli %0, %1, 0xC"
    : "=r" (a)
    : "r" (b)
);    // a contient la valeur 1 736 516 421 = 0x6781_2345
```

###### Mask et Match :

| **Mask** | Match |
| --- | --- |
| `0x707f` | `0x2b` |

## Instruction "mdroli"

Cette instruction permet de réaliser une rotation vers la droite de 7 décalage de la valeur immédiate et d'enregistrer le résultat dans le registre destination. Son opcode est `CUSTOM2`, elle est inspirée des instructions de **type-U**.

###### Syntaxe Assembleur :

```
mdroli rd imm20
```

###### Exemple :

```
int a;
asm volatile(
    "mdroli   %0, 0x12345678" // 305419896 = 0x1234_5678
    : "=r" (a)
);    // a contient la valeur ???
```

###### Mask et Match :

| **Mask** | Match |
| --- | --- |
| `0x7f` | `0x5b` |

## Instruction "jumb"

Cette instruction permet de faire un saut de 2 instructions si les deux registres sources sont égaux. Son opcode est `CUSTOM3`, elle est inspirée des instructions de **type-J**.

###### Syntaxe Assembleur :

```
jumb rs1 rs2 imm12
```

###### Exemple :

```
int a = 1;
int b = 305419896;
asm volatile(
    "jumb %0 %0 0x123 \n\t"
    "mroli %0, %1, 0xC\n\t"
    "mdroli   %0, 0x12345678"
    : "=r" (a)
    : "r" (b)
); // a contient la valeur ???
```

###### Mask et Match :

| **Mask** | Match |
| --- | --- |
| `0x707f` | `0x7b` |

# Modification de la Toolchain

## Générer le Mask et le Match de la nouvelle instruction

Le MASK montre les champs qui ne sont pas des variables, identifiant l'instruction. Pour l'instruction mod, le MASK est aux positions de funct3, funct7 et de l'opcode. Le MATCH définit la valeur attendue des bits fixes dans les champs déterminés par le MASK.
Le MATCH montre la valeur de ces champs communs à toutes les instructions instanciées indépendamment des variables.
Ces valeurs sont utiles par la suite pour déclarer une nouvelle instruction dans la toolchain. Ces valeurs peuvent être facilement calculées par l'utilisateur ou par ce programme.

###### Récupération du dépôt :

```
git clone https://github.com/riscv/riscv-opcodes
cd riscv-opcodes
```

Modifier le fichier correspondant à l'extension souhaitée dans **riscv-opcodes/extensions**.

###### Nom de fichier :

- `rv_x`: contient des instructions courantes dans les modes 32 bits et 64 bits de l’extension X.
  
- `rv32_x`: contient des instructions présentes uniquement dans rv32x (absentes dans rv64x, par exemple. brev8)
  
- `rv64_x`: contient des instructions présentes uniquement dans rv64x (absentes dans rv32x, par exemple addw)
  
- `rv_x_y`: contient des instructions lorsque les extensions X et Y sont disponibles/activées. Il est recommandé de suivre l’ordre canonique pour les noms de fichiers spécifiés par la spécification.
  
- `unratified`: ce répertoire contiendra également des fichiers similaires aux politiques ci-dessus, mais correspondra à des instructions qui n’ont pas encore été ratifiées.
  

<u>Conseils:</u> modifier le fichier **rv_i**.

Rajouter l'instruction dans le fichier `rv_x`.

###### Syntaxe :

```
<instruction name> <arguments>
```

où l' `argument` est soit une variable (`rd, rs1, rs2, bimm12hi, ...`) ou la valeur des bits.

###### Exemple :

```
mod	    rd rs1 rs2    31..25=0    14..12=0 6..2=2    1..0=3
mroli   rd rs1 imm12              14..12=0 6..2=0xA  1..0=3
mdroli  rd imm20                           6..2=0x16 1..0=3
jumb    bimm12hi rs1 rs2 bimm12lo 14..12=0 6..2=0x1E 1..0=3
```

Compiler pour obtenir le mask et match des instructions associées.

```
cd riscv-opcodes
make
```

Lire les valeurs dans le fichier **riscv-opcodes/encoding.out.h**.

```
#define MATCH_MOD 0x200000b    // Valeur du MATCH
#define MASK_MOD 0xfe00707f    // Valeur du MASK
#define MATCH_MROLI 0x2b
#define MASK_MROLI 0x707f
#define MATCH_MDROLI 0x5b
#define MASK_MDROLI 0x7f
#define MATCH_JUMB 0x7b
#define MASK_JUMB 0x707f

DECLARE_INSN(mod, MATCH_MOD, MASK_MOD)
DECLARE_INSN(mroli, MATCH_MROLI, MASK_MROLI )
DECLARE_INSN(mdroli, MATCH_MDROLI, MASK_MDROLI)
DECLARE_INSN(jumb, MATCH_JUMB, MASK_JUMB)
```

Pour plus de détails, voir le README du dépôt **riscv-opcode**.

## Déclarer la nouvelle instruction dans la ToolChain

```
cd riscv-gnu-toolchain
```

### Déclaration

Dans le fichier **riscv-gnu-toolchain/binutils/include/opcode/riscv-opc.h**, rajouter dans `RISCV_ENCODING_H` la déclaration du MATCH et du MASK:

```
#ifndef RISCV_ENCODING_H
#define RISCV_ENCODING_H
#define MATCH_MOD 0xb
#define MASK_MOD 0xfe00707f
#define MATCH_MROLI 0x2b
#define MASK_MROLI 0x707f
#define MATCH_MDROLI 0x5b
#define MASK_MDROLI 0x7f
#define MATCH_JUMB 0x7b
#define MASK_JUMB 0x707f
```

Dans `DECLARE_INSN` écrire:

```
#endif /* RISCV_ENCODI
DECLARE_INSN(mod, MATCH_MOD, MASK_MOD)
DECLARE_INSN(mroli, MATCH_MROLI, MASK_MROLI )
DECLARE_INSN(mdroli, MATCH_MDROLI, MASK_MDROLI)
DECLARE_INSN(jumb, MATCH_JUMB, MASK_JUMB)
```

### Définition

Dans le fichier **riscv-gnu-toolchain/binutils/opcodes/riscv-opc.c**, définir dans la structure `const struct riscv_opcode riscv_opcodes[]`la nouvelle instruction sous cette forme:

```
{ name, xlen, isa, operands, match, mask, match_func, pinfo}
```

avec:

- `name`: nom de l'instruction
  
- `xlen`: longueur d'un registre d'entiers en bits
  
- `isa`: l'extension ISA
  
- `operands`: syntaxe de l'instruction basée sur ce fichier **riscv-gnu-toolchain/binutils/gas/config/tc-riscv.c**
  

Ci-dessous voici des exemples d'opérandes de l'ISA RISC-V:

| **Code** | **Champ RISC-V** | **Type d'instruction** |
| --- | --- | --- |
| **`d`** | **rd** | R, I, U, J |
| **`s`** | **rs1** | R, I, S, B |
| **`t`** | **rs2** | R, S, B |
| **`j`** | **imm12** | I   |
| **`u`** | **imm20** | U   |
| **`p`** | **bimm12** | B   |

- `match`: MATCH de l'instruction
  
- `mask`: MASK de l'instruction
  
- `match_func`: pointeur vers la fonction qui vérifie que l'instruction lue est bien une instance de l'instruction déclarée
  

```
static int
match_opcode (const struct riscv_opcode *op, insn_t insn)
{
    return ((insn ^ op->match) & op->mask) == 0;
}
```

- `pinfo`: ce sont des flags pour l'assembleur définis dans **riscv-gnu-toolchain/binutils/include/opcode/riscv.h**, mettre `0` s'il n'y a pas besoin de flags.

Ci-dessous voici un exemple des flags standards de l'ISA RISC-V:

| **Flags** | **Description** |
| --- | --- |
| `INSN_ALIAS` | Cette instruction est un alias. |
| `INSN_BRANCH` | Cette instruction est un branchement inconditionnel. |
| `INSN_CONDBRANCH` | Cette instruction est un branchement conditionnel. |
| `INSN_JSR` | Cette instruction est un saut. |

###### Exemple de définition d'instruction :

```
const struct riscv_opcode riscv_opcodes[] =
{
/* name, xlen, isa, operands, match, mask, match_func, pinfo.  */
{"mod",		0, INSN_CLASS_I, "d,s,t",	MATCH_MOD, 	MASK_MOD, 	match_opcode, 0},
{"mroli",	0, INSN_CLASS_I, "d,s,j",	MATCH_MROLI, 	MASK_MROLI, 	match_opcode, 0},
{"mdroli",	0, INSN_CLASS_I, "d,u",		MATCH_MDROLI, 	MASK_MDROLI, 	match_opcode, 0},
{"jumb",	0, INSN_CLASS_I, "s,t,p",	MATCH_JUMB, 	MASK_JUMB, 	match_opcode, INSN_CONDBRANCH},
```

## Compilation de la Toolchain

```
cd riscv-gnu-toolchain
make clean
./configure --prefix=/opt/riscv_custom
make -j$(nproc)

```

Note : Pour mon ubuntu vide, j'ai dû effectuer cette commande pour installer toutes les dépendances liés au make : sudo apt install autoconf automake autotools-dev curl libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev

Vérifier que les fichier de la toolchain se trouve dans le dossier **/opt/riscv_custom/bin/**
## Exporter la Toolchain

```
export PATH=/opt/riscv_custom/bin:$PATH
```

###### Vérifier la Toolchain sélectionnée

```
which riscv64-unknown-elf-gcc
```

## Instanciation

Dans le code C/C++, instancier le mot-clé `asm()` selon la spécification ci-dessous qui fonctionne pour le compilateur gcc.

###### Syntaxe :

```
asm  asm-qualifiers ( assembler template
    : output operands                   (optional)
    : input operands                    (optional)
    : clobbered registers list          (optional)
    );
```

###### Qualifiers :

Voici différents qualifieurs:

- `inline`: Lorsque GCC compile, il estime le nombre d'instruction assembleur. Lorsqu'on utilise la commande `asm()` il a plus de mal a estimé le nombre d'instructionset peut le surestimer. Ainsi, on utilise le qualifieur `inline` pour signaler au compilateur de choisir le coût minimum et ainsi mieux optimiser le code. (Pour plus de détails, voir les sources à la fin de la partie)
  
- `volatile`: Ce qualifieur force le compilateur à garder l'instruction `asm()` et l'empêche de faire des optimisations qui pourrait nuir au code.
  

###### Assembler template :

``assembler template`` est la chaîne de caractères de l'instruction écrite en assembleur. On peut écrire plusieurs instructions en les séparant par les caractères retour à la ligne et tab '`\n\t`'

**Formats spéciaux de strings :**

- `%%`: affiche un seul caractère `%` dans le code assembleur
  
- `%{`, `%|`, `%[`: Génère les caractères `{`, `|` et `[`dans le code assembleur.
  

###### Output Operands :

Les opérandes ont cette syntaxe:

```
[ [asmSymbolicName] ] constraint (cvariablename)
```

avec

- `asmSymbolicName`: C'est le nom de l'opérande dans l'assembler template entouré de crochets (i.e. `%[SymbolicName]`). Si l'asmSymbolicName n'est pas utilisé, l'index dans la liste d'opérandes de l'assembleur template est utilisé.

**Exemple :** deux instructions équivalentes

```
int a, b, c;
asm ("add %[dst], %[src1], %[src2]"
    : [dst] "=r" (a)
    : [src1] "r" (b), [src2] "r" (c));
asm ("add %0, %1, %2"
    : "=r" (a)
    : "r" (b), "r" (c));
```

- `constraint`: C'est une constante qui spécifie une contrainte sur le placement de l'opérande. Les contraintes de sorties doivent commencer par `"="` si c'est une variable qui écrase une valeur existante ou par `"+"` si on réalise une lecture-écriture. Après le préfixe, il doit y avoir une contrainte additionnelle ou plus qui décrit où la valeur se trouve: dans un registre la syntaxe est `"r"` ou en mémoire `"m"`. Il est aussi possible d'écrire `"rm"`, dans ce cas le compilateur choisira la mémoire la plus adaptée.

**Exemple :**

```
asm ("add %0, %1, %0"
    : "+r" (a)
    : "r" (b));
```

- `cvariablename`: spécifie le nom de la variable en C entre parenthèses.

###### Input Operands :

- `constraint`: La contrainte ne doit ni être `"="` ou `"+"`. Pour forcer une entrée à utiliser le même registre qu'une sortie on utilise la syntaxe `"0"` (pour la variable à l'index `0`)

**Exemple :** deux syntaxes équivalentes

```
asm volatile (
    "addl %1, %0"
    : "=r" (a)
    : "r" (b), "0" (a)
);
asm volatile (
    "addl %[val_b], %[val_a]"
    : [val_a] "=r" (a)
    : [val_b] "r" (b), "[val_a]" (a)
);
```

###### Clobbers and Scratch Registers

Certaines instructions peuvent modifier des registres inattendus pour le compilateur. Par exemple, une instruction qui requiert un registre additionnel et qui peut écraser sa valeur initiale par effet de bord. Pour informer le compilateur de ces changements on utilise le champ `clobbered registers list`. Il pourra au préalable enregistrer la valeur de ces registres dans la RAM avant qu'ils ne soient modifiés par l'instruction.

**Exemple :** Voici quelques exemples d'arguments

- `cc`: Il indique que l'assembleur modifie les flags registers
  
- `memory`: Il indique que l'instruction peut modifier des emplacements mémoire autre que ceux listés dans les arguments de l'instruction.
  

> **Source:** [Using Assembly Language with C (Using the GNU Compiler Collection (GCC))](https://gcc.gnu.org/onlinedocs/gcc-8.5.0/gcc/Using-Assembly-Language-with-C.html)

# Modification du processeur Ibex

Pour que l'ibex comprenne notre instruction custom, nous avons besoin de modifier le package, l'étage de décode, l'étage d'exécution ainsi qu'ajouter une functional unit (FU) pour éviter de directement modifier l'ALU.
Tous les fichiers a modifié se trouvent dans **ibex/rtl/**.
Ces modifications sont les modifications minimales pour que l'ibex comprenne notre instruction custom, mais pour aller plus loin, on peut rajouter beaucoup plus de signaux autour de notre FU afin de la rendre complètement indépendante et éviter des confusions entre l'ALU et la FU.

L'ibex que nous avons utilisé est celui-ci, en configuration small : 

```
git clone https://github.com/lowRISC/ibex.git
```

Fuseoc et Verilator sont des outils de simulation que l'on a utilisés. Le python-requirements permet d'installer certaines dépendances. 

```
pip install fusesoc
pip3 install -U -r python-requirements.txt
sudo apt install verilator
```

## Le package

Dans le fichier ibex_pkg.sv, il faut ajouter les opcodes que l'on a crée ainsi que des constantes pour notre FU : 

```
typedef enum logic [6:0] {
    OPCODE_CUSTOM0  = 7'h0b,    
    OPCODE_CUSTOM1  = 7'h2b,    
    OPCODE_CUSTOM2  = 7'h5b,    
    ...
  } opcode_e;

typedef enum logic [6:0] {
    FU_NULL,
    FU_MOD,    
    FU_ROTI, // instruction mroli   
    FU_ROTU  // instruction mdroli
  } fu_op_e;
```

## L'étage de décode

Dans le fichier ibex_decoder.sv, il faut gérer le traitement de notre décode afin d'activer les signaux voulus.
Pour notre rotation voilà les deux blocs qui sont liés à l'opcode de la rotation et qui active les bons signaux de contrôle. 

```
OPCODE_CUSTOM1: begin // Custom I-type
	rf_ren_a_o       = 1'b1;
	rf_we            = 1'b1;

	unique case (instr[14:12])          3'b000: illegal_insn = 1'b0;
	  default: illegal_insn = 1'b1;
	endcase

  end

```

- `rf_ren_a_o` permet d'indiquer qu'on va lire dans le registre de l'opérand a
- `rf_we` permet d'indiquer qu'on va écrire dans le registre de destination
- `illegal_insn` permet d'indiquer si l'isntruction est reconnu ou non pas notre ibex


La FU utilisera les mêmes entrées que l'ALU pour des opérations de mêmes types donc on réutilise des signaux liés à l'ALU. Cependant, si on fait une instruction avec des opérands de dimensions différentes et qui ne respescte plus les types R, I, J etc; alors il faudra créer des signaux propre à la FU potentiellement.
```
OPCODE_CUSTOM1: begin
        alu_op_a_mux_sel_o  = OP_A_REG_A;
        alu_op_b_mux_sel_o  = OP_B_IMM;
        imm_b_mux_sel_o     = IMM_B_I;
        unique case (instr_alu[14:12])
          3'b000: fu_operator_o = FU_ROTI;
          default: ;
        endcase
      end
```

- `alu_op_a_mux_sel_o` permet d'indiquer quelle donnée on va sélectionner pour être l'opérand a
- `alu_op_b_mux_sel_o` permet d'indiquer quelle donnée on va sélectionner pour être l'opérand b, pour la rotation une immédiate
- `fu_operator` indique à notre FU l'opération qu'on va faire (similaire à l'alu_operator pour l'ALU)

Comme on utilise un signal différent pour choisir l'opération de l'ALU et de la FU, on assign une valeur par défaut au `fu_operator_o`

```
always_comb begin
    alu_operator_o     = ALU_SLTU;
    fu_operator_o = FU_NULL;
    alu_op_a_mux_sel_o = OP_A_IMM;
    alu_op_b_mux_sel_o = OP_B_IMM;
```

Signal de sortie pour contrôler la FU
```
output ibex_pkg::fu_op_e     fu_operator_o,
```

## L'étage d'exécution

Instanciation de la FU

```
ibex_fu #(
) fu_i (
    .operator_i         (fu_operator_i),
    .operand_a_i        (alu_operand_a_i),
    .operand_b_i        (alu_operand_b_i),
    .fu_use             (custom_inst),
    .result_o           (fu_result)
);
```
Afin d'utiliser le résultat sortant de la FU, on utilise le signal de validation de FU dans le MUX du choix des sorties.

```
assign result_ex_o = custom_inst ? fu_result : multdiv_sel ? multdiv_result : alu_result;
```

Définitions des signaux utiles à la FU
```
logic custom_inst;
```
```
input  ibex_pkg::fu_op_e      fu_operator_i,
```
```
logic [31:0] alu_result, multdiv_result, fu_result;
```
### La functional unit

Il faut créer un nouveau fichier pour notre nouveau module. Par exemple, ibex_fu.sv : 

```
module ibex_fu #(
) (
  input  ibex_pkg::fu_op_e operator_i,
  input  logic [31:0]       operand_a_i,
  input  logic [31:0]       operand_b_i,

  output logic [31:0]       result_o,
  output logic fu_use
);
    import ibex_pkg::*;
    logic [31:5] unused_bits = operand_b_i[31:5];
    logic [4:0] shamt;
    logic [4:0] test;

    always_comb begin
        result_o = '0;
        fu_use = 0;
        
        shamt = operand_b_i[4:0];
        test = shamt;
        shamt = test;
        if (operator_i != 0) begin
            $display("result = %h", operator_i);
        end
        unique case (operator_i)

            FU_ROTI: begin
                result_o = (operand_a_i << shamt) | (operand_a_i >> (32 - shamt));
                $display("result = %h et entrée = %h", result_o, operand_a_i); //Display qui permet d'afficher dans le terminal l'entrée et la sortie de la FU pour voir si l'opération est correcte
                fu_use = 1'b1;
                // result_o = operand_a_i % operand_b_i;
            end

            default: result_o = '0;

        endcase
    end
endmodule

```

- `fu_use` c'est l'entrée qui permet de sélectionner l'opération choisi

### Pilotage du mux

On utilise le signal custom_inst qui sort de notre FU afin d'envoyer le résultat du FU lorsqu'il y a l'utilisation d'une instruction custom.

```
assign result_ex_o = custom_inst ? fu_result : multdiv_sel ? multdiv_result : alu_result;

assign alu_adder_result_ex_o = custom_inst ? fu_result : alu_adder_result_ex_r;
```

## Id stage

Afin de faire la transition entre l'étage de décode et le core, on rajoute les différents signaux liés au contrôle de notre FU.
```
output ibex_pkg::fu_op_e          fu_operator_ex_o,
```
```
assign fu_operator_ex_o            = fu_operator;
```
```
// ALU Control
  alu_op_e     alu_operator;
  fu_op_e      fu_operator;
  op_a_sel_e   alu_op_a_mux_sel, alu_op_a_mux_sel_dec;
  op_b_sel_e   alu_op_b_mux_sel, alu_op_b_mux_sel_dec;
```

Rajout de la sortie de l'étage de décode :
```
// ALU
    .alu_operator_o    (alu_operator),
    .fu_operator_o (fu_operator),
    .alu_op_a_mux_sel_o(alu_op_a_mux_sel_dec),
    .alu_op_b_mux_sel_o(alu_op_b_mux_sel_dec),
    .alu_multicycle_o  (alu_multicycle_dec),
```

## Core 

Dans le core, on doit juste rajouter les liens des signaux que l'on a rajouté pour faire la connection entre l'étage d'exécution et l'étage de décode, notamment le signal `fu_operator_ex` qui porte l'opération de la FU.

```
fu_op_e     fu_operator_ex;
```

Pour l'id stage :

```
.alu_operator_ex_o (alu_operator_ex),
.fu_operator_ex_o (fu_operator_ex),
.alu_operand_a_ex_o(alu_operand_a_ex),
.alu_operand_b_ex_o(alu_operand_b_ex),
```

Pour l'exécution stage :
```
.alu_operator_i         (alu_operator_ex),
.fu_operator_i (fu_operator_ex),
.alu_operand_a_i        (alu_operand_a_ex),
.alu_operand_b_i        (alu_operand_b_ex),
.alu_instr_first_cycle_i(instr_first_cycle_id),
```
## Ajout du fichier de la functional unit dans le build

Pour que le nouveau module qu'est la functional unit soit pris en compte lors du build de l'Ibex, il faut rajouter une ligne dans le fichier ibex/ibex_core.core
```
files:
      - rtl/ibex_alu.sv
      - rtl/ibex_fu.sv
      - rtl/ibex_branch_predict.sv
      - rtl/ibex_compressed_decoder.sv
      - rtl/ibex_controller.sv
```

# Compilation et Exécution du code

###### Code C sur le processeur Ibex :

Voici le template général du code pour éviter les warnings et les erreurs lors de la compilation:

```
#define SIM_HALT 0x20008

void simple_exc_handler(void) { while(1); }
void simple_timer_handler(void) { while(1); }

void stop_simulation() {
    volatile unsigned int *p = (unsigned int *)SIM_HALT;
    *p = 1;
}

int main() {
    stop_simulation();
    return 0;
}
```

Fonction utile pour afficher un message dans le fichier log:

```
#define UART_OUT 0x20000
void print_char(char c) {
    volatile unsigned int *p = (unsigned int *) UART_OUT;
    *p = c;
}
```


###### Makefile 

Pour build l'Ibex, compiler le .elf ainsi qu'exécuter le programme, il suffit d'utiliser ce Makefile ainsi qu'effectuer ces commandes dans le dossier racine (au-dessus d'Ibex) :
```
make setup_sim
make nom_du_fichier.elf
make nom_du_fichier.run
```
```
# --- Configuration des chemins ---
#export PATH=/opt/riscv_custom/bin:$PATH
#export PATH=/opt/riscv_custom/bin:$PATH
VENV = ibex/venv/bin/activate

# --- Paramètres de compilation ---
CC      = riscv64-unknown-elf-gcc
OBJCOPY = riscv64-unknown-elf-objcopy
OBJDUMP = riscv64-unknown-elf-objdump
CFLAGS  = -march=rv32imc -mabi=ilp32 -static -nostartfiles -O2
LDFLAGS = -T ibex/examples/sw/simple_system/common/link.ld
CRT0    = ibex/examples/sw/simple_system/common/crt0.S

# --- Paramètres d' exécution ---
SIM     = ./ibex/build/lowrisc_ibex_ibex_simple_system_0/sim-verilator/Vibex_simple_system

# --- Cibles par défaut ---
.PHONY: all clean setup_sim

all: help

# Nettoyage des logs et des exécutables
clean:
	@echo "Nettoyage des fichiers log, elf, dis, hex, bin et csv..."
	rm -f *.log *.elf *.dis *.hex *.bin *.csv

# Initialisation de l'environnement de simulation (FuseSoC)
setup_sim:
	@echo "Lancement de la simulation FuseSoC..."
	bash -c "source $(VENV) && cd ibex && fusesoc --cores-root . run --target=sim --setup --build lowrisc:ibex:ibex_simple_system $(util/ibex_config.py small fusesoc_opts)"

# Compilation : produit un .elf à partir d'un .c
%.elf: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(LDFLAGS) $(CRT0) $< -o $@
	@echo "Generating Hex file..."
	$(OBJCOPY) -O verilog $@ $*.hex
	@echo "Generating Bin file..."
	$(OBJCOPY) -O binary $@ $*.bin
	@echo "Generating Disassembly..."
	$(OBJDUMP) -d $@ > $*.dis
# riscv32-unknown-elf-gcc -march=rv32imc -mabi=ilp32 -static -nostartfiles -T ibex/examples/sw/simple_system/common/link.ld ibex/examples/sw/simple_system/common/crt0.S test_mod.c -o test_mod.elf
# riscv32-unknown-elf-objcopy -O verilog test_mod.elf test_mod.hex
# riscv32-unknown-elf-objdump -d test_mod.elf test_mod.dis

# Exécution : compile puis lance le simulateur
%.run: %.elf
	@echo "Running simulation for $<..."
	$(SIM) --meminit=ram,$<
	
toolchain:
	which riscv64-unknown-elf-gcc


```
###### Commande de lancement de la simulation 

Premièrement il faut lancer la simulation du processeur. On utilise la simulation `simple_system`. Le processeur est en taille "small" et n'a que 2 étages.

Dans le répertoire qui contient le clone du processeur ibex, qui contient le fichier **/ibex**, exécutez la commande suivante :

```
bash -c "source ibex/venv/bin/activate && \
cd ibex && \
fusesoc --cores-root . run --target=sim --setup --build \
lowrisc:ibex:ibex_simple_system \
$(util/ibex_config.py small fusesoc_opts)"
pip3 install -U -r python-requirements.txt (A FAIRE DANS LENV PYTHON)

```

###### Commande de compilation :

Ensuite, on peut compiler le code. Exécuter la commande suivante au même niveau que le dossier **/ibex**:

```
riscv32-unknown-elf-gcc -march=rv32imc -mabi=ilp32 -static \
-nostartfiles -O2 -T ibex/examples/sw/simple_system/common/link.ld \
ibex/examples/sw/simple_system/common/crt0.S \
chemin/vers/file.c -o chemin/ou/sera/cree/file.elf
```

Le flag `-O2` n'est pas indispensable, il indique seulement au processeur de faire plus d'optimisations ce qui permet notamment d'enlever les store et load des variables avant et après chaque instruction. Nous l'avons utilisé pour tester les dépendances entre les instructions.

Après avoir créé le fichier .elf, pour aider au debuggage, on peut aussi compiler les fichiers `.hex` et `.dis`:

```
riscv32-unknown-elf-objcopy -O verilog chemin/vers/file.elf chemin/vers/file.hex
riscv32-unknown-elf-objdump -d chemin/vers/file.elf > chemin/vers/file.dis
```

###### Commande d'exécution :

Pour exécuter le programme, entrez la commande suivante au même niveau que le fichier **/ibex**:

```
./ibex/build/lowrisc_ibex_ibex_simple_system_0/sim-verilator/Vibex_simple_system \
--meminit=ram,chemin/vers/file.elf
```

# Sources
> https://pcotret.gitlab.io/riscv-custom/sw_toolchain.html#adding-a-custom-instruction-in-the-cross-compiler
> 
