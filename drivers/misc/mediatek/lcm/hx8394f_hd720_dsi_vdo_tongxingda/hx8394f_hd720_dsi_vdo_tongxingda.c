/*modify@zte.com.cn 20160612 begin*/
/* BEGIN PN: , Added by guohongjin, 2014.08.13*/
//modify@zte.com.cn 201600606 begin
/*
#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
#endif
*/
//modify@zte.com.cn 201600606 end
#ifdef BUILD_LK
#include <platform/gpio_const.h>
#include <platform/mt_gpio.h>
#include <platform/upmu_common.h>
#else
//modify@zte.com.cn 201600606 begin
    #include <linux/string.h>
    #if defined(BUILD_UBOOT)
        #include <asm/arch/mt_gpio.h>
    #else
		#include <mt-plat/mt_gpio.h>
		#include <mach/gpio_const.h>

    #endif
//modify@zte.com.cn 201600606 begin
#endif


#include "lcm_drv.h"


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define LCM_DSI_CMD_MODE									0
#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)
#define LCM_ID_HX8394F                                                              (0x8394)
//const static unsigned char LCD_MODULE_ID = 0x05;  //modify@zte.com.cn 20160315

#define REGFLAG_DELAY             								0xFC
#define REGFLAG_END_OF_TABLE      							0xFD   // END OF REGISTERS MARKER

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

//modify@zte.com.cn 201600606 begin
#define GPIO_LCD_BIAS_ENP_PIN         (GPIO119 | 0x80000000)
//modify@zte.com.cn 201600606 end
static unsigned int lcm_esd_test = FALSE;      ///only for ESD test
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

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
#if defined(CONFIG_WIND_LCM_ROTATION180)//modify@zte.com.cn 20160815
//modify@zte.com.cn  at 20160822 begin
static struct LCM_setting_table lcm_initialization_setting1[] = 
{
	{0xB9,03,{0xFF,0x83,0x94}},
          
        {0xB1,10,{0x50,0x15,0x75,0x09,0x32,0x44,0x71,0x31,0x55,0x2F}},
          
	{0xBA,06,{0x63,0x03,0x68,0x6B,0xB2,0xC0}},
          
	{0xD2,01,{0x88}},
          
	{0xB2,05,{0x00,0x80,0x64,0x10,0x07}},
			  
	{0xB4, 21, {0x05,0x70,0x05,0x70,0x01,0x70,0x01,0x12,0x86,0x97,0x00,0x3F,0x01,0x74,0x01,0x74,0x01,0x74,0x01,0x0C,0x86}}, 
			  
	{0xD3, 33, {0x00,0x00,0x07,0x07,0x40,0x1E,0x08,0x00,0x32,0x10,0x08,0x00,0x08,0x54,0x15,0x10,0x05,0x04,0x02,0x12,0x10,0x05,0x07,0x33,0x34,0x0C,0x0C,0x37,0x10,0x07,0x17,0x11,0x40}},
			  
	{0xD5,44,{0x19,0x19,0x18,0x18,0x1B,0x1B,0x1A,0x1A,0x04,0x05,0x06,0x07,0x00,0x01,0x02,0x03,0x20,0x21,0x18,0x18,0x22,0x23,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}},
			  
	{0xD6,44,{0x18,0x18,0x19,0x19,0x1B,0x1B,0x1A,0x1A,0x03,0x02,0x01,0x00,0x07,0x06,0x05,0x04,0x23,0x22,0x18,0x18,0x21,0x20,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}},
			  
	//{0xE0,58,{0x00,0x01,0x05,0x07,0x08,0x0B,0x0C,0x09,0x12,0x20,0x2E,0x2C,0x36,0x47,0x4D,0x55,0x68,0x72,0x74,0x8B,0xA3,0x53,0x54,0x5B,0x61,0x67,0x70,0x7C,0x7F,0x00,0x01,0x05,0x07,0x08,0x0B,0x0C,0x09,0x12,0x20,0x2E,0x2C,0x36,0x47,0x4D,0x55,0x68,0x72,0x74,0x8B,0xA3,0x53,0x54,0x5B,0x61,0x67,0x70,0x7C,0x7F}},
	
    {0xE0,58,{0x00,0x05,0x0B,0x0F,0x11,0x13,0x16,0x13,0x27,0x36,0x46,0x46,0x50,0x63,0x6A,0x70,0x80,0x86,0x85,0x97,0xAA,0x54,0x54,0x5A,0x5F,0x62,0x6C,0x7C,0x7F,0x00,0x05,0x0B,0x0F,0x11,0x13,0x16,0x13,0x27,0x36,0x46,0x46,0x50,0x63,0x6A,0x70,0x80,0x86,0x85,0x97,0xAA,0x54,0x54,0x5A,0x5F,0x62,0x6C,0x7C,0x7F}},
		  
