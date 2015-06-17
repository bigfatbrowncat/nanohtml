//
//  gl_tools.h
//  nanohtml
//
//  Created by Ilya Mizus on 16.06.15.
//
//

#ifndef __nanohtml__gl_tools__
#define __nanohtml__gl_tools__

#ifdef __cplusplus
extern "C" {
#endif
	
typedef char BOOL;

#ifndef TRUE
#	define TRUE		1
#endif
#ifndef FALSE
#	define FALSE	0
#endif

BOOL glInit();

#ifdef __cplusplus
}
#endif

#endif /* defined(__nanohtml__gl_tools__) */
