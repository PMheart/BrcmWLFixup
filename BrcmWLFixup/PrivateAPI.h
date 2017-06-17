//
//  PrivateAPI.h
//  BrcmWLFixup
//
//  Created by Vanilla on 6/17/17.
//  Copyright © 2017 Vanilla. All rights reserved.
//

#ifndef PrivateAPI_h
#define PrivateAPI_h

// get array size, from syscl's repo
// https://github.com/syscl/EnableLidWake/blob/master/EanbleLidWake/syscl_lib.h#L41
#define getArrayLength(Array) ((sizeof(Array)/sizeof(0[Array])) / ((size_t)(!(sizeof(Array) % sizeof(0[Array])))))

#endif /* PrivateAPI_h */
