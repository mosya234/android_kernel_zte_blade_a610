//modify@zte.com.cn 20140813 end
/* BEGIN PN: , Added by guohongjin, 2014.08.13*/
#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK

#include <platform/gpio_const.h>
#include <platform/mt_gpio.h>
#include <platform/upmu_common.h>
#include <cust_gpio_usage.h>
#else
#include <mach/gpio_const.h>
#include <mt-plat/mt_gpio.h>
//#include <cust_gpio_usage.h>
#include <linux/string.h>
#endif



// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define LCM_DSI_CMD_MODE									0
#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)
#define LCM_ID_ILI9881                                                              (0x9881)

#define REGFLAG_DELAY             								0xFC
#define REGFLAG_END_OF_TABLE      							0xFD   // END OF REGISTERS MARKER

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

//static unsigned int lcm_esd_test = FALSE;      ///only for ESD test
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------
#define GPIO_LCM_PWR_EN   (2 | 0x80000000)
static LCM_UTIL_FUNCS lcm_util;

#define __SAME_IC_COMPATIBLE__

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))
#define MDELAY(n) 											(lcm_util.mdelay(n))

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

 struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};
//modify@zte.com.cn modify at 20151204  begin
static struct LCM_setting_table lcm_initialization_setting1[] = {
	
