subdir-ccflags-y += -Werror
ccflags-y += -I$(srctree)/drivers/misc/mediatek/include/mt-plat/
ccflags-y += -I$(srctree)/drivers/input/touchscreen/mediatek
# In case the platform does NOT support this type of sensors
obj-y   +=  mtk_tpd.o
obj-y   +=  tpd_button.o
obj-y   +=  tpd_calibrate.o
obj-y   +=  tpd_debug.o
obj-y   +=  tpd_default.o
obj-y   +=  tpd_init.o
obj-y   +=  tpd_misc.o
obj-y   +=  tpd_setting.o

obj-$(CONFIG_TOUCHSCREEN_MTK_GT1151)	+=  GT1151/
obj-$(CONFIG_TOUCHSCREEN_MTK_GT910)	+=  GT910/
obj-$(CONFIG_TOUCHSCREEN_MTK_GT9XXTB_HOTKNOT)	+=  GT9XXTB_hotknot/
obj-$(CONFIG_TOUCHSCREEN_MTK_SYNAPTICS_I2C_RMI4)	+=  synaptics_i2c_rmi4/
obj-$(CONFIG_TOUCHSCREEN_MTK_GT928)	+=  GT928/
obj-$(CONFIG_TOUCHSCREEN_MTK_GT9XX_HOTKNOT_SCP)	+=  GT9XX_hotknot_scp/
obj-$(CONFIG_TOUCHSCREEN_MTK_GT911)	+=  GT911/
obj-$(CONFIG_TOUCHSCREEN_MTK_FT5X0X)	+=  ft5x0x/
#modify@zte.com.cn 20160612 begin
ifeq ($(CONFIG_TOUCHSCREEN_HIMAX_HX852xES_E188F),y)
obj-y	+=  hx852xes_E188F/
endif
#modify@zte.com.cn 20160612 end
#modify@zte.com.cn 20160510 begin
ifeq ($(CONFIG_TOUCHSCREEN_HIMAX_HX852xES),y)
obj-y	+=  hx852xes/
endif
ifeq ($(CONFIG_TOUCHSCREEN_MTK_MSG5846),y)
obj-y	+=  MSG5846/
endif
ifeq ($(CONFIG_TOUCHSCREEN_MTK_GT915_E181),y)
obj-y	+=  GT915_E181/
endif
#modify@zte.com.cn 20160104 end
##modify@zte.com.cn add at 20160518 begin
ifeq ($(CONFIG_TOUCHSCREEN_MTK_FT3427),y)
obj-y	+=  FT3427_E188/
endif
ifeq ($(CONFIG_TOUCHSCREEN_MTK_FT5436),y)
obj-y	+=  FT5436i_E169/
endif
ifeq ($(CONFIG_TOUCHSCREEN_MTK_GT915L),y)
obj-y	+=  GT915/
endif
obj-$(CONFIG_TOUCHSCREEN_MTK_GT9XX_M) +=  GT9157/
##modify@zte.com.cn add at 20160518 end
##modify@zte.com.cn add at 20160605 end
ifeq ($(CONFIG_TOUCHSCREEN_MTK_FT5446),y)
obj-y	+=  FT5446/
endif
##modify@zte.com.cn add at 20160605 end