FROM ubuntu:16.04

MAINTAINER qunfengqiu

COPY ./CH_HCNetSDK_V5.3.5.2_build20171124_Linux64 /opt/CH_HCNetSDK_V5.3.5.2_build20171124_Linux64

ENV ICEGRID_VERSION 3.7.2

RUN  buildDeps=' \
     wget \
    gcc \
    g++ \
    make \
    libtool \
    cmake \
    git \
    zeroc-ice-all-runtime \
    zeroc-ice-all-dev \
    autoconf \
    automake \
    libtool \
' \ 
    && apt-key adv --keyserver keyserver.ubuntu.com --recv B6391CB2CFBA643D \
    && echo "deb http://download.zeroc.com/Ice/3.7/ubuntu16.04 stable main" >> /etc/apt/sources.list.d/ice.list \
    && apt-get update \
    && apt-get install --no-install-recommends --no-install-suggests -y \
            $buildDeps
    #&& rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/*

COPY jthread-1.3.3.tar.gz /tmp/jthread-1.3.3.tar.gz
COPY jrtplib-3.11.1.tar.gz /tmp/jrtplib-3.11.1.tar.gz

RUN cd /tmp \
    && mkdir /tmp/jthread \
    && mkdir /tmp/jrtplib \
    && tar -zxvf jthread-1.3.3.tar.gz -C /tmp/jthread --strip-components=1 \
    && tar -zxvf jrtplib-3.11.1.tar.gz -C /tmp/jrtplib --strip-components=1 \
    && cd /tmp/jthread \
    && cmake . \
    && make \
    && make install \
    && cd /tmp/jrtplib \
    && cmake . \
    && make \
    && make install
 
RUN cd /tmp \
    && git clone https://github.com/google/glog.git \
    && cd /tmp/glog \
    && ./autogen.sh \
    && ./configure \
    && make \
    && make install

COPY repo /tmp/repo

RUN apt-get install --no-install-recommends --no-install-suggests -y libboost-dev

RUN cp -r /usr/share/ice /usr/share/Ice-3.7.2 \
    && cd /tmp/repo \
    && mkdir build \
    && cd /tmp/repo/build \
    && cmake .. \
    && make \
    && ls bin


EXPOSE 4061 4062

VOLUME ["/var/lib/ice/icegrid"]


ENTRYPOINT ["/usr/bin/icegridregistry", "--Ice.Config=/etc/icegridregistry.conf", "--IceGrid.Registry.Data=/var/lib/ice/icegrid"]