	{0xFF,5,{0xAA,0x55,0xA5,0x80}},
{0x6F,2,{0x11,0x00}},
{0xF7,2,{0x20,0x00}},
{0x6F,1,{0x06}},
{0xF7,1,{0xA0}},
{0x6F,1,{0x19}},
{0xF7,1,{0x12}},
{0x6F,1,{0x10}},
{0xF5,1,{0x70}},
	{0xF0,5,{0x55,0xAA,0x52,0x08,0x00}},
	{0xC8,1,{0x80}},
{0xB1,2,{0x60,0x23}},
{0xB6,1,{0x0F}},
{0xBC,2,{0x00,0x00}},
{0xBD,5,{0x02,0xB0,0x1E,0x1E,0x00}},
	{0xF0,5,{0x55,0xAA,0x52,0x08,0x01}},
{0xBC,2,{0x78,0x00}},
{0xBD,2,{0x78,0x00}},
	{0xCA,1,{0x01}},
	{0xC0,1,{0x0C}},
{0xBE,1,{0x4E}},  
	{0xB3,2,{0x38,0x38}},
	{0xB4,2,{0x11,0x11}},
	{0xB6,2,{0x05,0x05}},
{0xB9,2,{0x35,0x35}},
{0xBA,2,{0x14,0x14}},
	{0xC4,2,{0x11,0x11}},
	{0xCE,1,{0x66}},
	{0xF0,5,{0x55,0xAA,0x52,0x08,0x02}},
{0xEE,1,{0x01}},     //解决弹窗偏红问题 
{0xB0,16,{0x00,0x00,0x00,0x16,0x00,0x3A,0x00,0x57,0x00,0x6C,0x00,0x92,0x00,0xB2,0x00,0xE2}},
{0xB1,16,{0x01,0x09,0x01,0x47,0x01,0x78,0x01,0xC5,0x02,0x03,0x02,0x05,0x02,0x40,0x02,0x80}},
{0xB2,16,{0x02,0xA7,0x02,0xDC,0x03,0x00,0x03,0x30,0x03,0x50,0x03,0x7B,0x03,0x96,0x03,0xBE}},
{0xB3,4,{0x03,0xF3,0x03,0xFF}},	
	{0xF0,5,{0x55,0xAA,0x52,0x08,0x06}},
	{0xB0,2,{0x29,0x2A}},
	{0xB1,2,{0x10,0x12}},
	{0xB2,2,{0x14,0x16}},
	{0xB3,2,{0x18,0x1A}},
	{0xB4,2,{0x02,0x04}},
	{0xB5,2,{0x34,0x34}},
	{0xB6,2,{0x34,0x2E}},
	{0xB7,2,{0x2E,0x2E}},
	{0xB8,2,{0x34,0x00}},
	{0xB9,2,{0x34,0x34}},
	{0xBA,2,{0x34,0x34}},
	{0xBB,2,{0x01,0x34}},
	{0xBC,2,{0x2E,0x2E}},
	{0xBD,2,{0x2E,0x34}},
	{0xBE,2,{0x34,0x34}},
	{0xBF,2,{0x05,0x03}},
	{0xC0,2,{0x1B,0x19}},
	{0xC1,2,{0x17,0x15}},
	{0xC2,2,{0x13,0x11}},
	{0xC3,2,{0x2A,0x29}},
	{0xE5,2,{0x2E,0x2E}},
	{0xC4,2,{0x29,0x2A}},
	{0xC5,2,{0x1B,0x19}},
	{0xC6,2,{0x17,0x15}},
	{0xC7,2,{0x13,0x11}},
	{0xC8,2,{0x01,0x05}},
	{0xC9,2,{0x34,0x34}},
	{0xCA,2,{0x34,0x2E}},
	{0xCB,2,{0x2E,0x2E}},
	{0xCC,2,{0x34,0x03}},
	{0xCD,2,{0x34,0x34}},
	{0xCE,2,{0x34,0x34}},
	{0xCF,2,{0x02,0x34}},
	{0xD0,2,{0x2E,0x2E}},
	{0xD1,2,{0x2E,0x34}},
	{0xD2,2,{0x34,0x34}},
	{0xD3,2,{0x04,0x00}},
	{0xD4,2,{0x10,0x12}},
	{0xD5,2,{0x14,0x16}},
	{0xD6,2,{0x18,0x1A}},
	{0xD7,2,{0x2A,0x29}},
	{0xE6,2,{0x2E,0x2E}},
	{0xD8,5,{0x00,0x00,0x00,0x54,0x00}},
	{0xD9,5,{0x00,0x15,0x00,0x00,0x00}},
	{0xE7,1,{0x00}},
	{0xF0,5,{0x55,0xAA,0x52,0x08,0x03}},
	{0xB1,2,{0x00,0x00}},
	{0xB0,2,{0x00,0x00}},
	{0xB2,5,{0x05,0x00,0x00,0x00,0x00}},
	{0xB3,5,{0x05,0x00,0x00,0x00,0x00}},
	{0xB4,5,{0x05,0x00,0x00,0x00,0x00}},
	{0xB5,5,{0x05,0x00,0x17,0x00,0x00}},
	{0xB6,5,{0x12,0x00,0x19,0x00,0x00}},
	{0xB7,5,{0x12,0x00,0x19,0x00,0x00}},
	{0xB8,5,{0x12,0x00,0x19,0x00,0x00}},
	{0xB9,5,{0x12,0x00,0x19,0x00,0x00}},
	{0xBA,5,{0x57,0x00,0x00,0x00,0x00}},
	{0xBB,5,{0x57,0x00,0x00,0x00,0x00}},
	{0xBC,5,{0x75,0x00,0x1A,0x00,0x00}},
	{0xBD,5,{0x53,0x00,0x1A,0x00,0x00}},
	{0xC0,4,{0x00,0x34,0x00,0x00}},
	{0xC1,4,{0x00,0x34,0x00,0x00}},
	{0xC2,4,{0x00,0x34,0x00,0x00}},
	{0xC3,4,{0x00,0x34,0x00,0x00}},
	{0xC4,1,{0x20}},
	{0xC5,1,{0x00}},
	{0xC6,1,{0x00}},
	{0xC7,1,{0x00}},
	{0xF0,5,{0x55,0xAA,0x52,0x08,0x05}},
	{0xED,1,{0x30}},
	{0xB0,2,{0x17,0x06}},
	{0xB8,1,{0x08}},
	{0xBD,5,{0x03,0x07,0x00,0x03,0x00}},
	{0xB1,2,{0x17,0x06}},
	{0xB9,1,{0x00}},
	{0xB2,2,{0x00,0x00}},
	{0xBA,1,{0x00}},
	{0xB3,2,{0x17,0x06}},
	{0xBB,1,{0x0A}},
	{0xB4,2,{0x17,0x06}},
	{0xB5,2,{0x17,0x06}},
	{0xB6,2,{0x14,0x03}},
	{0xB7,2,{0x00,0x00}},
	{0xBC,1,{0x02}},
	{0xE5,1,{0x06}},
	{0xE6,1,{0x06}},
	{0xE7,1,{0x00}},
	{0xE8,1,{0x06}},
	{0xE9,1,{0x06}},
	{0xEA,1,{0x06}},
	{0xEB,1,{0x00}},
	{0xEC,1,{0x00}},
	{0xC0,1,{0x07}},
	{0xC1,1,{0x80}},
	{0xC2,1,{0xA4}},
	{0xC3,1,{0x05}},
	{0xC4,1,{0x00}},
	{0xC5,1,{0x02}},
	{0xC6,1,{0x22}},
	{0xC7,1,{0x03}},
	{0xC8,2,{0x05,0x30}},
	{0xC9,2,{0x01,0x31}},
	{0xCA,2,{0x03,0x21}},
	{0xCB,2,{0x01,0x20}},
	{0xD1,5,{0x00,0x05,0x09,0x07,0x10}},
	{0xD2,5,{0x10,0x05,0x0E,0x03,0x10}},
	{0xD3,5,{0x20,0x00,0x48,0x07,0x10}},
	{0xD4,5,{0x30,0x00,0x43,0x07,0x10}},
	{0xD0,1,{0x00}},
	{0xCC,3,{0x00,0x00,0x3E}},
	{0xCD,3,{0x00,0x00,0x3E}},
	{0xCE,3,{0x00,0x00,0x02}},
	{0xCF,3,{0x00,0x00,0x02}},
	{0x6F,1,{0x11}},
	{0xF3,1,{0x01}},


