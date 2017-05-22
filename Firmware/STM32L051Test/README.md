Software Structure:  
* /Menuitem/  
  declare struct mfa_t for each Menuitem and find a way in  
  in main.c, we set menu_l1_time as the default Menuitem.  
  each mfa_t should offer serval event callback function pointers,  
  -- 00.on init:  
 when power on  
  -- 01.on enter:  
 when entered from another Menuitem  
  -- 02.on exit:  
 when menu_enter is called  
  -- 03-06.on key:  
 when a key is pressed  
  -- 07.on accelerometer event:  
 when accelerometer interrupt is triggered  
  -- 08.loop:  
 after a key scan and screen refresh is completed, before the system enters a short sleep for power saving, this function is called.  
  -- 09-11.reserved  
    
  - menu_l1_time  
  Simple digital clock with date  
  - menu_l2_apps  
  "Apps" menu, central hub for other Menuitems  
  - menu_l3_RPN  
  RPN calculator, fully utilizes math.h ;D  
  - menu_l3_config  
  global configure menu, supports different range and postchange callback  
  
* /Src/  
 - HCMS29xx.c:  
   Low level drivers  
 - HCMS29xx_Util.c:  
   VirtualBuffer for shade and smooth scrolling  
   RPL fontset  
   StringBuilder  
 - l_simple_queue.c  
   Array based simple queue  
   supports any types of variable (uint8_t* + length)  
   
Todo:  
* Make neat, now the code is still a mess  
* Finish other Apps (especially, memo and calendar)  
* Reduce power consumption, although now the screen consums the most power(about 4mA)  
* Utilize tim22  
   
