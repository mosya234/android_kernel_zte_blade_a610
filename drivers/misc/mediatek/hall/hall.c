/*
 * Copyright (C) 2015 transsion, Inc.
 *
 * Author: peixuan qiu <peixuan.qiu@reallytek.com>
 * this module is intended for supporting hall function separately
 * 
 */
 //#modify@zte.com.cn add at 20160126 begin
#include "hall.h"

#ifdef CONFIG_OF
	#include <linux/of.h>
	#include <linux/of_address.h>
	#include <linux/of_irq.h>
	#include <linux/irq.h>
	#include <linux/gpio.h>
#endif

#ifdef USE_EXTERN_IDEV
extern struct input_dev *kpd_input_dev;
#endif

/******************external include****************/
#include <mt_boot_common.h>
/**********************************************/

int (*tpd_switch_status)(int status) = NULL; 
static struct hall_context *hall_data;
static int hall_state = HALL_OPEN;
u32 hall_gpio;

#if defined(USE_WORK_STRUCT)
	static struct workqueue_struct *hall_wq;
	static void hall_eint_work(struct work_struct * work);
#else
	static void hall_eint_work(unsigned long work);
	static DECLARE_TASKLET(hall_tasklet, hall_eint_work, 0);
#endif

/*******staic struct declaration**********/
#ifdef USE_MISCDEV
static struct miscdevice hall_miscdev;
#endif
/*****************************************/

#ifdef CONFIG_OF
struct platform_device *hallpdev;
#endif

/*------------------------------------------------------------------------*/

#if !defined(CONFIG_HAS_EARLYSUSPEND) && !defined(USE_EARLY_SUSPEND)
static int hall_suspend(struct platform_device *dev, pm_message_t state)
{
	HDBG_FUNC();
	return 0;
}

static int hall_resume(struct platform_device *dev)
{
	HDBG_FUNC();
	return 0;
}
#else
static void hall_early_suspend(struct early_suspend *h)
{
	HDBG_FUNC();
}

static void hall_late_resume(struct early_suspend *h)
{
	HDBG_FUNC();
}
#endif

