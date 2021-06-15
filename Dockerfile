FROM alpine:latest

RUN apk update && apk upgrade
RUN apk add alpine-sdk && apk add linux-headers && rm -rf /var/cache/apk/*

RUN git clone https://github.com/cdes5804/NetCut.git

WORKDIR /NetCut
RUN make

ENTRYPOINT [ "/NetCut/bin/main" ]