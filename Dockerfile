FROM alpine:latest

RUN apk update && apk upgrade && apk add alpine-sdk && apk add linux-headers && apk add cmake && apk add meson && rm -rf /var/cache/apk/*

# setup pistache
RUN git clone https://github.com/pistacheio/pistache.git
WORKDIR /pistache
RUN meson setup build && meson install -C build

# setup netcut
WORKDIR /
RUN git clone https://github.com/cdes5804/NetCut.git
WORKDIR /NetCut
RUN make

ENTRYPOINT [ "/NetCut/bin/main" ]