/*------------------------------------------------------------------------*/
#if defined(USE_WORK_STRUCT)
static void hall_eint_work(struct work_struct * work)
#else
static void hall_eint_work(unsigned long work)
#endif
{
	u8 old_state = hall_state;
	HDBG_FUNC();
	
#ifndef CONFIG_OF
	if (mt_get_gpio_in(GPIO_MHALL_EINT_PIN) == GPIO_IN_ZERO) {
		HDBG_LOG("hall_handler close");
		hall_state = HALL_CLOSE;
	} else {
		HDBG_LOG("hall_handler open");
		hall_state = HALL_OPEN;

	}
	if (old_state == hall_state) {
		mt_eint_unmask(CUST_EINT_MHALL_NUM);
    		return;
	}
#else
	if (gpio_get_value(hall_data->pin_number) == GPIO_IN_ZERO) {
		HDBG_LOG("hall_handler close\n");
		hall_state = HALL_CLOSE;
	} else {
		HDBG_LOG("hall_handler open\n");
		hall_state = HALL_OPEN;

	}
	if (old_state == hall_state) {
		enable_irq(hall_data->irq);
    		return;
	}
#endif

	if (get_boot_mode() == FACTORY_BOOT)
	{
	#if 1
		static bool hall_test_close = 0;
		static bool hall_test_open = 0;
		if (hall_state == HALL_CLOSE) {
			hall_test_close = 1;
		} else {
			hall_test_open = 1;
		}
		if((1 == hall_test_close) && (1 == hall_test_open)) {
		#ifndef USE_EXTERN_IDEV
			input_report_key(hall_data->idev, KEY_F11, 1);
			input_sync(hall_data->idev);
			input_report_key(hall_data->idev, KEY_F11, 0);
			input_sync(hall_data->idev);
		#else
			input_report_key(kpd_input_dev, KEY_F11, 1);
			input_sync(kpd_input_dev);
			input_report_key(kpd_input_dev, KEY_F11, 0);
			input_sync(kpd_input_dev);
		#endif
			hall_test_close = 0;
			hall_test_open = 0;
		}
	#else
		#ifndef USE_EXTERN_IDEV
		input_report_key(hall_data->idev, KEY_F11, (hall_state == HALL_CLOSE));
		input_sync(hall_data->idev);  
		#else
		input_report_key(kpd_input_dev, KEY_F11, (hall_state == HALL_CLOSE));
		input_sync(kpd_input_dev);  
		#endif
	#endif
	} else {
		if (hall_state == HALL_CLOSE) {
			HDBG_LOG("hall_handler report close and set sys/class/switch/hall/state 0");
		#ifndef USE_EXTERN_IDEV
			input_report_key(hall_data->idev, KEY_F11, 1);
			input_sync(hall_data->idev); 
			input_report_key(hall_data->idev, KEY_F11, 0);
			input_sync(hall_data->idev); 
		#else
			input_report_key(kpd_input_dev, KEY_F11, 1);
			input_sync(kpd_input_dev); 
			input_report_key(kpd_input_dev, KEY_F11, 0);
			input_sync(kpd_input_dev); 
		#endif		 
			switch_set_state(&hall_data->sdev, HALL_CLOSE);	 
		#if 1
			if (tpd_switch_status) {
				tpd_switch_status(1);
			}
		#endif
		} else {
			HDBG_LOG("hall_handler report close and set sys/class/switch/hall/state 0");
		#ifndef USE_EXTERN_IDEV
			input_report_key(hall_data->idev, KEY_F12, 1);
			input_sync(hall_data->idev); 
			input_report_key(hall_data->idev, KEY_F12, 0);
			input_sync(hall_data->idev); 
		#else
			input_report_key(kpd_input_dev, KEY_F12, 1);
			input_sync(kpd_input_dev); 
			input_report_key(kpd_input_dev, KEY_F12, 0);
			input_sync(kpd_input_dev); 
		#endif		 
			switch_set_state(&hall_data->sdev, HALL_OPEN);
		#if 1
			if(tpd_switch_status) {
				tpd_switch_status(0);
			}
		#endif
		}
	}
	
#ifndef CONFIG_OF
	if (mt_get_gpio_in(GPIO_MHALL_EINT_PIN) == GPIO_IN_ZERO) {
    		HDBG_LOG("hall_handler set int polarity  high");
		mt_eint_set_polarity(CUST_EINT_MHALL_NUM, CUST_EINT_POLARITY_HIGH);	
		//irq_set_irq_type();
	} else {
    		HDBG_LOG("hall_handler set int polarity  low");
		mt_eint_set_polarity(CUST_EINT_MHALL_NUM, CUST_EINT_POLARITY_LOW);    
	}
	mt_eint_unmask(CUST_EINT_MHALL_NUM);	
#else
	if (gpio_get_value(hall_data->pin_number) == GPIO_IN_ZERO) {
    		HDBG_LOG("hall_handler set int polarity  high");
		irq_set_irq_type(hall_data->irq, IRQ_TYPE_LEVEL_HIGH);	
	} else {
    		HDBG_LOG("hall_handler set int polarity  low");
		irq_set_irq_type(hall_data->irq, IRQ_TYPE_LEVEL_LOW);    
	}

	enable_irq(hall_data->irq);
#endif
}

