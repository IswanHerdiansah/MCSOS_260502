set confirm off
set pagination off

file build/kernel.elf

target remote localhost:1234

break kmain
break vmm_map_page
break vmm_unmap_page

continue