	{0x11,1,{0x00}},              
{0x36,1,{0x40}},          
	{REGFLAG_DELAY, 120, {}},
	{0x29,1,{0x00}},         
	{REGFLAG_DELAY, 20, {}},           
	{REGFLAG_END_OF_TABLE, 0x00, {}},
};
//modify@zte.com.cn modify at 20151204  end
#if 0
static struct LCM_setting_table lcm_sleep_out_setting[] = {
    //Sleep Out
    {0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
    {0x29, 1, {0x00}},
    {REGFLAG_DELAY, 20, {}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif
static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
    // Display off sequence
    {0x28, 1, {0x00}},
    {REGFLAG_DELAY, 10, {}},

    // Sleep Mode On
    {0x10, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    {REGFLAG_END_OF_TABLE, 0x00, {}}
	
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++)
    {
        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;

            default:
                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
        }
    }
}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{

		memset(params, 0, sizeof(LCM_PARAMS));
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		params->dbi.te_mode				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		//LCM_DBI_TE_MODE_DISABLED;
		//LCM_DBI_TE_MODE_VSYNC_ONLY;  
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING; 
		/////////////////////   
		//if(params->dsi.lcm_int_te_monitor)  
		//params->dsi.vertical_frontporch *=2;  
		//params->dsi.lcm_ext_te_monitor= 0;//TRUE; 
	//	params->dsi.noncont_clock= TRUE;//FALSE;   
	//	params->dsi.noncont_clock_period=2;
		params->dsi.cont_clock=1;    //modify  yudengwu
		////////////////////          
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;  
		// DSI    /* Command mode setting */  
		params->dsi.LANE_NUM				= LCM_FOUR_LANE;      
		//The following defined the fomat for data coming from LCD engine.  
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;   
		params->dsi.data_format.trans_seq	= LCM_DSI_TRANS_SEQ_MSB_FIRST; 
		params->dsi.data_format.padding 	= LCM_DSI_PADDING_ON_LSB;    
		params->dsi.data_format.format	  = LCM_DSI_FORMAT_RGB888;       
		// Video mode setting		   
		params->dsi.intermediat_buffer_num = 2;  
		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;  
		params->dsi.packet_size=256;    
		// params->dsi.word_count=480*3;	
		//DSI CMD mode need set these two bellow params, different to 6577   
		// params->dsi.vertical_active_line=800;   
		params->dsi.vertical_sync_active				= 10; //4   
		params->dsi.vertical_backporch				       = 30;  //14  
		params->dsi.vertical_frontporch				       = 30;  //16  
		params->dsi.vertical_active_line				       = FRAME_HEIGHT;     
		params->dsi.horizontal_sync_active				= 20;   //4
		params->dsi.horizontal_backporch				= 80;  //60  
		params->dsi.horizontal_frontporch				= 80;    //60
		params->dsi.horizontal_blanking_pixel				= 60;   
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;  		
		
	//	params->dsi.pll_div1=1;		   
	//	params->dsi.pll_div2=1;		   
	//	params->dsi.fbk_div =28;//28	
//modify@zte.com.cn 20150327 beign
// To fix rf
		params->dsi.PLL_CLOCK = 212;	   // 245;

	//	params->dsi.ss

//		params->dsi.CLK_TRAIL = 17;
			
//modify@zte.com.cn 20140820 end
		params->dsi.esd_check_enable = 1;
                params->dsi.customization_esd_check_enable = 1;
                params->dsi.lcm_esd_check_table[0].cmd                  = 0x0a;
                params->dsi.lcm_esd_check_table[0].count                = 1;
                params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
	//modify@zte.com.cn modify at 20150923  begin			
		  params->dsi.lcm_esd_check_table[1].cmd                  = 0x0d;
                params->dsi.lcm_esd_check_table[1].count                = 1;
                params->dsi.lcm_esd_check_table[1].para_list[0] = 0x00;
   //modify@zte.com.cn modify at 20150923  end
}
	//modify@zte.com.cn begin
//#ifndef BUILD_LK

#if 0
#include <linux/kernel.h>
#include <linux/module.h>  
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/irq.h>
//#include <linux/jiffies.h>
#include <linux/uaccess.h>
//#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
/***************************************************************************** 
 * Define
 *****************************************************************************/
//#ifndef FPGA_EARLY_PORTING
#define TPS_I2C_BUSNUM  0//for I2C channel 0
#define I2C_ID_NAME "ili9881"
#define TPS_ADDR 0x3E
/***************************************************************************** 
 * GLobal Variable
 *****************************************************************************/
static struct i2c_board_info __initdata ili9881_board_info = {I2C_BOARD_INFO(I2C_ID_NAME, TPS_ADDR)};
static struct i2c_client *ili9881_i2c_client = NULL;


/***************************************************************************** 
 * Function Prototype
 *****************************************************************************/ 
static int ili9881_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int ili9881_remove(struct i2c_client *client);
/***************************************************************************** 
 * Data Structure
 *****************************************************************************/

