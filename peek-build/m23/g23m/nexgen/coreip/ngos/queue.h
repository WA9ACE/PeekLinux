/*****************************************************************************
 * $Id: queue.h,v 1.4 2001/03/28 14:53:47 rf Exp $
 * $Name: rel_1_3_b_10 $
 *----------------------------------------------------------------------------
 * NexGenOS v1.3
 * Generic Queue Management
 *----------------------------------------------------------------------------
 *    Copyright (c) 1998-2001 NexGen Software.
 *
 *  All rights reserved. NexGen Software' source code is an unpublished
 *  work and the use of a copyright notice does not imply otherwise.
 *  This source code contains confidential, trade secret material of
 *  NexGen Software. Any attempt or participation in deciphering, decoding,
 *  reverse engineering or in any way altering the source code is
 *  strictly prohibited, unless the prior written consent of
 *  NexGen Software is obtained.
 *
 *    This software is  supplied  under  the terms of a
 *    license agreement or nondisclosure agreement with
 *    NexGen Software, and may not be copied or disclosed
 *    except  in  accordance  with  the  terms of  that
 *    agreement.
 *
 *----------------------------------------------------------------------------
 * 08/09/98 - Regis Feneon
 * 18/01/99 -
 *  macros redefinition in respect to the specs
 * 15/02/2000 -
 *  added NG_NODE_INIT()
 * 30/01/2001 -
 *  new NGsnode structure that can be embededded in any structure
 *****************************************************************************/

#ifndef __NG_QUEUE_H_INCLUDED__
#define __NG_QUEUE_H_INCLUDED__

/* NG_BEGIN_DECLS // confuses Source Insight */

/*
 * Node
 */
typedef struct NGnode_S {
    struct NGnode_S *prev;
    struct NGnode_S *next;
} NGnode;

/* initialize single node */
#define NG_NODE_INIT( n) { \
    ((NGnode *)(n))->prev = (NGnode *)(n); \
    ((NGnode *)(n))->next = (NGnode *)(n); \
}

/* insert n2 before n1 in the circular list */
#define NG_NODE_IN( n1, n2) { \
    ((NGnode *)(n2))->prev = ((NGnode *)(n1))->prev; \
    ((NGnode *)(n2))->next = (NGnode *)(n1); \
    ((NGnode *)(n1))->prev->next = (NGnode *)(n2); \
    ((NGnode *)(n1))->prev = (NGnode *)(n2); \
}

/* extract n2 following n1 in the circular list */
#define NG_NODE_OUT( n1, n2) { \
    if( ((NGnode *)(n1))->next == (NGnode *)(n1)) { \
        (n2) = NULL; \
    } \
    else { \
        (n2) = (void *)(((NGnode *)(n1))->next); \
        ((NGnode *)(n1))->next = ((NGnode *)(n2))->next; \
        ((NGnode *)(n2))->next->prev = (NGnode *)(n1); \
        ((NGnode *)(n2))->prev = (NGnode *)(n2); \
        ((NGnode *)(n2))->next = (NGnode *)(n2); \
    } \
}

/* extract n from the circular list */
#define NG_NODE_DETACH( n) { \
    ((NGnode *)(n))->next->prev = ((NGnode *)(n))->prev; \
    ((NGnode *)(n))->prev->next = ((NGnode *)(n))->next; \
    ((NGnode *)(n))->prev = (NGnode *)(n); \
    ((NGnode *)(n))->next = (NGnode *)(n); \
}

/* Super-Nodes... */
typedef struct NGsnode_S {
  void *prev; /* next structure */
  void *next; /* previous structure */
} NGsnode;

/* initialize node structure */
/*  p     pointer to structure containing the node */
/*  type  type of structure */
/*  elm   name of snode element in structure */
#define NG_SNODE_INIT( p, type, elm) { \
  (p)->elm.prev = (void *) (p); \
  (p)->elm.next = (void *) (p); \
}

/* get next and previous elements in the list */
#define NG_SNODE_NEXT( p, type, elm) ((type *)(p)->elm.next)
#define NG_SNODE_PREV( p, type, elm) ((type *)(p)->elm.prev)

/* insert p2 before p1 in the circular list */
#define NG_SNODE_IN_BEFORE( p1, p2, type, elm) { \
  (p2)->elm.prev = (p1)->elm.prev; \
  (p2)->elm.next = (void *)(p1); \
  ((type *)(p1)->elm.prev)->elm.next = (void *)(p2); \
  (p1)->elm.prev = (void *)(p2); \
}

/* insert p2 after p1 in the circular list */
#define NG_SNODE_IN_AFTER( p1, p2, type, elm) { \
  (p2)->elm.prev = (void *)(p1); \
  (p2)->elm.next = (p1)->elm.next; \
  ((type *)(p1)->elm.next)->elm.prev = (void *)(p2); \
  (p1)->elm.next = (void *)(p2); \
}

/* extract p from the circular list */
#define NG_SNODE_DETACH( p, type, elm) { \
  ((type *)((p)->elm.next))->elm.prev = (p)->elm.prev; \
  ((type *)((p)->elm.prev))->elm.next = (p)->elm.next; \
  (p)->elm.prev = (void *)(p); \
  (p)->elm.next = (void *)(p); \
}

/*
 * Message Buffer Queue
 */
typedef struct {
    NGnode *qu_head;
    NGnode *qu_tail;
    int qu_nel;
    int qu_nelmax;
} NGqueue;

/* Initialize the fifo queue */
#define NG_QUEUE_INIT( q, nmax) { \
    (q)->qu_head = NULL; \
    (q)->qu_tail = NULL; \
    (q)->qu_nel = 0; \
    (q)->qu_nelmax = nmax; \
}

/* add node at the tail of the queue */
#define NG_QUEUE_IN( q, n) { \
    ((NGnode *) (n))->next = NULL; \
    ((NGnode *) (n))->prev = (q)->qu_tail; \
    if( (q)->qu_tail) { \
        (q)->qu_tail->next = (NGnode *) (n); \
        (q)->qu_tail = (NGnode *) (n); \
    } \
    else { \
        (q)->qu_head = (NGnode *) (n); \
        (q)->qu_tail = (NGnode *) (n); \
    } \
    (q)->qu_nel++; \
}

/* get node at the head of the queue */
#define NG_QUEUE_OUT( q, n) { \
    (n) = (void *) ((q)->qu_head); \
    if( n) { \
        (q)->qu_head = ((NGnode *) (n))->next; \
        if( (q)->qu_head ) (q)->qu_head->prev = NULL; \
        else (q)->qu_tail = NULL; \
        ((NGnode *) (n))->prev = NULL; \
        ((NGnode *) (n))->next = NULL; \
        (q)->qu_nel--; \
    } \
}

/* peek the first element */
#define NG_QUEUE_PEEK( q, n) { \
    (n) = (void *) ((q)->qu_head); \
}

/* get the next element following n1 in n2*/
#define NG_QUEUE_NEXT( q, n1, n2) { \
    (n2) = (void *) (((NGnode *)(n1))->next); \
}

/* test if queue is empty */
#define NG_QUEUE_EMPTY( q) ((q)->qu_nel == 0)

/* test if queue is full */
#define NG_QUEUE_FULL( q) ((q)->qu_nel >= (q)->qu_nelmax)

/* NG_END_DECLS // confuses Source Insight */

#endif

