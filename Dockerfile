FROM alpine:latest AS builder

RUN apk update && apk upgrade && apk add alpine-sdk && apk add linux-headers && apk add cmake && apk add meson && rm -rf /var/cache/apk/*

# setup pistache
RUN git clone https://github.com/pistacheio/pistache.git
WORKDIR /pistache
RUN meson setup build && meson install -C build

# setup netcut
RUN mkdir -p /NetCut
WORKDIR /NetCut
COPY . .
RUN make

FROM alpine:latest
RUN apk add --no-cache libstdc++
COPY --from=builder /usr/local/lib /usr/local/lib
COPY --from=builder /NetCut/bin/netcut /netcut

ENTRYPOINT [ "/netcut" ]
CMD [ "--port", "9090" ]