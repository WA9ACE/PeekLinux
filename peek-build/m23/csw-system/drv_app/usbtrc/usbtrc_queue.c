#include "usbtrc/usbtrc_i.h"
#include "usbtrc/usbtrc_env.h"

/*#include "rv/rv_general.h"
#include "rvm/rvm_gen.h"*/
#include <string.h>


/**
 * Local variables
 */

static T_USBTRC_XFER_QUEUE qw;
static T_USBTRC_XFER_QUEUE qr;


/**
 *---------------------------------------------------------------------------------*
 * @name usbtrc_q_tx_create
 *
 * allocate memory for write queue
 *
 * @param void
 *
 * @return	void
 *---------------------------------------------------------------------------------*
 */
/*@{*/
void usbtrc_q_tx_create (void)
{
	T_USBTRC_USB_DEVICE *usbdev;
	USBTRC_SEND_TRACE("USBTRC: usbtrc_q_tx_create", RV_TRACE_LEVEL_WARNING);

	usbdev = usbtrc_env_ctrl_blk_p->usb_device;
	qw = usbdev->queue_write;

//	usbtrc_get_mem(&qw.buffer, BUFFER_TX_SIZE);
	qw.buffer = NULL;

#if( WCP_PROF == 1)
	usbtrc_get_mem(&usbdev->buffer_tx_temp, BUFFER_TX_SIZE);
#else
	usbtrc_get_mem(&usbdev->buffer_tx_temp, USBTRC_MAX_PACKET_SIZE);	
#endif
/*###==>ANO USBTRC 10222004 YL*/
    //OMAPS72906 change to static mutex
	rvf_initialize_static_mutex(&usbtrc_env_ctrl_blk_p->q_tx_mutex);
/*<==###ANO USBTRC*/
}
/*@}*/


/**
 *---------------------------------------------------------------------------------*
 * @name usbtrc_q_tx_destroy
 *
 * free memory for write queue
 *
 * @param void
 *
 * @return	void
 *---------------------------------------------------------------------------------*
 */
/*@{*/
void usbtrc_q_tx_destroy (void)
{
	T_USBTRC_USB_DEVICE *usbdev;
	usbdev = usbtrc_env_ctrl_blk_p->usb_device;

	USBTRC_SEND_TRACE("USBTRC: usbtrc_q_tx_destroy", RV_TRACE_LEVEL_WARNING);

	rvf_free_buf(usbdev->buffer_tx_temp);
	//rvf_free_buf(qw.buffer);
/*###==>ANO USBTRC 10222004 YL*/
	rvf_delete_mutex(&usbtrc_env_ctrl_blk_p->q_tx_mutex);
/*<==###ANO USBTRC*/
}
/*@}*/

/**
 *---------------------------------------------------------------------------------*
 * @name usbtrc_q_tx_init
 *
 * initialize pointers values
 *
 * @param void
 *
 * @return	void
 *---------------------------------------------------------------------------------*
 */