/*--------------------------------------------------------------------------*/
#ifndef CONFIG_OF
static void hall_eint_handler(void)
#else
static irqreturn_t hall_eint_handler(int flag, void * name)
#endif
{
	HDBG_FUNC();
#if defined(USE_WORK_STRUCT)
	queue_work(hall_wq, &hall_data->report);
#else
	tasklet_schedule(&hall_tasklet);
#endif

#ifdef CONFIG_OF
	disable_irq_nosync(hall_data->irq);
	return IRQ_HANDLED;
#endif
}
/*-------------------------------------------------------------------------*/
static int hall_setup_eint(void)
{
	int ret = 0;
	
#ifndef CONFIG_OF
	mt_set_gpio_mode(GPIO_MHALL_EINT_PIN, GPIO_MHALL_EINT_PIN_M_EINT);
	mt_set_gpio_pull_enable(GPIO_MHALL_EINT_PIN, TRUE);
	mt_set_gpio_pull_select(GPIO_MHALL_EINT_PIN, GPIO_PULL_UP);
	mt_set_gpio_dir(GPIO_MHALL_EINT_PIN, GPIO_DIR_IN);
	mt_eint_set_hw_debounce(CUST_EINT_MHALL_NUM, CUST_EINT_MHALL_DEBOUNCE_CN);
	if (mt_get_gpio_in(GPIO_MHALL_EINT_PIN) == GPIO_IN_ZERO) { 
		 hall_state = HALL_CLOSE;   
		 switch_set_state(&hall_data->sdev, HALL_CLOSE);
	    	 mt_eint_registration(CUST_EINT_MHALL_NUM, CUST_EINTF_TRIGGER_HIGH, hall_eint_handler, 0);
         	HDBG_LOG("hall_probe init register hall int polarity  high");
    	} else {
        	 hall_state = HALL_OPEN;   
		 switch_set_state(&hall_data->sdev, HALL_OPEN);
        	 mt_eint_registration(CUST_EINT_MHALL_NUM, CUST_EINTF_TRIGGER_LOW, hall_eint_handler, 0);
       	  	HDBG_LOG("hall_probe init register hall int polarity  low");
   	}
	mt_eint_unmask(CUST_EINT_MHALL_NUM);
#else

	u32 ints[2] = {0, 0};

	struct pinctrl *pinctrl;
	struct pinctrl_state *pins_default;
	struct pinctrl_state *pins_cfg;

/* gpio setting */
	pinctrl = devm_pinctrl_get(&hallpdev->dev);
	if (IS_ERR(pinctrl)) {
		ret = PTR_ERR(pinctrl);
		HDBG_LOG("Cannot find hall pinctrl!\n");
	}
	pins_default = pinctrl_lookup_state(pinctrl, "pin_default");
	if (IS_ERR(pins_default)) {
		ret = PTR_ERR(pins_default);
		HDBG_LOG("Cannot find hall pinctrl default!\n");

	}

	pins_cfg = pinctrl_lookup_state(pinctrl, "pin_cfg");
	if (IS_ERR(pins_cfg)) {
		ret = PTR_ERR(pins_cfg);
		HDBG_LOG("Cannot find hall pinctrl pin_cfg!\n");\
	}

/* eint request */
	if (hall_data->irq_node) {
		of_property_read_u32_array(hall_data->irq_node, "debounce", ints, ARRAY_SIZE(ints));
		gpio_request(ints[0], "hall");
		gpio_set_debounce(ints[0], ints[1]);
		hall_data->pin_number = ints[0];
		hall_gpio = ints[0];
		pinctrl_select_state(pinctrl, pins_cfg);
		HDBG_LOG("ints[0] = %d, ints[1] = %d!!\n", ints[0], ints[1]);

		hall_data->irq = irq_of_parse_and_map(hall_data->irq_node, 0);
		HDBG_LOG("hall_data->irq = %d\n", hall_data->irq);
		if (!hall_data->irq) {
			HDBG_LOG("irq_of_parse_and_map fail!!\n");
			return -EINVAL;
		}
		
		if (gpio_get_value(hall_data->pin_number) == GPIO_IN_ZERO) {
			HDBG_LOG("hall_handler set int polarity  high");
    			 hall_state = HALL_CLOSE;   
		 	switch_set_state(&hall_data->sdev, HALL_CLOSE);
			if (request_irq(hall_data->irq, hall_eint_handler, IRQ_TYPE_LEVEL_HIGH, "HALL-eint", NULL)) {
				HDBG_LOG("IRQ LINE NOT AVAILABLE!!\n");
				return -EINVAL;
			}
		} else {
    			HDBG_LOG("hall_handler set int polarity  low");
			 hall_state = HALL_OPEN;   
			 switch_set_state(&hall_data->sdev, HALL_OPEN);
			 if (request_irq(hall_data->irq, hall_eint_handler, IRQ_TYPE_LEVEL_LOW, "HALL-eint", NULL)) {
				HDBG_LOG("IRQ LINE NOT AVAILABLE!!\n");
				return -EINVAL;
			}
		}
		
		//enable_irq(hall_data->irq);
	} else {
		HDBG_LOG("null irq node!!\n");
		return -EINVAL;
	}
#endif
	return ret;
}
/*--------------------------------------------------------------------------*/
static int hall_probe(struct platform_device *pdev)
{
	int err = 0;
	HDBG_FUNC();
#ifdef CONFIG_OF
	hallpdev = pdev;
#endif
	hall_data = kzalloc(sizeof(*hall_data), GFP_KERNEL);
	if (!hall_data) {
		err = -ENOMEM;
		goto err_kzalloc;
	}

//for input device
#ifndef USE_EXTERN_IDEV
	hall_data->idev = input_allocate_device();
	if (!hall_data->idev) {
		err =  -ENOMEM;
		HDBG_LOG("init input device failled!!!\n");
		goto err_input_allocate_device;
	}

	set_bit(EV_KEY, hall_data->idev->evbit);
	set_bit(EV_SYN, hall_data->idev->evbit);

	if (get_boot_mode() == FACTORY_BOOT) {
		input_set_capability(hall_data->idev, EV_KEY, KEY_F11);
		//keymap[8] = 0;
	} else {
		input_set_capability(hall_data->idev, EV_KEY, KEY_F11);
		input_set_capability(hall_data->idev, EV_KEY, KEY_F12);
		//keymap[8] = 0;
	}
	hall_data->idev->name = HALL_INPUT_NAME;
	hall_data->idev->dev.parent = &pdev->dev;
	if ((err = input_register_device(hall_data->idev))) {
		HDBG_LOG("failed to register input device!!!\n");
		goto err_input_register_device;
	}
#else
	if (get_boot_mode() == FACTORY_BOOT) {
		input_set_capability(kpd_input_dev, EV_KEY, KEY_F11);
		//keymap[8] = 0;
	} else {
		input_set_capability(kpd_input_dev, EV_KEY, KEY_F11);
		input_set_capability(kpd_input_dev, EV_KEY, KEY_F12);
		//keymap[8] = 0;
	}
#endif
//for misc device
#ifdef USE_MISCDEV
	hall_data->mdev = &hall_miscdev;
	err = misc_register(hall_data->mdev);
	if(err)
	{
		HDBG_LOG("hall_miscdev register failed\n");
		goto exit_misc_register;
	}
#endif
//for switch device
	hall_data->sdev.name = "hall"; // /sys/class/switch/hall/
	hall_data->sdev.index = 0;
	hall_data->sdev.state = HALL_OPEN;		
	if ((err = switch_dev_register(&hall_data->sdev))) {
		HDBG_LOG("failed to register switch device!!!\n");
		goto err_switch_dev_register;
	}

//for interrupt work
#ifdef USE_WORK_STRUCT
	hall_wq = create_singlethread_workqueue("hall_wq");
	if (!hall_wq) {    
		HDBG_LOG(KERN_ERR "failed to create hall workqueue!\n");
		err =  -ENOMEM;
		goto err_create_singlethread_workqueue;
	}   
	INIT_WORK(&hall_data->report, hall_eint_work); 
#endif

#ifdef CONFIG_OF
	hall_data->irq_node = of_find_compatible_node(NULL, NULL, "mediatek, hall_1-eint");
	if (!hall_data->irq_node) {
		HDBG_LOG("retrieve hall's irq_node failed!!!\n");
		goto err_request_irq;
	}
#endif

	hall_setup_eint();

//early suspend
#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(USE_EARLY_SUSPEND)
	//hall_data->early_drv.level    = EARLY_SUSPEND_LEVEL_DISABLE_FB - 1,
	//hall_data->early_drv.suspend  = hall_early_suspend,
	//hall_data->early_drv.resume   = hall_late_resume,
	//register_early_suspend(&hall_data->early_drv);
#endif

	HDBG_LOG("probe done!!!\n");
	return err;

#ifdef USE_WORK_STRUCT	
err_create_singlethread_workqueue:
#endif
err_request_irq:
	switch_dev_unregister(&hall_data->sdev);

err_switch_dev_register:
#ifdef USE_MISCDEV
exit_misc_register:
	misc_deregister(hall_data->mdev);
#endif
#ifndef USE_EXTERN_IDEV
	input_unregister_device(hall_data->idev);
err_input_register_device:
	input_free_device(hall_data->idev);
#endif
#ifndef USE_EXTERN_IDEV
err_input_allocate_device:
	kfree(hall_data);
#endif
err_kzalloc:
	return err;
}

