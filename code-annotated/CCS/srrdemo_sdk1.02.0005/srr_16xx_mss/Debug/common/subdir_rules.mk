################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
common/%.obj: ../common/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ti-cgt-arm_16.9.6.LTS/bin/armcl" -mv7R4 --code_state=16 --float_support=VFPv3D16 -me -O3 --include_path="F:/Workspace/CCS/srr_16xx_mss" --include_path="C:/ti/mmwave_sdk_01_02_00_05" --include_path="C:/ti/mmwave_sdk_01_02_00_05/packages" --include_path="C:/ti/ti-cgt-arm_16.9.6.LTS/include" --define=_LITTLE_ENDIAN --define=SOC_XWR16XX --define=SUBSYS_MSS --define=DOWNLOAD_FROM_CCS --define=MMWAVE_L3RAM_SIZE=0x40000 --define=DebugP_ASSERT_ENABLED -g --c99 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --enum_type=int --abi=eabi --preproc_with_compile --preproc_dependency="common/$(basename $(<F)).d_raw" --obj_directory="common" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


