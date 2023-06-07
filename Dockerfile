FROM ubuntu:20.04

LABEL description="COSMOS Container for Artemis Flight Software on the Raspberry Pi" 

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

# fix missing library
RUN ln -s /usr/lib/x86_64-linux-gnu/libmpfr.so.6 /usr/lib/x86_64-linux-gnu/libmpfr.so.4

# prepare cross compiler for the dungeonpi (RPI4)
RUN mkdir ~/cosmos/toolchain
RUN cd ~/cosmos/toolchain
RUN wget https://github.com/Pro/raspi-toolchain/releases/latest/download/raspi-toolchain.tar.gz
RUN sudo tar xfz raspi-toolchain.tar.gz --strip-components=1 -C /opt
