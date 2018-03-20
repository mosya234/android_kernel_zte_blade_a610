# In case the platform does NOT support this type of sensors
#modify@zte.com.cn 20160202 begin
ccflags-y += -I$(srctree)/drivers/misc/mediatek/hwmon/include
#ccflags-y += -I$(srctree)/drivers/misc/mediatek/accelerometer/inc
#modify@zte.com.cn 20160202 end
obj-y	+= tilt_detector.o

ifeq ($(CONFIG_CUSTOM_KERNEL_TILT_DETECTOR_SENSOR),"OTHER_VENDOR")
obj-y += other_vendor/
endif