/*@{*/
void usbtrc_q_tx_init (void)
{

	USBTRC_SEND_TRACE("USBTRC: usbtrc_q_tx_init", RV_TRACE_LEVEL_WARNING);

	qw.read		= qw.buffer;
	qw.write	= qw.buffer;
	qw.size		= 0;
	qw.max_size = BUFFER_TX_SIZE;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------*
 * @name usbtrc_q_tx_try
 *
 * check if the given amount of data can be written
 *
 * @param UINT16 nbytes : the number of request bytes to write
 *
 * @return	BOOL : TRUE if this amount of data can be written
 *---------------------------------------------------------------------------------*
 */
/*@{*/
BOOL usbtrc_q_tx_try (UINT16 nbytes)
{
	USBTRC_TRACE_WARNING_PARAM("USBTRC: usbtrc_q_tx_try", nbytes);


	if(nbytes > BUFFER_TX_SIZE) return FALSE;
	if(nbytes > BUFFER_TX_SIZE - qw.size) return FALSE;

	return TRUE;
}
/*@}*/


/**
 *---------------------------------------------------------------------------------*
 * @name usbtrc_q_tx_put
 *
 * writes into the tx Queue
 *
 * @param char *buf_p : the source buffer
 * @param UINT16 nbytes : the number of request bytes to write
 *
 * @return	UINT16 : the amount of data but into the fifo
 *---------------------------------------------------------------------------------*
 */
/*@{*/
UINT16 usbtrc_q_tx_put (char *buf_p, UINT16 nbytes)
{
	UINT16 written_bytes;

	USBTRC_TRACE_WARNING_PARAM("USBTRC: usbtrc_q_tx_put", nbytes);

	written_bytes = 0;
/*###==>ANO USBTRC 10222004 YL*/
	tx_lock();
/*<==###ANO USBTRC*/
	written_bytes = usbtrc_write_fifo(&qw,buf_p,nbytes);
/*###==>ANO USBTRC 10222004 YL*/
	tx_unlock();
/*<==###ANO USBTRC*/

	return written_bytes;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------*
 * @name usbtrc_q_tx_get
 *
 * retreive data from tx queue
 *
 * @param char *buf_p : the destination buffer
 * @param UINT16 nbytes : the number of requested bytes
 *
 * @return	UINT16 : the amount of data retreived from fifo
 *---------------------------------------------------------------------------------*
 */
/*@{*/
UINT16 usbtrc_q_tx_get (char *buf_p, UINT16 nbytes)
{
	UINT16 read_bytes = 0;

	USBTRC_TRACE_WARNING_PARAM("USBTRC: usbtrc_q_tx_get", nbytes);
/*###==>ANO USBTRC 10222004 YL*/
	tx_lock();
/*<==###ANO USBTRC*/
	read_bytes = usbtrc_read_fifo(&qw,buf_p,nbytes);
/*###==>ANO USBTRC 10222004 YL*/
	tx_unlock();
/*<==###ANO USBTRC*/

	return read_bytes;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------*
 * @name usbtrc_q_rx_create
 *
 * allocate the transfer buffer en queue four IN requests
 *
 * @param char void
 *
 * @return	void
 *---------------------------------------------------------------------------------*
 */
/*@{*/
void usbtrc_q_rx_create (void)
{
	T_USBTRC_USB_DEVICE *usbd_p;
	usbd_p = usbtrc_env_ctrl_blk_p->usb_device;

	USBTRC_SEND_TRACE("USBTRC: usbtrc_q_rx_create", RV_TRACE_LEVEL_WARNING);

	qr = usbd_p->queue_read;

	/*allocate the buffers*/
	usbtrc_get_mem(&usbd_p->buffer_rx_temp, USBTRC_BULK_ENDPOINT_SIZE);
	usbtrc_get_mem(&qr.buffer, BUFFER_RX_SIZE + USBTRC_BULK_ENDPOINT_SIZE);
/*###==>ANO USBTRC 10222004 YL*/
    //OMAPS72906 change to static mutex
	rvf_initialize_static_mutex(&usbtrc_env_ctrl_blk_p->q_rx_mutex);
/*<==###ANO USBTRC*/
}
/*@}*/

/**
 *---------------------------------------------------------------------------------*
 * @name usbtrc_q_rx_destroy
 *
 * free the transfer buffer en queue four IN requests
 *
 * @param char void
 *
 * @return	void
 *---------------------------------------------------------------------------------*
 */
/*@{*/
void usbtrc_q_rx_destroy (void)
{
	T_USBTRC_USB_DEVICE *usbd_p;
	usbd_p = usbtrc_env_ctrl_blk_p->usb_device;

	USBTRC_SEND_TRACE("USBTRC: usbtrc_q_rx_destroy", RV_TRACE_LEVEL_WARNING);

	rvf_free_buf(usbd_p->buffer_rx_temp);
	rvf_free_buf(qr.buffer);
/*###==>ANO USBTRC 10222004 YL*/
	rvf_delete_mutex(&usbtrc_env_ctrl_blk_p->q_tx_mutex);
/*<==###ANO USBTRC*/
}
/*@}*/


/**
 *---------------------------------------------------------------------------------*
 * @name usbtrc_q_rx_init
 *
 * inti RX fifo pointers
 *
 * @param char void
 *
 * @return	void
 *---------------------------------------------------------------------------------*
 */
/*@{*/

void usbtrc_q_rx_init (void)
{

	USBTRC_SEND_TRACE("USBTRC: usbtrc_q_rx_init", RV_TRACE_LEVEL_WARNING);

	qr.read		= qr.buffer;
	qr.write	= qr.buffer;
	qr.size		= 0;
	qr.max_size = BUFFER_RX_SIZE + USBTRC_BULK_ENDPOINT_SIZE;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------*
 * @name usbtrc_q_rx_try
 *
 * test if the given amount of data can be written
 *
 * @param UINT16 nbytes : the number of requested bytes
 *
 * @return	BOOL : true if the given number of data can be written
 *---------------------------------------------------------------------------------*
 */
/*@{*/
BOOL usbtrc_q_rx_try (UINT16 nbytes)
{
	USBTRC_TRACE_WARNING_PARAM("USBTRC: usbtrc_q_rx_try", nbytes);

	if(nbytes > BUFFER_RX_SIZE) return FALSE;
	if(nbytes > (BUFFER_RX_SIZE - qr.size))
		return FALSE;

	return TRUE;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------*
 * @name usbtrc_q_rx_put
 *
 * queue data in the rx fifo
 *
 * @param char *buf_p : the source buffer
 * @param UINT16 nbytes : the number of request bytes to write
 *
 * @return	UINT16 : the amount of data but into the fifo
 *---------------------------------------------------------------------------------*
 */
/*@{*/
UINT16 usbtrc_q_rx_put (char *buf_p, UINT16 nbytes)
{
	UINT16 written_bytes;

	USBTRC_TRACE_WARNING_PARAM("USBTRC: usbtrc_q_tx_put", nbytes);

	written_bytes = 0;
/*###==>ANO USBTRC 10222004 YL*/
	rx_lock();
/*<==###ANO USBTRC*/
	written_bytes = usbtrc_write_fifo(&qr,buf_p,nbytes);
	rx_unlock();
/*###==>ANO USBTRC 10222004 YL*/
	rx_unlock();
/*<==###ANO USBTRC*/

	return written_bytes;
}
/*@}*/


/**
 *---------------------------------------------------------------------------------*
 * @name usbtrc_q_rx_get
 *
 * queue data in the rx fifo
 *
 * @param char *buf_p : the destination buffer
 * @param UINT16 nbytes : the number of request bytes to read
 *
 * @return	UINT16 : the amount of data but into the user buffer
 *---------------------------------------------------------------------------------*
 */
/*@{*/
UINT16 usbtrc_q_rx_get (char *buf_p, UINT16 nbytes)
{
		UINT16 read_bytes = 0;

	USBTRC_TRACE_WARNING_PARAM("USBTRC: usbtrc_q_tx_get", nbytes);
/*###==>ANO USBTRC 10222004 YL*/
	rx_lock();
/*<==###ANO USBTRC*/
	read_bytes = usbtrc_read_fifo(&qr,buf_p,nbytes);
/*###==>ANO USBTRC 10222004 YL*/
	rx_unlock();
/*<==###ANO USBTRC*/
	return read_bytes;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------*
 * @name usbtrc_q_rx_info
 *
 * return number of bytes in queue
 *
 * @param void
 *
 * @return	UINT16 : the amount of data in queue
 *---------------------------------------------------------------------------------*
 */
/*@{*/
UINT16 usbtrc_q_rx_info (void)
{
	USBTRC_SEND_TRACE("USBTRC: usbtrc_q_rx_info", RV_TRACE_LEVEL_WARNING);

	return qr.size;
}
/*@}*/


/**
 *---------------------------------------------------------------------------------*
 * @name usbtrc_write_fifo
 *
 * generic function to write in a fifo
 *
 * @param T_USBTRC_XFER_QUEUE *fifo_struct : the fifo to write in
 * @param char *buf_p : the input buffer
 * @param UINT16 size : the amount of data to write
 *
 * @return	UINT16 : the amount of data written in queue
 *---------------------------------------------------------------------------------*
 */
/*@{*/
UINT16 usbtrc_write_fifo(T_USBTRC_XFER_QUEUE *fifo_struct, char *buf_p, UINT16 size)
{
	char		*src_p,*read_p,*write_p,*endbuf_p;
	UINT16		stop,i,insize;
	UINT16		max_size = 0;

	max_size = fifo_struct->max_size;

	/*Get back buffer information*/
	src_p = fifo_struct->buffer;

	/*init our variables*/
	read_p = fifo_struct->read;
	write_p = fifo_struct->write;
	insize = fifo_struct->size;
	endbuf_p = src_p + max_size - 1;

	stop = size;
	i = 0;
	while(i < stop)
	{
		/* if fifo not empty, advance pointer before writing something*/
		if(insize != 0)
		{
			write_p++;
			if(write_p == read_p)
			{
				write_p--;
				stop = i;
				/*buffer full*/
			}
			if(write_p > endbuf_p)
			{
				if(read_p != src_p)
				{
					/*we arrive at the end of fifo and it's not full*/
					write_p = src_p;
				}
				else
				{
					//*we arrive at the end of fifo and it's full*/
					write_p--;
					stop = i;
				}
			}
		}
		/*if we can write , let's write 1 byte*/
		if(stop != i)
		{
			*write_p = *buf_p++;
			i++;
			insize++;
		}
	}
	// update of structure
	fifo_struct->write = write_p;
	//	fifo_struct->Size = insize;
	fifo_struct->size += stop;
	return stop;
}
/*@}*/

/**
 *---------------------------------------------------------------------------------*
 * @name usbtrc_read_fifo
 *
 * generic function to write in a fifo
 *
 * @param T_USBTRC_XFER_QUEUE *fifo_struct : the fifo to read from
 * @param char *buf_p : the output buffer
 * @param UINT16 size : the amount of data to read
 *
 * @return	UINT16 : the amount of data read from queue
 *---------------------------------------------------------------------------------*
 */
/*@{*/
UINT16 usbtrc_read_fifo(T_USBTRC_XFER_QUEUE *fifo_struct, char *buf_p, UINT16 size)
{
	char		*src_p,*read_p,*endbuf_p;
	UINT16		stop,i,insize;
	UINT16		max_size = 0;

	max_size = fifo_struct->max_size;

	/*Get back buffer information*/
	src_p = fifo_struct->buffer;

	read_p = fifo_struct->read;
	insize = fifo_struct->size;
	endbuf_p = src_p + max_size - 1;

	stop = size;
	i = 0;
	while((i < stop) && (insize != 0 ))
	{
		*buf_p++ = *read_p;
		insize--;
		i++;
		/*move of the pointer*/
		if(insize != 0)
		{
			read_p++;
		}
		if(read_p > endbuf_p)
		{
			/*we are at the end of the fifo*/
			read_p = src_p;
		}
	}
	/*update of structure*/

	fifo_struct->read = read_p;
	fifo_struct->size -= i;

	return i;
}
/*@}*/