 struct ili9881_dev	{	
	struct i2c_client	*client;
	
};

static const struct i2c_device_id ili9881_id[] = {
	{ I2C_ID_NAME, 0 },
	{ }
};

//#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
//static struct i2c_client_address_data addr_data = { .forces = forces,};
//#endif
static struct i2c_driver ili9881_iic_driver = {
	.id_table	= ili9881_id,
	.probe		= ili9881_probe,
	.remove		= ili9881_remove,
	//.detect		= mt6605_detect,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "ili9881",
	},
 
};
/***************************************************************************** 
 * Extern Area
 *****************************************************************************/ 
 
 

/***************************************************************************** 
 * Function
 *****************************************************************************/ 
static int ili9881_probe(struct i2c_client *client, const struct i2c_device_id *id)
{  
	printk( "ili9881_iic_probe\n");
	printk("TPS: info==>name=%s addr=0x%x\n",client->name,client->addr);
	ili9881_i2c_client  = client;		
	return 0;      
}


static int ili9881_remove(struct i2c_client *client)
{  	
  printk( "ili9881_remove\n");
  ili9881_i2c_client = NULL;
   i2c_unregister_device(client);
  return 0;
}


 static int ili9881_write_bytes(unsigned char addr, unsigned char value)
{	
	int ret = 0;
	struct i2c_client *client = ili9881_i2c_client;
	char write_data[2]={0};	
	write_data[0]= addr;
	write_data[1] = value;
    ret=i2c_master_send(client, write_data, 2);
	if(ret<0)
	printk("ili9881 write data fail !!\n");	
	return ret ;
}



