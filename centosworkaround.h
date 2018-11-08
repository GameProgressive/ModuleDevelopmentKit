#ifndef _MDK_CENTOSWORDAROUND_H_
#define _MDK_CENTOSWORDAROUND_H_

// This work is to force CentOS operative system to include default errmsg rather than rh-mariadb102
#include "/usr/include/mysql/errmsg.h"
#undef ER

#endif
