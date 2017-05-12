/** @file
*  Various utilities. (C99 standard)
*
*  @author Piotr Styczyński <piotrsty1@gmail.com>
*  @copyright MIT
*  @date 2017-05-13
*/
#ifndef __STY_COMMON_UTILS_H__
#define __STY_COMMON_UTILS_H__

/**
* @def DEBUG
* Debug symbol definitions
*
* Use DEBUG 0 to disable debug macro DBG
* Use DEBUG 1 to enable debug macro DBG
*/
#define DEBUG 0

/**
* @def DBG
* Macro used for debug purposes
* The code block after it will be executed
* only if DEBUG macro was set to 1
*
* Works as if clause e.g.
* @code
* DBG {
*   printf("Hello");
* }
* @endcode
*/
#define DBG if(DEBUG)


#endif /*  __STY_COMMON_UTILS_H__ */
