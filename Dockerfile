FROM ubuntu:20.04

LABEL description="COSMOS Container for HyTI flight software on iOBC" 

# Install apt packages
RUN apt-get update && apt upgrade -y
# Utility packages 
# TZDATA is now required to install cmake, quick fix to not break the docker install
RUN DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get -y install tzdata
RUN apt-get install git cmake -y 
RUN apt-get install build-essential -y 
RUN apt-get install rsync zip -y 
RUN apt-get install openssh-server -y 
RUN apt-get install -y iputils-ping

# Run COSMOS quick installer
RUN git clone https://github.com/hsfl/cosmos.git ~/cosmos
RUN chmod +x ~/cosmos/cosmos-install.sh
RUN chmod +x ~/cosmos/scripts/cosmos-repos.sh
RUN chmod +x ~/cosmos/scripts/cosmos-deploy.sh
RUN chmod +x ~/cosmos/scripts/cosmos-build.sh

WORKDIR /root/cosmos
RUN /root/cosmos/cosmos-install.sh

# make sure cosmos bin is in the path (although this should be resolved in the install step prior) 
ENV PATH="/root/cosmos/bin:${PATH}"


# prepare cross compiler for iOBC
RUN wget https://s3.amazonaws.com/kubos-world-readable-assets/iobc_toolchain.tar.gz
RUN mkdir /opt/kubos/
RUN tar xvzf iobc_toolchain.tar.gz -C /opt/kubos/

# fix missing library
RUN ln -s /usr/lib/x86_64-linux-gnu/libmpfr.so.6 /usr/lib/x86_64-linux-gnu/libmpfr.so.4

# prepare cross compiler for the dungeonpi (RPI4)
RUN mkdir ~/cosmos/toolchain
RUN cd ~/cosmos/toolchain
RUN wget https://github.com/Pro/raspi-toolchain/releases/latest/download/raspi-toolchain.tar.gz
RUN sudo tar xfz raspi-toolchain.tar.gz --strip-components=1 -C /opt

# clone hyti repositories
#RUN git clone git@192.168.150.7:cosmos/modules/devices/gps/oemv.git ~/cosmos/source/modules/devices/gps/oemv
#RUN git clone git@192.168.150.7:cosmos/modules/devices/cubespace.git ~/cosmos/source/modules/devices/cubespace
#RUN git clone git@192.168.150.7:cosmos/modules/devices/eps/isis.git ~/cosmos/source/modules/devices/eps/isis
#RUN git clone git@192.168.150.7:cosmos/modules/devices/radio/isis.git ~/cosmos/source/modules/devices/radio/isis


# Build code
#./do_cmake_iobc_release
#cd iobc_hyti_release
#make install
#rsync -auv ~/cosmos/iobc-flight/bin/ localuser@unibapfm:cosmos/iobc/bin
#ssh localuser@unibapfm
#scp -p cosmos/iobc/bin/{desired program} root@iobc:cosmos/bin
