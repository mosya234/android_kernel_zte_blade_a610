//modified modify@zte.com.cn 201500824 begin
/* BEGIN PN: , Added by guohongjin, 2014.08.13*/
#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK

#include <platform/gpio_const.h>
#include <platform/mt_gpio.h>
#include <platform/upmu_common.h>
#else
#include <mach/gpio_const.h>
#include <mt-plat/mt_gpio.h>
//#include <mach/mt_pm_ldo.h>
#include <linux/string.h>
#endif


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define LCM_DSI_CMD_MODE									0
#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)
#define LCM_ID_S6D7AAXO62                                                              (0xaa)

#define REGFLAG_DELAY             								0xFC
#define REGFLAG_END_OF_TABLE      							0xFD   // END OF REGISTERS MARKER

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif
#define GPIO_LCM_PWR_EN   (119 | 0x80000000)
//static unsigned int lcm_esd_test = FALSE;      ///only for ESD test
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


static struct LCM_setting_table lcm_initialization_setting[] = {
	{0xF0, 2 , {0x5A,0x5A}},
	{0xF1,	2 ,{0x5A,0x5A}},
	//{0xFC,	2 ,{0xA5,0xA5}},

	{0xB1,	1 ,{0x12}},
	
	{0xB2, 4 , {0x14,0x22,0x2F,0x04}},
	{0xB3,	1 ,{0x11}},
		
	{0xBC, 3 , {0x01,0x4E,0x0B}}, //pwm
	{0xEE,	8,{0x88,0x08,0x9E,0x00,0x88,0x08,0x9E,0x00}},
	
	{0xEF,	17 ,{0x45,0x23,0xFE,0xFE,0x60,0x01,0x24,0x80,0x80,80,0x00,0x00,0x00,0x44,0x80,0x82,0x02}},
	
	{0xF2, 5 , {0x02,0x10,0x10,0x40,0x10}},
	{0xF5,	11 ,{0x7E,0x43,0x43,0x58,0x3B,0x8C,0x52,0x0F,0x33,0x44,0x04}},
	
	{0xF6, 6 , {0x20,0x25,0x15,0x00,0x00,0x00}},
	{0xF7,	32 ,{0x10,0x01,0x1B,0x1A,0x0C,0x08,0x01,0x01,0x01,0x04,0x06,0x0D,0x01,0x01,0x01,0x09,0x11,0x01,0x1B,0x1A,0x0E,0x0A,0x01,0x01,0x01,0x05,0x07,0x0F,0x01,0x01,0x01,0x0B}},
	{0xFA, 17 , {0x10,0x30,0x16,0x1C,0x12,0x17,0x1C,0x1B,0x1C,0x25,0x26,0x25,0x25,0x23,0x21,0x1F,0x25}},
	{0xFB, 17 , {0x10,0x30,0x16,0x1C,0x12,0x17,0x1C,0x1B,0x1C,0x25,0x26,0x25,0x25,0x23,0x21,0x1F,0x25}},
		
	{0x11, 1, {0x00}},
  {REGFLAG_DELAY, 120, {}},

    // Display ON
  {0x29, 1, {0x00}},
  {REGFLAG_DELAY, 20, {}},
  {REGFLAG_END_OF_TABLE, 0x00, {}}
};

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
    // Sleep Mode On
    {0x28, 1, {0x00}},
    {REGFLAG_DELAY, 40, {}},
    
    {0x10, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},
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
		params->dsi.vertical_sync_active				= 4; //4   
		params->dsi.vertical_backporch				       = 12;  //14  
		params->dsi.vertical_frontporch				       = 16;  //16  
		params->dsi.vertical_active_line				       = FRAME_HEIGHT;     
		params->dsi.horizontal_sync_active				= 16;   //4
		params->dsi.horizontal_backporch				= 48;  //60  
		params->dsi.horizontal_frontporch				= 16;    //60