/*-------------------------------------------------------------------------*/
/* should never be called */
static int hall_remove(struct platform_device *pdev)
{
	HDBG_FUNC();
#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(USE_EARLY_SUSPEND)
	unregister_early_suspend(&hall_data->early_drv);
#endif
	switch_dev_unregister(&hall_data->sdev);
#ifdef USE_MISCDEV
	misc_deregister(hall_data->mdev);
#endif
#ifndef USE_EXTERN_IDEV
	input_unregister_device(hall_data->idev);
#endif
	kfree(hall_data);

	return 0;
}
/*------------------------------------------------------------------------*/
#ifdef USE_MISCDEV
static int hall_open(struct inode *inode, struct file *file)
{
	file->private_data = hall_data;

	if (!file->private_data)
	{
		HDBG_LOG("null pointer!!\n");
		return -EINVAL;
	}
	
	return nonseekable_open(inode, file);
}

static int hall_release(struct inode *inode, struct file *file)
{
	file->private_data = NULL;
	return 0;
}

static long hall_unlocked_ioctl(struct file *file, unsigned int cmd,
       unsigned long arg)       
{ 
	long err = 0;
	//void __user *ptr = (void __user*) arg;
	switch (cmd) {
	default:
		break;
	}
	return err;
}

static struct file_operations hall_fops = {
	//.owner = THIS_MODULE,
	.open = hall_open,
	.release = hall_release,
	.unlocked_ioctl = hall_unlocked_ioctl,
};

