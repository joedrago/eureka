// ---------------------------------------------------------------------------
//                         Copyright Joe Drago 2012.
//         Distributed under the Boost Software License, Version 1.0.
//            (See accompanying file LICENSE_1_0.txt or copy at
//                  http://www.boost.org/LICENSE_1_0.txt)
// ---------------------------------------------------------------------------

#ifndef DYNBASE_H
#define DYNBASE_H

#ifndef dynInt
#define dynInt int
#endif

#ifndef dynSize
#define dynSize int
#endif

#ifndef dynSizeFormat
#define dynSizeFormat "%d"
#endif

// P1 and P2 are "prefixed arguments", useful for passing userdata into a destroy function
typedef void (*dynDestroyFunc)(void *p);
typedef void (*dynDestroyFuncP1)(void *p1, void *p);
typedef void (*dynDestroyFuncP2)(void *p1, void *p2, void *p);

#endif
