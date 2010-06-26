/*=============================================================================
 *    Copyright Texas Instruments 2003-2004. All Rights Reserved. 
 */
#ifndef BSPUTIL_ASSERT_HEADER
#define BSPUTIL_ASSERT_HEADER

/*===========================================================================
 */
/*!
 * @define BSPUTIL_ASSERT
 * 
 * @discussion
 * <b> Description  </b><br>
 *    This macro implements an ANSI C like assert macro. This is used instead
 *    of ANSI C assert to perform platform specific indications of the assert.
 *
 * <b> Context </b><br>
 *    Can be called anywhere.
 *
 * <b> Global Variables </b><br>
 *    None.
 *
 * <b> Returns </b><br>
 *    Nothing.
 * 
 * Param  _condition
 *            This is the condition to check for assertion.
 */
#define BSPUTIL_ASSERT( _condition )         \
{                                            \
    if( !(_condition) )                      \
    {                                        \
        while(1);                            \
    }                                        \
}

#endif