/*
 * module load/unload record keeping
 */

static int __init ili9881_iic_init(void)
{

   printk( "ili9881_iic_init\n");
   i2c_register_board_info(TPS_I2C_BUSNUM, &ili9881_board_info, 1);
   printk( "ili9881_iic_init2\n");
   i2c_add_driver(&ili9881_iic_driver);
   printk( "ili9881_iic_init success\n");	
   return 0;
}

static void __exit ili9881_iic_exit(void)
{
  printk( "ili9881_iic_exit\n");
  i2c_del_driver(&ili9881_iic_driver);  
}


module_init(ili9881_iic_init);
module_exit(ili9881_iic_exit);

MODULE_AUTHOR("Xiaokuan Shi");
MODULE_DESCRIPTION("MTK ili9881 I2C Driver");
MODULE_LICENSE("GPL"); 
//#endif
#endif

#if 0

#define ili9881_I2C_ID	I2C0
static struct mt_i2c_t ili9881_i2c;
#define ili9881_SLAVE_ADDR_WRITE   0x7c
#define ili9881_SLAVE_ADDR_READ    0x7d
#include <platform/mt_i2c.h>
/*
kal_uint32 ili9881_write_byte(kal_uint8 addr, kal_uint8 value)
{
    kal_uint32 ret_code = I2C_OK;
    kal_uint8 write_data[2];
    kal_uint16 len;

    write_data[0]= addr;
    write_data[1] = value;

    ili9881_i2c.id = ili9881_I2C_ID;
    // Since i2c will left shift 1 bit, we need to set ili9881 I2C address to >>1 
    ili9881_i2c.addr = (ili9881_SLAVE_ADDR_WRITE >> 1);
    ili9881_i2c.mode = ST_MODE;
    ili9881_i2c.speed = 100;
    len = 2;

    ret_code = i2c_write(&ili9881_i2c, write_data, len);

    if(I2C_OK != ret_code)
        dprintf(INFO, "%s: i2c_write: ret_code: %d\n", __func__, ret_code);

    return ret_code;
}
*/
kal_uint32 ili9881_read_byte (kal_uint8 addr, kal_uint8 *dataBuffer) 
{
    kal_uint32 ret_code = I2C_OK;
    kal_uint16 len;
    *dataBuffer = addr;

    ili9881_i2c.id = ili9881_I2C_ID;
    /* Since i2c will left shift 1 bit, we need to set ili9881 I2C address to >>1 */
    ili9881_i2c.addr = (ili9881_SLAVE_ADDR_READ >> 1);
    ili9881_i2c.mode = ST_MODE;
    ili9881_i2c.speed = 100;
    len = 1;

    ret_code = i2c_write_read(&ili9881_i2c, dataBuffer, len, len);

    if(I2C_OK != ret_code)
        dprintf(INFO, "%s: i2c_read: ret_code: %d\n", __func__, ret_code);

    return ret_code;
}
#endif

