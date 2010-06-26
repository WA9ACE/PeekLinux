#ifndef __USBTRC_QUEUE_H_
#define __USBTRC_QUEUE_H_

/**
 * structures definition
 */
typedef struct
{ 
	char	*read;		/* ptr to read position */
	char	*write;		/* ptr to write position */
	UINT16	size;		/* amount of bytes ready to be consumed */
	UINT16	max_size;	/*total size of the fifo*/
	char	*buffer;	/* start of buffer */
} T_USBTRC_XFER_QUEUE;


/**
 * Functions
 */
extern void usbtrc_q_tx_create (void);
extern void usbtrc_q_tx_destroy (void);
extern void usbtrc_q_tx_init (void);
extern BOOL usbtrc_q_tx_try (UINT16 nbytes);
extern UINT16 usbtrc_q_tx_put (char *buf_p, UINT16 nbytes);
extern UINT16 usbtrc_q_tx_get (char *buf_p, UINT16 nbytes);
extern void usbtrc_q_rx_create (void);
extern void usbtrc_q_rx_destroy (void);
extern void usbtrc_q_rx_init (void);
extern BOOL usbtrc_q_rx_try (UINT16 nbytes);
extern UINT16 usbtrc_q_rx_put (char *buf_p, UINT16 nbytes);
extern UINT16 usbtrc_q_rx_get (char *buf_p, UINT16 nbytes);
extern UINT16 usbtrc_q_rx_info (void);

#endif // __USBTRC_QUEUE_H_
