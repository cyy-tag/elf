# elf function
Finds the dynamic library path and the offset value of the corresponding function for uprobe event use
# make example
$make  
./output/address targetpid libname funcname  
ex: ./output/address pid c malloc