static struct miscdevice hall_miscdev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "hall",
	.fops = &hall_fops,
};
#endif
/*------------------------------------------------------------------------*/

#if !defined(CONFIG_OF)
static struct platform_device hall_device = {
	.name = HALL_NAME,
	.id = -1,
};
#else
static const struct of_device_id of_match[] = {
	{ .compatible = "mediatek,hall", },
	{},
};
#endif
/*------------------------------------------------------------------------*/

static struct platform_driver hall_driver = {
	.probe = hall_probe,
	.remove = hall_remove,
#if !defined(CONFIG_HAS_EARLYSUSPEND) && !defined(USE_EARLY_SUSPEND)
	.suspend    = hall_suspend,
	.resume     = hall_resume,
#endif	
	.driver = {
		.name = HALL_NAME,
		.owner = THIS_MODULE,
#ifdef CONFIG_OF
		.of_match_table = of_match,
#endif
	},
};
/*------------------------------------------------------------------------*/

static int __init hall_init(void)
{
	int err = 0;
	HDBG_FUNC();
#if !defined(CONFIG_OF)
	err = platform_device_register(&hall_device);
	if (err) {
		HDBG_LOG("failed to register platform device!\n");
		goto err_platform_device_register;
	}
#endif
	err = platform_driver_register(&hall_driver);
	if (err) {
		HDBG_LOG("register platform driver failed!!!\n");
		goto err_platform_driver_register;
	}
	return err;
err_platform_driver_register:
#if !defined(CONFIG_OF)
	platform_device_unregister(&hall_device);
err_platform_device_register:
#endif
	return err;
}

/* should never be called */
static void __exit hall_exit(void)
{
	HDBG_FUNC();
}
late_initcall(hall_init);
module_exit(hall_exit);

module_param(hall_state, int, 0644);	//insmod kpd.ko 1
MODULE_AUTHOR("modify@zte.com.cn");
MODULE_DESCRIPTION("hall support for wind");
MODULE_LICENSE("GPL");
 //#modify@zte.com.cn add at 20160126 end