#if 0
//#ifndef FPGA_EARLY_PORTING
#define ili9881_SLAVE_ADDR_WRITE  0x7C  
static struct mt_i2c_t ili9881_i2c;

static int ili9881_write_byte(kal_uint8 addr, kal_uint8 value)
{
    kal_uint32 ret_code = I2C_OK;
    kal_uint8 write_data[2];
    kal_uint16 len;

    write_data[0]= addr;
    write_data[1] = value;

    ili9881_i2c.id = 0;//I2C2;
    /* Since i2c will left shift 1 bit, we need to set FAN5405 I2C address to >>1 */
    ili9881_i2c.addr = (ili9881_SLAVE_ADDR_WRITE >> 1);
    ili9881_i2c.mode = ST_MODE;
    ili9881_i2c.speed = 100;
    len = 2;

    ret_code = i2c_write(&ili9881_i2c, write_data, len);
    //printf("%s: i2c_write: ret_code: %d\n", __func__, ret_code);

    return ret_code;
}

#else
  
//	extern int mt8193_i2c_write(u16 addr, u32 data);
//	extern int mt8193_i2c_read(u16 addr, u32 *data);
	
//	#define ili9881_write_byte(add, data)  mt8193_i2c_write(add, data)
	//#define ili9881_read_byte(add)  mt8193_i2c_read(add)
  
//#endif
#endif

	//modify@zte.com.cn end
static void lcm_init(void)
{
	mt_set_gpio_mode(GPIO_LCM_PWR_EN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCM_PWR_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCM_PWR_EN, GPIO_OUT_ONE);
	MDELAY(10);
	/*
             MDELAY(50);
             SET_RESET_PIN(1);
		MDELAY(10);
		SET_RESET_PIN(0);
		MDELAY(20); 
		SET_RESET_PIN(1);
		MDELAY(120); 
*/
/*
#ifdef BUILD_LK
	ret=ili9881_write_byte(cmd,data);
    if(ret)    	
    dprintf(0, "[LK]ili9881----tps6132----cmd=%0x--i2c write error----\n",cmd);    	
	else
	dprintf(0, "[LK]ili9881----tps6132----cmd=%0x--i2c write success----\n",cmd);    		
#else
	ret=ili9881_write_bytes(cmd,data);
	if(ret<0)
	printk("[KERNEL]ili9881----tps6132---cmd=%0x-- i2c write error-----\n",cmd);
	else
	printk("[KERNEL]ili9881----tps6132---cmd=%0x-- i2c write success-----\n",cmd);
#endif
	//modify@zte.com.cn end
*/
	SET_RESET_PIN(1);
	MDELAY(1);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);
	
	      push_table(lcm_initialization_setting1, sizeof(lcm_initialization_setting1) / sizeof(struct LCM_setting_table), 1); 

}

static void lcm_suspend(void) 
{
     
	// when phone sleep , config output low, disable backlight drv chip  
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
	//modify@zte.com.cn add this at 20150805 begin
	mt_set_gpio_mode(GPIO_LCM_PWR_EN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCM_PWR_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCM_PWR_EN, GPIO_OUT_ZERO);
	MDELAY(10);
	//modify@zte.com.cn add this at 20150805 end

}

static void lcm_resume(void)
{
	lcm_init();

}

