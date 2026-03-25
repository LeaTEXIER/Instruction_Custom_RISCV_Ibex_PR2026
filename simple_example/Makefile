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

