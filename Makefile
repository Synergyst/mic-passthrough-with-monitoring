all: micproc
	@echo Build complete

install: micproc
	cp micproc /usr/local/bin/mic-passthrough-with-monitoring

micproc:
	gcc -std=gnu99 main.c -o micproc -march=armv8-a -mfpu=neon -ftree-vectorize -flax-vector-conversions -ldl -lm -lpthread -latomic
# TODO: add RNNoise option back at some point, currently having popping and cracking issues when it was implemented
#	gcc -std=gnu99 main.c -o micproc -march=armv8-a -mfpu=neon -ftree-vectorize -flax-vector-conversions -ldl -lm -lpthread -latomic `pkg-config --cflags --libs rnnoise`

clean:
	rm -f micproc