//		params->dsi.horizontal_blanking_pixel				= 60;   
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;  

	//	params->dsi.HS_TRAIL=14;
	//	params->dsi.pll_div1=1;		   
	//	params->dsi.pll_div2=1;		   
	//	params->dsi.fbk_div =28;//28	
//modify@zte.com.cn 20150327 beign
// To fix lcm rf
        params->dsi.PLL_CLOCK = 208;

//		params->dsi.CLK_TRAIL = 17;
	
	    params->dsi.cont_clock=1;    //modify@zte.com.cn modify at 20160425 
	    params->dsi.esd_check_enable = 1;
	    params->dsi.customization_esd_check_enable      = 1;

	    params->dsi.lcm_esd_check_table[0].cmd          = 0x0a;
	    params->dsi.lcm_esd_check_table[0].count        = 1;
	    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;

/*	    params->dsi.lcm_esd_check_table[1].cmd          = 0x09;
	    params->dsi.lcm_esd_check_table[1].count        = 3;
	    params->dsi.lcm_esd_check_table[1].para_list[0] = 0x80;
	    params->dsi.lcm_esd_check_table[1].para_list[1] = 0x73;
	    params->dsi.lcm_esd_check_table[1].para_list[2] = 0x04;*/
		
/*		params->dsi.lcm_esd_check_table[2].cmd          = 0x45;
		params->dsi.lcm_esd_check_table[2].count        = 2;
		params->dsi.lcm_esd_check_table[2].para_list[0] = 0x0f;
		params->dsi.lcm_esd_check_table[2].para_list[1] = 0xf0;	*/
	
}

static void lcm_init(void)
{
    mt_set_gpio_mode(GPIO_LCM_PWR_EN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCM_PWR_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCM_PWR_EN, GPIO_OUT_ONE);
	MDELAY(10);

	SET_RESET_PIN(1);
	MDELAY(1);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);  
}

static void lcm_suspend(void) 
{
	// when phone sleep , config output low, disable backlight drv chip  
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);

	mt_set_gpio_mode(GPIO_LCM_PWR_EN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCM_PWR_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCM_PWR_EN, GPIO_OUT_ZERO);
	MDELAY(10);

}

static void lcm_resume(void)
{
	lcm_init();

}

static unsigned int lcm_compare_id(void)
{
	unsigned int id = 0;
	unsigned char buffer[3];
	unsigned int data_array[16];  

	mt_set_gpio_mode(GPIO_LCM_PWR_EN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCM_PWR_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCM_PWR_EN, GPIO_OUT_ONE);

	SET_RESET_PIN(1);  //NOTE:should reset LCM firstly
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(50); 

	data_array[0] = 0x00023700;// read id return two byte,version and id
	dsi_set_cmdq(data_array, 1, 1);
	read_reg_v2(0xda, buffer, 1); //IC id:0XA1
	id = buffer[0]; //we only need ID

#if defined(BUILD_LK)||defined(BUILD_UBOOT)
	printf(" hzs %s id=%x id1=%x id2=%x \n",__func__,id, id1, id2);
#else
	printk("dzl------------- %s id=%x  \n",__func__,id);
#endif	

	if(LCM_ID_S6D7AAXO62==id)
		return 1;
	else
		return 0;
}

#if 0
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
LCM_DRIVER s6d7aaxo62_hd720_dsi_vdo_tianma_lcm_drv =
{
	.name           	= "s6d7aaxo62_hd720_dsi_vdo_tianma",
	.set_util_funcs 	= lcm_set_util_funcs,
	.get_params     	= lcm_get_params,
	.init           	= lcm_init,
	.suspend        	= lcm_suspend,
	.resume         	= lcm_resume,
	.compare_id     	= lcm_compare_id,
//	.esd_check = lcm_esd_check,
//	.esd_recover = lcm_esd_recover,
};
//late_initcall(lcm_init);
/* END PN: , Added by guohongjin, 2014.08.13*/
//modified modify@zte.com.cn 201500824 end
