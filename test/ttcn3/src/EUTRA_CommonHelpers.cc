// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: AGPL-3.0-or-later

#include "CommonDefs.hh"
#include <stdarg.h>
#include <string.h>
#include <time.h>

namespace CommonDefs {

extern void fx__GetSystemTime(Struct__tm__Type& p__Struct__tm, INTEGER& p__TimezoneInfo)
{
  // local system time and the difference between local time and UTC in seconds without daylight saving, e.g. 3600 for
  // CET, as described in CommonDefs.ttcn
  time_t    now = time(NULL);
  struct tm local_tm;
  tzset();
  localtime_r(&now, &local_tm);

  p__Struct__tm.tm__sec()   = local_tm.tm_sec;
  p__Struct__tm.tm__min()   = local_tm.tm_min;
  p__Struct__tm.tm__hour()  = local_tm.tm_hour;
  p__Struct__tm.tm__mday()  = local_tm.tm_mday;
  p__Struct__tm.tm__mon()   = local_tm.tm_mon;
  p__Struct__tm.tm__year()  = local_tm.tm_year;
  p__Struct__tm.tm__wday()  = local_tm.tm_wday;
  p__Struct__tm.tm__yday()  = local_tm.tm_yday;
  p__Struct__tm.tm__isdst() = local_tm.tm_isdst;

  // timezone holds the seconds west of UTC of the standard time
  p__TimezoneInfo = (int)-timezone;
}

} // namespace CommonDefs
