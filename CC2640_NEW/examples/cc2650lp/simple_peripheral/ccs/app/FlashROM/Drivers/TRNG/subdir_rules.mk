################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Drivers/TRNG/TRNGCC26XX.obj: C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/hal/src/target/_common/TRNGCC26XX.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" --cmd_file="C:/ti/simplelink/ble_sdk_2_02_01_18/examples/cc2650lp/simple_peripheral/ccs/app/../../iar/stack/../../../../../src/config/build_components.opt" --cmd_file="C:/ti/simplelink/ble_sdk_2_02_01_18/examples/cc2650lp/simple_peripheral/ccs/app/../../iar/stack/build_config.opt" --cmd_file="C:/ti/simplelink/ble_sdk_2_02_01_18/examples/cc2650lp/simple_peripheral/ccs/app/../../iar/stack/../../ccs/config/ccs_compiler_defines.bcfg"  -mv7M3 --code_state=16 --abi=eabi -me -O4 --opt_for_speed=0 --include_path="C:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/examples/simple_peripheral/cc26xx/app" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/icall/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/profiles/roles/cc26xx" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/profiles/roles" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/profiles/dev_info" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/profiles/simple_profile/cc26xx" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/profiles/simple_profile" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/common/cc26xx" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/heapmgr" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/controller/cc26xx/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/hal/src/target/_common" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/target" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/hal/src/target/_common/cc26xx" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/hal/src/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/osal/src/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/services/src/sdata" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/services/src/saddr" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/components/icall/src/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_01_18/src/rom" --include_path="C:/ti/tirtos_cc13xx_cc26xx_2_20_01_08/products/cc26xxware_2_24_02_17393" -g --c99 --gcc --define=BOARD_DISPLAY_EXCLUDE_LCD --define=BOARD_DISPLAY_EXCLUDE_UART --define=CC2650_LAUNCHXL --define=CC26XX --define=Display_DISABLE_ALL --define=HEAPMGR_SIZE=0 --define=ICALL_MAX_NUM_ENTITIES=6 --define=ICALL_MAX_NUM_TASKS=3 --define=POWER_MEASURE --define=POWER_SAVING --define=USE_ICALL --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --diag_wrap=off --diag_suppress=48 --diag_warning=225 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="Drivers/TRNG/TRNGCC26XX.d" --obj_directory="Drivers/TRNG" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


