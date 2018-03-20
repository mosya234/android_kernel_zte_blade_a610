 //#modify@zte.com.cn add at 20160126 begin
/*
 * Copyright (C) 2015 transsion, Inc.
 *
 * Author: peixuan qiu <peixuan.qiu@reallytek.com>
 * this module is intended for supporting hall function separately
 * 
 */
#ifndef HALL_H_
#define HALL_H_

#include <linux/ioctl.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
//#include <linux/earlysuspend.h>
#include <aee.h>
#include <asm/atomic.h>
#include <asm/uaccess.h>
//#include <mach/hal_pub_kpd.h>
#include <linux/switch.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/irq.h>
#include <mt_gpio.h>
//#include "cust_eint.h"

/********************control macro**************************************/

//#define USE_EXTERN_IDEV	//use external input device defined in kpd.c
#define USE_WORK_STRUCT	// use work struct report state instead of tasklet
#define HALL_DEBUG 1	//open or close debug message
#define USE_MISCDEV

#ifdef CONFIG_OF
//#undef CONFIG_OF
#endif

#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(USE_EARLY_SUSPEND)
#undef CONFIG_HAS_EARLYSUSPEND
#undef USE_EARLY_SUSPEND
#endif

//debug
#define TAG "<wind-hall>"
#define HALL_NAME "wind_hall"
#define HALL_INPUT_NAME	"hdata"
#define HDBG_FUNC()\
	printk(TAG"%s\n", __func__)

#if HALL_DEBUG
	#define HDBG_LOG(fmt, arg...)	printk(TAG fmt, ##arg)
#else
	#define HDBG_LOG(fmt, arg...)	
#endif

//polarity
#define CUST_EINT_POLARITY_LOW	0  //EINT status
#define CUST_EINT_POLARITY_HIGH	1  // EINT status 

//hall state
#define    HALL_CLOSE  0   
#define    HALL_OPEN   1  
 
struct hall_context {
#ifndef USE_EXTERN_IDEV
	struct input_dev *idev;
#endif
#ifdef USE_MISCDEV
	struct miscdevice *mdev;
#endif
	struct switch_dev sdev;
#ifdef USE_WORK_STRUCT
	struct work_struct report;
#endif

#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(USE_EARLY_SUSPEND)
	//struct early_suspend early_drv;
#endif

#if defined(CONFIG_OF)
	struct device_node *irq_node;
	int		irq;
	int pin_number;
#endif
};

#endif
 //#modify@zte.com.cn add at 20160126 end