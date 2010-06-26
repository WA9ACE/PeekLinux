/**
 * @file   kpd_virtual_key_table_mgt.c
 *
 * Coding of the access function to configuration keypad table.
 * These functions allows to determine :
 *    - the ASCII value associated to a virtual key,
 *    - The virtual key ID associated to a physical key Id
 *
 *
 * @author   Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  10/10/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "kpd/kpd_virtual_key_table_mgt.h"
#include "kpd/kpd_virtual_key_table_def.h"


/**
 * @name Virtual keys table management functions
 *
 */
/*@{*/

/**
 * function: kpd_initialize_ascii_table
 */
T_RV_RET kpd_initialize_ascii_table(void)
{
   UINT8 i;
   INT8 position = KPD_POS_NOT_AVAILABLE;
   T_VIRTUAL_ASCII_TABLE table;
   
   /* Copy all the table */
   for (i = 0; i < KPD_NB_PHYSICAL_KEYS; i++)
   {
      table[i].key_id = virtual_ascii_table[i].key_id;
      table[i].default_value_p = virtual_ascii_table[i].default_value_p;
      table[i].alphanumeric_value_p = virtual_ascii_table[i].alphanumeric_value_p;
   }

   for (i = 0; i < KPD_NB_PHYSICAL_KEYS; i++)
   {
      if (table[i].key_id != KPD_KEY_NULL)
      {
         /* Retrieve virtual key position (define in fact the pkysical key Id) */
         kpd_retrieve_virtual_key_position(table[i].key_id,
                                           KPD_DEFAULT_MODE,
                                           &position);

         if (position == KPD_POS_NOT_AVAILABLE)
            return RV_INTERNAL_ERR;

         /* Update ASCII according to the physical key Id */
         virtual_ascii_table[position].key_id = position;
         virtual_ascii_table[position].default_value_p = table[i].default_value_p;
         virtual_ascii_table[position].alphanumeric_value_p = table[i].alphanumeric_value_p;
      }
   }

   return RV_OK;
}


/**
 * function: kpd_get_virtual_key
 */
T_KPD_VIRTUAL_KEY_ID kpd_get_virtual_key( T_KPD_PHYSICAL_KEY_ID key_id,
                                          T_KPD_MODE mode)
{
   return vpm_table[key_id][mode];
}


/**
 * function: kpd_get_ascii_key_value
 */
T_RV_RET kpd_get_ascii_key_value(T_KPD_PHYSICAL_KEY_ID key_id,
                                 T_KPD_MODE mode,
                                 char** ascii_code_pp)
{
   /* Retrieve ASCII code according to the mode */
   if (mode == KPD_DEFAULT_MODE)
      *ascii_code_pp = virtual_ascii_table[key_id].default_value_p;
   else if (mode == KPD_ALPHANUMERIC_MODE)
      *ascii_code_pp = virtual_ascii_table[key_id].alphanumeric_value_p;
   else
      *ascii_code_pp = "";

   return RV_OK;
}

/**
 * function: kpd_retrieve_virtual_key_position
 */
T_RV_RET kpd_retrieve_virtual_key_position(  T_KPD_VIRTUAL_KEY_ID key_id,
                                             T_KPD_MODE mode,
                                             INT8* position_p)
{
   T_RV_RET ret = RV_INVALID_PARAMETER;
   INT8 i = 0;

   *position_p = KPD_POS_NOT_AVAILABLE;

   if (key_id != KPD_KEY_NULL)
   {
      for (i = 0; i < KPD_NB_PHYSICAL_KEYS; i++)
         if (vpm_table[i][mode] == key_id)
         {
            *position_p = i;
            ret = RV_OK;
            break;
         }
   }

   return ret;
}


/**
 * function: kpd_get_default_keys
 */
T_RV_RET kpd_get_default_keys( T_KPD_VIRTUAL_KEY_TABLE* available_keys_p)
{
   UINT8 nb_available_keys = 0;
   UINT8 i;

   for (i = 0; i < KPD_NB_PHYSICAL_KEYS; i++)
   {
      if (vpm_table[i][KPD_DEFAULT_MODE] != KPD_KEY_NULL)
      {
         available_keys_p->notified_keys[nb_available_keys] = vpm_table[i][KPD_DEFAULT_MODE];
         nb_available_keys++;
      }
   }
   available_keys_p->nb_notified_keys = nb_available_keys;

   return RV_OK;
}

/**
 * function: kpd_check_key_table
 */
T_RV_RET kpd_check_key_table(T_KPD_VIRTUAL_KEY_TABLE* keys_table_p, T_KPD_MODE mode)
{
   UINT8 i;
   INT8 position;
   T_KPD_VIRTUAL_KEY_ID virtual_key;
   UINT8 nb_notified_keys = 0;
   T_RV_RET ret = RV_OK;

   if (keys_table_p->nb_notified_keys == KPD_NB_PHYSICAL_KEYS)
   {
      /* Facility: if number of key is KPD_NB_PHYSICAL_KEYS, table of notified_keys
         is automatically fulfilled */
      for (i = 0; i < KPD_NB_PHYSICAL_KEYS; i++)
      {
         virtual_key = kpd_get_virtual_key(i, mode);
         if (virtual_key != KPD_KEY_NULL)
         {
            keys_table_p->notified_keys[nb_notified_keys] = virtual_key;
            nb_notified_keys++;
         }
      }
      keys_table_p->nb_notified_keys = nb_notified_keys;
   }
   else
   {
      /* In another case (number of key less than KPD_NB_PHYSICAL_KEYS), verify
         that all the virtual keys are defined in the mode requested by the client */
      for (i = 0; i < keys_table_p->nb_notified_keys; i++)
      {
         ret = kpd_retrieve_virtual_key_position(keys_table_p->notified_keys[i],
                                                 mode,
                                                 &position);
         if ( (ret != RV_OK) || (keys_table_p->notified_keys[i] == KPD_KEY_NULL) )
            return RV_INVALID_PARAMETER;
      }
   }
   return RV_OK;
}

/**
 * function: kpd_define_new_config
 */
void kpd_define_new_config(T_KPD_VIRTUAL_KEY_TABLE* reference_keys_p,
                           T_KPD_VIRTUAL_KEY_TABLE* new_keys_p)
{
   UINT8 i;
   INT8 position;

   /* Unset old values in the table */
   for (i = 0; i < KPD_NB_PHYSICAL_KEYS; i++)
      vpm_table[i][KPD_MODE_CONFIG] = KPD_KEY_NULL;

   /* Set values in the table */
   for (i = 0; i < reference_keys_p->nb_notified_keys; i++)
   {
      /*  */
      kpd_retrieve_virtual_key_position(reference_keys_p->notified_keys[i],
                                        KPD_DEFAULT_MODE,
                                        &position);
      vpm_table[position][KPD_MODE_CONFIG] = new_keys_p->notified_keys[i];
   }
}

/**
 * function: kpd_vpm_table_is_valid
 */
BOOL kpd_vpm_table_is_valid(void)
{
   UINT8 i;

   for (i = 0; i < KPD_NB_PHYSICAL_KEYS; i++)
   {
      if (vpm_table[i][0] != i) return FALSE;
   }

   return TRUE;
}

/*@}*/
