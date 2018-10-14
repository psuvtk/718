################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CFG_SRCS += \
../mss_srr.cfg 

CMD_SRCS += \
../mss_srr_linker.cmd \
../r4f_linker.cmd 

C_SRCS += \
../mss_main.c \
../mss_srr_cli.c 

GEN_CMDS += \
./configPkg/linker.cmd 

GEN_FILES += \
./configPkg/linker.cmd \
./configPkg/compiler.opt 

GEN_MISC_DIRS += \
./configPkg/ 

C_DEPS += \
./mss_main.d \
./mss_srr_cli.d 

GEN_OPTS += \
./configPkg/compiler.opt 

OBJS += \
./mss_main.obj \
./mss_srr_cli.obj 

GEN_MISC_DIRS__QUOTED += \
"configPkg\" 

OBJS__QUOTED += \
"mss_main.obj" \
"mss_srr_cli.obj" 

C_DEPS__QUOTED += \
"mss_main.d" \
"mss_srr_cli.d" 

GEN_FILES__QUOTED += \
"configPkg\linker.cmd" \
"configPkg\compiler.opt" 

C_SRCS__QUOTED += \
"../mss_main.c" \
"../mss_srr_cli.c" 