static unsigned int lcm_compare_id(void)
{
	
	unsigned char buffer[3];
	unsigned int array[16];
	unsigned int data_array[16];
	//modify@zte.com.cn add this at 20150805 begin
	mt_set_gpio_mode(GPIO_LCM_PWR_EN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCM_PWR_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCM_PWR_EN, GPIO_OUT_ONE);
	MDELAY(10);
  //modify@zte.com.cn add this at 20150805 end
	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(1);
	
	SET_RESET_PIN(1);
	MDELAY(120); 

	data_array[0] = 0x00063902;
	data_array[1] = 0x52AA55F0;
	data_array[2] = 0x00000108;
	
	dsi_set_cmdq(data_array, 3, 1);
	
	
	array[0] = 0x00033700;
	
	dsi_set_cmdq(array, 1, 1);
	
	read_reg_v2(0xC5, buffer, 3);
	
	printk("darren,nt35521 read id buffer[0] = 0x%x,buffer[1] = 0x%x,buffer[2] = 0x%x\n",buffer[0],buffer[1],buffer[2]);

	if(buffer[0] == 0x55 && buffer[1] == 0x21)
    	return 1;
    else
        return 0;
}
#if 0
static int err_count = 0;
static unsigned int lcm_esd_check(void)
{
  #ifndef BUILD_LK
    unsigned char buffer[8] = {0};

    unsigned int array[4];
    int i =0;

	if(lcm_esd_test)
	{
		lcm_esd_test = FALSE;
		return TRUE;
	}

    array[0] = 0x00013700;    

    dsi_set_cmdq(array, 1,1);

    read_reg_v2(0x0A, buffer,8);

	printk( "ili9881 lcm_esd_check: buffer[0] = %d,buffer[1] = %d,buffer[2] = %d,buffer[3] = %d,buffer[4] = %d,buffer[5] = %d,buffer[6] = %d,buffer[7] = %d\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7]);

    if((buffer[0] != 0x9C))/*LCD work status error,need re-initalize*/

    {

        printk( "ili9881 lcm_esd_check buffer[0] = %d\n",buffer[0]);

        return TRUE;

    }

    else

    {

#if 0
        if(buffer[3] != 0x02) //error data type is 0x02

        {
		//  is not 02, 
             err_count = 0;

        }

        else

        {
		// is 02, so ,
             if((buffer[4] == 0x40) || (buffer[5] == 0x80))
             {
			 // buffer[4] is not 0, || (huo),buffer[5] is 0x80.
			   err_count = 0;
             }
             else

             {
			// is  0,0x80,  
			   err_count++;
             }             

             if(err_count >=2 )
             {
			
                 err_count = 0;

                 printk( "ili9881 lcm_esd_check buffer[4] = %d , buffer[5] = %d\n",buffer[4],buffer[5]);

                 return TRUE;

             }

        }
#endif
        return FALSE;

    }
#endif
	
}
static unsigned int lcm_esd_recover(void)
{
    #ifndef BUILD_LK
    printk( "ili9881 lcm_esd_recover\n");
    #endif
	lcm_init();
//	lcm_resume();

	return TRUE;
}
#endif
//modify@zte.com.cn modify at 20150921 begin
#ifdef CONFIG_WIND_LCD_POWER_SUPPLY_SUPPORT
extern void lcm_init_power(void);
extern void lcm_resume_power(void);
extern void lcm_suspend_power(void);
#endif
LCM_DRIVER nt35521s_hd720_dsi_vdo_txd_lcm_drv =
{
    .name           	= "nt35521s_hd720_dsi_vdo_txd",
    .set_util_funcs 	= lcm_set_util_funcs,
    .get_params     	= lcm_get_params,
    .init           	= lcm_init,
    .suspend        	= lcm_suspend,
    .resume         	= lcm_resume,
    .compare_id     	= lcm_compare_id,
//	.esd_check = lcm_esd_check,
//	.esd_recover = lcm_esd_recover,
#ifdef CONFIG_WIND_LCD_POWER_SUPPLY_SUPPORT
	.init_power		= lcm_init_power,
	.resume_power   = lcm_resume_power,
	.suspend_power  = lcm_suspend_power,
#endif    
};
//modify@zte.com.cn modify at 20150921 end
//late_initcall(lcm_init);
/* END PN: , Added by guohongjin, 2014.08.13*/
//modify@zte.com.cn 20140813 end
