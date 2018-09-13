#Makefile

OFILES=src/spaceflyer.o src/util.o src/spacecraft.o src/glad.o
CFILES=src/spaceflyer.cpp src/util.cpp src/spacecraft.cpp src/glad.c
CC=gcc
CFLAGS=

LIB= -lglfw -lX11 -lGL -lm -ldl -lstdc++ -I"../../include" -L"/usr/lib" sound/libIrrKlang.so -pthread
LIBM= -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo -lm -ldl -lstdc++ -I "/usr/local/lib" -L"/usr/local/lib" sound/libIrrKlang.dylib -pthread

all : spaceflyer

spaceflyer : $(OFILES)
	$(CC) $(OFILES) -o spaceflyer.x $(LIB)
	@rm -f src/*.o

mac : $(OFILES)
	$(CC) $(OFILES) -o spaceflyer.x $(LIBM)
	@rm -f src/*.o

	
clean : 
	@echo "CLEANING"
	@rm -f src/*.o
	
depend : 
	@echo "UPDATING DEPENDENCIES"
	@(sed '/^# DEPENDANCES/q' Makefile && \
	  $(CC) -MM $(CFLAGS) $(CFILES) | \
	  egrep -v "/usr/include" \
	 ) >Makefile.new
	@mv Makefile.new Makefile

# DEPENDANCES
spaceflyer.o: src/spaceflyer.cpp src/glad.h src/khrplatform.h \
  src/irrklang/irrKlang.h src/irrklang/ik_irrKlangTypes.h \
  src/irrklang/ik_vec3d.h src/irrklang/ik_IRefCounted.h \
  src/irrklang/ik_IVirtualRefCounted.h \
  src/irrklang/ik_ESoundOutputDrivers.h \
  src/irrklang/ik_ESoundEngineOptions.h src/irrklang/ik_EStreamModes.h \
  src/irrklang/ik_SAudioStreamFormat.h src/irrklang/ik_ISoundEngine.h \
  src/irrklang/ik_ISoundSource.h src/irrklang/ik_ISound.h \
  src/irrklang/ik_ISoundEffectControl.h src/irrklang/ik_IFileFactory.h \
  src/irrklang/ik_ISoundMixedOutputReceiver.h \
  src/irrklang/ik_IAudioStream.h src/irrklang/ik_IAudioStreamLoader.h \
  src/irrklang/ik_IFileReader.h \
  src/irrklang/ik_ISoundStopEventReceiver.h \
  src/irrklang/ik_ISoundDeviceList.h src/irrklang/ik_IAudioRecorder.h \
  src/util.hpp src/PerlinNoise.hpp src/spacecraft.hpp src/vertices.h
util.o: src/util.cpp src/glad.h src/khrplatform.h src/util.hpp \
  src/stb_image.h
spacecraft.o: src/spacecraft.cpp src/glad.h src/khrplatform.h \
  src/spacecraft.hpp src/util.hpp src/PerlinNoise.hpp
glad.o: src/glad.c src/glad.h src/khrplatform.h
