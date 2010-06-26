/**
 * @file	camd_commands.h
 *
 * Internal commands definitions for CAMD.
 *
 * @author	raymond zandbergen ()
 * @version 0.1
 */

#ifndef CAMD_COMMANDS_H_
#define CAMD_COMMANDS_H_

/**
 *
 * Sends 'stop sensor' commands to sensor
 *
 * @param none                
 *
 * @return none
 *
 */

extern void camd_stop_sensor (void);

/**
 *
 * Sends 'switch snapshot' commands to sensor
 *
 * @param none                
 *
 * @return none
 *
 */

extern void camd_switch_snapshot (void);

/**
 *
 * Sends 'switch preview' commands to sensor
 *
 * @param none                
 *
 * @return none
 *
 */

extern void camd_switch_preview (void);

/**
 *
 * Sends 'init sensor' commands to sensor
 *
 * @param none                
 *
 * @return none
 *
 */

extern void camd_init_sensor (void);
/**
 *
 * Initialises the APLL clock register
 *
 * @param none                
 *
 * @return none
 *
 */

extern void camd_init_sensor_clock (void);

/**
 *
 * Stops the sensor clock
 *
 * @param none                
 *
 * @return none
 *
 */

//added on 5/5/06

/**
 *
 * changes the image dimensions for viewfinder and snapshot
 *
 * @param - height and width
 *
 * @return none
 *
 */

extern void camd_change_viewfinder_imagedim (UINT16 height, UINT16 width, T_CAMD_ENCODING encoding, BOOL flip_x, BOOL flip_y, UINT16 zoom);

extern void camd_change_snapshot_imagedim (UINT16 height, UINT16 width, T_CAMD_ENCODING encoding, BOOL flip_x, BOOL flip_y, UINT16 zoom);


//added-e

extern void camd_stop_sensor_clock (void);

/**
 *
 * Starts the sensor clock
 *
 * @param none                
 *
 * @return none
 *
 */

extern void camd_start_sensor_clock (void);


/**
 *
 * writes the viewfinder gamma correction to the camera
 *
 * @param gamma_correction
 *
 * @return T_RV_RET
 *
 */

extern T_RV_RET camd_write_viewfinder_gamma_correction_to_camera(T_CAMD_GAMMA gamma_correction);



/**
 *
 * writes the snapshot gamma correction to the camera
 *
 * @param gamma_correction
 *
 * @return T_RV_RET
 *
 */


extern T_RV_RET camd_write_snapshot_gamma_correction_to_camera(T_CAMD_GAMMA gamma_correction);

/**
 *
 * camd_tristate_on() function Tristates the Camera so that NAND FLASH and PARALLEL port are accessible simultaneously
 *
 * camd_tristate_off() function Switches off the Camera Trisate so that Parallel port is accessible.
 *
 *
 *
 */

extern void camd_tristate_on (void);

extern void camd_tristate_off (void);	


// UNUSED
// extern void camd_setup_sensor_preview (void);
// extern void camd_setup_sensor_snapshot (void);
// extern void camd_start_sensor (void);

#endif /* CAMD_COMMANDS_H_ */