	{0xCC, 1, {0x07}}, 
			  
	{0xC0,02,{0x1F,0x31}},
			  
	{0xB6,02,{0x90,0x90}},
			  
	{0xD4,01,{0x02}},
			  
	{0xBD,01,{0x01}},
			  
	{0xB1,01,{0x00}},
          
	{0xBD,01,{0x00}},
			  
	{0xBF,07,{0x40,0x81,0x50,0x00,0x1A,0xFC,0x01}},
				  
	{0x11, 1,{0x00}},
	
	{REGFLAG_DELAY, 120, {0}},	

       {0xB2,12,{0x00,0x80,0x64,0x10,0x07,0x2F,0x00,0x00,0x00,0x00,0xC0,0x18}},
			  
	{0x29, 1,{0x00}},

        {REGFLAG_DELAY, 20, {0}},	
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}

};
//modify@zte.com.cn  at 20160822 end
#else
static struct LCM_setting_table lcm_initialization_setting1[] = 
{
	{0xB9,03,{0xFF,0x83,0x94}},
          
        {0xB1,10,{0x50,0x15,0x75,0x09,0x32,0x44,0x71,0x31,0x55,0x2F}},
          
	{0xBA,06,{0x63,0x03,0x68,0x6B,0xB2,0xC0}},
          
	{0xD2,01,{0x88}},
          
	{0xB2,05,{0x00,0x80,0x64,0x10,0x07}},
			  
	{0xB4,21,{0x01,0x74,0x01,0x74,0x01,0x74,0x01,0x0C,0x86,0x75,0x00,0x3F,0x01,0x74,0x01,0x74,0x01,0x74,0x01,0x0C,0x86}},
			  
	{0xD3,33,{0x00,0x00,0x07,0x07,0x40,0x1E,0x08,0x00,0x32,0x10,0x08,0x00,0x08,0x54,0x15,0x10,0x05,0x04,0x02,0x12,0x10,0x05,0x07,0x23,0x23,0x0C,0x0C,0x27,0x10,0x07,0x07,0x10,0x40}},
			  
	{0xD5,44,{0x19,0x19,0x18,0x18,0x1B,0x1B,0x1A,0x1A,0x04,0x05,0x06,0x07,0x00,0x01,0x02,0x03,0x20,0x21,0x18,0x18,0x22,0x23,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}},
			  
	{0xD6,44,{0x18,0x18,0x19,0x19,0x1B,0x1B,0x1A,0x1A,0x03,0x02,0x01,0x00,0x07,0x06,0x05,0x04,0x23,0x22,0x18,0x18,0x21,0x20,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18}},
			  
	//{0xE0,58,{0x00,0x01,0x05,0x07,0x08,0x0B,0x0C,0x09,0x12,0x20,0x2E,0x2C,0x36,0x47,0x4D,0x55,0x68,0x72,0x74,0x8B,0xA3,0x53,0x54,0x5B,0x61,0x67,0x70,0x7C,0x7F,0x00,0x01,0x05,0x07,0x08,0x0B,0x0C,0x09,0x12,0x20,0x2E,0x2C,0x36,0x47,0x4D,0x55,0x68,0x72,0x74,0x8B,0xA3,0x53,0x54,0x5B,0x61,0x67,0x70,0x7C,0x7F}},
	
    {0xE0,58,{0x00,0x05,0x0D,0x12,0x14,0x18,0x1B,0x19,0x35,0x45,0x56,0x56,0x60,0x73,0x7A,0x7F,0x90,0x93,0x90,0xA0,0xB1,0x58,0x57,0x5B,0x60,0x65,0x6D,0x7A,0x7F,0x00,0x05,0x0D,0x12,0x14,0x18,0x1B,0x19,0x35,0x45,0x56,0x56,0x60,0x73,0x7A,0x7F,0x90,0x93,0x90,0xA0,0xB1,0x58,0x57,0x5B,0x60,0x65,0x6D,0x7A,0x7F}},
		  
	{0xCC,01,{0x0b}},
			  
	{0xC0,02,{0x1F,0x31}},
			  
	{0xB6,02,{0x5D,0x5D}},
			  
	{0xD4,01,{0x02}},
			  
	{0xBD,01,{0x01}},
			  
