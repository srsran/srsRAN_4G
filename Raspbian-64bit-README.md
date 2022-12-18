
## Additional srsran install instructions for Raspberry Pi 4 running Raspian 64-bit OS (Bullseye)

### Intro
Please read in conjuction with the article on docs at
https://docs.srsran.com/en/latest/app_notes/source/pi4/source/index.html

### Update kernel
Update the kernel to the latest versions (heeding the warning on this not being a standard update!)...
```sudo rpi-update```

#### Install dependencies
Install ZeroMQ...
```sudo apt-get install libzmq3-dev```

Install pcsc development and tool support...
```sudo apt-get install libccid pcscd libpcsclite-dev```

### GNU C compiler - Debian v10.x bug
We need to suppress the stringop-overflow warning (which becomes an error) otherwise we get...
`/home/pi/srsRAN/lib/src/phy/ue/ue_ul.c:914:14: error: writing 16 bytes into a region of size 0 [-Werror=stringop-overflow=]`
...during a make : see GNU compiler bug commentary @ https://gcc.gnu.org/bugzilla/show_bug.cgi?id=99578 

This is done via a (partially-conditional) statement added to `CmakeLists.txt` at line 460...
```if(CMAKE_C_COMPILER_ID MATCHES "GNU" AND NOT CMAKE_C_COMPILER_VERSION VERSION_LESS 10.0)```
```  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-stringop-overflow")```
```endif()```

### Set CPU governor mode to performance
Install cpu utilities...
```sudo apt-get install cpufrequtils sysfsutils```

Check current governor mode...
```cpufreq-info```

Change mode across all CPUs (cores) to performance...
```sudo cpufreq-set -g performance```


Nick Hall

Inmarsat CTO

18th December 2022
