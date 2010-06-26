/******************************************************************************/
/*                                                                            */
/* Place the definition of main_func_sp here. boot.asm references this	      */
/* symbol and we should prevent unneeded modules/routines from getting	      */
/* linked in while referring main_func_sp.				      */
/*                                                                            */
/******************************************************************************/

unsigned long main_func_sp       = 0; /* SP of main              */