	{0xB1,01,{0x00}},
          
	{0xBD,01,{0x00}},
			  
	{0xBF,07,{0x40,0x81,0x50,0x00,0x1A,0xFC,0x01}},
				  
	{0x11, 1,{0x00}},
	
	{REGFLAG_DELAY, 120, {0}},	

       {0xB2,12,{0x00,0x80,0x64,0x10,0x07,0x2F,0x00,0x00,0x00,0x00,0xC0,0x18}},
			  
	{0x29, 1,{0x00}},

        {REGFLAG_DELAY, 10, {0}},	
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}

};
#endif
//modify@zte.com.cn 201600606 begin
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
//modify@zte.com.cn 201600606 end
static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
    // Display off sequence
    // Sleep Mode On
    {0x10, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},
	//modify@zte.com.cn begin
   // {0xFF,	3,		{0x98,0x81,0x01}},
   // {0x58, 1, {0x01}},
//{REGFLAG_DELAY, 20, {}},
		//modify@zte.com.cn end
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};
//unsigned int vcomadj=0x58;
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
	/*			
			case  0xb6:
			table[i].para_list[0]=vcomadj;
			table[i].para_list[1]=vcomadj;
			dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
			vcomadj+=5;
			break;
*/
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
//		params->dsi.cont_clock=1;
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
		//modify@zte.com.cn at 20160624 begin
		params->dsi.vertical_sync_active				= 2;//4; //4   
		params->dsi.vertical_backporch				       = 16;//12;  //14  
		params->dsi.vertical_frontporch				       = 9;//16;  //16  
		params->dsi.vertical_active_line				       = FRAME_HEIGHT; 
//modify@zte.com.cn  at 20160822 beigin
#if defined(CONFIG_WIND_LCM_ROTATION180)
		params->dsi.horizontal_sync_active				= 60;   //4
		params->dsi.horizontal_backporch				= 76;  //60  
		params->dsi.horizontal_frontporch				= 76;    //60
#else
		//modify@zte.com.cn 201600612 begin		
		params->dsi.horizontal_sync_active				= 30;//85;  //16;   //4
		params->dsi.horizontal_backporch				= 75;//90;  //48;  //60  
		params->dsi.horizontal_frontporch				= 75;//90;  //16;    //60
		//modify@zte.com.cn 201600612 end
#endif
//modify@zte.com.cn  at 20160822 end
		//modify@zte.com.cn at 20160624 end
//		params->dsi.horizontal_blanking_pixel				= 60;   
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;  

	//	params->dsi.HS_TRAIL=14;
	//	params->dsi.pll_div1=1;		   
	//	params->dsi.pll_div2=1;		   
	//	params->dsi.fbk_div =28;//28	
//modify@zte.com.cn 20150327 beign
// To fix lcm rf
        params->dsi.PLL_CLOCK = 212;//232; //modify@zte.com.cn at 20160624 

//		params->dsi.CLK_TRAIL = 17;
	
	    params->dsi.cont_clock=1;    
	    params->dsi.esd_check_enable = 1;
	    params->dsi.customization_esd_check_enable      = 1;

	    params->dsi.lcm_esd_check_table[0].cmd          = 0xd9;
	    params->dsi.lcm_esd_check_table[0].count        = 1;
	    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x80;

	    params->dsi.lcm_esd_check_table[1].cmd          = 0x09;
	    params->dsi.lcm_esd_check_table[1].count        = 3;
	    params->dsi.lcm_esd_check_table[1].para_list[0] = 0x80;
	    params->dsi.lcm_esd_check_table[1].para_list[1] = 0x73;
	    params->dsi.lcm_esd_check_table[1].para_list[2] = 0x04;
		 
		params->dsi.lcm_esd_check_table[2].cmd          = 0x45;
        params->dsi.lcm_esd_check_table[2].count        = 2;
		//modify@zte.com.cn 201600612 begin
		params->dsi.lcm_esd_check_table[2].para_list[0] = 0x05;//0x0f;
		params->dsi.lcm_esd_check_table[2].para_list[1] = 0x19; // 0x1e;//0xf0 //modify@zte.com.cn at 20160624 
		//modify@zte.com.cn 201600612 end
}

