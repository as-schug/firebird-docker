FROM debian:bullseye-slim

ENV PREFIX=/opt/firebird
ENV VOLUME=/firebird
ENV DEBIAN_FRONTEND noninteractive
ENV FBURL=https://github.com/FirebirdSQL/firebird/releases/download/R2_5_9/FirebirdCS-2.5.9.27139-0.amd64.tar.gz
ENV DBPATH=/opt/firebird
ENV ICU_URL=https://github.com/unicode-org/icu/releases/download/release-52-2/icu4c-52_2-src.tgz

RUN apt-get update && \
    apt-get install -qy --no-install-recommends \
        bzip2 \
        ca-certificates \
        curl \
	libssl-dev \
        g++ \
        gcc \
        libncurses5-dev \
        make \
        netbase \
        procps 
RUN mkdir -p /home/icu && \
        cd /home/icu && \
        curl -L -o icu4c.tar.gz -L "${ICU_URL}" && \
        tar --strip=1 -xf icu4c.tar.gz && \
        cd source && \
        ./configure --prefix=/usr && \
        make -j$(awk '/^processor/{n+=1}END{print n}' /proc/cpuinfo) && \
        make install && \
        cd / && \
        rm -rf /home/icu 
COPY ./inetd.conf /etc/	
RUN mkdir -p /home/firebird && \
    cd /home/firebird && \
    curl -L -o firebird.tar.gz -L \
        "${FBURL}" && \
    tar --strip=1 -xf firebird.tar.gz && \
    ./install -silent
    cd / && \

RUN ln -s /usr/lib/x86_64-linux-gnu/libssl.so /usr/lib/x86_64-linux-gnu/libssl.so.1.0.0

COPY ./LIB/uuidlib ${PREFIX}/UDF/uuidlib
COPY ./LIB/psudflib ${PREFIX}/UDF/psudflib
COPY ./run.sh /run.sh
COPY ./LIB/libps_utils.so /usr/lib/libps_utils.so

CMD bash run.sh
