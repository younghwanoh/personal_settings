all: amd apple intel nvidia

is_64=$(shell s=`uname -m`; if (echo $$s | grep x86_64 > /dev/null); then echo 1; fi)

CFLAGS=-Wall -W -O2

# AMD
PATH_TO_AMD_INC=/opt/AMDAPP/include/
ifeq ($(is_64), 1)
PATH_TO_AMD_LIB=/opt/AMDAPP/lib/x86_64/
else
PATH_TO_AMD_LIB=/opt/AMDAPP/lib/x86/
endif

# Intel
PATH_TO_INTEL_INC=/usr/include/
ifeq ($(is_64), 1)
PATH_TO_INTEL_LIB=/usr/lib64/intel/
endif

# NVIDIA
PATH_TO_NVIDIA_INC=/usr/local/cuda/include/
PATH_TO_NVIDIA_LIB=/usr/lib64/

EXE_NAME=$(patsubst %.c,%,$(wildcard *.c))

amd:    $(join $(EXE_NAME),.amd)
apple:  $(join apple.,$(EXE_NAME))
intel:  $(join $(EXE_NAME),.intel)
nvidia: $(join $(EXE_NAME),.nvidia)

%.amd:%.c
	clang $(CFLAGS) -I$(PATH_TO_AMD_INC) -L$(PATH_TO_AMD_LIB) -Wl,-rpath,$(PATH_TO_AMD_LIB)  -o $@ $< -lOpenCL

apple.%:%.c
	clang $(CFLAGS) -framework opencl -o $@ $<

%.intel:%.c
#	gcc $(CFLAGS) -I$(PATH_TO_INTEL_INC) -L$(PATH_TO_INTEL_LIB) -Wl,-rpath,$(PATH_TO_INTEL_LIB) -o $@ $< -lOpenCL
	gcc $(CFLAGS) -L/usr/lib/intel -Wl,-rpath,/usr/lib/intel -I$(PATH_TO_INTEL_INC) -o $@ $< -lOpenCL

%.nvidia:%.c
	clang $(CFLAGS) -I$(PATH_TO_NVIDIA_INC) -L$(PATH_TO_NVIDIA_LIB) -o $@ $< -lOpenCL

clean:
	rm -f *.amd apple.* *.intel *.nvidia *.o