static void lcm_init(void)
{
//modify@zte.com.cn 201600606 begin

#ifdef GPIO_LCD_BIAS_ENP_PIN
	mt_set_gpio_mode(GPIO_LCD_BIAS_ENP_PIN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_BIAS_ENP_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ONE);
#endif
//modify@zte.com.cn 201600606 end

	MDELAY(10);

	SET_RESET_PIN(1);
	MDELAY(1);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);
	printk(" gemingming hx8394f init  \n");
	push_table(lcm_initialization_setting1, sizeof(lcm_initialization_setting1) / sizeof(struct LCM_setting_table), 1); 

		  
}

static void lcm_suspend(void) 
{
	// when phone sleep , config output low, disable backlight drv chip  
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
		MDELAY(10);
//modify@zte.com.cn 201600606 begin

#ifdef GPIO_LCD_BIAS_ENP_PIN
	mt_set_gpio_mode(GPIO_LCD_BIAS_ENP_PIN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_BIAS_ENP_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ONE);
#endif
//modify@zte.com.cn 201600606 end

	MDELAY(10);

}

static void lcm_resume(void)
{
	lcm_init();

}

static unsigned int lcm_compare_id(void)
{
	unsigned int id=0,id1=0,id2=0;
	unsigned char buffer[3];
	unsigned int data_array[16]; 
	unsigned char LCD_ID_value = 0;
	//modify@zte.com.cn 201600606 begin
#ifdef GPIO_LCD_BIAS_ENP_PIN
	mt_set_gpio_mode(GPIO_LCD_BIAS_ENP_PIN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_BIAS_ENP_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ONE);
#endif
	//modify@zte.com.cn 201600606 end
	SET_RESET_PIN(1);  //NOTE:should reset LCM firstly
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(50); 

	data_array[0]=0x00043902;
	data_array[1]=0x9483FFB9;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(10);

	data_array[0]=0x00023902;
	data_array[1]=0x000013ba;
	dsi_set_cmdq(data_array, 2, 1);
	MDELAY(10);

	data_array[0] = 0x00023700;// return byte number
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(10);

	read_reg_v2(0xDA, buffer, 1);
	id1= buffer[0]; //should be 0x83
	read_reg_v2(0xDB, buffer, 1);
	id2= buffer[0]; //should be 0x94

	id=(id1 << 8) | id2;

#if defined(BUILD_LK)||defined(BUILD_UBOOT)
	printk(" hzs %s id=%x id1=%x id2=%x \n",__func__,id, id1, id2);
#endif	

	if(id == LCM_ID_HX8394F)
	{
		LCD_ID_value = which_lcd_module_triple();
		printk("LCD_ID_value = %d\n",LCD_ID_value);
		if(LCD_ID_value ==5 )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
    else
    {
        return 0;
    }
}
//static int err_count = 0;
static unsigned int lcm_esd_check(void)
{
  #ifndef BUILD_LK
    unsigned char buffer[8] = {0};

    unsigned int array[4];
    //int i =0;

	if(lcm_esd_test)
	{
		lcm_esd_test = FALSE;
		return TRUE;
	}

    array[0] = 0x00013700;    

    dsi_set_cmdq(array, 1,1);

    read_reg_v2(0x0A, buffer,8);

	printk( "hx8394f_tongxingda lcm_esd_check: buffer[0] = %d,buffer[1] = %d,buffer[2] = %d,buffer[3] = %d,buffer[4] = %d,buffer[5] = %d,buffer[6] = %d,buffer[7] = %d\n",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5],buffer[6],buffer[7]);

    if((buffer[0] != 0x9C))/*LCD work status error,need re-initalize*/

    {

        printk( "hx8394f_tongxingda lcm_esd_check buffer[0] = %d\n",buffer[0]);

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

                 printk( "hx8394f_tongxingda lcm_esd_check buffer[4] = %d , buffer[5] = %d\n",buffer[4],buffer[5]);

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
    printk( "hx8394f_tongxingda lcm_esd_recover\n");
    #endif
	lcm_init();
//	lcm_resume();

	return TRUE;
}

LCM_DRIVER hx8394f_hd720_dsi_vdo_tongxingda_lcm_drv =
{
	.name           	= "hx8394f_hd720_dsi_vdo_tongxingda",
	.set_util_funcs 	= lcm_set_util_funcs,
	.get_params     	= lcm_get_params,
	.init           	= lcm_init,
	.suspend        	= lcm_suspend,
	.resume         	= lcm_resume,
	.compare_id     	= lcm_compare_id,
	.esd_check = lcm_esd_check,
	.esd_recover = lcm_esd_recover,
};
//late_initcall(lcm_init);
/* END PN: , Added by guohongjin, 2014.08.13*/
//modified modify@zte.com.cn 201500824 end
/*modify@zte.com.cn 20160612 end*/
