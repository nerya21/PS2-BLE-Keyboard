################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Drivers/PIN/PINCC26XX.obj: C:/ti/tirtos_cc13xx_cc26xx_2_18_00_03/products/tidrivers_cc13xx_cc26xx_2_16_01_13/packages/ti/drivers/pin/PINCC26XX.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/bin/armcl" --cmd_file="C:/ti/simplelink/ble_sdk_2_02_00_31/examples/cc2650em/hid_emu_kbd/ccs/app/../../iar/stack/../../../../../src/config/build_components.opt" --cmd_file="C:/ti/simplelink/ble_sdk_2_02_00_31/examples/cc2650em/hid_emu_kbd/ccs/app/../../iar/stack/build_config.opt" --cmd_file="C:/ti/simplelink/ble_sdk_2_02_00_31/examples/cc2650em/hid_emu_kbd/ccs/app/../../iar/stack/../../ccs/config/ccs_compiler_defines.bcfg"  -mv7M3 --code_state=16 --abi=eabi -me -O4 --opt_for_speed=0 --include_path="C:/ti/ccsv6/tools/compiler/arm_15.12.3.LTS/include" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/examples/hid_emu_kbd/cc26xx/app" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/icall/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/profiles/roles/cc26xx" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/profiles/roles" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/profiles/dev_info" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/profiles/hid_dev_kbd/cc26xx" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/profiles/hid_dev_kbd" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/profiles/scan_param/cc26xx" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/profiles/scan_param" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/profiles/hid_dev/cc26xx" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/profiles/batt/cc26xx" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/common/cc26xx" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/components/heapmgr" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/controller/cc26xx/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/components/hal/src/target/_common" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/target" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/components/hal/src/target/_common/cc26xx" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/components/hal/src/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/components/osal/src/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/components/services/src/sdata" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/components/services/src/saddr" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/components/icall/src/inc" --include_path="C:/ti/simplelink/ble_sdk_2_02_00_31/src/inc" --include_path="C:/ti/tirtos_cc13xx_cc26xx_2_18_00_03/products/cc26xxware_2_23_03_17162" --c99 --define=USE_ICALL --define=Display_DISABLE_ALL --define=HIDDEVICE_TASK_STACK_SIZE=530 --define=GAPROLE_TASK_STACK_SIZE=520 --define=HEAPMGR_SIZE=0 --define=ICALL_MAX_NUM_TASKS=4 --define=ICALL_MAX_NUM_ENTITIES=6 --define=MAX_NUM_BLE_CONNS=1 --define=xdc_runtime_Assert_DISABLE_ALL --define=xdc_runtime_Log_DISABLE_ALL --define=CC2650_LAUNCHXL --define=CC26XX --diag_wrap=off --diag_suppress=48 --diag_warning=225 --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="Drivers/PIN/PINCC26XX.d" --obj_directory="Drivers/PIN" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


